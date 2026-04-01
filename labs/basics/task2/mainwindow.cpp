#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <algorithm>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPixmap>
#include <QIcon>
#include <QLabel>
#include <QDebug>
#include <QString>
#include <QHeaderView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QComboBox>
#include <QTimer>
#include <QMessageBox>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    //, ui(new Ui::MainWindow)
{
    //ui->setupUi(this);

    QWidget *main_window = new QWidget(this);
    setCentralWidget(main_window);

    this->setFixedSize(550, 550);

    QVBoxLayout *layout = new QVBoxLayout(main_window);

    imageButton = new QPushButton(this);
    QPixmap pix("/home/xxine/humster/beginner.png");

    if (pix.isNull()) {
        qDebug() << "Изображение не загружено! Проверьте путь к файлу.";
    } else {
        imageButton->setIcon(QIcon(pix));
        imageButton->setIconSize(pix.size());
    }
    imageButton->setText("");

    QPixmap coi("/home/xxine/humster/coin.png");
    if (coi.isNull()) {
        qDebug() << "Изображение монетки не загружено! Проверьте путь к файлу.";
    } else {
        coi = coi.scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    lab_coins = new QLabel(this);
    lab_coins->setPixmap(coi);

    lab_coins_count = new QLabel(this);
    lab_coins_count->setText(/*(coins > 1000) ? QString::number(static_cast<int>(coins / 1000)) + "к" : */QString::number(static_cast<int>(coins)));

    QLabel *brillIconLabel = new QLabel(this);
    QPixmap brillIcon("/home/xxine/humster/brill.png");
    if (brillIcon.isNull()) {
        qDebug() << "Изображение brill.png не загружено! Проверьте путь к файлу.";
    } else {
        brillIcon = brillIcon.scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        brillIconLabel->setPixmap(brillIcon);
    }

    awards = new QPushButton("Награды", this);

    QLabel *awardIconLabel = new QLabel(this);
    QPixmap awardIcon("/home/xxine/humster/award.png");
    if (awardIcon.isNull()) {
        qDebug() << "Изображение award.png не загружено! Проверьте путь к файлу.";
    } else {
        awardIcon = awardIcon.scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        awardIconLabel->setPixmap(awardIcon);
    }

    records = new QPushButton("Достижения", this);

    shop = new QPushButton("Магазин", this);
    QPixmap shopIcon("/home/xxine/humster/shop.png");
    if (shopIcon.isNull()) {
        qDebug() << "Изображение магазина не загружено! Проверьте путь к файлу.";
    } else {
        shopIcon = shopIcon.scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    connect(shop, &QPushButton::clicked, this, &MainWindow::showShop);

    connect(records, &QPushButton::clicked, this, &MainWindow::showAchievements);

    QLabel *shopIconLabel = new QLabel(this);
    shopIconLabel->setPixmap(shopIcon);

    QHBoxLayout *coinsLayout = new QHBoxLayout();
    coinsLayout->addWidget(lab_coins);
    coinsLayout->addWidget(lab_coins_count);
    coinsLayout->addStretch();
    coinsLayout->addWidget(brillIconLabel);
    coinsLayout->addWidget(awards);
    coinsLayout->addWidget(awardIconLabel);
    coinsLayout->addWidget(records);
    coinsLayout->addWidget(shopIconLabel);
    coinsLayout->addWidget(shop);

    lab_lvl = new QLabel(this);
    lab_lvl->setText("Хомяк имеет: " + QString::number((lvl + 1) * 1000) + " мощи");

    QHBoxLayout *labelLayout = new QHBoxLayout();
    labelLayout->addStretch();
    labelLayout->addWidget(lab_lvl);
    labelLayout->addStretch();

    progressBar = new QProgressBar(this);
    progressBar->setRange(0, nums[lvl]);
    progressBar->setValue(0);

    layout->addLayout(coinsLayout);
    layout->addLayout(labelLayout);
    layout->addWidget(imageButton);
    layout->addWidget(progressBar);

    layout->setSpacing(20);
    layout->setContentsMargins(20, 20, 20, 20);

    main_window->setLayout(layout);

    connect(imageButton, &QPushButton::clicked, this, &MainWindow::humsterClick);
    connect(awards, &QPushButton::clicked, this, [this]() {
        int amount = 0;
        if (isGiven[0] == 0 && taps_copy >= 1) {
            amount += moneys[0];
            isGiven[0] = 1;
        }
        if (isGiven[1] == 0 && taps_copy >= 100) {
            amount += moneys[1];
            isGiven[0] = 1;
        }
        if (isGiven[2] == 0 && taps_copy >= 1000) {
            amount += moneys[2];
            isGiven[2] = 1;
        }
        if (isGiven[3] == 0 && sales.size() == 4) {
            amount += moneys[3];
            isGiven[3] = 1;
        }
        if (isGiven[4] == 0 && lvl == 1) {
            amount += moneys[4];
            isGiven[4] = 1;
        }
        if (isGiven[5] == 0 && lvl == 2) {
            amount += moneys[5];
            isGiven[5] = 1;
        }
        coins += amount;
        if (amount != 0)
            QMessageBox::information(this, "Награда!", "За достижения вы получили "+ QString::number(amount) + " монет");
        else
            QMessageBox::information(this, "Пустое уведомление", "К сожалени вы недостаточно играли, так что монет не ждите)");
    });
}

MainWindow::~MainWindow()
{
    //delete ui;
}

void MainWindow::humsterClick() {
    taps++;
    taps_copy++;
    if (taps < nums[lvl]) {
        progressBar->setValue(taps);
        if (lvl == 0) {
            coins += 10;
        } else if (lvl == 1) {
            coins += 52;
        } else {
            coins += 100;
        }
    } else {
        if (lvl == 0) {
            taps = 0;
            coins += 10;
            lvl++;
            QPixmap pix("/home/xxine/humster/intermediate.png");

            if (pix.isNull()) {
                qDebug() << "Изображение не загружено! Проверьте путь к файлу.";
            } else {
                imageButton->setIcon(QIcon(pix));
                imageButton->setIconSize(pix.size());
            }
        } else if (lvl == 1) {
            taps = 0;
            coins += 52;
            lvl++;
            QPixmap pix("/home/xxine/humster/pro.png");

            if (pix.isNull()) {
                qDebug() << "Изображение не загружено! Проверьте путь к файлу.";
            } else {
                imageButton->setIcon(QIcon(pix));
                imageButton->setIconSize(pix.size());
            }
        }

        lab_lvl->setText("Хомяк имеет: " + QString::number((lvl + 1) * 1000) + " мощи");

        progressBar->setRange(0, nums[lvl]);
        progressBar->setValue(taps);
    }

    lab_coins_count->setText(/*(coins > 1000) ? QString::number(static_cast<int>(coins / 1000)) + "к" : */QString::number(static_cast<int>(coins)));

    if (awardsWindow && awardsWindow->isVisible()) {
        updateBars();
    }
}

void MainWindow::showAchievements() {
    if (!awardsWindow) {
        awardsWindow = new QWidget();
        awardsWindow->setWindowTitle("Достижения");

        QHBoxLayout *layout = new QHBoxLayout(awardsWindow);
        table_awards = new QTableWidget();
        table_awards->setRowCount(6);
        table_awards->setColumnCount(2);

        table_awards->horizontalHeader()->setVisible(false);
        table_awards->verticalHeader()->setVisible(false);

        table_awards->setColumnWidth(0, 180);
        table_awards->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

        layout->addWidget(table_awards);
        awardsWindow->setLayout(layout);
    }

    updateBars();
    awardsWindow->show();
}

void MainWindow::updateBars() {
    for (int i = 0; i < 6; ++i) {
        QWidget *container = new QWidget();
        QHBoxLayout *containerLayout = new QHBoxLayout(container);

        QProgressBar *progressBar = new QProgressBar();
        bool fl = false;
        switch (i) {
        case 0:
            progressBar->setRange(0, 1);
            progressBar->setValue(taps_copy != 0 ? 1 : 0);
            break;
        case 1:
            progressBar->setRange(0, 100);
            progressBar->setValue(std::min(100, static_cast<int>(taps_copy)));
            break;
        case 2:
            progressBar->setRange(0, 1000);
            progressBar->setValue(std::min(1000, static_cast<int>(taps_copy)));
            break;
        case 3:
            progressBar->setRange(0, 4);
            progressBar->setValue(std::min(4, static_cast<int>(sales.size())));
            break;
        case 4:
            progressBar->setRange(0, 1);
            progressBar->setValue(lvl >= 1 ? 1 : 0);
            break;
        case 5:
            progressBar->setRange(0, 1);
            progressBar->setValue(lvl >= 2 ? 1 : 0);
            break;
        default:
            break;
        }

        QLabel *textLabel = new QLabel();
        if (progressBar->value() == progressBar->maximum()) {
            textLabel->setText("✅ " + vec_records[i]);

        } else {
            textLabel->setText("🔴 " + vec_records[i]);
        }

        QPalette palette = progressBar->palette();
        palette.setColor(QPalette::Highlight, QColor("#CC9933"));
        progressBar->setPalette(palette);

        containerLayout->addWidget(progressBar);
        containerLayout->setContentsMargins(0, 0, 0, 0);

        table_awards->setCellWidget(i, 0, textLabel);
        table_awards->setCellWidget(i, 1, container);
    }
}

void MainWindow::showShop() {
    if (!shopWindow) {
        shopWindow = new QWidget();
        shopWindow->setWindowTitle("Магазин");
        shopWindow->resize(800, 400);

        QVBoxLayout *layout = new QVBoxLayout(shopWindow);

        QStandardItemModel *model = new QStandardItemModel(farm.size(), 6, shopWindow);
        model->setHorizontalHeaderLabels({"", "Название", "Описание", "Цена", "Время", "Купить"});

        QVector<int> prices = {100, 500, 1000, 3000};

        QTableView *tableView = new QTableView(shopWindow);
        tableView->setModel(model);

        for (int i = 0; i < farm.size(); ++i) {
            QString iconPath = QString("/home/xxine/humster/%1.png").arg(farm[i].toLower());
            QPixmap iconPixmap(iconPath);

            if (!iconPixmap.isNull()) {
                iconPixmap = iconPixmap.scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                QStandardItem *iconItem = new QStandardItem(QIcon(iconPixmap), "");
                model->setItem(i, 0, iconItem);
            }

            model->setItem(i, 1, new QStandardItem(farm[i]));
            model->setItem(i, 2, new QStandardItem(QString("Описание: %1").arg(farm[i])));
            model->setItem(i, 3, new QStandardItem(QString::number(prices[i])));

            QComboBox *comboBox = new QComboBox();
            comboBox->addItems({"1 минута", "5 минут", "10 минут"});
            tableView->setIndexWidget(model->index(i, 4), comboBox);

            QPushButton *buyButton = new QPushButton("Купить");
            tableView->setIndexWidget(model->index(i, 5), buyButton);

            connect(buyButton, &QPushButton::clicked, this, [this, comboBox, prices, i]() {
                int timeMultiplier = (comboBox->currentIndex() == 1) ? 5 : (comboBox->currentIndex() == 2) ? 10 : 1;
                //auto timeMultiplier = 2;
                int cost = prices[i] * timeMultiplier;

                if (coins >= cost) {
                    coins -= cost;
                    int x = QRandomGenerator::global()->bounded(0, 2 * prices[i] * timeMultiplier);
                    coins += x;
                    QMessageBox::information(this, "Успех", QString("Куплен %1 на %2 минут, вы получили %3").arg(farm[i]).arg(timeMultiplier).arg(x));
                    //QWidget *wid = new QWidget(this);
                    //, wid->show();
                    lab_coins_count->setText(QString::number(static_cast<int>(coins)));
                    //qDebug() << "norm";
                    sales.push_back(1);
                } else {
                    QMessageBox::warning(this, "Ошибка", "Недостаточно средств");
                    //qDebug() << QString::number(static_cast<int>(coins));
                }
                //updateBars();
            });
            /*connect(buyButton, &QPushButton::clicked, this, [this, comboBox]() {
                qDebug() << "hello";
                int timeMultiplier = (comboBox->currentIndex() == 1) ? 5 : (comboBox->currentIndex() == 2) ? 10 : 1;
                qDebug() << "Time multiplier:" << timeMultiplier;
            });*/
        }

        tableView->setColumnWidth(0, 50);
        tableView->setColumnWidth(1, 100);
        tableView->setColumnWidth(2, 200);
        tableView->setColumnWidth(3, 100);
        tableView->setColumnWidth(4, 150);
        tableView->setColumnWidth(5, 100);

        tableView->horizontalHeader()->setStretchLastSection(true);
        tableView->verticalHeader()->setVisible(false);

        layout->addWidget(tableView);
        shopWindow->setLayout(layout);
    }
    shopWindow->show();
}
