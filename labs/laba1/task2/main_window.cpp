#include "main_window.h"
#include <QApplication>
#include <QCalendarWidget>
#include <QCheckBox>
#include <QCloseEvent>
#include <QColor>
#include <QComboBox>
#include <QDate>
#include <QFile>
#include <QFileDialog>
#include <QFont>
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
#include <QScrollArea>
#include <QSettings>
#include <QShortcut>
#include <QSize>
#include <QSlider>
#include <QSplitter>
#include <QStringList>
#include <QTextCharFormat>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWidget>

class MainWindow : public QWidget {
   public:
    MainWindow() {
        buildUi();
        applyStyle();
        configureCalendarAppearance();
        setupShortcuts();
        connectSignals();
        loadSettings();

        if (stops.isEmpty()) {
            generateRoute();
        }

        updateCaptainHeader();
        updateWeatherBadge();
        updateWaveLabel(waveSlider->value());
        updateCalendarInfo();
        updateRiskStatus();
        recomputeRouteLogic();

        if (routeList->count() > 0 && routeList->currentRow() < 0) {
            ensureVisibleSelection();
        } else {
            updateDetailsPanel();
        }
    }

   protected:
    void closeEvent(QCloseEvent* event) override {
        saveSettings();
        QWidget::closeEvent(event);
    }

   private:
    struct RouteStop {
        QString title;
        QString action;
        QString hint;
        bool visited = false;
        int maxSafeRisk = 60;
        bool forbiddenInStorm = false;
    };

    enum class StopState { Planned, Current, Blocked, Visited };

    QLineEdit* captainEdit = nullptr;
    QLineEdit* searchEdit = nullptr;
    QComboBox* weatherBox = nullptr;
    QSlider* waveSlider = nullptr;
    QPushButton* generateButton = nullptr;
    QPushButton* resetVisitsButton = nullptr;
    QPushButton* exportButton = nullptr;
    QCalendarWidget* calendar = nullptr;
    QListWidget* routeList = nullptr;
    QProgressBar* readinessBar = nullptr;
    QProgressBar* riskBar = nullptr;
    QCheckBox* onlyUnvisitedBox = nullptr;

    QLabel* headerTitleLabel = nullptr;
    QLabel* headerSubtitleLabel = nullptr;
    QLabel* controlCatLabel = nullptr;
    QLabel* weatherBadge = nullptr;
    QLabel* waveValueLabel = nullptr;
    QLabel* moodLabel = nullptr;
    QLabel* summaryLabel = nullptr;
    QLabel* routeBannerLabel = nullptr;
    QLabel* selectedStopTitle = nullptr;
    QTextEdit* selectedStopDescription = nullptr;
    QLabel* shortcutHint = nullptr;
    QLabel* riskLabel = nullptr;
    QLabel* calendarInfoLabel = nullptr;

    QVector<RouteStop> stops;

    #include "main_window_helpers.inl"
    #include "main_window_ui.inl"
    #include "main_window_logic.inl"
    #include "main_window_settings.inl"

};


QWidget* createMainWindow() {
    return new MainWindow();
}
