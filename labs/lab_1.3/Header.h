#ifndef LAB_1_3_HEADER_H
#define LAB_1_3_HEADER_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QLabel>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtCore/QTimer>
#include <QtCore/QDateTime>
#include <QtGui/QPixmap>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>

class WaifuManager : public QMainWindow {
public:
    WaifuManager(QWidget *parent = nullptr);
    ~WaifuManager();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void onPetButtonClicked();
    void onEcchiModeToggled(bool enabled);
    void onSpeedChanged(int value);
    void onOutfitChanged(const QString &outfit);
    void onHappinessDecay();

private:
    void setupUI();
    void applyEcchiMode(bool enabled);
    void updateCharacterImage(const QString &imageName);
    void resetShyness();
    
    QWidget *centralWidget;
    QHBoxLayout *mainLayout;
    QVBoxLayout *leftLayout;
    QVBoxLayout *rightLayout;
    
    QLabel *characterLabel;
    QPushButton *petButton;
    QProgressBar *happinessBar;
    QLCDNumber *clickCounter;
    QCheckBox *ecchiModeCheckBox;
    QSlider *speedSlider;
    QScrollArea *wardrobeScrollArea;
    QWidget *wardrobeWidget;
    QVBoxLayout *wardrobeLayout;
    
    QTimer *happinessTimer;
    QTimer *shynessTimer;
    
    int clickCount;
    int happiness;
    bool isShy;
    bool ecchiMode;
    QString currentOutfit;
    
    static const QRect HEAD_AREA;
    static const QStringList OUTFITS;
};

#endif // LAB_1_3_HEADER_H
