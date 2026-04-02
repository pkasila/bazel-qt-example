#include "mainwindow.h"
#include "ticketdialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QString>
#include <QDebug>
#include <QColor>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent) {
    srand(time(0));

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setStyleSheet("background-color:#352e4a");

    QHBoxLayout *topLayout = new QHBoxLayout;
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    progBarStarted_ = new QProgressBar(this);
    progBarStarted_->setSizePolicy(QSizePolicy::Preferred,
                                   QSizePolicy::Fixed);
    progBarStarted_->setStyleSheet(
        "QProgressBar {"
        "   border: 2px solid grey;"
        "   border-radius: 5px;"
        "   background-color: #f0f0f0;"
        "   text-align: center;"
        "}"
        "QProgressBar::chunk {"
        "   background-color: #F8E45C;"
        "   width: 10px;"
        "}"
        );

    progBarTotal_ = new QProgressBar(this);
    progBarTotal_->setSizePolicy(QSizePolicy::Preferred,
                                 QSizePolicy::Fixed);
    progBarTotal_->setStyleSheet(
        "QProgressBar {"
        "   border: 2px solid grey;"
        "   border-radius: 5px;"
        "   background-color: #f0f0f0;"
        "   text-align: center;"
        "}"
        "QProgressBar::chunk {"
        "   background-color: #33D179;"
        "   width: 10px;"
        "}"
        );

    spinBoxTicketsNum_ = new QSpinBox(this);
    spinBoxTicketsNum_->setRange(0, 100);
    spinBoxTicketsNum_ ->setSizePolicy(QSizePolicy::Fixed,
                                       QSizePolicy::Fixed);
    spinBoxTicketsNum_->setStyleSheet("color : white;");

    topLayout->addStretch(1);
    topLayout->addWidget(progBarStarted_);
    topLayout->addWidget(progBarTotal_);
    topLayout->addWidget(spinBoxTicketsNum_);

    listWidget_ = new QListWidget(this);
    listWidget_->setSizePolicy(QSizePolicy::Expanding,
                               QSizePolicy::Expanding);
    listWidget_->setSpacing(1);
    listWidget_->setStyleSheet(
        "QListWidget {"
        "   background-color: #241f31;"
        "   border: 2px solid grey;"
        "   border-radius: 5px;"
        "}"
        );


    buttonNext_ = new QPushButton(this);
    buttonNext_->setText("Next question");
    buttonNext_->setSizePolicy(QSizePolicy::Expanding,
                               QSizePolicy::Preferred);
    buttonNext_->setMinimumHeight(40);
    buttonNext_->setStyleSheet(
        "QPushButton {"
        "   background-color: #433a5c;"
        "   border: 2px solid grey;"
        "   border-radius: 5px;"
        "   color: #deddda;"
        "}"
        );

    buttonPrev_ = new QPushButton(this);
    buttonPrev_->setEnabled(false);
    buttonPrev_->setText("Previous question");
    buttonPrev_->setSizePolicy(QSizePolicy::Expanding,
                               QSizePolicy::Preferred);
    buttonPrev_->setStyleSheet(
        "QPushButton {"
        "   background-color: #433a5c;"
        "   border: 2px solid grey;"
        "   border-radius: 5px;"
        "   color: #deddda;"
        "}"
        );

    bottomLayout->addWidget(buttonPrev_);
    bottomLayout->addWidget(buttonNext_);


    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(listWidget_);
    mainLayout->addLayout(bottomLayout);
    centralWidget->setLayout(mainLayout);


    connect(spinBoxTicketsNum_, &QSpinBox::valueChanged,
            this, &MainWindow::RebuildList);

    connect(buttonNext_, &QPushButton::clicked,
            this, &MainWindow::NextTicket);

    connect(buttonPrev_, &QPushButton::clicked,
            this, &MainWindow::PreviousTicket);

    connect(listWidget_, &QListWidget::itemDoubleClicked,
            this, &MainWindow::OnItemDoubleClicked);

    connect(listWidget_, &QListWidget::itemClicked,
            this, &MainWindow::OnItemClicked);

    singleClickTimer_ = new QTimer(this);
    singleClickTimer_->setSingleShot(true);
    singleClickTimer_->setInterval(200);

    connect(singleClickTimer_, &QTimer::timeout,
            this, &MainWindow::OpenDialog);
}


