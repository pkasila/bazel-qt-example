#include "hdrs/RanobeReader.h"

#include "hdrs/ChapterView.h"

#include <QScrollArea>

RanobeReader::RanobeReader(QWidget* parent) : QMainWindow(parent) {
    central_ = new QWidget;
    setCentralWidget(central_);

    auto* layout = new QVBoxLayout;

    cfg_ = JsonWork::parseJson(":/resources/cfg.json");

    ranobeList_ = new RanobeList;
    ranobeList_->uploadRanobeList(cfg_);

    ranobeScrollList_ = new QScrollArea;

    connect(ranobeList_, &QListWidget::itemDoubleClicked, this, &RanobeReader::RanobeChosen);

    widgetStack_ = new QStackedWidget;
    layout->addWidget(widgetStack_);

    central_->setLayout(layout);

    setRanobeList();
}

void RanobeReader::clearWindow() const {
    if (widgetStack_->count() > 0) {
        widgetStack_->removeWidget(widgetStack_->currentWidget());
    }
}

void RanobeReader::setWindowLayout(QLayout* layout) const {
    clearWindow();
    auto* widget = new QWidget;
    widget->setLayout(layout);
    widgetStack_->addWidget(widget);
}

void RanobeReader::setWindowWidget(QWidget* widget) const {
    clearWindow();
    widgetStack_->addWidget(widget);
}

void RanobeReader::RanobeChosen(QListWidgetItem* item) {
    auto* ranobe = dynamic_cast<RanobeListItem*>((item->listWidget())->itemWidget(item));
    setRanobeView(ranobe->getTitleName());
}

void RanobeReader::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);

    int iconWidth = event->size().width();
    int iconHeight = event->size().height();
    switch (currentLayout) {
        case LayoutType::EmptyLayout:
            break;
        case LayoutType::RanobeList:
            ranobeList_->setIconSize(QSize(iconWidth / 4, iconHeight / 4));
            break;
        case LayoutType::RanobeView:
            ranobeView_->setIconSize(QSize(iconWidth / 3, iconHeight / 3));
            break;
        case LayoutType::ChapterView:
            break;
    }
}

void RanobeReader::setChapterView(const QString& titleName, const int& chapterIndex) {
    const QJsonObject titleInfo = cfg_[titleName].toObject();
    const QString ruTitleName = titleInfo["ru"].toString();
    chapterView_ = new ChapterView(titleName, ruTitleName, chapterIndex);
    setWindowWidget(chapterView_);
    connect(chapterView_, &ChapterView::toRanobeView, this, &RanobeReader::setRanobeView);
    currentLayout = LayoutType::ChapterView;
}

void RanobeReader::setRanobeList() {
    auto* layout = new QVBoxLayout;

    auto* label = new QLabel(QString("Каталог"));
    label->setStyleSheet("font-weight: bold; color: orange");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);

    ranobeList_->setIconSize(QSize(width() / 4, height() / 4));

    ranobeScrollList_->setWidget(ranobeList_);
    ranobeScrollList_->setWidgetResizable(true);
    ranobeScrollList_->adjustSize();

    layout->addWidget(ranobeScrollList_);
    setWindowLayout(layout);

    currentLayout = LayoutType::RanobeList;
}

void RanobeReader::setRanobeView(const QString& titleName) {
    auto* layout = new QVBoxLayout;

    const QJsonObject titleInfo = cfg_[titleName].toObject();

    const QString ruTitleName = titleInfo["ru"].toString();

    auto* toRanobeList = new QPushButton(QString("К катологу"));
    layout->addWidget(toRanobeList);

    auto* label = new QLabel(ruTitleName);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-weight: bold; color: orange");
    layout->addWidget(label);

    QIcon icon(QString(":/resources/images/") + titleName + QString(".jpg"));

    ranobeView_ = new RanobeView(titleName, ruTitleName, std::move(icon));

    layout->addWidget(ranobeView_);
    setWindowLayout(layout);
    ranobeView_->setIconSize(QSize(width() / 3, height() / 3));

    connect(ranobeView_, &RanobeView::chapterChosen, this, &RanobeReader::setChapterView);
    connect(toRanobeList, &QPushButton::pressed, this, &RanobeReader::setRanobeList);

    currentLayout = LayoutType::RanobeView;
}