## 1. 数据库设计 (MySQL)

根据要求书中的“菜品信息数据库管理” ，设计以下表结构。

### 1.1 用户表 (`t_user`)

| 字段名        | 类型        | 约束                        | 说明              |
| ------------- | ----------- | --------------------------- | ----------------- |
| user_id       | INT         | PRIMARY KEY, AUTO_INCREMENT | 用户唯一ID (主键) |
| username      | VARCHAR(50) | NOT NULL, UNIQUE            | 用户账号          |
| password      | VARCHAR(64) | NOT NULL                    | 登录密码          |
| register_time | DATETIME    | -                           | 注册时间          |

### 1.2 菜品表 (`t_dish`)

用于存储菜单信息。

| 字段名        | 类型          | 约束                        | 说明               |
| ------------- | ------------- | --------------------------- | ------------------ |
| `dish_id`     | INT           | PRIMARY KEY, AUTO_INCREMENT | 菜品ID             |
| `name`        | VARCHAR(50)   | NOT NULL                    | 菜品名称           |
| `price`       | DECIMAL(10,2) | NOT NULL                    | 价格               |
| `category`    | VARCHAR(20)   | -                           | 菜系 (川菜/粤菜等) |
| `rating`      | FLOAT         | DEFAULT 5.0                 | 好评度             |
| `url`         | VARCHAR(50)   | NOT NULL                    | 图片路径           |
| `description` | VARCHAR(200)  | -                           | 详细描述           |



### 1.3 订单主表 (`t_order`)

用于存储订单概要。

| 字段名         | 类型          | 约束                        | 说明         |
| -------------- | ------------- | --------------------------- | ------------ |
| `order_id`     | INT           | PRIMARY KEY, AUTO_INCREMENT | 订单号       |
| `user_id`      | INT           | NOT NULL                    | 下单用户 ID  |
| `total_amount` | DECIMAL(10,2) | -                           | 总金额       |
| `create_time`  | DATETIME      | -                           | 下单时间     |
| `comment`      | VARCHAR(200)  | -                           | 订单评价内容 |

### 1.4 订单-菜品关联表 (`t_order_dish`)

存储订单中包含哪些菜品

| 字段名     | 类型 | 约束                        | 说明   |
| ---------- | ---- | --------------------------- | ------ |
| `id`       | INT  | PRIMARY KEY, AUTO_INCREMENT | ID     |
| `order_id` | INT  | NOT NULL                    | 订单ID |
| `dish_id`  | INT  | NOT NULL                    | 菜品ID |

---

# 2 登录界面

## 2.1 用户登录

### 请求格式

```json
{
  "type": "login",
  "data": {
    "username": "xyz",
    "password": "123456"
  }
}
```

### 响应格式

**登陆成功**

```json
{
  "code": 200,
  "data": {
    "user_id": 1 
  }
  "msg": "",
}
```

**密码错误**

```json
{
  "code": 401,
  "data": {
  }
  "msg": "密码错误！",
}
```

**用户不存在**

```json
{
  "code": 402,
  "data": {
  }
  "msg": "该用户不存在！",
}
```

# 3 注册界面

## 3.1 用户注册

### 请求格式

```json
{
  "type": "register",
  "data": {
    "username": "xyz",
    "password": "123456"
  }
}
```

### 响应格式

**注册成功**

```json
{
    "code": 200
    "data": {
    }
    "msg":"注册成功"
}
```

**密码格式不合法**

```json
{
    "code":402
    "data":{
    }
    "msg":"密码只能包含字母和数字！"
}
```

# 4 点餐界面

## 4.1 获取菜品详细信息

### 请求格式

```json
{
  "type": "dish_list",
  "data": {
  }
}
```

### 响应格式

**获取菜品详细信息成功**

