# UI - Controller - NetworkManager 集成文档

## 项目概览

本项目采用分层架构：
```
UI层 (Qt 界面) 
  ↓
Controller层 (业务逻辑)
  ↓
NetworkManager (网络通信)
  ↓
服务端 (TCP JSON协议)
```

当前状态：
- NetworkManager 所有网络接口已完成
- 主窗口各页面待集成

---

## 实现任务清单

### HomePage 页面 - 菜品列表展示

**需求说明**
- 页面打开时自动获取菜品列表
- 展示所有菜品信息（名称、价格、评分、图片、描述）
- 支持添加菜品到购物车

**涉及文件**
- `client/homepage.h` / `client/homepage.cpp`
- `client/clientmainwindow.cpp` (信号连接)

**集成步骤**

1. **修改 HomePage 构造函数，接收 NetworkManager 指针**
   ```cpp
   class HomePage : public QWidget {
       // ... 
   private:
       NetworkManager* m_networkMgr = nullptr;
   };
   ```

2. **在 HomePage 初始化时请求菜品列表**
   ```cpp
   HomePage::HomePage(NetworkManager* networkMgr, QWidget* parent)
       : QWidget(parent), m_networkMgr(networkMgr) {
       // 页面初始化后请求菜品列表
       if (m_networkMgr) {
           connect(m_networkMgr, &NetworkManager::dishListReceived, 
                   this, &HomePage::onDishListReceived);
           m_networkMgr->getDishList();
       }
   }
   ```

3. **处理菜品列表响应**
   ```cpp
   void HomePage::onDishListReceived(const QJsonArray& dishes) {
       // 解析 QJsonArray，构造 Dish 对象列表
       // 更新 UI 显示所有菜品
       for (const QJsonValue& value : dishes) {
           QJsonObject dishObj = value.toObject();
           Dish dish;
           dish.id = dishObj["dish_id"].toInt();
           dish.name = dishObj["name"].toString();
           dish.price = dishObj["price"].toDouble();
           dish.rating = dishObj["rating"].toDouble();
           dish.imageUrl = dishObj["url"].toString();
           dish.description = dishObj["description"].toString();
           // ... 添加到显示列表
       }
   }
   ```

4. **在 ClientMainWindow 中传递 NetworkManager**
   ```cpp
   m_home = new HomePage(m_networkMgr, this);
   ```

**相关 NetworkManager 接口**
- `void getDishList()` - 获取菜品列表
- 信号：`dishListReceived(const QJsonArray& dishes)`
- 信号：`dishListError(const QString& error)`

---

### CartPage 页面 - 订单提交

**需求说明**
- 展示购物车中的菜品列表
- 计算订单总价
- 提交订单到服务端
- 显示提交结果（成功/失败）

**涉及文件**
- `client/cartpage.h` / `client/cartpage.cpp`
- `client/cartmanager.h` / `client/cartmanager.cpp`

**集成步骤**

1. **修改 CartPage 构造函数，接收 NetworkManager**
   ```cpp
   class CartPage : public QWidget {
   private:
       NetworkManager* m_networkMgr = nullptr;
   };
   ```

2. **处理订单提交信号**
   ```cpp
   CartPage::CartPage(CartManager* cart, NetworkManager* networkMgr, QWidget* parent)
       : QWidget(parent), m_cart(cart), m_networkMgr(networkMgr) {
       // 连接提交按钮信号
       connect(submitBtn, &QPushButton::clicked, this, &CartPage::onSubmitOrder);
       
       // 连接网络响应
       if (m_networkMgr) {
           connect(m_networkMgr, &NetworkManager::orderSubmitSuccess,
                   this, &CartPage::onOrderSuccess);
           connect(m_networkMgr, &NetworkManager::orderSubmitFailed,
                   this, &CartPage::onOrderFailed);
       }
   }
   ```

3. **构造订单请求并发送**
   ```cpp
   void CartPage::onSubmitOrder() {
       // 从 m_cart 中读取所有菜品
       QJsonArray dishArray;
       for (const auto& item : m_cart->getDishes()) {
           QJsonObject dishObj;
           dishObj["dish_id"] = item.dish.id;
           dishObj["count"] = item.quantity;
           dishArray.append(dishObj);
       }
       
       if (m_networkMgr && !dishArray.isEmpty()) {
           m_networkMgr->submitOrder(dishArray);
           // 禁用提交按钮，显示加载状态
           submitBtn->setEnabled(false);
       }
   }
   ```

