#pragma once

#include <QLabel>

#include <functional>

class ClickLabel final : public QLabel {
 public:
  using Callback = std::function<void()>;

  explicit ClickLabel(const QString& text, Callback callback, QWidget* parent = nullptr);

 protected:
  void mousePressEvent(QMouseEvent* event) override;

 private:
  Callback callback_;
};
