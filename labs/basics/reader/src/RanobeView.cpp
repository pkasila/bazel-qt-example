#include "hdrs/RanobeView.h"

#include "hdrs/JsonWork.h"

#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QScrollArea>
#include <QString>
#include <utility>

RanobeView::RanobeView(
    const QString& titleName, const QString& ruTitleName, QIcon icon, QWidget* parent)
    : QWidget(parent), titleName_(titleName), icon_(std::move(icon)) {
    auto* mainLayout = new QHBoxLayout;
    auto* titleLayout = new QVBoxLayout;
    auto* chaptersLayout = new QVBoxLayout;

    iconLabel_ = new QLabel;
    iconLabel_->setAlignment(Qt::AlignTop);
    titleLayout->addWidget(iconLabel_);

    mainLayout->addLayout(titleLayout);

    chapterList_ = new QListWidget;
    chapterList_->setDragEnabled(false);
    chapterList_->setMovement(QListView::Static);
    chapterList_->setSelectionMode(QAbstractItemView::NoSelection);

    auto titleInfo = JsonWork::parseJson(
        QString(":/resources/books/") + QString(titleName_) + QString("/titleInfo.json"));
    const int chaptersCount = titleInfo["chapterCount"].toInt();
    const QJsonObject chapterNames = titleInfo["chapterNames"].toObject();
    chapterFilenamesArray_ = titleInfo["chapterFilenames"].toArray();
    for (int i = 0; i < chaptersCount; i++) {
        const QString chapterFilename = chapterFilenamesArray_.at(i).toString();
        const QString chapterName = chapterNames[chapterFilename].toString();

        auto* chapterNameLabel = new QLabel(chapterName);

        auto* item = new QListWidgetItem;
        chapterList_->addItem(item);
        chapterList_->setItemWidget(item, chapterNameLabel);
    }

    auto* synopsisLabel = new QLabel(QString("О тайтле\n"));
    synopsisLabel->setStyleSheet("font-weight: bold; color: orange");

    auto* synopsis = new QLabel(titleInfo["synopsis"].toString() + QString("\n"));
    synopsis->setWordWrap(true);
    synopsis->setContentsMargins(0, 0, 0, 0);

    synopsisScrollable_ = new QScrollArea;
    synopsisScrollable_->setWidget(synopsis);
    synopsisScrollable_->setWidgetResizable(true);
    synopsisScrollable_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    auto* chaptersLabel = new QLabel(QString("Главы\n"));
    chaptersLabel->setStyleSheet("font-weight: bold; color: orange");

    chaptersLayout->setContentsMargins(2, 2, 2, 2);

    chaptersLayout->addWidget(synopsisLabel);
    chaptersLayout->addWidget(synopsisScrollable_);
    chaptersLayout->addWidget(chaptersLabel);
    chaptersLayout->addWidget(chapterList_);

    setIconSize(QSize(100, 90));

    mainLayout->addLayout(chaptersLayout);

    connect(chapterList_, &QListWidget::itemDoubleClicked, this, &RanobeView::chapterDoubleClicked);

    setLayout(mainLayout);
}

void RanobeView::chapterDoubleClicked(QListWidgetItem* item) {
    emit chapterChosen(titleName_, item->listWidget()->row(item));
}

void RanobeView::setIconSize(const QSize& size) {
    iconLabel_->setPixmap(icon_.pixmap(size));
}