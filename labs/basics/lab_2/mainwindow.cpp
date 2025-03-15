#include "mainwindow.h"
#include "OrderForm.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QStandardItem>
#include <QLabel>
#include <QListView>
#include <QPushButton>
#include <QScrollArea>

MainWindow::~MainWindow() {
    delete networkManager;
    delete guitarModel;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), networkManager(nullptr) {
    setupUI();
}

void MainWindow::showOrderForm() {
    OrderForm form(this);
    if (form.exec() == QDialog::Accepted) {
        if (form.isConfirmed()) {
            QString customerName = form.getCustomerName();
            QString message = QString(
                                  "<b>Спасибо за заказ, %1!</b><br><br>"
                                  "Дата доставки: %2<br>"
                                  "Город: %3<br>"
                                  "Способ оплаты: %4"
                                  ).arg(
                                      customerName.isEmpty() ? "Уважаемый клиент" : customerName,
                                      form.getDeliveryDate(),
                                      form.getCity(),
                                      form.getPaymentMethod()
                                      );

            QMessageBox::information(this, "Успешно", message);
        } else {
            QMessageBox::warning(this, "Ошибка", "Подтвердите условия покупки!");
        }
    }
}
void MainWindow::imageDownloaded(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QPixmap pixmap;
        pixmap.loadFromData(reply->readAll());
        guitarImageLabel->setPixmap(pixmap.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        guitarImageLabel->setText("Изображение недоступно");
        guitarImageLabel->setStyleSheet("color: #666; font-style: italic;");
    }
    reply->deleteLater();
}

void MainWindow::onGuitarSelected(const QModelIndex &index) {
    if (!index.isValid()) return;

    GuitarInfo guitar = index.data(Qt::UserRole).value<GuitarInfo>();
    guitarTitle->setText(guitar.title);
    guitarPrice->setText("Цена: " + guitar.price);
    guitarDescription->setText(guitar.description);
    loadImage(guitar.imageUrl);
}

void MainWindow::loadImage(const QString &url) {
    if (networkManager) {
        networkManager->deleteLater();
    }
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::imageDownloaded);
    QNetworkRequest request(url);
    networkManager->get(request);
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(20);

    guitarListView = new QListView();
    guitarListView->setFixedWidth(200);
    guitarListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setAlignment(Qt::AlignTop);

    guitarImageLabel = new QLabel();
    guitarImageLabel->setAlignment(Qt::AlignCenter);
    guitarImageLabel->setMinimumSize(300, 300);
    guitarImageLabel->setMaximumSize(400, 400);
    guitarImageLabel->setStyleSheet("margin-bottom: 20px;");

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    QWidget *textContainer = new QWidget();
    QVBoxLayout *textLayout = new QVBoxLayout(textContainer);
    textLayout->setAlignment(Qt::AlignCenter);

    guitarTitle = new QLabel();
    guitarTitle->setAlignment(Qt::AlignCenter);
    guitarTitle->setStyleSheet("font-size: 22px; font-weight: bold; margin-bottom: 15px;");

    guitarPrice = new QLabel();
    guitarPrice->setAlignment(Qt::AlignCenter);
    guitarPrice->setStyleSheet("font-size: 18px; color: #d32f2f; margin-bottom: 20px;");

    guitarDescription = new QLabel();
    guitarDescription->setAlignment(Qt::AlignCenter);
    guitarDescription->setWordWrap(true);
    guitarDescription->setStyleSheet("font-size: 16px; text-align: center; margin: 0 30px;");

    textLayout->addWidget(guitarTitle);
    textLayout->addWidget(guitarPrice);
    textLayout->addWidget(guitarDescription);
    scrollArea->setWidget(textContainer);

    orderButton = new QPushButton("Заказать");
    orderButton->setFixedSize(150, 40);
    orderButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #4CAF50;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 4px;"
        "}"
        "QPushButton:hover { background-color: #45a049; }"
        );

    rightLayout->addWidget(guitarImageLabel, 0, Qt::AlignCenter);
    rightLayout->addWidget(scrollArea);
    rightLayout->addWidget(orderButton, 0, Qt::AlignCenter);

    guitarModel = new QStandardItemModel(this);
    QVector<GuitarInfo> guitars = {
        {
            "ESP LTD Alexi 600 Greeny",
            "149 990 ₽",
            "<div style='text-align: center;'>"
            "<b>Характеристики:</b><br>"
            "• Корпус: ясень<br>"
            "• Гриф: клен<br>"
            "• Мензура: 24.75\"<br>"
            "• 22 лада<br>"
            "• Цвет: Greeny"
            "</div>",
            "https://images.musicstore.de/images/1280/esp-ltd-alexi-600-greeny-alexi-laiho-signature_1_GIT0049020-000.jpg"
        },
        {
            "Fender Stratocaster",
            "215 990 ₽",
            "<div style='text-align: center;'>"
            "<b>Характеристики:</b><br>"
            "• Корпус: ольха<br>"
            "• Гриф: клен<br>"
            "• Мензура: 25.5\"<br>"
            "• 22 лада<br>"
            "• Цвет: Olympic White"
            "</div>",
            "https://klavisha.by/components/com_jshopping/files/img_products/full_elektrogitara-fender-standard-stratocaster-rw-arctic-white-tint-1.jpg"
        }
    };

    for (const auto& guitar : guitars) {
        QStandardItem* item = new QStandardItem(guitar.title);
        item->setData(QVariant::fromValue(guitar), Qt::UserRole);
        guitarModel->appendRow(item);
    }

    guitarListView->setModel(guitarModel);

    mainLayout->addWidget(guitarListView);
    mainLayout->addWidget(rightPanel);

    setCentralWidget(centralWidget);
    setWindowTitle("Магазин гитар");
    setMinimumSize(800, 500);

    connect(guitarListView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &MainWindow::onGuitarSelected);
    connect(orderButton, &QPushButton::clicked, this, &MainWindow::showOrderForm);

    guitarListView->setCurrentIndex(guitarModel->index(0, 0));
}
