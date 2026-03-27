#ifndef STATUSCOMBOBOX_H
#define STATUSCOMBOBOX_H
#include <QWidget>
#include <QComboBox>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QListView>
//
#include "ProjectConstants.h"

//Delegate class for ComboBox items styling
class ColorDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
};

class StatusComboBox : public QComboBox {
    Q_OBJECT

public:
    explicit StatusComboBox(QWidget *parent = nullptr);

    void updateComboBoxColor(int colorIndex);
    void updateComboBoxColorWithSignal(int colorIndex);

signals:
    void statusIsChanged(int newStatus);

private:
    void setAllItemsVisible();
    void setItemHidden(int index);
};
#endif // STATUSCOMBOBOX_H
