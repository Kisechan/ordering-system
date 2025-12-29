# 客户端网络模块说明

## 架构设计

网络模块采用分层设计，从底层到业务层分别为：

```
┌─────────────────────────────────────┐
│    Controller 层（UI逻辑控制）        │
├─────────────────────────────────────┤
│   NetworkManager（业务网络）          │  <- Controller 直接使用此层
├─────────────────────────────────────┤
│   Protocol（协议构建）                │
│   ResponseParser（响应解析）          │
├─────────────────────────────────────┤
│   TcpClient（底层TCP客户端）           │
├─────────────────────────────────────┤
│   NetworkConfig（配置常量）           │
└─────────────────────────────────────┘
```

## 模块说明

### 1. NetworkConfig.h
配置文件，包含：
- `SERVER_IP`：服务器地址
- `SERVER_PORT`：服务器端口
- `TCP_TIMEOUT_MS`：TCP连接超时时间

### 2. Protocol.h/cpp
协议构建层，功能：
- 定义所有消息类型常量
- 提供构建各种请求JSON的函数
- 所有请求自动包含 `type`、`data`、`timestamp` 字段

**API 列表：**
```cpp
// 用户认证
QJsonObject buildLoginRequest(username, password);
QJsonObject buildRegisterRequest(username, password);

// 菜品
QJsonObject buildDishListRequest();

// 订单
QJsonObject buildOrderSubmitRequest(dishes);
QJsonObject buildOrderListRequest();
QJsonObject buildOrderCommentRequest(orderId, comment);

// 其他
QJsonObject buildCallWaiterRequest();
```

### 3. ResponseParser.h/cpp
响应解析层，功能：
- 解析服务器返回的JSON响应
- 提取 `code`、`msg`、`data` 字段
- 定义响应码枚举 (SUCCESS=200, PASSWORD_ERROR=301 等)
- 提供错误描述查询

**主要结构体：**
```cpp
struct Response {
    int code;              // 响应码
    QString message;       // 响应消息
    QJsonObject data;      // 响应数据
    bool isValid;          // 响应是否有效
};
```

### 4. TcpClient.h/cpp
底层TCP客户端，功能：
- 基于 QTcpSocket 的TCP连接管理
- 自动连接超时控制（使用 NetworkConfig::TCP_TIMEOUT_MS）
- 接收数据后自动解析JSON对象
- 错误处理和状态管理

**信号：**
- `connected()` - 连接成功
- `disconnected()` - 连接断开
- `jsonReceived(QJsonObject)` - 收到完整JSON
- `connectionError(QString)` - 连接错误

### 5. NetworkManager.h/cpp
业务网络层（**Controller 直接使用此类**）

**功能概览：**
- 管理服务器连接
- 提供业务级API（登录、注册、获取菜品等）
- 自动处理请求-响应匹配
- 发出业务级信号供Controller处理

**主要API：**
```cpp
// 连接管理
void connectToServer(ip, port);
void disconnectFromServer();
bool isConnected() const;

// 用户认证
void login(username, password);
void registerUser(username, password);

// 菜品
void getDishList();

// 订单
void submitOrder(dishes);
void getOrderList();
void submitOrderComment(orderId, comment);

// 其他
void callWaiter();
```

**信号列表：**
```cpp
// 连接相关
connected()
disconnected()
connectionError(error)

// 用户认证
loginSuccess()
loginFailed(error)
registerSuccess()
registerFailed(error)

// 菜品
dishListReceived(dishes)
dishListError(error)

// 订单
orderSubmitSuccess()
orderSubmitFailed(error)
orderListReceived(orders)
orderListError(error)
orderCommentSuccess()
orderCommentFailed(error)

// 其他
waiterCalled()
waiterCallError(error)
```

## 使用示例（Controller层）

```cpp
#include "NetworkManager.h"

class LoginController : public QObject {
    Q_OBJECT

private:
    NetworkManager* m_network;

public slots:
    void onLoginButtonClicked(const QString& username, const QString& password) {
        // 确保连接到服务器
        if (!m_network->isConnected()) {
            m_network->connectToServer("127.0.0.1", 8080);
        }

        // 发起登录请求
        m_network->login(username, password);
    }

private slots:
    void onLoginSuccess() {
        // 登录成功，切换界面
        qDebug() << "登录成功";
    }

    void onLoginFailed(const QString& error) {
        // 显示错误信息
        qDebug() << "登录失败：" << error;
    }

    void onConnectionError(const QString& error) {
        qDebug() << "连接错误：" << error;
    }
};
```

## 功能说明

当前网络模块实现了以下最小功能集合：

1. **TCP连接管理** - 连接、超时、断开、错误处理
2. **请求构建** - 自动构建符合协议的JSON请求
3. **响应解析** - 自动解析和验证服务器响应
4. **业务级API** - 提供7类业务操作的高级接口
5. **信号通知** - 完整的成功/失败回调机制
