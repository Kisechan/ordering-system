#include "PlaceholderPage.h"

#include <QVBoxLayout>

#include "ElaText.h"

PlaceholderPage::PlaceholderPage(const QString& text, QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);

    auto* t = new ElaText(text, 16, this);
    t->setAlignment(Qt::AlignCenter);

    layout->addStretch(1);
    layout->addWidget(t);
    layout->addStretch(1);
}
