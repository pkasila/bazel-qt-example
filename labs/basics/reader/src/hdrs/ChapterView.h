#ifndef CHAPTERVIEW_H
#define CHAPTERVIEW_H

#include <QComboBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

class ChapterView : public QWidget {
    Q_OBJECT
   public:
    ChapterView(
        const QString& titleName, const QString& ruTitleName, const int& chapterIndex,
        QWidget* parent = nullptr);

   private slots:
    void goToRanobeView();
    void topToNextChapter();
    void bottomToNextChapter();
    void toPrevChapter();
    void toChosenChapter();
    void openCommentDialog();

   signals:
    void toRanobeView(const QString& titleName);

   private:
    QString titleName_{};
    QString chapterFilename_{};
    QString comment_{};

    int chapterIndex_{};
    int chapterCount_{};

    QJsonArray chapterFilenames_{};
    QJsonObject chapterNames_{};

    QPushButton* toRanobeViewButton_{};
    QPushButton* topPrevChapterButton_{};
    QPushButton* topNextChapterButton_{};

    QPushButton* bottomPrevChapterButton_{};
    QPushButton* bottomNextChapterButton_{};

    QPushButton* commentButton_{};

    QLabel* chapterText_{};
    QLabel* chapterName_;

    QComboBox* chapterChooseBox_{};

    QString readTxt(const QString& path);
    void saveComment();
    void loadComment();
    void setChapterText();
};

#endif  // CHAPTERVIEW_H
