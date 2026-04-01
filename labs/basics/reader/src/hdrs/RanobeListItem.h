#ifndef RANOBELISTITEM_H
#define RANOBELISTITEM_H

#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QListWidgetItem>
#include <QString>

class RanobeListItem : public QWidget {
    Q_OBJECT
   public:
    RanobeListItem(QWidget* parent = nullptr);

    RanobeListItem(
        const QString& titleName, const QString& ruTitleName, QIcon icon,
        QWidget* parent = nullptr);

    void setIconSize(const QSize& size);

    [[nodiscard]] const QString& getTitleName() const;

   private:
    QVBoxLayout* layout_{};
    QIcon icon_{};
    QLabel* iconLabel_{};
    QLabel* titleLabel_{};
    QString titleName_{};
};

#endif  // RANOBELISTITEM_H
