#include "hdrs/RanobeListItem.h"

#include <utility>

RanobeListItem::RanobeListItem(QWidget* parent) : QWidget(parent) {
}

RanobeListItem::RanobeListItem(
    const QString& titleName, const QString& ruTitleName, QIcon icon, QWidget* parent)
    : QWidget(parent), icon_(std::move(icon)), titleName_(titleName) {
    layout_ = new QVBoxLayout;
    layout_->setSizeConstraint(QLayout::SetFixedSize);

    iconLabel_ = new QLabel;
    iconLabel_->setAlignment(Qt::AlignCenter);
    layout_->addWidget(iconLabel_);

    titleLabel_ = new QLabel(ruTitleName);
    titleLabel_->setAlignment(Qt::AlignCenter);
    titleLabel_->setWordWrap(true);
    layout_->addWidget(titleLabel_);

    setLayout(layout_);

    setIconSize(QSize(100, 90));
}

void RanobeListItem::setIconSize(const QSize& size) {
    iconLabel_->setPixmap(icon_.pixmap(size));
    titleLabel_->setFixedWidth(size.width());
    setFixedSize(size.width() + 20, size.height() + 60);
}

const QString& RanobeListItem::getTitleName() const {
    return titleName_;
}