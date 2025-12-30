# 数据库线程安全性修复文档

## 问题描述

服务端运行时出现以下错误：
```
QODBCResult::exec: Unable to execute statement: "[Microsoft][ODBC 驱动程序管理器] 函数序列错误"
```

## 问题原因

Qt 中的 `QSqlDatabase` 连接**不能在多个线程间共享**。原有设计中：

1. **全局共享连接**：`DbManager` 只维护一个全局的 `QSqlDatabase` 连接
2. **多线程并发访问**：当服务端处理多个客户端连接时，每个连接的 `readData()` 信号槽可能在不同线程执行
3. **并发冲突**：多个线程同时访问同一个数据库连接，导致 SQL 语句执行顺序混乱，ODBC 驱动报"函数序列错误"

## 解决方案

### 1. **线程本地存储（Thread Local Storage）**

使用 Qt 的 `QThreadStorage` 模板为每个线程创建独立的数据库连接：

**DbManager.h 修改：**
```cpp
// 使用 QThreadStorage 为每个线程存储独立的数据库连接
mutable QThreadStorage<QSqlDatabase> threadLocalDb_;
```

**DbManager.cpp 修改：**
```cpp
QSqlDatabase DbManager::getOrCreateThreadConnection() const {
    // 检查当前线程是否已有连接
    QSqlDatabase threadDb = threadLocalDb_.localData();
    if (threadDb.isValid() && threadDb.isOpen()) {
        return threadDb;  // 返回已有连接
    }

    // 当前线程没有连接，创建一个新的
    QString connectionName = cfg_.connectionName + "_" + QString::number((qint64)QThread::currentThread());
    
    // 为新线程创建独立的连接...
    threadDb.setDatabaseName(connStr);
    threadDb.open();
    
    threadLocalDb_.setLocalData(threadDb);
    return threadDb;
}
```

### 2. **连接有效性检查**

在服务端请求处理前验证数据库连接：

**Server.cpp 修改：**
```cpp
void Server::processRequest(QTcpSocket* socket, const QJsonObject& request) {
    QSqlDatabase conn = db::DbManager::instance().db();
    
    // 检查连接是否有效
    if (!conn.isValid() || !conn.isOpen()) {
        qWarning() << "[Server] Database connection failed";
        response["code"] = 500;
        response["msg"] = "服务器数据库连接失败";
        sendJson(socket, response);
        return;
    }
    
    // 继续处理请求...
}
```

### 3. **登录响应处理改进**

修复登录成功时从响应中提取用户信息的逻辑：

```cpp
// 如果登录成功，记录到 userMap
if (response["code"].toInt() == 200) {
    UserSession userSession;
    if (response.contains("data")) {
        QJsonObject data = response["data"].toObject();
        userSession.userId = data["user_id"].toInt();
        userSession.username = data["username"].toString();
        userMap[socket] = userSession;
        qDebug() << "[Server] User logged in: " << userSession.username;
    }
}
```

## 工作原理

1. **初始化阶段**：`DbManager::init()` 只保存配置，不创建连接
2. **运行时**：每个线程第一次调用 `db()` 时，自动创建属于该线程的独立连接
3. **线程隔离**：使用 `QThreadStorage` 确保线程 A 的连接与线程 B 的连接完全独立
4. **连接复用**：同一线程的多个请求复用该线程的连接（提高效率）

## 效果

修改后：
- ✅ 每个线程有独立的数据库连接
- ✅ 消除了并发访问冲突
- ✅ 不再出现"函数序列错误"
- ✅ 支持多客户端并发请求

## 测试步骤

1. 重新编译服务端：
   ```bash
   cd d:/Projects/ordering-system/build-server-Desktop_Qt_5_12_0_MinGW_64_bit-Debug
   qmake && mingw32-make clean && mingw32-make
   ```

2. 运行服务端，查看日志：
   ```
   Server started, listening on port 5523
   [Server] User logged in: username ID: 1  ✓ (无错误)
   ```

3. 多个客户端同时登录和提交订单，确保无"函数序列错误"

## 相关文件修改

- `d:/Projects/ordering-system/database/db/DbManager.h` - 添加线程本地存储
- `d:/Projects/ordering-system/database/db/DbManager.cpp` - 实现线程安全的连接获取
- `d:/Projects/ordering-system/server/server.cpp` - 添加连接有效性检查
- `d:/Projects/ordering-system/server/server.h` - 添加 QThread 头文件
