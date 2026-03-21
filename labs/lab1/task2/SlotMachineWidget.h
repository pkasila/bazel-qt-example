#ifndef SLOTMACHINEWIDGET_H_
#define SLOTMACHINEWIDGET_H_

#include <QLCDNumber>  // NOLINT
#include <QTimer>
#include <QWidget>
#include <vector>

class SlotMachineWidget
    : public QWidget {  // NOLINT(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
    Q_OBJECT

   public:
    explicit SlotMachineWidget(QWidget* parent = nullptr);

    void StartSpinning();
    [[nodiscard]] bool IsSpinning() const;
    [[nodiscard]] std::vector<int> GetResults() const;

   signals:
    void SpinningStopped();

   private slots:
    void OnTimerTick();

   private:  // NOLINT(readability-redundant-access-specifiers)
    void SetupUi();

    QLCDNumber* reel1_{nullptr};
    QLCDNumber* reel2_{nullptr};
    QLCDNumber* reel3_{nullptr};
    QTimer* spin_timer_{nullptr};
    int ticks_remaining_{0};
    bool is_spinning_{false};
};

#endif  // SLOTMACHINEWIDGET_H_
