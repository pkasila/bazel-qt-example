#ifndef STATISTICSVIEWWIDGET_H
#define STATISTICSVIEWWIDGET_H

#include <QWidget>

class QTextBrowser;
class QPushButton;
class QTableWidget; // Added for table view
class StatisticsRepository;

class StatisticsViewWidget : public QWidget {
    Q_OBJECT
public:
    explicit StatisticsViewWidget(QWidget *parent = nullptr);
    void refreshData();

private:
    void loadAndDisplayStats();
    QString mapDifficultyIdToName(int id); // Helper

    QTextBrowser *m_statsSummaryDisplay; // For overall stats
    QTableWidget *m_sessionTable;      // For session history
    QPushButton *m_refreshButton;
    StatisticsRepository* m_statsRepo;
};

#endif // STATISTICSVIEWWIDGET_H