void MainWindow::RebuildList() {
    cnt_ = spinBoxTicketsNum_->value();
    cntTotal_ = 0;
    cntStarted_ = 0;
    currentTicket_ = -1;
    status_.resize(cnt_);
    status_.assign(cnt_, 0);
    ticketsHistory_.clear();
    listWidget_->clear();
    progBarStarted_->setValue(0);
    progBarTotal_->setValue(0);
    buttonPrev_->setEnabled(false);

    QColor clDefault(40, 62, 86);
    QColor clWhite(222, 221, 218);
    for (size_t i = 1; i <= cnt_; i++) {
        QString name = "Question №"
            + QString::fromStdString(std::to_string(i));
        listWidget_->addItem(name);

        listWidget_->item(i-1)->setForeground(clWhite);
        listWidget_->item(i-1)->setBackground(clDefault);

        }
    }


void MainWindow::ChooseRandomTicket() {
    size_t cntNotLearned = cnt_ - cntTotal_;
    if (cntNotLearned == 0){
        return;
    }
    int index = rand() % cntNotLearned + 1;
    for (size_t i = 0; i < cnt_; i++) {
        if (status_[i] != 2) {
            index--;
            if (index == 0) {
                qDebug()<<"stoped on "<<i<<'\n';
                ticketsHistory_.push_back(i);
                currentTicket_++;
                break;
            }
        }
    }
    if (currentTicket_ > 0) {
        buttonPrev_->setEnabled(true);
    }
    listWidget_->setCurrentRow(ticketsHistory_[currentTicket_]);
    lastClikedItem_ = listWidget_->currentItem();
    emit OpenDialog();
}


void MainWindow::ChangeStatus(QListWidgetItem *item) {
    int index = listWidget_->row(item);
    if (status_[index] == 0)
        cntStarted_++;
    else if(status_[index] == 1){
        cntTotal_++;
    } else {
        cntTotal_--;
    }
    progBarStarted_->setValue(static_cast<int>(100.0*cntStarted_/cnt_));
    progBarTotal_->setValue(static_cast<int>(100.0*cntTotal_/cnt_));

    status_[index] = status_[index] == 1 ? 2 : 1;
    QColor clGreen(51, 209, 122);
    QColor clYellow(248, 228, 92);
    QColor clDefault(40, 62, 86);
    QColor clBlack(0, 0, 0);
    item->setBackground(status_[index] == 1 ? clYellow : clGreen);
    item->setForeground(clBlack);
}

void MainWindow::OnItemClicked(QListWidgetItem *item) {
    lastClikedItem_ = item;
    singleClickTimer_->start();
}

void MainWindow::OnItemDoubleClicked(QListWidgetItem *item) {
    singleClickTimer_->stop();
    ChangeStatus(item);
}

void MainWindow::OpenDialog() {
    if(lastClikedItem_ == nullptr) {
        return;
    }
    TicketDialog dialog(this, lastClikedItem_);
    dialog.setMaximumHeight(720);
    dialog.setMaximumWidth(1080);
    dialog.setMinimumHeight(100);
    dialog.setMinimumWidth(200);

    dialog.exec();
}

void MainWindow::NextTicket() {
    if (currentTicket_ == ticketsHistory_.size()-1) {
        emit ChooseRandomTicket();
    } else {
        currentTicket_++;
        if (currentTicket_ > 0) {
            buttonPrev_->setEnabled(true);
        }
        listWidget_->setCurrentRow(ticketsHistory_[currentTicket_]);
        lastClikedItem_ = listWidget_->item(ticketsHistory_[currentTicket_]);
        emit OpenDialog();
    }
}

void MainWindow::PreviousTicket() {
    if (currentTicket_ == 0) {
        return;
    } else {
        currentTicket_--;
        if (currentTicket_ == 0) {
            buttonPrev_->setEnabled(false);
        }
        listWidget_->setCurrentRow(ticketsHistory_[currentTicket_]);
        lastClikedItem_ = listWidget_->currentItem();
        emit OpenDialog();
    }
}


MainWindow::~MainWindow()
{
}
