#ifndef RANOBE_READER_H
#define RANOBE_READER_H

#include "ChapterView.h"
#include "JsonWork.h"
#include "RanobeList.h"
#include "RanobeView.h"

#include <QJsonObject>
#include <QLabel>
#include <QLayout>
#include <QMainWindow>
#include <QResizeEvent>
#include <QScrollArea>
#include <QStackedWidget>
#include <QString>

enum class LayoutType { EmptyLayout, RanobeList, RanobeView, ChapterView };

class RanobeReader : public QMainWindow {
    Q_OBJECT
   public:
    RanobeReader(QWidget* parent = nullptr);

    void clearWindow() const;

    void setWindowLayout(QLayout* layout) const;

    void setWindowWidget(QWidget* widget) const;

   private slots:
    void RanobeChosen(QListWidgetItem* ranobe);

    void setChapterView(const QString& titleName, const int& chapterIndex);

    void setRanobeList();

    void setRanobeView(const QString& titleName);

   protected:
    void resizeEvent(QResizeEvent* event) override;

   private:
    QWidget* central_{};
    QStackedWidget* widgetStack_{};
    RanobeList* ranobeList_{};
    RanobeView* ranobeView_{};
    ChapterView* chapterView_{};

    QScrollArea* ranobeScrollList_{};

    LayoutType currentLayout = LayoutType::EmptyLayout;
    QJsonObject cfg_{};
};

#endif  // RANOBE_READER_H
