#ifndef TICKETCARDWIDGET_H_
#define TICKETCARDWIDGET_H_

#include "Ticket.h"

#include <QLabel>
#include <QPushButton>
#include <QWidget>

class TicketCardWidget  // NOLINT(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
    : public QWidget {
    Q_OBJECT
   public:
    explicit TicketCardWidget(const Ticket& ticket, bool is_selected, QWidget* parent = nullptr);

    void UpdateStatus(TicketStatus status);
    void UpdateSelection(bool selected);
    void UpdateName(const QString& name);

   signals:
    void ResetRequested(int id);

   private slots:
    void OnResetClicked();

   protected:
    void paintEvent(QPaintEvent* event) override;  // NOLINT(readability-identifier-naming)

   private:
    int id_{0};
    QString name_{};
    TicketStatus status_{TicketStatus::Default};
    bool selected_{false};

    QLabel* id_label_{nullptr};
    QLabel* name_label_{nullptr};
    QPushButton* reset_button_{nullptr};
};

#endif  // TICKETCARDWIDGET_H_
