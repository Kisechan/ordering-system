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

 Date: 28/12/2025 10:37:53
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for admin_user
-- ----------------------------
DROP TABLE IF EXISTS `admin_user`;
CREATE TABLE `admin_user`  (
  `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT,
  `username` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `password_hash` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `role` enum('ADMIN','STAFF','KITCHEN') CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT 'ADMIN',
  `is_active` tinyint(1) NOT NULL DEFAULT 1,
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE INDEX `username`(`username` ASC) USING BTREE,
  INDEX `idx_admin_user_active`(`is_active` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 2 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of admin_user
-- ----------------------------
INSERT INTO `admin_user` VALUES (1, 'admin', '$2y$10$REPLACE_WITH_BCRYPT_HASH', 'ADMIN', 1, '2025-12-28 10:10:30', '2025-12-28 10:10:30');

-- ----------------------------
-- Table structure for dish
-- ----------------------------
DROP TABLE IF EXISTS `dish`;
CREATE TABLE `dish`  (
  `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT,
  `category_id` bigint UNSIGNED NULL DEFAULT NULL,
  `name` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `description` varchar(512) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL,
  `price_cents` int NOT NULL,
  `image_url` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL,
  `is_on_sale` tinyint(1) NOT NULL DEFAULT 1,
  `stock` int NULL DEFAULT NULL,
  `rating_avg` decimal(3, 2) NOT NULL DEFAULT 0.00,
  `rating_count` int NOT NULL DEFAULT 0,
  `good_rate` decimal(5, 2) NOT NULL DEFAULT 0.00,
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `idx_dish_category_sale`(`category_id` ASC, `is_on_sale` ASC) USING BTREE,
  INDEX `idx_dish_name`(`name` ASC) USING BTREE,
  CONSTRAINT `fk_dish_category` FOREIGN KEY (`category_id`) REFERENCES `dish_category` (`id`) ON DELETE SET NULL ON UPDATE CASCADE,
  CONSTRAINT `dish_chk_1` CHECK (`price_cents` >= 0)
) ENGINE = InnoDB AUTO_INCREMENT = 6 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of dish
-- ----------------------------
INSERT INTO `dish` VALUES (1, 1, '宫保鸡丁', '经典川味，微辣', 3200, 'images/gongbao.jpg', 1, NULL, 0.00, 0, 0.00, '2025-12-28 10:10:30', '2025-12-28 10:10:30');
INSERT INTO `dish` VALUES (2, 1, '鱼香肉丝', '酸甜开胃', 2800, 'images/yuxiang.jpg', 1, NULL, 0.00, 0, 0.00, '2025-12-28 10:10:30', '2025-12-28 10:10:30');
INSERT INTO `dish` VALUES (3, 2, '拍黄瓜', '清爽解腻', 1200, 'images/huanggua.jpg', 1, NULL, 0.00, 0, 0.00, '2025-12-28 10:10:30', '2025-12-28 10:10:30');
INSERT INTO `dish` VALUES (4, 3, '米饭', '单份', 200, 'images/rice.jpg', 1, NULL, 0.00, 0, 0.00, '2025-12-28 10:10:30', '2025-12-28 10:10:30');
INSERT INTO `dish` VALUES (5, 4, '可乐', '冰', 600, 'images/cola.jpg', 1, NULL, 0.00, 0, 0.00, '2025-12-28 10:10:30', '2025-12-28 10:10:30');

