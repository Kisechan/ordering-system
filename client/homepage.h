#pragma once

#include "ElaScrollPage.h"

class HomePage : public ElaScrollPage
{
    Q_OBJECT
public:
    explicit HomePage(QWidget* parent = nullptr);
    ~HomePage() override = default;
};
