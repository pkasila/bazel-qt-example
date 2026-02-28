#include "ticket.h"
#include "okno.h"

ticket::ticket(int num, okno *m, QWidget *parent)
    : QWidget(parent){
    mama = m;
    number = num;
    status = 0;
    name = "Билет " + QString::number(number);
}
void ticket::cl(bool flag){
    QTimer::singleShot(300, [=]() {
        oneclick(flag);
    });
}
void ticket::oneclick(bool flag){
    if (doubleClickFlag) {doubleClickFlag = false; return;}
    if (opened) return;
    opened = true;
    if (flag) mama->addbilet(number - 1);
    QGroupBox *questionview = new QGroupBox(this);
    questionview->setWindowFlags(Qt::Window);
    questionview->setWindowTitle(name);
    questionview->resize(200, 100);

    QVBoxLayout *layout = new QVBoxLayout;
    questionview->setLayout(layout);

    QLabel *numberlabel = new QLabel(questionview);
    numberlabel->setNum(number);
    numberlabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(numberlabel, Qt::AlignCenter);


    QLabel *namelabel = new QLabel(questionview);
    namelabel->setText("Название: " + name);
    namelabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(namelabel, Qt::AlignCenter);

    QLineEdit *nameedit = new QLineEdit(questionview);

    nameedit->setPlaceholderText(name);
    connect(nameedit, &QLineEdit::returnPressed, this, [=](){
        if (nameedit->text()=="") return;
        name =  nameedit->text();
        namelabel->setText("Название: " + name);
        nameedit->setPlaceholderText(name);
        questionview->setWindowTitle(name);
        mama->updateticket(number - 1, name);
    });

    layout->addWidget(nameedit, Qt::AlignCenter);

    QComboBox *statusbox = new QComboBox;
    statusbox->addItems({"зелёный","жёлтый","дефолтный"});
    statusbox->setCurrentIndex(2-status);
    connect(statusbox, &QComboBox::currentIndexChanged, this, [&](int index) {
        std::array<int,3> m={2,1,0};
        status = m[index];
        mama->updateticket(number - 1, name);
        mama->updatebars();
    });
    layout->addWidget(statusbox, Qt::AlignCenter);

    questionview->setAttribute(Qt::WA_DeleteOnClose);
    connect(questionview, &QGroupBox::destroyed, this, [&]() {
        opened = false;
    });

    connect(mama, &QWidget::destroyed, questionview, &QWidget::deleteLater);

    questionview->show();
};

void ticket::dcl(){
    doubleClickFlag = 1;
    if (status != 2) status = 2; else status = 1;
    mama->updateticket(number - 1);
    mama->updatebars();
}

