#include "dishservice.h"

DishService* DishService::dishService = nullptr;

DishService::DishService() {

}

DishService *DishService::getInstance() {
    if (dishService == nullptr) {
        dishService = new DishService();
    }
    return dishService;
}