4. **处理提交结果**
   ```cpp
   void CartPage::onOrderSuccess() {
       submitBtn->setEnabled(true);
       showMessage("订单提交成功");
       m_cart->clear(); // 清空购物车
   }
   
   void CartPage::onOrderFailed(const QString& error) {
       submitBtn->setEnabled(true);
       showMessage("订单提交失败: " + error);
   }
   ```

5. **在 ClientMainWindow 中更新**
   ```cpp
   auto* cartPage = new CartPage(m_cart, m_networkMgr, this);
   ```

**相关 NetworkManager 接口**
- `void submitOrder(const QJsonArray& dishes)` - 提交订单
- 信号：`orderSubmitSuccess()`
- 信号：`orderSubmitFailed(const QString& error)`

---

### OrderHistoryPage 页面 - 订单记录展示

**需求说明**
- 页面打开时获取当前用户的订单历史
- 展示订单列表（订单号、总金额、时间、状态）
- 支持展开订单详情查看菜品列表
- 支持提交订单评价

**涉及文件**
- `client/OrderHistoryPage.h` / `client/OrderHistoryPage.cpp`

**集成步骤**

1. **修改 OrderHistoryPage 构造函数**
   ```cpp
   class OrderHistoryPage : public QWidget {
   private:
       NetworkManager* m_networkMgr = nullptr;
   };
   ```

2. **页面初始化时请求订单列表**
   ```cpp
   OrderHistoryPage::OrderHistoryPage(NetworkManager* networkMgr, QWidget* parent)
       : QWidget(parent), m_networkMgr(networkMgr) {
       
       if (m_networkMgr) {
           connect(m_networkMgr, &NetworkManager::orderListReceived,
                   this, &OrderHistoryPage::onOrderListReceived);
           connect(m_networkMgr, &NetworkManager::orderListError,
                   this, &OrderHistoryPage::onOrderListError);
           connect(m_networkMgr, &NetworkManager::orderCommentSuccess,
                   this, &OrderHistoryPage::onCommentSuccess);
           connect(m_networkMgr, &NetworkManager::orderCommentFailed,
                   this, &OrderHistoryPage::onCommentFailed);
           
           m_networkMgr->getOrderList();
       }
   }
   ```

3. **处理订单列表响应**
   ```cpp
   void OrderHistoryPage::onOrderListReceived(const QJsonArray& orders) {
       // 清空现有订单
       clearOrderList();
       
       // 解析订单数据
       for (const QJsonValue& value : orders) {
           QJsonObject orderObj = value.toObject();
           OrderInfo order;
           order.orderId = orderObj["order_id"].toInt();
           order.totalAmount = orderObj["total_amount"].toDouble();
           order.createTime = orderObj["create_time"].toString();
           order.comment = orderObj["comment"].toString();
           
           // 解析菜品列表
           QJsonArray dishArray = orderObj["dishes"].toArray();
           for (const QJsonValue& dishVal : dishArray) {
               QJsonObject dishObj = dishVal.toObject();
               // 构造菜品信息...
           }
           
           // 添加到显示列表
           addOrderToUI(order);
       }
   }
   ```

4. **提交订单评价**
   ```cpp
   void OrderHistoryPage::onSubmitComment(int orderId, const QString& comment) {
       if (m_networkMgr) {
           m_networkMgr->submitOrderComment(orderId, comment);
       }
   }
   
   void OrderHistoryPage::onCommentSuccess() {
       showMessage("评价提交成功");
       // 刷新订单列表
       if (m_networkMgr) {
           m_networkMgr->getOrderList();
       }
   }
   ```

5. **在 ClientMainWindow 中更新**
   ```cpp
   auto* orders = new OrderHistoryPage(m_networkMgr, this);
   ```

**相关 NetworkManager 接口**
- `void getOrderList()` - 获取订单列表
- `void submitOrderComment(int orderId, const QString& comment)` - 提交评价
- 信号：`orderListReceived(const QJsonArray& orders)`
- 信号：`orderListError(const QString& error)`
- 信号：`orderCommentSuccess()`
- 信号：`orderCommentFailed(const QString& error)`

---

### 呼叫服务员功能

**需求说明**
- 在合适的位置（如主窗口或点餐页面）添加"呼叫服务员"按钮
- 点击按钮发送请求到服务端
- 显示请求状态

**涉及文件**
- `client/clientmainwindow.cpp` 或 `client/homepage.cpp`

**集成步骤**

1. **在 UI 中添加呼叫按钮**
   ```cpp
   auto* callWaiterBtn = new ElaPushButton("呼叫服务员", this);
   connect(callWaiterBtn, &QPushButton::clicked, this, &SomePage::onCallWaiter);
   ```