-- ----------------------------
-- Table structure for dish_category
-- ----------------------------
DROP TABLE IF EXISTS `dish_category`;
CREATE TABLE `dish_category`  (
  `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT,
  `name` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `sort_order` int NOT NULL DEFAULT 0,
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE INDEX `name`(`name` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 5 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of dish_category
-- ----------------------------
INSERT INTO `dish_category` VALUES (1, '热菜', 1, '2025-12-28 10:10:30', '2025-12-28 10:10:30');
INSERT INTO `dish_category` VALUES (2, '凉菜', 2, '2025-12-28 10:10:30', '2025-12-28 10:10:30');
INSERT INTO `dish_category` VALUES (3, '主食', 3, '2025-12-28 10:10:30', '2025-12-28 10:10:30');
INSERT INTO `dish_category` VALUES (4, '饮品', 4, '2025-12-28 10:10:30', '2025-12-28 10:10:30');

-- ----------------------------
-- Table structure for order_item
-- ----------------------------
DROP TABLE IF EXISTS `order_item`;
CREATE TABLE `order_item`  (
  `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT,
  `order_id` bigint UNSIGNED NOT NULL,
  `dish_id` bigint UNSIGNED NULL DEFAULT NULL,
  `dish_name_snapshot` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `unit_price_cents_snapshot` int NOT NULL,
  `qty` int NOT NULL,
  `item_remark` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL,
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `idx_order_item_order`(`order_id` ASC) USING BTREE,
  INDEX `idx_order_item_dish`(`dish_id` ASC) USING BTREE,
  CONSTRAINT `fk_item_dish` FOREIGN KEY (`dish_id`) REFERENCES `dish` (`id`) ON DELETE SET NULL ON UPDATE CASCADE,
  CONSTRAINT `fk_item_order` FOREIGN KEY (`order_id`) REFERENCES `orders` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `order_item_chk_1` CHECK (`unit_price_cents_snapshot` >= 0),
  CONSTRAINT `order_item_chk_2` CHECK (`qty` > 0)
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of order_item
-- ----------------------------

-- ----------------------------
-- Table structure for orders
-- ----------------------------
DROP TABLE IF EXISTS `orders`;
CREATE TABLE `orders`  (
  `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT,
  `order_no` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `table_no` varchar(16) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `status` enum('CREATED','PAID','ACCEPTED','COOKING','SERVED','DONE','CANCELLED') CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT 'CREATED',
  `total_price_cents` int NOT NULL DEFAULT 0,
  `remark` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL,
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `paid_at` timestamp NULL DEFAULT NULL,
  `updated_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE INDEX `order_no`(`order_no` ASC) USING BTREE,
  INDEX `idx_orders_table_status_time`(`table_no` ASC, `status` ASC, `created_at` ASC) USING BTREE,
  INDEX `idx_orders_created_at`(`created_at` ASC) USING BTREE,
  CONSTRAINT `orders_chk_1` CHECK (`total_price_cents` >= 0)
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of orders
-- ----------------------------

-- ----------------------------
-- Table structure for review
-- ----------------------------
DROP TABLE IF EXISTS `review`;
CREATE TABLE `review`  (
  `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT,
  `order_id` bigint UNSIGNED NOT NULL,
  `rating` tinyint UNSIGNED NOT NULL,
  `content` varchar(512) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL,
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE INDEX `uk_review_order`(`order_id` ASC) USING BTREE,
  INDEX `idx_review_time`(`created_at` ASC) USING BTREE,
  CONSTRAINT `fk_review_order` FOREIGN KEY (`order_id`) REFERENCES `orders` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `review_chk_1` CHECK (`rating` between 1 and 5)
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of review
-- ----------------------------

-- ----------------------------
-- Table structure for review_item
-- ----------------------------
DROP TABLE IF EXISTS `review_item`;
CREATE TABLE `review_item`  (
  `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT,
  `review_id` bigint UNSIGNED NOT NULL,
  `dish_id` bigint UNSIGNED NULL DEFAULT NULL,
  `rating` tinyint UNSIGNED NOT NULL,
  `content` varchar(512) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `fk_review_item_review`(`review_id` ASC) USING BTREE,
  INDEX `idx_review_item_dish`(`dish_id` ASC) USING BTREE,
  CONSTRAINT `fk_review_item_dish` FOREIGN KEY (`dish_id`) REFERENCES `dish` (`id`) ON DELETE SET NULL ON UPDATE CASCADE,
  CONSTRAINT `fk_review_item_review` FOREIGN KEY (`review_id`) REFERENCES `review` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `review_item_chk_1` CHECK (`rating` between 1 and 5)
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of review_item
-- ----------------------------

-- ----------------------------
-- Table structure for service_call
-- ----------------------------
DROP TABLE IF EXISTS `service_call`;
CREATE TABLE `service_call`  (
  `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT,
  `table_no` varchar(16) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `call_type` enum('WATER','CUTLERY','PAY','HELP','OTHER') CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT 'OTHER',
  `content` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL,
  `status` enum('OPEN','ACKED','DONE','CANCELLED') CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT 'OPEN',
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `handled_at` timestamp NULL DEFAULT NULL,
  `handler_note` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `idx_call_status_time`(`status` ASC, `created_at` ASC) USING BTREE,
  INDEX `idx_call_table_time`(`table_no` ASC, `created_at` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of service_call
-- ----------------------------

-- ----------------------------
-- View structure for v_daily_sales
-- ----------------------------
DROP VIEW IF EXISTS `v_daily_sales`;
CREATE ALGORITHM = UNDEFINED SQL SECURITY DEFINER VIEW `v_daily_sales` AS select cast(`o`.`created_at` as date) AS `day`,count(0) AS `order_count`,sum(`o`.`total_price_cents`) AS `total_cents` from `orders` `o` where (`o`.`status` in ('PAID','ACCEPTED','COOKING','SERVED','DONE')) group by cast(`o`.`created_at` as date);

-- ----------------------------
-- View structure for v_order_detail
-- ----------------------------
DROP VIEW IF EXISTS `v_order_detail`;
CREATE ALGORITHM = UNDEFINED SQL SECURITY DEFINER VIEW `v_order_detail` AS select `o`.`id` AS `order_id`,`o`.`order_no` AS `order_no`,`o`.`table_no` AS `table_no`,`o`.`status` AS `status`,`o`.`total_price_cents` AS `total_price_cents`,`o`.`created_at` AS `created_at`,`i`.`id` AS `item_id`,`i`.`dish_id` AS `dish_id`,`i`.`dish_name_snapshot` AS `dish_name_snapshot`,`i`.`unit_price_cents_snapshot` AS `unit_price_cents_snapshot`,`i`.`qty` AS `qty`,`i`.`item_remark` AS `item_remark` from (`orders` `o` join `order_item` `i` on((`i`.`order_id` = `o`.`id`)));

-- ----------------------------
-- View structure for v_order_list
-- ----------------------------
DROP VIEW IF EXISTS `v_order_list`;
CREATE ALGORITHM = UNDEFINED SQL SECURITY DEFINER VIEW `v_order_list` AS select `o`.`id` AS `id`,`o`.`order_no` AS `order_no`,`o`.`table_no` AS `table_no`,`o`.`status` AS `status`,`o`.`total_price_cents` AS `total_price_cents`,`o`.`created_at` AS `created_at`,`o`.`paid_at` AS `paid_at` from `orders` `o`;

-- ----------------------------
-- Triggers structure for table order_item
-- ----------------------------
DROP TRIGGER IF EXISTS `trg_order_item_after_insert`;
delimiter ;;
CREATE TRIGGER `trg_order_item_after_insert` AFTER INSERT ON `order_item` FOR EACH ROW BEGIN
  UPDATE orders
  SET total_price_cents = (
    SELECT COALESCE(SUM(unit_price_cents_snapshot * qty), 0)
    FROM order_item
    WHERE order_id = NEW.order_id
  )
  WHERE id = NEW.order_id;
END
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table order_item
-- ----------------------------
DROP TRIGGER IF EXISTS `trg_order_item_after_update`;
delimiter ;;
CREATE TRIGGER `trg_order_item_after_update` AFTER UPDATE ON `order_item` FOR EACH ROW BEGIN
  UPDATE orders
  SET total_price_cents = (
    SELECT COALESCE(SUM(unit_price_cents_snapshot * qty), 0)
    FROM order_item
    WHERE order_id = NEW.order_id
  )
  WHERE id = NEW.order_id;
END
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table order_item
-- ----------------------------
DROP TRIGGER IF EXISTS `trg_order_item_after_delete`;
delimiter ;;
CREATE TRIGGER `trg_order_item_after_delete` AFTER DELETE ON `order_item` FOR EACH ROW BEGIN
  UPDATE orders
  SET total_price_cents = (
    SELECT COALESCE(SUM(unit_price_cents_snapshot * qty), 0)
    FROM order_item
    WHERE order_id = OLD.order_id
  )
  WHERE id = OLD.order_id;
END
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table review_item
-- ----------------------------
DROP TRIGGER IF EXISTS `trg_review_item_after_insert`;
delimiter ;;
CREATE TRIGGER `trg_review_item_after_insert` AFTER INSERT ON `review_item` FOR EACH ROW BEGIN
  IF NEW.dish_id IS NOT NULL THEN
    UPDATE dish d
    JOIN (
      SELECT
        dish_id,
        COUNT(*) AS cnt,
        AVG(rating) AS avg_rating,
        (SUM(CASE WHEN rating >= 4 THEN 1 ELSE 0 END) / COUNT(*)) * 100 AS good_rate_pct
      FROM review_item
      WHERE dish_id = NEW.dish_id
      GROUP BY dish_id
    ) s ON d.id = s.dish_id
    SET d.rating_count = s.cnt,
        d.rating_avg   = ROUND(s.avg_rating, 2),
        d.good_rate    = ROUND(s.good_rate_pct, 2)
    WHERE d.id = NEW.dish_id;
  END IF;
END
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table review_item
-- ----------------------------
DROP TRIGGER IF EXISTS `trg_review_item_after_update`;
delimiter ;;
CREATE TRIGGER `trg_review_item_after_update` AFTER UPDATE ON `review_item` FOR EACH ROW BEGIN
  IF OLD.dish_id IS NOT NULL THEN
    UPDATE dish d
    LEFT JOIN (
      SELECT
        dish_id,
        COUNT(*) AS cnt,
        AVG(rating) AS avg_rating,
        (SUM(CASE WHEN rating >= 4 THEN 1 ELSE 0 END) / COUNT(*)) * 100 AS good_rate_pct
      FROM review_item
      WHERE dish_id = OLD.dish_id
      GROUP BY dish_id
    ) s ON d.id = s.dish_id
    SET d.rating_count = COALESCE(s.cnt, 0),
        d.rating_avg   = COALESCE(ROUND(s.avg_rating, 2), 0.00),
        d.good_rate    = COALESCE(ROUND(s.good_rate_pct, 2), 0.00)
    WHERE d.id = OLD.dish_id;
  END IF;

  IF NEW.dish_id IS NOT NULL THEN
    UPDATE dish d
    JOIN (
      SELECT
        dish_id,
        COUNT(*) AS cnt,
        AVG(rating) AS avg_rating,
        (SUM(CASE WHEN rating >= 4 THEN 1 ELSE 0 END) / COUNT(*)) * 100 AS good_rate_pct
      FROM review_item
      WHERE dish_id = NEW.dish_id
      GROUP BY dish_id
    ) s ON d.id = s.dish_id
    SET d.rating_count = s.cnt,
        d.rating_avg   = ROUND(s.avg_rating, 2),
        d.good_rate    = ROUND(s.good_rate_pct, 2)
    WHERE d.id = NEW.dish_id;
  END IF;
END
;;
delimiter ;

-- ----------------------------
-- Triggers structure for table review_item
-- ----------------------------
DROP TRIGGER IF EXISTS `trg_review_item_after_delete`;
delimiter ;;
CREATE TRIGGER `trg_review_item_after_delete` AFTER DELETE ON `review_item` FOR EACH ROW BEGIN
  IF OLD.dish_id IS NOT NULL THEN
    UPDATE dish d
    LEFT JOIN (
      SELECT
        dish_id,
        COUNT(*) AS cnt,
        AVG(rating) AS avg_rating,
        (SUM(CASE WHEN rating >= 4 THEN 1 ELSE 0 END) / COUNT(*)) * 100 AS good_rate_pct
      FROM review_item
      WHERE dish_id = OLD.dish_id
      GROUP BY dish_id
    ) s ON d.id = s.dish_id
    SET d.rating_count = COALESCE(s.cnt, 0),
        d.rating_avg   = COALESCE(ROUND(s.avg_rating, 2), 0.00),
        d.good_rate    = COALESCE(ROUND(s.good_rate_pct, 2), 0.00)
    WHERE d.id = OLD.dish_id;
  END IF;
END
;;
delimiter ;

SET FOREIGN_KEY_CHECKS = 1;
