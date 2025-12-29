#include "HomePage.h"

#include <QVBoxLayout>
#include <QWidget>

#include "ElaScrollPageArea.h"
#include "ElaText.h"
#include "ElaLineEdit.h"
#include "ElaImageCard.h"

HomePage::HomePage(QWidget* parent)
    : ElaScrollPage(parent)
{
    // 主页一般不需要 ScrollPage 自带标题
    setTitleVisible(false);
    setPageTitleSpacing(0);

    // ScrollPage 的中央内容：先放一个“可滚动的容器壳子”
    auto* content = new QWidget(this);
    auto* root = new QVBoxLayout(content);
    root->setContentsMargins(20, 18, 20, 18);
    root->setSpacing(14);

    // 做一个“内容区域壳子”（类似你图里右侧白色大面板）
    auto* area = new ElaScrollPageArea(content);
    area->setBorderRadius(12);

    auto* areaLayout = new QVBoxLayout(area);
    areaLayout->setContentsMargins(18, 16, 18, 16);
    areaLayout->setSpacing(12);

    // 顶部 banner（占位，图里是轮播；你先不做内容也行）
    auto* banner = new ElaImageCard(area);
    banner->setFixedHeight(160);
    banner->setBorderRadius(12);
    banner->setIsPreserveAspectCrop(true);
    areaLayout->addWidget(banner);

    // 搜索框（图里右侧顶部有一个搜索/筛选输入）
    auto* search = new ElaLineEdit(area);
    search->setPlaceholderText(QStringLiteral("搜索 / 筛选（占位）"));
    search->setClearButtonEnabled(true);
    search->setMinimumHeight(36);
    areaLayout->addWidget(search);

    // 这里开始是“滚动内容占位块”
    // 做高一点，确保你能看到右侧滚动条/滚动效果（scrollball 壳子）
    auto* placeholder = new QWidget(area);
    placeholder->setMinimumHeight(1200);

    auto* phLayout = new QVBoxLayout(placeholder);
    phLayout->setContentsMargins(0, 0, 0, 0);
    phLayout->setSpacing(10);

    auto* tip = new ElaText(QStringLiteral("这里是右侧 Scroll 区域（先做壳子，不填内容）"), 14, placeholder);
    tip->setAlignment(Qt::AlignCenter);
    phLayout->addStretch(1);
    phLayout->addWidget(tip);
    phLayout->addStretch(1);

    areaLayout->addWidget(placeholder);
    root->addWidget(area);

    // 关键：把 content 加进 ElaScrollPage（它内部会包滚动体系）
    addCentralWidget(content, /*isWidgetResizeable=*/true, /*isVerticalGrabGesture=*/true);
}
