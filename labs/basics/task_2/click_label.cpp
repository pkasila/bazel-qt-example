#include "click_label.h"

#include <QMouseEvent>

ClickLabel::ClickLabel(const QString& text, Callback callback, QWidget* parent)
    : QLabel(text, parent), callback_(std::move(callback)) {
  setCursor(Qt::PointingHandCursor);
  setAlignment(Qt::AlignCenter);
}

void ClickLabel::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton && callback_) {
    callback_();
    event->accept();
    return;
  }
  QLabel::mousePressEvent(event);
}
