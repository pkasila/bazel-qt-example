#include "mainwindow.h"
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), Score(0), ClickPower(1), UpgradeCost(10) {

    QWidget *CentralWidget = new QWidget(this);
    QHBoxLayout *MainLayout = new QHBoxLayout;

    QVBoxLayout *InfoLayout = new QVBoxLayout;

    ScoreLabel = new QLabel("Очки: 0", this);
    ScoreLabel->setAlignment(Qt::AlignLeft);
    InfoLayout->addWidget(ScoreLabel);

    UpgradeCostLabel = new QLabel("Цена улучшения: 10", this);
    UpgradeCostLabel->setAlignment(Qt::AlignLeft);
    InfoLayout->addWidget(UpgradeCostLabel);

    UpgradeButton = new QPushButton("Улучшить клик", this);
    connect(UpgradeButton, &QPushButton::clicked, this, &MainWindow::UpgradeClick);
    InfoLayout->addWidget(UpgradeButton);

    LoadImageButton = new QPushButton("Загрузить изображение", this);
    connect(LoadImageButton, &QPushButton::clicked, this, &MainWindow::LoadImage);
    InfoLayout->addWidget(LoadImageButton);

    MainLayout->addLayout(InfoLayout);


    QVBoxLayout *ImageLayout = new QVBoxLayout;

    ImageLabel = new QLabel(this);
    ImageLabel->setAlignment(Qt::AlignCenter);
    ImageLabel->setText("Загрузите изображение");
    ImageLabel->setStyleSheet("border: 2px dashed gray; padding: 10px;");
    ImageLabel->setMinimumSize(300, 300);
    ImageLayout->addWidget(ImageLabel);

    ProgressBar = new QProgressBar(this);
    ProgressBar->setRange(0, 100);
    ProgressBar->setValue(0);
    ProgressBar->setStyleSheet("QProgressBar { text-align: center; }");
    ProgressBar->setMinimumHeight(30);
    ImageLayout->addWidget(ProgressBar);

    MainLayout->addLayout(ImageLayout);

    CentralWidget->setLayout(MainLayout);
    setCentralWidget(CentralWidget);

    LoadProgress();
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (ImageLabel->geometry().contains(event->pos()) && !ImageLabel->pixmap().isNull()) {
        Score += ClickPower;
        ScoreLabel->setText(QString("Очки: %1").arg(Score));
        ProgressBar->setValue(std::min(Score * 100 / UpgradeCost, 100));
        SaveProgress();
    }
}

void MainWindow::UpgradeClick() {
    if (Score >= UpgradeCost) {
        Score -= UpgradeCost;
        ClickPower++;
        UpgradeCost = static_cast<int>(UpgradeCost * 1.5);
        ScoreLabel->setText(QString("Очки: %1").arg(Score));
        UpgradeCostLabel->setText(QString("Цена улучшения: %1").arg(UpgradeCost));
        ProgressBar->setValue(std::min(Score * 100 / UpgradeCost, 100));
        SaveProgress();
    }
}

void MainWindow::LoadImage() {
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите изображение", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    if (!fileName.isEmpty()) {
        QPixmap pixmap(fileName);
        ImageLabel->setPixmap(pixmap.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ImageLabel->setStyleSheet("");
        LoadImageButton->hide();
    }
}

void MainWindow::SaveProgress() {
    QFile file("progress.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << Score << " " << ClickPower << " " << UpgradeCost << " " << ProgressBar->value();
        file.close();
    }
}

void MainWindow::LoadProgress() {
    QFile file("progress.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        int ProgressPercent;
        in >> Score >> ClickPower >> UpgradeCost >> ProgressPercent;
        file.close();
        ScoreLabel->setText(QString("Очки: %1").arg(Score));
        UpgradeCostLabel->setText(QString("Цена улучшения: %1").arg(UpgradeCost));
        ProgressBar->setValue(ProgressPercent);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    if (!ImageLabel->pixmap().isNull()) {
        ImageLabel->setPixmap(ImageLabel->pixmap().scaled(ImageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}
