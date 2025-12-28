#include "dishservice.h"

DishService* DishService::dishService = new DishService();

DishService::DishService() {

}

DishService *DishService::getInstance() {
    return dishService;
}

