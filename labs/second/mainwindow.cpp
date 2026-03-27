#include "mainwindow.h"
#include "merchwindow.h"
#include "ticketwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), isPilotsView(true)
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    setMinimumSize(1400, 800);

    backgroundLabel = new QLabel(centralWidget);
    backgroundPixmap = QPixmap(":/images/f1_background.png");
    backgroundLabel->setPixmap(backgroundPixmap);
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setGeometry(0, 0, width(), height());
    backgroundLabel->lower();

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout *hLayout = new QHBoxLayout();

    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setPixmap(QPixmap(":/images/f1_logo.png"));
    imageLabel->setStyleSheet("border: 2px solid gray;");
    imageLabel->setScaledContents(true);
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    hLayout->addWidget(imageLabel, 2);

    QVBoxLayout *rightLayout = new QVBoxLayout();

    listWidget = new QListWidget(this);
    listWidget->setIconSize(QSize(300, 150));
    listWidget->setViewMode(QListWidget::IconMode);
    listWidget->setMinimumSize(323,400);
    setupPilots();

    connect(listWidget, &QListWidget::itemClicked, this, &MainWindow::handleSingleClick);
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::handleDoubleClick);

    switchButton = new QPushButton("Показать трассы", this);
    connect(switchButton, &QPushButton::clicked, this, &MainWindow::switchView);

    trackInfoLabel = new QLabel("Информация о трассе будет здесь", this);

    scheduleLabel = new QLabel(this);
    setupSchedule();

    calendarWidget = new QCalendarWidget(this);

    rightLayout->addWidget(listWidget);
    rightLayout->addWidget(switchButton);
    rightLayout->addWidget(calendarWidget);
    rightLayout->addWidget(trackInfoLabel);
    rightLayout->addWidget(scheduleLabel);

    hLayout->addLayout(rightLayout, 1);
    mainLayout->addLayout(hLayout);
}

MainWindow::~MainWindow() {}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    backgroundLabel->setPixmap(backgroundPixmap.scaled(event->size(), Qt::KeepAspectRatioByExpanding));
    backgroundLabel->setGeometry(0, 0, event->size().width(), event->size().height());
    QMainWindow::resizeEvent(event);
}

void MainWindow::handleSingleClick(QListWidgetItem *item)
{
    QString itemName = item->text();
    updateLeftView(itemName);
}

void MainWindow::updateLeftView(const QString &itemName)
{
    if (isPilotsView) {
        if (itemName == "Mercedes") {
            imageLabel->setPixmap(QPixmap(":/images/mercedes_l.jpg"));
        } else if (itemName == "Red Bull") {
            imageLabel->setPixmap(QPixmap(":/images/red_bull_l.jpg"));
        } else if (itemName == "Ferrari") {
            imageLabel->setPixmap(QPixmap(":/images/ferrari_l.jpg"));
        } else if (itemName == "McLaren") {
            imageLabel->setPixmap(QPixmap(":/images/mclaren_l.jpg"));
        } else if (itemName == "Alpine") {
            imageLabel->setPixmap(QPixmap(":/images/alpine_l.jpg"));
        } else if (itemName == "Aston Martin") {
            imageLabel->setPixmap(QPixmap(":/images/f1_background.png"));
        } else if (itemName == "Haas") {
            imageLabel->setPixmap(QPixmap(":/images/f1_background.png"));
        } else if (itemName == "Racing Bulls") {
            imageLabel->setPixmap(QPixmap(":/images/racing_bulls_l.jpg"));
        } else if (itemName == "Williams") {
            imageLabel->setPixmap(QPixmap(":/images/willams_l.jpg"));
        } else if (itemName == "Sauber") {
            imageLabel->setPixmap(QPixmap(":/images/f1_background.png"));
        }
    } else {
        if (itemName == "Spa-Francorchamps") {
            imageLabel->setPixmap(QPixmap(":/images/left_spa.png"));
        } else if (itemName == "Monza") {
            imageLabel->setPixmap(QPixmap(":/images/left_monza.png"));
        } else if (itemName == "Montreal") {
            imageLabel->setPixmap(QPixmap(":/images/left_montr.png"));
        } else if (itemName == "Melbourne") {
            imageLabel->setPixmap(QPixmap(":/images/left_melb.png"));
        } else if (itemName == "Monaco") {
            imageLabel->setPixmap(QPixmap(":/images/left_monaco.png"));
        } else {
            imageLabel->setPixmap(QPixmap(":/images/f1_logo.png"));
        }
    }
}

void MainWindow::handleDoubleClick(QListWidgetItem *item)
{
    if (isPilotsView) {
        openMerchWindow(item);
    } else {
        openTicketWindow(item);
    }
}

