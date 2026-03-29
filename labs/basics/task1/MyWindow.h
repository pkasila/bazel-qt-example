#pragma once

#include "QuestionView.h"
#include "Ticket.h"
#include "GoodDelegate.h"
#include <QWidget>
#include <QTimer>
#include <QSpinBox>
#include <QPushButton>
#include <QProgressBar>
#include <future>
#include <QCloseEvent>

class MyWindow : public QWidget {
    Q_OBJECT
public:
    MyWindow(QWidget* parent = nullptr) : QWidget(parent) {
        box = new QSpinBox;
        list = new QListWidget(this);
        dummy = new QWidget;
        nextButton = new QPushButton("Следующий вопрос", this);
        prevButton = new QPushButton("Предыдущий вопрос", this);
        manager = new QuestionManager(0);
        totalProgress = new QProgressBar(this);
        greenProgress = new QProgressBar(this);
        totalProgress->setStyleSheet(
            "QProgressBar {"
            "   border: 2px solid grey;"
            "   border-radius: 3px;"
            "   text-align: center;"
            "}"
            "QProgressBar::chunk {"
            "   background-color: rgb(5, 154, 189);"
            "   width: 10px;"
            "}"
            );
        greenProgress->setStyleSheet(
            "QProgressBar {"
            "   border: 2px solid grey;"
            "   border-radius: 3px;"
            "   text-align: center;"
            "}"
            "QProgressBar::chunk {"
            "   background-color: rgb(34, 216, 32);"
            "   width: 10px;"
            "}"
            );
        box->setRange(1,100);
        box->setSingleStep(1);
        box->setToolTip("Выбери число билетов");

        connect(box, QOverload<int>::of(&QSpinBox::valueChanged),this, &MyWindow::update);
        timer = new QTimer(this);
        timer->setSingleShot(true);
        connect(list, &QListWidget::itemDoubleClicked, this, &MyWindow::double_click);
        connect(list, &QListWidget::itemClicked, this, &MyWindow::one_click);
        connect(timer, &QTimer::timeout, this, &MyWindow::click);
        connect(nextButton, &QPushButton::clicked, this, &MyWindow::nextQuestion);
        connect(prevButton, &QPushButton::clicked, this, &MyWindow::prevQuestion);

        layout = new QVBoxLayout(this);
        upLayout = new QHBoxLayout();
        upLayout->addWidget(box);
        upLayout->addWidget(dummy);
        upLayout->addWidget(prevButton);
        upLayout->addWidget(nextButton);
        upLayout->setStretch(0, 4);
        upLayout->setStretch(1, 1);
        upLayout->setStretch(2, 4);
        upLayout->setStretch(3, 4);
        layout->addLayout(upLayout);
        layout->addWidget(totalProgress);
        layout->addWidget(greenProgress);
        layout->addWidget(list);
        setLayout(layout);

        update(1);
    }

private Q_SLOTS:
    void update(int ticketCount) {
        manager->resize(ticketCount);
        *tv = totalValue = 0;
        *gv = greenValue = 0;
        //qDebug() << totalValue << " " << greenValue;
        totalProgress->setMaximum(2*ticketCount);
        totalProgress->setValue(totalValue);
        totalProgress->setTextVisible(true);
        totalProgress->setFormat("%p%");
        greenProgress->setMaximum(ticketCount);
        greenProgress->setValue(greenValue);
        greenProgress->setTextVisible(true);
        greenProgress->setFormat("%p%");
        list->clear();
        list->setItemDelegate(new GoodDelegate(this));
        for(int i=0;i<ticketCount;i++) {
            Ticket* item = new Ticket(QString("Билет № %1").arg(i + 1), list);
            item->setNumber(i + 1);
            item->setName(QString("Билет № %1").arg(i + 1));
        }
    }

    void one_click(QListWidgetItem* item) {
        clickeditem = item;
        timer->start(100);
    }

