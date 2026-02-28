#include "ticket.h"
#include "okno.h"

okno::okno(QWidget *parent) : QWidget(parent){
    QPalette palette;
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::ButtonText, Qt::black);
    palette.setColor(QPalette::ToolTipText, Qt::black); 
    palette.setColor(QPalette::PlaceholderText, Qt::black);


    this->setPalette(palette);
    
    this->setStyleSheet("background:rgb(240, 165, 252)");
    mainlayout = new QVBoxLayout();

    topwidget = new QWidget();
    toplayout = new QHBoxLayout();
    topwidget->setLayout(toplayout);
    

    bottomwidget = new QWidget();
    bottomlayout = new QVBoxLayout(bottomwidget);


    topwidget->setStyleSheet("background:rgb(199, 216, 231)");
    //bottomwidget->setStyleSheet("background: #0000D0");
    mainlayout->addWidget(topwidget);
    mainlayout->addWidget(bottomwidget);

    mainlayout->setStretch(0, 1);
    mainlayout->setStretch(1, 5);


    //toplayout
    QSpinBox *but =  new QSpinBox(this);
    but->setMaximum(50);
    but->setMinimum(0);
    connect(but, &QSpinBox::valueChanged, this, &okno::numtickets);
    but->setPalette(palette);

    QPushButton *prev =  new QPushButton(this);
    QPushButton *next =  new QPushButton(this);
    toplayout->addWidget(but, Qt::AlignLeft);
    toplayout->addWidget(prev);
    toplayout->addWidget(next);
    prev->setPalette(palette);
    next->setPalette(palette);
    //prev->setStyleSheet("background-color:rgb(83, 114, 142)");
    //next->setStyleSheet("background-color:rgb(83, 114, 142)");
    prev->setStyleSheet("background:rgb(199, 216, 231)");
    next->setStyleSheet("background:rgb(199, 216, 231)");

    connect(prev, &QPushButton::clicked, this, &okno::prevbutton);
    connect(next, &QPushButton::clicked, this, &okno::nextbutton);
    but->setStyleSheet("background: rgb(253, 235, 133)");
    prev->setText("предыдущий");
    next->setText("следующий");



    //bottomlayout
    list = new QListWidget(this);
    bottomlayout->addWidget(list);
    list->setStyleSheet("background:rgb(253, 235, 133)");
    list->setPalette(palette);
    connect(list, &QListWidget::itemClicked, [&](){tickets[list->currentRow()]->cl(true);});
    connect(list, &QListWidget::itemDoubleClicked, [&](){tickets[list->currentRow()]->dcl();});

    //progressbar
    yellowbar = new QProgressBar;
    mainlayout->addWidget(yellowbar);
    yellowbar->setRange(0, 100);
    yellowbar->setValue(42);
    yellowbar->setPalette(palette);
    greenbar = new QProgressBar;
    mainlayout->addWidget(greenbar);
    greenbar->setRange(0, 100);
    greenbar->setValue(42);
    greenbar->setPalette(palette);

    QPalette palette2 = greenbar->palette();
    palette.setColor(QPalette::Text, Qt::black);
    palette2.setColor(QPalette::Highlight, Qt::yellow);
    yellowbar->setPalette(palette2);
    palette2.setColor(QPalette::Highlight, QColor(0, 200, 0, 255));
    greenbar->setPalette(palette2);
    yellowbar->setStyleSheet("color: black");
    greenbar->setStyleSheet("color: black");

    setLayout(mainlayout);
}

void okno::updateticket(int i, QString nm){
    if (nm == "") nm = tickets[i]->name;
    ticketbuttons[i]->setText(nm);
    if(tickets[i]->status == 0) ticketbuttons[i]->setBackground(QColor(209,200,200));
    if(tickets[i]->status == 1) ticketbuttons[i]->setBackground(Qt::yellow);
    if(tickets[i]->status == 2) ticketbuttons[i]->setBackground(Qt::green);
}

void okno::addbilet(int n){
    if (lasttickets.empty() || lasttickets.back()!=n) lasttickets.push_back(n);
    if (lasttickets.size()>4613) lasttickets.pop_front();
}

void okno::updatebars(){
    if (kolbil==0){
        yellowbar->setRange(0, 100);
        yellowbar->setValue(42);
        greenbar->setRange(0, 100);
        greenbar->setValue(42);
        return;
    }
    int kolyg=0;
    int kolg=0;
    for (auto u : tickets){
        if (u->status>=1) kolyg++;
        if (u->status==2) kolg++;
    }
    yellowbar->setRange(0, kolbil);
    yellowbar->setValue(kolyg);
    greenbar->setRange(0, kolbil);
    greenbar->setValue(kolg);
}

void okno::closeEvent(QCloseEvent *event){
    for (ticket* t : tickets) {
        t->close();
        t->deleteLater();
    }
    QWidget::closeEvent(event);
}

void okno::prevbutton(){
    if (lasttickets.size() == 0) return;
    list->setCurrentRow(lasttickets.back());
    if (lasttickets.size()>=2) lasttickets.pop_back();
    tickets[list->currentRow()]->cl(false);
}

void okno::numtickets(int kol){
    if (kolbil == kol) return;
    kolbil = kol;
    for (ticket* t : tickets) {
        t->close();
        t->deleteLater();
    }
    lasttickets.clear();
    tickets.clear();
    ticketbuttons.clear();
    list->clear();
    for (int i=0;i<kol;i++){
        tickets.push_back(new ticket(i+1, this, nullptr));
        ticketbuttons.push_back(new QListWidgetItem(list));
        ticketbuttons[i]->setForeground(Qt::black);
        updateticket(i);
    }
    updatebars();
    setLayout(mainlayout);
}

void okno::nextbutton(){
    int rnd;
    bool flag=1;
    for (auto u:tickets) flag&=(u->status==2);
    if (flag) return;
    do {
        rnd = QRandomGenerator::global()->bounded(kolbil);
    } while (tickets[rnd]->status == 2);
    list->setCurrentRow(rnd);
    tickets[list->currentRow()]->cl(true);
}
