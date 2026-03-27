#include "StatusComboBox.h"

void ColorDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &index) const {
    QColor color = index.data(Qt::UserRole).value<QColor>();  //Getting color from UserRole
    painter->fillRect(option.rect, color);  //Filling item background
}

StatusComboBox::StatusComboBox(QWidget *parent) :
    QComboBox(parent) {
    for(int i = 0; i < ColorMaps::statusColors.count(); ++i) {
        addItem(WindowTexts::statusComboBoxText);
        setItemData(i, QColor(ColorMaps::statusColors[i]), Qt::UserRole);
    }

    setItemDelegate(new ColorDelegate(this));

    //Changing ComboBox header color after new status is chosen
    connect(this, &QComboBox::currentIndexChanged, this,
            &StatusComboBox::updateComboBoxColorWithSignal);
}

void StatusComboBox::updateComboBoxColor(int colorIndex) {
    setStyleSheet(Styles::statusComboBoxStyle.arg(ColorMaps::statusColors[colorIndex]));
    setAllItemsVisible();
    setItemHidden(colorIndex);
}

void StatusComboBox::updateComboBoxColorWithSignal(int colorIndex) {
    emit statusIsChanged(colorIndex);
    updateComboBoxColor(colorIndex);
}

void StatusComboBox::setAllItemsVisible() {
    for (int i = 0; i < count(); ++i) {
        qobject_cast<QListView*>(view())->setRowHidden(i, false);
    }
}

void StatusComboBox::setItemHidden(int index) {
    if (0 <= index && index < count()) {
        qobject_cast<QListView*>(view())->setRowHidden(index, true);
    }
}