```json
{
    "code":200,
    "data": [
        {
        "dish_id": 1,
        "name": "宫保鸡丁",
        "price": 28.00,
        "category": "川菜",
        "rating": 4.8,
        "url": "/img/gongbao.jpg",
        "description": "经典川菜，微辣香脆"
        },
        {
        "dish_id": 2,
        "name": "白切鸡",
        "price": 32.00,
        "category": "粤菜",
        "rating": 4.6,
        "url": "/img/baiji.jpg",
        "description": "肉质鲜嫩，清淡爽口"
        }
    ],
    "msg":""
}
```

## 4.2 呼叫服务员

### 请求格式

```json
{
  "type": "call_waiter",
  "data": {
  }
}
```

### 响应格式

```json
{
  "code": 200,
  "msg": "服务员已收到您的呼叫，请耐心等待~",
  "data": {
  }
}
```


# 5 订单提交界面

## 5.1 订单提交

### 请求格式

```json
{
  "type": "order_submit",
  "data": {
    "dishes": [
      {
        "dish_id": 1,
        "count": 2
      },
      {
        "dish_id": 3,
        "count": 1
      }
    ]
  }
}
```

### 响应格式

**订单提交成功**

```json
{
  "code": 200,
  "msg": "订单成功提交",
  "data": {
  }
}
```

# 6 用户历史订单显示页

## 6.1 显示历史订单

### 请求格式

```json
{
  "type": "order_list",
  "data": {
  }
}
```

### 响应格式

**获取成功**

```json
{
  "code": 200,
  "msg": "get order history success",
  "data": [
    {
      "order_id": 10001,
      "total_amount": 88.00,
      "create_time": "2025-01-03 12:45:30",
      "comment": "味道很好，下次还来",
      "dishes": [
        {
          "dish_id": 1,
          "name": "宫保鸡丁",
          "price": 28.00,
          "count": 2
        },
        {
          "dish_id": 3,
          "name": "鱼香肉丝",
          "price": 32.00,
          "count": 1
        }
      ]
    },
    {
      "order_id": 10002,
      "total_amount": 56.00,
      "create_time": "2025-01-05 18:20:10",
      "comment": "",
      "dishes": [
        {
          "dish_id": 2,
          "name": "白切鸡",
          "price": 32.00,
          "count": 1
        }
      ]
    }
  ]
}
```

## 6.2 提交评价

### 请求格式

```json
{
  "type": "order_comment",
  "data": {
    "order_id": 10001,
    "comment": "菜品新鲜，配送及时，整体体验很好"
  }
}
```

### 响应格式

**成功提交评价**

```json
  "code": 200,
  "msg": "评价提交成功",
  "data": {}
```

# 7 菜品信息管理界面

## 7.1 查询菜品

### 请求格式

直接调函数

### 响应格式

```json
{
    "code":200,
    "data": [
        {
        "dish_id": 1,
        "name": "宫保鸡丁",
        "price": 28.00,
        "category": "川菜",
        "rating": 4.8,
        "url": "/img/gongbao.jpg",
        "description": "经典川菜，微辣香脆"
        },
        {
        "dish_id": 2,
        "name": "白切鸡",
        "price": 32.00,
        "category": "粤菜",
        "rating": 4.6,
        "url": "/img/baiji.jpg",
        "description": "肉质鲜嫩，清淡爽口"
        }
    ],
    "msg":""
}
```

## 7.2 新增菜品

### 请求格式

直接调函数

### 响应格式

是否增加成功

## 7.3 修改菜品

### 请求格式

直接调函数

### 响应格式

是否修改成功

## 7.4 删除菜品

### 请求格式

直接调函数

### 响应格式

是否删除成功

# 8 服务端订单记录页面

## 8.1 查询所有订单简要信息

### 请求格式

直接调函数

### 响应格式

**获取成功**

```json
{
  "code": 200,
  "msg": "",
  "data": [
    {
      "order_id": 10001,
      "username": "user1",
      "total_amount": 88.00,
      "create_time": "2025-01-03 12:45:30",
      "comment": "味道很好，下次还来"
    },
    {
      "order_id": 10002,
      "username": "user2",
      "total_amount": 56.00,
      "create_time": "2025-01-05 18:20:10",
      "comment": ""
    }
  ]
}
```