void MainWindow::openTicketWindow(QListWidgetItem *item)
{
    QString trackName = item->text();
    QDate eventDate;

    if (trackName == "Spa-Francorchamps") {
        eventDate = QDate(2025, 7, 28);
    } else if (trackName == "Monza") {
        eventDate = QDate(2025, 9, 1);
    } else if (trackName == "Montreal") {
        eventDate = QDate(2025, 6, 9);
    } else if (trackName == "Melbourne") {
        eventDate = QDate(2025, 3, 24);
    } else if (trackName == "Monaco") {
        eventDate = QDate(2025, 5, 26);
    } else {
        eventDate = QDate::currentDate().addDays(30);
    }

    QDate selectedDate = calendarWidget->selectedDate();

    if (selectedDate > eventDate) {
        QMessageBox::warning(this, "Ошибка", "Событие уже закончилось. Невозможно купить билет.");
        return;
    }
    TicketWindow *ticketWindow = new TicketWindow(trackName, eventDate, this);
    ticketWindow->exec();
}

void MainWindow::setupPilots()
{
    listWidget->clear();
    QListWidgetItem *item1 = new QListWidgetItem(QIcon(":/images/mercedes.jpg"), "Mercedes");
    QListWidgetItem *item2 = new QListWidgetItem(QIcon(":/images/red_bull.jpg"), "Red Bull");
    QListWidgetItem *item3 = new QListWidgetItem(QIcon(":/images/ferrari.jpg"), "Ferrari");
    QListWidgetItem *item4 = new QListWidgetItem(QIcon(":/images/mclaren.jpg"), "McLaren");
    QListWidgetItem *item5 = new QListWidgetItem(QIcon(":/images/alpine.jpg"), "Alpine");
    QListWidgetItem *item6 = new QListWidgetItem(QIcon(":/images/aston_martin.jpg"), "Aston Martin");
    QListWidgetItem *item7 = new QListWidgetItem(QIcon(":/images/haas.jpg"), "Haas");
    QListWidgetItem *item8 = new QListWidgetItem(QIcon(":/images/racing_bulls.jpg"), "Racing Bulls");
    QListWidgetItem *item9 = new QListWidgetItem(QIcon(":/images/willams.jpg"), "Williams");
    QListWidgetItem *item10 = new QListWidgetItem(QIcon(":/images/sauber.jpg"), "Sauber");

    listWidget->setIconSize(QSize(300, 150));
    listWidget->addItem(item1);
    listWidget->addItem(item2);
    listWidget->addItem(item3);
    listWidget->addItem(item4);
    listWidget->addItem(item5);
    listWidget->addItem(item6);
    listWidget->addItem(item7);
    listWidget->addItem(item8);
    listWidget->addItem(item9);
    listWidget->addItem(item10);
}

void MainWindow::setupTracks()
{
    listWidget->clear();
    QListWidgetItem *item1 = new QListWidgetItem(QIcon(":/images/australia2024.png"), "Melbourne");
    QListWidgetItem *item2 = new QListWidgetItem(QIcon(":/images/monaco.jpeg"), "Monaco");
    QListWidgetItem *item3 = new QListWidgetItem(QIcon(":/images/montreal.jpeg"), "Montreal");
    QListWidgetItem *item4 = new QListWidgetItem(QIcon(":/images/spa.jpeg"), "Spa-Francorchamps");
    QListWidgetItem *item5 = new QListWidgetItem(QIcon(":/images/monza.jpeg"), "Monza");

    listWidget->addItem(item1);
    listWidget->addItem(item2);
    listWidget->addItem(item3);
    listWidget->addItem(item4);
    listWidget->addItem(item5);
}

void MainWindow::setupSchedule()
{
    scheduleLabel->setText("Расписание заездов:\n"
        "Melbourne: 24 марта 2025\n"
        "Monaco: 26 мая 2025\n"
        "Montreal: 9 июня 2025\n"
        "Spa: 28 июля 2025\n"
        "Monza: 1 сентября 2025\n");
}

void MainWindow::switchView()
{
    if (isPilotsView) {
        setupTracks();
        switchButton->setText("Показать пилотов");
        imageLabel->setPixmap(QPixmap(":/images/f1_logo.png"));
    } else {
        setupPilots();
        switchButton->setText("Показать трассы");
    }
    isPilotsView = !isPilotsView;
}

void MainWindow::showDriverImage(QListWidgetItem *item)
{
    QString driverName = item->text();
    if (driverName == "Льюис Хэмилтон") {
        imageLabel->setPixmap(QPixmap(":/images/pilot1.png"));
    } else if (driverName == "Макс Ферстаппен") {
        imageLabel->setPixmap(QPixmap(":/images/pilot2.png"));
    }
}

void MainWindow::updateTrackInfo(int index)
{
    QString trackName = comboBox->itemText(index);
    trackInfoLabel->setText("Информация о трассе: " + trackName);
    updateLeftView(trackName);
}

void MainWindow::openMerchWindow(QListWidgetItem *item)
{
    MerchWindow *merchWindow = new MerchWindow(item->text(), this);
    merchWindow->exec();
}
