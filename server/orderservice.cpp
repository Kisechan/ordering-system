#include "orderservice.h"

OrderService* OrderService::orderService = new OrderService();

OrderService::OrderService() {

}

OrderService *OrderService::getInstance() {
    return orderService;
}