    void double_click(QListWidgetItem* item) {
        clickeditem = nullptr;
        timer->stop();
        Ticket* ticket = dynamic_cast<Ticket*>(item);
        if(!ticket) {
            return;
        }
        if(ticket->getColor() == grey || ticket->getColor() == yellow) {
            ticket->setColor(green, manager, *tv, *gv);
            totalValue = *tv;
            greenValue = *gv;
            totalProgress->setValue(totalValue);
            greenProgress->setValue(greenValue);
        } else {
            ticket->setColor(yellow, manager, *tv, *gv);
            totalValue = *tv;
            greenValue = *gv;
            totalProgress->setValue(totalValue);
            greenProgress->setValue(greenValue);
        }
    }

    void click() {
        if(clickeditem) {
            Ticket* ticket = dynamic_cast<Ticket*>(clickeditem);
            if(!ticket) {
                return;
            }
            QuestionView* question = ticket->getQuestionView();
            QuestionManager* manager2 = manager;
            connect(question, &QuestionView::nameUpdated, this, [ticket](const QString& name){ticket->setName(name);});
            connect(question, &QuestionView::statusUpdated, this, [ticket, manager2, this](const QColor& color)
                    {
                    ticket->setColor(color, manager2, *tv, *gv);
                    totalValue = *tv;
                    greenValue = *gv;
                    //qDebug() << totalValue << " " << greenValue;
                    this->totalProgress->setValue(totalValue);
                    this->greenProgress->setValue(greenValue);
                    }
                    );
            question->show();
        }
    }

    void nextQuestion() {
        int pos = manager->nextQuestion();
        if (pos == -1) {
            return;
        }
        Ticket* ticket = dynamic_cast<Ticket*>(list->item(pos));
        list->setCurrentRow(pos);
        if (ticket == nullptr) {
            return;
        }
        QuestionView* question = ticket->getQuestionView();
        QuestionManager* manager2 = manager;
        connect(question, &QuestionView::nameUpdated, this, [ticket](const QString& name){ticket->setName(name);});
        connect(question, &QuestionView::statusUpdated, this, [ticket, manager2, this](const QColor& color)
                {
                    ticket->setColor(color, manager2, *tv, *gv);
                    totalValue = *tv;
                    greenValue = *gv;
                    this->totalProgress->setValue(totalValue);
                    this->greenProgress->setValue(greenValue);
                }
                );
        question->show();
    }

    void prevQuestion() {
        int pos = manager->prevQuestion();
        if (pos == -1) {
            return;
        }
        Ticket* ticket = dynamic_cast<Ticket*>(list->item(pos));
        list->setCurrentRow(pos);
        if (ticket == nullptr) {
            return;
        }
        QuestionView* question = ticket->getQuestionView();
        QuestionManager* manager2 = manager;
        connect(question, &QuestionView::nameUpdated, this, [ticket](const QString& name){ticket->setName(name);});
        connect(question, &QuestionView::statusUpdated, this, [ticket, manager2, this](const QColor& color)
                {
                    ticket->setColor(color, manager2, *tv, *gv);
                    totalValue = *tv;
                    greenValue = *gv;
                    this->totalProgress->setValue(totalValue);
                    this->greenProgress->setValue(greenValue);
                }
                );
        question->show();
    }

protected:
    void closeEvent(QCloseEvent *event) override {
        for (int i = 0; i < list->count(); ++i) {
            if (auto ticket = dynamic_cast<Ticket*>(list->item(i))) {
                if (auto qv = ticket->getQuestionView()) {
                    qv->close();
                }
            }
        }
        event->accept();
    }

private:
    QVBoxLayout* layout;
    QHBoxLayout* upLayout;
    QSpinBox* box;
    QWidget* dummy;
    QPushButton* prevButton;
    QPushButton* nextButton;
    QTimer* timer;
    QListWidget* list;
    QListWidgetItem* clickeditem = nullptr;
    QuestionManager* manager;
    QProgressBar* totalProgress;
    QProgressBar* greenProgress;
    int totalValue = 0;
    int greenValue = 0;
    std::shared_ptr<int> tv = std::make_shared<int>(totalValue);
    std::shared_ptr<int> gv = std::make_shared<int>(greenValue);
};
