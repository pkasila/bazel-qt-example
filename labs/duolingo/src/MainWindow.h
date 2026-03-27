#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "AudioPlayer.h"
#include "ExerciseTypes.h"
#include "ImageView.h"
#include "Settings.h"
#include "TaskView.h"

#include <QComboBox>
#include <QLabel>
#include <QMainWindow>
#include <QStackedWidget>
#include <QString>
#include <memory>

class MainWindow : public QMainWindow {
    Q_OBJECT
   public:
    explicit MainWindow(QWidget* parent = nullptr);

   private slots:
    void ShowHelpMessage();
    void ExerciseChanged(const int&) const;

   private:
    const QString kLocalFilePrefix = "labs/duolingo/data/";
    const QUrl kDuolingoPicPath = QUrl::fromLocalFile(kLocalFilePrefix + "lofi_duolingo.png");

    std::shared_ptr<Settings> settings_{};
    QStackedWidget* stacked_widget_ = nullptr;
    ImageView* image_view_ = nullptr;
    TaskView* task_view_ = nullptr;

    QLabel* difficulty_label_ = nullptr;
    QLabel* score_label_ = nullptr;
    QComboBox* exercise_combo_box_ = nullptr;

    std::shared_ptr<AudioPlayer> audio_player_{};

    void SetChill() const;
    void SetTask(ExerciseType) const;
};

#endif  // MAINWINDOW_H
