#pragma once

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QColor>
#include <QMouseEvent>
#include <QTableWidgetItem>
#include <QLCDNumber>

class DrawingTable : public QTableWidget {
    Q_OBJECT
public:
    DrawingTable(QWidget* parent, QLCDNumber* number) : QTableWidget(parent), isDrawing(false), curItem(nullptr), count(number) {
        setSelectionMode(QAbstractItemView::NoSelection);
        setFocusPolicy(Qt::NoFocus);
        setMouseTracking(true);
    }

    void paint (const QPoint& pos) {
        int y = this->rowAt(pos.y());
        int x = this->columnAt(pos.x());
        if (x < 0 || x >= this->columnCount() || y < 0 || y >= this->rowCount()) {
            return;
        }
        curItem = this->item(y, x);
        if (curItem) {
            if (curcolor != curItem->background().color()) {
                if (curcolor == Qt::white) {
                    cnt--;
                }
                if (curItem->background().color() == Qt::white) {
                    cnt++;
                }
            }
            count->display(cnt);
            curItem->setBackground(curcolor);
        }
    }

    void clearColor(const QPoint& pos) {
        int y = this->rowAt(pos.y());
        int x = this->columnAt(pos.x());
        if (x < 0 || x >= this->columnCount() || y < 0 || y >= this->rowCount()) {
            return;
        }
        curItem = this->item(y, x);
        if (curItem) {
            if (curItem->background().color() != Qt::white) {
                cnt--;
            }
            count->display(cnt);
            curItem->setBackground(Qt::white);
        }
    }

    void clearColor(int y, int x) {
        curItem = this->item(y, x);
        if (curItem) {
            if (curItem->background().color() != Qt::white) {
                cnt--;
            }
            count->display(cnt);
            curItem->setBackground(Qt::white);
        }
    }

    void setColor(const QColor& color) {
        curcolor = color;
    }

    QColor getColor() const {
        return curcolor;
    }

    int getCount() {
        return cnt;
    }

protected:
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            isDrawing = true;
            paint(event->pos());
        }
        if (event->button() == Qt::RightButton) {
            isClear = true;
            clearColor(event->pos());
        }
        QTableWidget::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        if (isDrawing) {
            paint(event->pos());
        }
        if (isClear) {
            clearColor(event->pos());
        }
        QTableWidget::mouseMoveEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            isDrawing = false;
        }
        if (event->button() == Qt::RightButton) {
            isClear = false;
        }
        QTableWidget::mouseReleaseEvent(event);
    }

private:
    int cnt = 0;
    bool isDrawing = false;
    bool isClear = false;
    QColor curcolor = Qt::black;
    QTableWidgetItem* curItem;
    QLCDNumber* count;
};
