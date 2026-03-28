#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QOverload>
#include <QSpinBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), count(new QSpinBox(this)), list(new TicketList(this)), questionView(new InfoBox(this))
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->addWidget(count);
    layout->addWidget(list);
    layout->addWidget(questionView);
    count->setMinimum(1);
    list->updateTickets(1);

    button = new QPushButton("Случайный билет", this);
    layout->addWidget(button);

    buttonprev = new QPushButton("Предыдущий билет", this);
    layout->addWidget(buttonprev);

    QLabel *totalprogresslabel = new QLabel("Общий прогресс");
    totalprogress = new QProgressBar(this);
    totalprogress->setFormat("Общий прогресс: %p%");
    totalprogress->setRange(0, 100);
    layout->addWidget(totalprogresslabel);
    layout->addWidget(totalprogress);

    QLabel *progresslabel = new QLabel("Прогресс");
    progress = new QProgressBar(this);
    totalprogress->setFormat("Прогресс: %p%");
    totalprogress->setRange(0, 100);
    layout->addWidget(progresslabel);
    layout->addWidget(progress);

    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    connect(count, QOverload<int>::of(&QSpinBox::valueChanged), list, &TicketList::updateTickets);
    connect(list, &TicketList::ticketSelected, this, &MainWindow::updateInfoBox);
    connect(button, &QPushButton::clicked, list, &TicketList::randomTick);
    connect(buttonprev, &QPushButton::clicked, list, &TicketList::previousQues);
    connect(list, &TicketList::progressUpdated, this, &MainWindow::updateProgress);

}

MainWindow::~MainWindow() {}

void MainWindow::updateInfoBox(TicketItem *ticket) {
    questionView->setTicket(ticket);
}


void MainWindow::updateProgress() {
    int totalCount = list->count();
    if (totalCount == 0) return;

    int reviewedCount = 0;
    int needReviewCount = 0;

    for (int i = 0; i < totalCount; ++i) {
        TicketItem *ticket = dynamic_cast<TicketItem *>(list->item(i));
        if (!ticket) continue;

        if (ticket->getStatus() == TicketItem::Reviewed) {
            reviewedCount++;
        } else if (ticket->getStatus() == TicketItem::NeedReview) {
            needReviewCount++;
        }
    }

    int totalProgressValue = (reviewedCount + needReviewCount) * 100 / totalCount;
    int greenProgressValue = reviewedCount * 100 / totalCount;

    totalprogress->setValue(totalProgressValue);
    progress->setValue(greenProgressValue);
}
