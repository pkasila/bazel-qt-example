#include "hdrs/RanobeList.h"

#include <QFile>

RanobeList::RanobeList(QWidget* parent) : QListWidget(parent) {
    setViewMode(QListWidget::IconMode);
    setSpacing(10);
    setResizeMode(QListView::Adjust);
    setDragEnabled(false);
    setMovement(QListView::Static);
    setSelectionMode(QAbstractItemView::NoSelection);
    setStyleSheet("QListWidget::item { margin: 5px; }");
}

void RanobeList::addRanobe(const QString& titleName, const QString& ruTitleName, QIcon icon) {
    auto* item = new QListWidgetItem;
    item->setSizeHint(QSize(160, 150));

    auto* ranobe = new RanobeListItem(titleName, ruTitleName, std::move(icon));
    addItem(item);
    setItemWidget(item, ranobe);
    ranobeList_.emplace_back(ranobe);
}

void RanobeList::setIconSize(const QSize& size) {
    for (int i = 0; i < std::ssize(ranobeList_); i++) {
        item(i)->setSizeHint(QSize(size.width() + 20, size.height() + 60));
        ranobeList_.at(i)->setIconSize(size);
    }
}

void RanobeList::uploadRanobeList(const QJsonObject& json) {
    int ranobeCount = json["ranobeCount"].toInt();
    QJsonArray titles = json["ranobeInfo"].toArray();
    for (int i = 0; i < ranobeCount; i++) {
        QJsonValue value = titles.at(i);
        QString title = value["title"].toString();
        QJsonObject titleData = json[title].toObject();
        QIcon icon(QString(":/resources/images/") + title + QString(".jpg"));
        if (!QFile::exists(QString(":/resources/images/") + title + QString(".jpg"))) {
            qDebug() << QString(":/resources/images/") + title + QString(".jpg") << '\n';
        }
        QString ruTitle = titleData["ru"].toString();
        addRanobe(title, ruTitle, std::move(icon));
    }
}