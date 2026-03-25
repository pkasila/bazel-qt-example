#pragma once

#include <QCalendarWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QDate>
#include <QDial>
#include <QLCDNumber>
#include <QLineEdit>
#include <QListWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QSlider>
#include <QString>
#include <QTabWidget>
#include <QTextEdit>
#include <QTimer>
#include <QWidget>

class MainWindow : public QWidget {
    Q_OBJECT

   public:
    explicit MainWindow(QWidget* parent = nullptr);

   private slots:
    void onDialChanged(int value);
    void onTimerTick();
    void onStartStopClicked();
    void onResetClicked();
    void onCalendarClicked(const QDate& date);
    void updateTheme(int index);

   private:
    void setupUi();
    void applyTheme(const QString& theme);

    // Widgets (Basic)
    QLineEdit* sessionLabel_;
    QDial* durationDial_;
    QTextEdit* notesArea_;
    QComboBox* modeSelector_;
    QSlider* volumeSlider_;
    QCheckBox* notificationsCheck_;

    // Widgets (Other)
    QCalendarWidget* calendar_;
    QLCDNumber* timeDisplay_;
    QProgressBar* progressBar_;
    QTabWidget* mainTabs_;

    // Controls
    QPushButton* startStopBtn_;
    QPushButton* resetBtn_;

    // Logic members
    QTimer* timer_;
    int remainingSeconds_;
    int totalSeconds_;
    bool isRunning_;
};
