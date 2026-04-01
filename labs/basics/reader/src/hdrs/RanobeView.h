#ifndef RANOBEVIEW_H
#define RANOBEVIEW_H

#include <QIcon>
#include <QJsonArray>
#include <QLabel>
#include <QListWidget>
#include <QScrollArea>
#include <QWidget>

class RanobeView : public QWidget {
    Q_OBJECT
   public:
    RanobeView(
        const QString& titleName, const QString& ruTitleName, QIcon icon,
        QWidget* parent = nullptr);

    void setIconSize(const QSize& size);
   private slots:
    void chapterDoubleClicked(QListWidgetItem* item);
   signals:
    void chapterChosen(const QString& titleName, const int& chapterIndex);

   private:
    QString titleName_{};
    QIcon icon_{};
    QLabel* iconLabel_{};
    QListWidget* chapterList_{};
    QJsonArray chapterFilenamesArray_{};
    QScrollArea* synopsisScrollable_{};
};

#endif  // RANOBEVIEW_H