2. **实现呼叫服务员方法**
   ```cpp
   void SomePage::onCallWaiter() {
       if (m_networkMgr) {
           callWaiterBtn->setEnabled(false);
           callWaiterBtn->setText("呼叫中...");
           m_networkMgr->callWaiter();
       }
   }
   
   void SomePage::onWaiterCalled() {
       callWaiterBtn->setEnabled(true);
       callWaiterBtn->setText("服务员已收到，请耐心等待~");
       // 2秒后恢复按钮文本
       QTimer::singleShot(2000, [this]() {
           callWaiterBtn->setText("呼叫服务员");
       });
   }
   
   void SomePage::onWaiterCallError(const QString& error) {
       callWaiterBtn->setEnabled(true);
       callWaiterBtn->setText("呼叫服务员");
       showMessage("呼叫失败: " + error);
   }
   ```

3. **连接信号**
   ```cpp
   if (m_networkMgr) {
       connect(m_networkMgr, &NetworkManager::waiterCalled,
               this, &SomePage::onWaiterCalled);
       connect(m_networkMgr, &NetworkManager::waiterCallError,
               this, &SomePage::onWaiterCallError);
   }
   ```

**相关 NetworkManager 接口**
- `void callWaiter()` - 呼叫服务员
- 信号：`waiterCalled()`
- 信号：`waiterCallError(const QString& error)`

---

## NetworkManager 接口汇总

| 接口 | 参数 | 响应信号 | 用途 |
|------|------|---------|------|
| `login()` | username, password | loginSuccess / loginFailed | 用户登录 |
| `registerUser()` | username, password | registerSuccess / registerFailed | 用户注册 |
| `getDishList()` | 无 | dishListReceived / dishListError | 获取菜品列表 |
| `submitOrder()` | QJsonArray dishes | orderSubmitSuccess / orderSubmitFailed | 提交订单 |
| `getOrderList()` | 无 | orderListReceived / orderListError | 获取订单历史 |
| `submitOrderComment()` | orderId, comment | orderCommentSuccess / orderCommentFailed | 提交评价 |
| `callWaiter()` | 无 | waiterCalled / waiterCallError | 呼叫服务员 |

---

## 集成检查清单

- [ ] `HomePage` 已集成 `getDishList()`
- [ ] `CartPage` 已集成 `submitOrder()`
- [ ] `OrderHistoryPage` 已集成 `getOrderList()` 和 `submitOrderComment()`
- [ ] 呼叫服务员功能已添加
- [ ] 所有页面都正确传递 `NetworkManager` 指针
- [ ] 所有网络请求都有超时/错误处理
- [ ] UI 显示加载状态和错误消息
- [ ] 登录用户信息已保存（用于获取个人订单等）

---

## 数据结构参考

### Dish 结构（菜品）
```cpp
struct Dish {
    int id;                    // dish_id
    QString name;              // 菜品名称
    double price;              // 价格
    QString category;          // 菜系
    double rating;             // 评分
    QString imageUrl;          // 图片URL (url字段)
    QString description;       // 描述
};
```

### OrderInfo 结构（订单）
```cpp
struct OrderInfo {
    int orderId;               // order_id
    double totalAmount;        // 总金额
    QString createTime;        // 创建时间
    QString comment;           // 评价内容
    QList<Dish> dishes;        // 订单中的菜品列表
};
```

### CartItem 结构（购物车项）
```cpp
struct CartItem {
    Dish dish;                 // 菜品信息
    int quantity;              // 数量
};
```

---

## 常见问题

**如何获取当前登录用户ID？**
从 `NetworkManager` 获取：
```cpp
int userId = m_networkMgr->getUserId();
```

**订单提交后如何自动刷新订单列表？**
在订单提交成功的槽函数中调用：
```cpp
void onOrderSuccess() {
    m_networkMgr->getOrderList();
}
```

**如何处理网络连接失败？**
检查 `isConnected()` 并显示错误提示：
```cpp
if (!m_networkMgr->isConnected()) {
    showError("网络连接已断开");
    // 尝试重新连接
    m_networkMgr->connectToServer(SERVER_IP, SERVER_PORT);
}
```

**菜品图片如何加载？**
从 JSON 的 `url` 字段获取图片路径，支持本地路径：
```cpp
QPixmap pixmap(dishObj["url"].toString());
// 或从资源文件加载
```

---

## 下一步工作

完成上述四个 Task 后，项目将实现完整的客户端-服务端交互流程：
1. 用户登录/注册
2. 浏览菜品
3. 添加到购物车
4. 提交订单
5. 查看订单历史
6. 提交评价
7. 呼叫服务员

所有网络通信将通过 NetworkManager 统一管理，UI 通过信号槽模式响应服务端数据。
