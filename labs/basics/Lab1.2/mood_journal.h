#ifndef MOOD_JOURNAL_H
#define MOOD_JOURNAL_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSlider>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QCalendarWidget>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QLabel>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QApplication>

class MoodJournal : public QWidget {
    Q_OBJECT

public:
    MoodJournal(QWidget *parent = nullptr);
    ~MoodJournal();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onCatchStarClicked();
    void onMoodChanged(int value);
    void onThoughtEntered();
    void onGlitterToggled(int state);
    void onDateSelected();

private:
    void setupUI();
    void applyGlitterStyle(bool enabled);
    void updateStatusBar();

    // UI Components
    QVBoxLayout *mainLayout;
    QHBoxLayout *starLayout;
    
    QPushButton *catchStarButton;
    QLabel *starCountLabel;
    QLineEdit *thoughtInput;
    QSlider *moodSlider;
    QCheckBox *glitterCheckBox;
    QCalendarWidget *calendar;
    QProgressBar *magicBar;
    QLabel *statusLabel;
    
    // Data
    int starCount;
    bool glitterMode;
};

#endif // MOOD_JOURNAL_H
