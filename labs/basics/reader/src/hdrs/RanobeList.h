#ifndef RANOBELIST_H
#define RANOBELIST_H

#include "RanobeListItem.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QListWidget>
#include <QString>
#include <cstddef>
#include <iterator>
#include <utility>
#include <vector>

class RanobeList : public QListWidget {
    Q_OBJECT
   public:
    RanobeList(QWidget* parent = nullptr);

    void addRanobe(const QString& titleName, const QString& ruTitleName, QIcon icon);

    void setIconSize(const QSize& size);

    void uploadRanobeList(const QJsonObject& json);

   private:
    std::vector<RanobeListItem*> ranobeList_{};
};

#endif  // RANOBELIST_H
