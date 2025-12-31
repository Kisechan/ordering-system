/*
 Navicat Premium Dump SQL

 Source Server         : mysql
 Source Server Type    : MySQL
 Source Server Version : 80040 (8.0.40)
 Target Server Type    : MySQL
 Target Server Version : 80040 (8.0.40)
 File Encoding         : 65001

 Database              : restaurant
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for t_user
-- ----------------------------
DROP TABLE IF EXISTS `t_user`;
CREATE TABLE `t_user` (
  `user_id` INT NOT NULL AUTO_INCREMENT,
  `username` VARCHAR(50) NOT NULL,
  `password` VARCHAR(64) NOT NULL,
  `register_time` DATETIME NOT NULL,
  PRIMARY KEY (`user_id`),
  UNIQUE KEY `uk_user_username` (`username`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Records of t_user
-- ----------------------------
INSERT INTO `t_user` VALUES
(1, 'admin', 'admin123', '2025-01-01 09:00:00'),
(2, 'user1', 'user1123', '2025-01-02 10:05:00'),
(3, 'user2', 'user2123', '2025-01-03 11:10:00'),
(4, 'user3', 'user3123', '2025-01-04 12:20:00'),
(5, 'user4', 'user4123', '2025-01-05 13:35:00'),
(6, 'user5', 'user5123', '2025-01-06 14:50:00');

-- ----------------------------
-- Table structure for t_dish
-- ----------------------------
DROP TABLE IF EXISTS `t_dish`;
CREATE TABLE `t_dish` (
  `dish_id` INT NOT NULL AUTO_INCREMENT,
  `name` VARCHAR(50) NOT NULL,
  `price` DECIMAL(10,2) NOT NULL,
  `category` VARCHAR(20) NOT NULL,
  `url` VARCHAR(50) NOT NULL,
  `description` VARCHAR(200) NOT NULL DEFAULT '',
  PRIMARY KEY (`dish_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Records of t_dish
-- ----------------------------
INSERT INTO `t_dish` VALUES
(1,'宫保鸡丁',28.00,'川菜',':/include/Image/gongbaojiding.jpg','经典川菜，微辣香脆'),
(2,'白切鸡',32.00,'粤菜',':/include/Image/baiqieji.jpg','肉质鲜嫩，清淡爽口'),
(3,'鱼香肉丝',32.00,'川菜',':/include/Image/yuxiangrousi.jpg','酸甜微辣，下饭神器'),
(4,'麻婆豆腐',18.00,'川菜',':/include/Image/mapodoufu.jpg','麻辣鲜香，豆腐嫩滑'),
(5,'回锅肉',36.00,'川菜',':/include/Image/huiguorou.jpg','肥而不腻，蒜苗香'),
(6,'糖醋里脊',30.00,'鲁菜',':/include/Image/tangculiji.jpg','酸甜酥脆，老少皆宜'),
(7,'红烧肉',38.00,'本帮',':/include/Image/hongshaorou.jpg','肥瘦相间，入口即化'),
(8,'清蒸鲈鱼',48.00,'粤菜',':/include/Image/qingzhengluyu.jpg','鲜嫩清爽，葱姜提鲜');

-- ----------------------------
-- Table structure for t_order
-- ----------------------------
DROP TABLE IF EXISTS `t_order`;
CREATE TABLE `t_order` (
  `order_id` INT NOT NULL AUTO_INCREMENT,
  `user_id` INT NOT NULL,
  `total_amount` DECIMAL(10,2) NOT NULL,
  `create_time` DATETIME NOT NULL,
  `comment` VARCHAR(200) NOT NULL DEFAULT '',
  PRIMARY KEY (`order_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Records of t_order
-- ----------------------------
INSERT INTO `t_order` VALUES
(1,2,88.00,'2025-01-03 12:45:30','味道很好，下次还来'),
(2,3,56.00,'2025-01-05 18:20:10',''),
(3,2,64.00,'2025-01-06 19:05:00','速度快');

-- ----------------------------
-- Table structure for t_order_dish
-- ----------------------------
DROP TABLE IF EXISTS `t_order_dish`;
CREATE TABLE `t_order_dish` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `order_id` INT NOT NULL,
  `dish_id` INT NOT NULL,
  `count` INT NOT NULL,
  `rating` INT DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ----------------------------
-- Records of t_order_dish
-- ----------------------------
INSERT INTO `t_order_dish` VALUES
(1,1,1,2,5),
(2,1,3,1,4),
(3,2,2,1,4),
(4,2,29,1,NULL),
(5,3,22,1,5),
(6,3,27,2,4);

SET FOREIGN_KEY_CHECKS = 1;
