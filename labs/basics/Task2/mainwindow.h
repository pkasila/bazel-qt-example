#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QPixmap>
#include <QProgressBar>
#include <QMouseEvent>
#include <QFile>
#include <QTextStream>
#include <QPushButton>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void SaveProgress();
    void LoadProgress();
    void resizeEvent(QResizeEvent *event) override;

protected:
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void UpgradeClick();
    void LoadImage();

private:
    QLabel *ImageLabel;
    QLabel *ScoreLabel;
    QLabel *UpgradeCostLabel;
    QPushButton *UpgradeButton;
    QPushButton *LoadImageButton;
    QProgressBar *ProgressBar;
    int Score;
    int ClickPower;
    int UpgradeCost;
};

#endif
