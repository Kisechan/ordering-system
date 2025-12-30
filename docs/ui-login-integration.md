# 客户端UI与网络模块集成

## 概述

客户端UI已完整集成NetworkManager网络模块，实现了以下业务流程：

## 集成路径

### 1. 登录流程 (LoginDialog)

**文件：logindialog.h/cpp**

**流程：**
```
用户输入用户名/密码
    ↓
点击"登录"按钮 → onLoginClicked()
    ↓
验证输入非空 → 调用 m_network->login(username, password)
    ↓
NetworkManager 发送登录请求到服务器
    ↓
收到响应 → processLoginResponse()
    ↓
成功：保存 userId → 关闭Dialog，显示主窗口
失败：显示错误提示
```

**关键实现：**
- 连接 `NetworkManager::loginSuccess` 和 `NetworkManager::loginFailed` 信号
- 从响应中提取并保存 `user_id`
- 防止重复提交（禁用登录按钮）

### 2. 注册流程 (RegisterDialog)

**文件：registerdialog.h/cpp**

**流程：**
```
用户输入用户名/密码/确认密码
    ↓
点击"注册"按钮 → onRegisterClicked()
    ↓
验证输入 → 调用 m_network->registerUser(username, password)
    ↓
NetworkManager 发送注册请求到服务器
    ↓
收到响应 → processRegisterResponse()
    ↓
成功：显示成功提示，返回登录界面
失败：根据错误类型显示对应提示
```

**关键实现：**
- 连接 `NetworkManager::registerSuccess` 和 `NetworkManager::registerFailed` 信号
- 根据错误信息判断失败原因（用户存在、密码格式等）
- 防止重复提交（禁用注册按钮）

### 3. 主窗口流程 (ClientMainWindow)

**文件：clientmainwindow.h/cpp**

#### 3.1 菜品列表加载

**流程：**
```
主窗口显示
    ↓
触发 showEvent() → 调用 m_network->getDishList()
    ↓
NetworkManager 发送菜品列表请求
    ↓
收到响应 → dishListReceived(QJsonArray)
    ↓
遍历 QJsonArray，构建 Dish 对象
    ↓
将菜品添加到 HomePage 显示
```

**关键实现：**
- 连接 `NetworkManager::dishListReceived` 信号
- 从 JSON 数组中提取 dish_id、name、price 等字段
- 将菜品数据传递给 HomePage 进行显示

#### 3.2 订单提交

**流程：**
```
用户选择菜品并添加到购物车
    ↓
点击"提交订单"按钮 → CartPage::orderRequested()
    ↓
构建 QJsonArray，包含每个菜品的 dish_id 和 count
    ↓
调用 m_network->submitOrder(dishArray)
    ↓
NetworkManager 发送订单提交请求
    ↓
收到响应 → orderSubmitSuccess() 或 orderSubmitFailed()
    ↓
成功：清空购物车，显示成功提示
失败：显示错误提示
```

**关键实现：**
- 连接 `CartPage::orderRequested` 信号
- 构建符合协议的 JSON 数组格式
- 连接 `NetworkManager::orderSubmitSuccess/Failed` 信号
- 订单成功后清空购物车

#### 3.3 订单历史查询

**流程：**
```
用户点击"订单记录"页面
    ↓
触发 OrderHistoryPage::ordersRequested()
    ↓
调用 m_network->getOrderList()
    ↓
NetworkManager 发送订单列表请求
    ↓
收到响应 → orderListReceived(QJsonArray)
    ↓
遍历数组，构建订单信息
    ↓
更新 OrderHistoryPage 显示
```

**关键实现：**
- 连接 `OrderHistoryPage::ordersRequested` 信号
- 连接 `NetworkManager::orderListReceived` 信号
- 从响应中提取订单列表

#### 3.4 订单评价提交

**流程：**
```
用户查看订单详情
    ↓
输入评价内容，点击"提交评价"
    ↓
触发 OrderHistoryPage::updateCommentRequested()
    ↓
调用 m_network->submitOrderComment(orderId, comment)
    ↓
NetworkManager 发送评价提交请求
    ↓
收到响应 → orderCommentSuccess() 或 orderCommentFailed()
    ↓
成功：显示成功提示
失败：显示错误提示
```

**关键实现：**
- 连接 `OrderHistoryPage::updateCommentRequested` 信号
- 传递 orderId 和 comment 到 NetworkManager
- 连接 `NetworkManager::orderCommentSuccess/Failed` 信号

## 网络模块API列表

### ConnectionManager::NetworkManager

| 方法 | 功能 | 响应信号 |
|------|------|--------|
| `connectToServer(ip, port)` | 连接到服务器 | `connected()` / `connectionError()` |
| `disconnectFromServer()` | 断开连接 | `disconnected()` |
| `isConnected()` | 查询连接状态 | - |
| `getUserId()` | 获取当前登录用户ID | - |
| `getUsername()` | 获取当前登录用户名 | - |
| `login(username, password)` | 用户登录 | `loginSuccess()` / `loginFailed(error)` |
| `registerUser(username, password)` | 用户注册 | `registerSuccess()` / `registerFailed(error)` |
| `getDishList()` | 获取菜品列表 | `dishListReceived(array)` / `dishListError(error)` |
| `submitOrder(dishes)` | 提交订单 | `orderSubmitSuccess()` / `orderSubmitFailed(error)` |
| `getOrderList()` | 获取订单历史 | `orderListReceived(array)` / `orderListError(error)` |
| `submitOrderComment(orderId, comment)` | 提交订单评价 | `orderCommentSuccess()` / `orderCommentFailed(error)` |
| `callWaiter()` | 呼叫服务员 | `waiterCalled()` / `waiterCallError(error)` |

## 错误处理

所有网络操作都有对应的错误处理：

1. **连接错误** - `connectionError(QString error)`
   - 未连接到服务器
   - 网络超时
   - Socket 错误

2. **业务错误** - 各业务特定的 Failed 信号
   - 登录失败（用户不存在、密码错误）
   - 注册失败（用户已存在、密码格式不合法）
   - 订单操作失败

## 状态管理

### 用户会话信息

NetworkManager 维护用户会话信息：
- `m_userId` - 当前登录用户ID（-1表示未登录）
- `m_username` - 当前登录用户名

登录成功后自动保存，可通过 `getUserId()` 和 `getUsername()` 查询。

### 请求追踪

NetworkManager 使用 `m_lastRequestType` 记录最后一次请求类型，用于匹配响应。

## 数据格式

### 菜品数据
```json
{
  "dish_id": 1,
  "name": "宫保鸡丁",
  "price": 28.00,
  "category": "川菜",
  "rating": 4.8,
  "url": "/img/gongbao.jpg",
  "description": "经典川菜，微辣香脆"
}
```

### 订单数据
```json
{
  "dish_id": 1,
  "count": 2
}
```

### 订单历史数据
```json
{
  "order_id": 10001,
  "total_amount": 88.00,
  "create_time": "2025-01-03 12:45:30",
  "comment": "味道很好，下次还来",
  "dishes": [...]
}
```

## 配置

**NetworkConfig.h**
```cpp
constexpr const char* SERVER_IP = "127.0.0.1";
constexpr int SERVER_PORT = 8080;
constexpr int TCP_TIMEOUT_MS = 5000;
```

根据实际服务器地址修改。
