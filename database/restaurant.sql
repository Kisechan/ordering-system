/*
 Navicat Premium Dump SQL

 Source Server         : mysql
 Source Server Type    : MySQL
 Source Server Version : 80040 (8.0.40)
 Source Host           : localhost:3306
 Source Schema         : restaurant

 Target Server Type    : MySQL
 Target Server Version : 80040 (8.0.40)
 File Encoding         : 65001

 Date: 28/12/2025 14:52:39
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for t_dish
-- ----------------------------
DROP TABLE IF EXISTS `t_dish`;
CREATE TABLE `t_dish`  (
  `dish_id` int NOT NULL AUTO_INCREMENT,
  `name` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `price` decimal(10, 2) NOT NULL,
  `category` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL,
  `rating` float NULL DEFAULT 5,
  `url` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `description` varchar(200) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL,
  PRIMARY KEY (`dish_id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 33 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of t_dish
-- ----------------------------
INSERT INTO `t_dish` VALUES (1, '宫保鸡丁', 28.00, '川菜', 4.8, '/img/coke.png', '经典川菜，微辣香脆');
INSERT INTO `t_dish` VALUES (2, '白切鸡', 32.00, '粤菜', 4.6, '/img/coke.png', '肉质鲜嫩，清淡爽口');
INSERT INTO `t_dish` VALUES (3, '鱼香肉丝', 32.00, '川菜', 4.7, '/img/coke.png', '酸甜微辣，下饭神器');
INSERT INTO `t_dish` VALUES (4, '麻婆豆腐', 18.00, '川菜', 4.7, '/img/coke.png', '麻辣鲜香，豆腐嫩滑');
INSERT INTO `t_dish` VALUES (5, '回锅肉', 36.00, '川菜', 4.5, '/img/coke.png', '肥而不腻，蒜苗香');
INSERT INTO `t_dish` VALUES (6, '糖醋里脊', 30.00, '鲁菜', 4.6, '/img/coke.png', '酸甜酥脆，老少皆宜');
INSERT INTO `t_dish` VALUES (7, '红烧肉', 38.00, '本帮', 4.7, '/img/coke.png', '肥瘦相间，入口即化');
INSERT INTO `t_dish` VALUES (8, '清蒸鲈鱼', 48.00, '粤菜', 4.8, '/img/coke.png', '鲜嫩清爽，葱姜提鲜');
INSERT INTO `t_dish` VALUES (9, '蒜蓉粉丝虾', 52.00, '粤菜', 4.7, '/img/coke.png', '蒜香浓郁，粉丝入味');
INSERT INTO `t_dish` VALUES (10, '酸菜鱼', 46.00, '川菜', 4.6, '/img/coke.png', '酸辣开胃，鱼片滑嫩');
INSERT INTO `t_dish` VALUES (11, '小炒黄牛肉', 42.00, '湘菜', 4.5, '/img/coke.png', '锅气十足，微辣爽');
INSERT INTO `t_dish` VALUES (12, '剁椒鱼头', 58.00, '湘菜', 4.6, '/img/coke.png', '剁椒香辣，鱼头鲜');
INSERT INTO `t_dish` VALUES (13, '口水鸡', 34.00, '川菜', 4.6, '/img/coke.png', '红油麻辣，鸡肉嫩');
INSERT INTO `t_dish` VALUES (14, '干锅花菜', 26.00, '湘菜', 4.4, '/img/coke.png', '干锅香辣，脆嫩');
INSERT INTO `t_dish` VALUES (15, '地三鲜', 24.00, '东北', 4.5, '/img/coke.png', '茄子土豆青椒，家常');
INSERT INTO `t_dish` VALUES (16, '锅包肉', 32.00, '东北', 4.6, '/img/coke.png', '外酥里嫩，酸甜');
INSERT INTO `t_dish` VALUES (17, '西红柿炒蛋', 16.00, '家常', 4.7, '/img/coke.png', '经典家常，酸甜适中');
INSERT INTO `t_dish` VALUES (18, '青椒肉丝', 22.00, '家常', 4.5, '/img/coke.png', '清爽下饭');
INSERT INTO `t_dish` VALUES (19, '可乐鸡翅', 28.00, '家常', 4.6, '/img/coke.png', '甜咸适口，色泽诱人');
INSERT INTO `t_dish` VALUES (20, '番茄牛腩', 45.00, '家常', 4.7, '/img/coke.png', '汤汁浓郁，牛腩软烂');
INSERT INTO `t_dish` VALUES (21, '扬州炒饭', 18.00, '苏菜', 4.4, '/img/coke.png', '粒粒分明，配料丰富');
INSERT INTO `t_dish` VALUES (22, '担担面', 15.00, '川菜', 4.5, '/img/coke.png', '麻辣咸香，面条劲道');
INSERT INTO `t_dish` VALUES (23, '牛肉拉面', 20.00, '西北', 4.6, '/img/coke.png', '汤清味浓，牛肉实在');
INSERT INTO `t_dish` VALUES (24, '炸酱面', 16.00, '京味', 4.4, '/img/coke.png', '酱香浓郁');
INSERT INTO `t_dish` VALUES (25, '酸辣汤', 12.00, '家常', 4.3, '/img/coke.png', '开胃暖身');
INSERT INTO `t_dish` VALUES (26, '紫菜蛋花汤', 10.00, '家常', 4.2, '/img/coke.png', '清淡鲜美');
INSERT INTO `t_dish` VALUES (27, '可乐', 5.00, '饮品', 4.5, '/img/coke.png', '冰爽解腻');
INSERT INTO `t_dish` VALUES (28, '雪碧', 5.00, '饮品', 4.5, '/img/coke.png', '清爽气泡');
INSERT INTO `t_dish` VALUES (29, '米饭', 2.00, '主食', 4.6, '/img/coke.png', '香软可口');
INSERT INTO `t_dish` VALUES (30, '馒头', 2.00, '主食', 4.2, '/img/coke.png', '松软好吃');
INSERT INTO `t_dish` VALUES (31, '拍黄瓜', 10.00, '凉菜', 4.4, '/img/coke.png', '清爽解腻');
INSERT INTO `t_dish` VALUES (32, '凉拌木耳', 12.00, '凉菜', 4.5, '/img/coke.png', '爽脆开胃');

-- ----------------------------
-- Table structure for t_order
-- ----------------------------
DROP TABLE IF EXISTS `t_order`;
CREATE TABLE `t_order`  (
  `order_id` int NOT NULL AUTO_INCREMENT,
  `user_id` int NOT NULL,
  `total_amount` decimal(10, 2) NULL DEFAULT NULL,
  `create_time` datetime NULL DEFAULT NULL,
  `comment` varchar(200) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL,
  PRIMARY KEY (`order_id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 11 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of t_order
-- ----------------------------
INSERT INTO `t_order` VALUES (1, 2, 88.00, '2025-01-03 12:45:30', '味道很好，下次还来');
INSERT INTO `t_order` VALUES (2, 3, 56.00, '2025-01-05 18:20:10', '');
INSERT INTO `t_order` VALUES (3, 2, 64.00, '2025-01-06 19:05:00', '速度快');
INSERT INTO `t_order` VALUES (4, 4, 38.00, '2025-01-07 11:30:15', '');
INSERT INTO `t_order` VALUES (5, 5, 102.00, '2025-01-08 20:10:05', '鱼很鲜');
INSERT INTO `t_order` VALUES (6, 6, 34.00, '2025-01-09 13:15:40', '');
INSERT INTO `t_order` VALUES (7, 2, 53.00, '2025-01-10 12:00:00', '速度快');
INSERT INTO `t_order` VALUES (8, 3, 70.00, '2025-01-11 18:40:20', '');
INSERT INTO `t_order` VALUES (9, 4, 28.00, '2025-01-12 09:50:10', '');
INSERT INTO `t_order` VALUES (10, 5, 94.00, '2025-01-13 21:05:55', '下次再点');

-- ----------------------------
-- Table structure for t_order_dish
-- ----------------------------
DROP TABLE IF EXISTS `t_order_dish`;
CREATE TABLE `t_order_dish`  (
  `id` int NOT NULL AUTO_INCREMENT,
  `order_id` int NOT NULL,
  `dish_id` int NOT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 27 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of t_order_dish
-- ----------------------------
INSERT INTO `t_order_dish` VALUES (1, 1, 1);
INSERT INTO `t_order_dish` VALUES (2, 1, 1);
INSERT INTO `t_order_dish` VALUES (3, 1, 3);
INSERT INTO `t_order_dish` VALUES (4, 2, 2);
INSERT INTO `t_order_dish` VALUES (5, 2, 18);
INSERT INTO `t_order_dish` VALUES (6, 2, 29);
INSERT INTO `t_order_dish` VALUES (7, 3, 22);
INSERT INTO `t_order_dish` VALUES (8, 3, 23);
INSERT INTO `t_order_dish` VALUES (9, 3, 25);
INSERT INTO `t_order_dish` VALUES (10, 3, 31);
INSERT INTO `t_order_dish` VALUES (11, 3, 27);
INSERT INTO `t_order_dish` VALUES (12, 3, 29);
INSERT INTO `t_order_dish` VALUES (13, 4, 7);
INSERT INTO `t_order_dish` VALUES (14, 5, 8);
INSERT INTO `t_order_dish` VALUES (15, 5, 9);
INSERT INTO `t_order_dish` VALUES (16, 5, 29);
INSERT INTO `t_order_dish` VALUES (17, 6, 13);
INSERT INTO `t_order_dish` VALUES (18, 7, 19);
INSERT INTO `t_order_dish` VALUES (19, 7, 21);
INSERT INTO `t_order_dish` VALUES (20, 7, 27);
INSERT INTO `t_order_dish` VALUES (21, 7, 29);
INSERT INTO `t_order_dish` VALUES (22, 8, 10);
INSERT INTO `t_order_dish` VALUES (23, 8, 15);
INSERT INTO `t_order_dish` VALUES (24, 9, 1);
INSERT INTO `t_order_dish` VALUES (25, 10, 10);
INSERT INTO `t_order_dish` VALUES (26, 10, 8);

-- ----------------------------
-- Table structure for t_user
-- ----------------------------
DROP TABLE IF EXISTS `t_user`;
CREATE TABLE `t_user`  (
  `user_id` int NOT NULL AUTO_INCREMENT,
  `username` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `password` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `register_time` datetime NULL DEFAULT NULL,
  PRIMARY KEY (`user_id`) USING BTREE,
  UNIQUE INDEX `uk_user_username`(`username` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 7 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of t_user
-- ----------------------------
INSERT INTO `t_user` VALUES (1, 'admin', 'admin123', '2025-01-01 09:00:00');
INSERT INTO `t_user` VALUES (2, 'user1', 'user1123', '2025-01-02 10:05:00');
INSERT INTO `t_user` VALUES (3, 'user2', 'user2123', '2025-01-03 11:10:00');
INSERT INTO `t_user` VALUES (4, 'user3', 'user3123', '2025-01-04 12:20:00');
INSERT INTO `t_user` VALUES (5, 'user4', 'user4123', '2025-01-05 13:35:00');
INSERT INTO `t_user` VALUES (6, 'user5', 'user5123', '2025-01-06 14:50:00');

SET FOREIGN_KEY_CHECKS = 1;
