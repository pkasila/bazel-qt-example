#include "main_window.h"
#include <QApplication>
#include <QCloseEvent>
#include <QColor>
#include <QComboBox>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPixmap>
#include <QProgressBar>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSettings>
#include <QSignalBlocker>
#include <QSize>
#include <QSpinBox>
#include <QSplitter>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>
#include <QtGlobal>

class MainWindow : public QWidget {
   public:
    MainWindow() {
        buildUi();
        applyStyle();
        loadSettings();
        connectSignals();

        if (tickets.isEmpty()) {
            resetTickets(countSpin->value());
        } else {
            updateProgressBars();
            updateQuestionView();
        }
    }

   protected:
    void closeEvent(QCloseEvent* event) override {
        saveSettings();
        QWidget::closeEvent(event);
    }

   private:
    enum class TicketStatus { Default = 0, Yellow = 1, Green = 2 };

    struct Ticket {
        QString name;
        TicketStatus status = TicketStatus::Default;
    };

    QSpinBox* countSpin = nullptr;
    QListWidget* view = nullptr;

    QLabel* numberValue = nullptr;
    QLabel* nameValue = nullptr;
    QLabel* statusBadge = nullptr;
    QLineEdit* nameEdit = nullptr;
    QComboBox* statusCombo = nullptr;
    QPushButton* nextButton = nullptr;
    QPushButton* previousButton = nullptr;

    QProgressBar* totalProgress = nullptr;
    QProgressBar* greenProgress = nullptr;

    QVector<Ticket> tickets;
    QVector<int> history;
    int historyPos = -1;
    int currentIndex = -1;

    #include "main_window_ui.inl"
    #include "main_window_logic.inl"
    #include "main_window_settings.inl"

};


QWidget* createMainWindow() {
    return new MainWindow();
}
