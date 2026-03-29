#pragma once

#include "GoodDelegate.h"
#include "Dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QString>
#include <vector>

const std::vector<std::vector<QString> > pictures =
    {
    {
     "2 вправо",
     "1 вверх",
     "1 вправо",
     "1 вверх",
     "1 вправо",
     "1 вверх",
     "1 вправо",
     "1 вверх",
     "1 вправо",
     "2 вверх",
     "1 вправо",
     "2 вверх",
     "1 вправо",
     "1 вверх",
     "1 влево",
     "1 вверх",
     "1 влево",
     "1 вверх",
     "2 влево",
     "1 вниз",
     "1 влево",
     "1 вниз",
     "2 влево",
     "1 вверх",
     "1 влево",
     "1 вверх",
     "2 влево",
     "1 вниз",
     "1 влево",
     "1 вниз",
     "1 влево",
     "1 вниз",
     "1 влево",
     "1 вниз",
     "1 вправо",
     "2 вниз",
     "1 вправо",
     "2 вниз",
     "1 вправо",
     "1 вниз",
     "1 вправо",
     "1 вниз",
     "1 вправо",
     "1 вниз",
     "1 вправо"
    },
    {
        "4 вверх",
        "3 вправо"
    }
};

const std::vector<QString> names  = {
    "Сердце",
    "Буква Г"
};

class PictureView : public QWidget {
    Q_OBJECT
public:
    PictureView(QWidget* parent = nullptr) : QWidget(parent) {
        layout = new QVBoxLayout(this);

        list = new QListWidget(this);
        list->setItemDelegate(new GoodDelegate(this));

        QScrollArea* scrollArea = new QScrollArea(this);
        scrollArea->setWidgetResizable(true);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

        for (const auto& u : names) {
            item = new QListWidgetItem();
            item->setText(u);
            item->setFont(QFont("arial", 12));
            item->setBackground(QColor(80, 211, 7));
            list->addItem(item);
        }

        scrollArea->setWidget(list);

        QPushButton* showDialogButton = new QPushButton("Показать детали", this);
        connect(showDialogButton, &QPushButton::clicked, this, &PictureView::showDialog);

        layout->addWidget(scrollArea);
        layout->addWidget(showDialogButton);
        setLayout(layout);
    }

private Q_SLOTS:
    void showDialog() {
        QListWidgetItem* selectedItem = list->currentItem();
        if (selectedItem) {
            int index = list->row(selectedItem);
            if (index >= 0 && index < pictures.size()) {
                Dialog* dialog = new Dialog(pictures[index], this);
                dialog->show();
            }
        }
    }

private:
    QVBoxLayout* layout;
    QListWidget* list;
    QListWidgetItem* item;
};
