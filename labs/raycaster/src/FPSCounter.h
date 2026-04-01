#ifndef FPSCOUNTER_H
#define FPSCOUNTER_H

#include <QLabel>
#include <QTimer>
#include <QWidget>

class FPSCounter final : public QLabel {
    Q_OBJECT
   public:
    explicit FPSCounter(QWidget* parent = nullptr);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

   private slots:
    void UpdateFPS();

   private:
    QTimer* timer_{};
    int frames_{};
};

#endif  // FPSCOUNTER_H
