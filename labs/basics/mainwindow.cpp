#include "mainwindow.h"
#include "countwidget.h"
#include "viewwidget.h"
void MainWindow::updateItemAppearance(QListWidgetItem* item) {
    if (!item) return;

    Ticket* ticket = item->data(Qt::UserRole).value<Ticket*>();
    item->setText(ticket->name);

    QColor color;
    switch (ticket->status) {
    case Ticket::Default: color = Qt::gray; break;
    case Ticket::Yellow: color = Qt::yellow; break;
    case Ticket::Green: color = Qt::green; break;
    }
    item->setBackground(color);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    auto* central = new QWidget(this);  // NOLINT
    setCentralWidget(central);
    CountWidget *countWidget = new CountWidget;
    ViewWidget *viewWidget = new ViewWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(countWidget);
    mainLayout->addWidget(viewWidget);
    connect(countWidget, &CountWidget::ticketCountChanged,
                             viewWidget, &ViewWidget::setItemCount);
    central->setLayout(mainLayout);
}

