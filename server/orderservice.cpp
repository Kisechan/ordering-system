#include "orderservice.h"

OrderService* OrderService::orderService = nullptr;

OrderService::OrderService() {

}

OrderService *OrderService::getInstance() {
    if (orderService == nullptr) {
        orderService = new OrderService();
    }
    return orderService;
}

