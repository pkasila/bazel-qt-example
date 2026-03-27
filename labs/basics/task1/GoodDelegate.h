#pragma once

#include <QApplication>
#include <QStyledItemDelegate>
#include <QPainter>

class GoodDelegate : public QStyledItemDelegate {
public:
    GoodDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyleOptionViewItem opt = option;
        opt.rect.adjust(3, 3, -3, -3);
        QColor borderColor = index.data(Qt::ForegroundRole).value<QColor>();
        int borderWidth = index.data(Qt::UserRole + 1).toInt();
        QColor backgroundColor = index.data(Qt::BackgroundRole).value<QColor>();

        initStyleOption(&opt, index);
        if (opt.state & QStyle::State_Selected) {
            QColor highlightColor = backgroundColor;
            highlightColor.setAlpha(150);
            painter->fillRect(opt.rect, highlightColor);
        } else {
            QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter, nullptr);
        }

        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(QPen(borderColor, borderWidth));
        painter->drawRoundedRect(opt.rect, 3, 3);
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        return size + QSize(0, 10);
    }
};
