#include "StatisticsViewWidget.h"
#include "db/DatabaseManager.h"
#include "db/StatisticsRepository.h"
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QDateTime> // For date formatting
#include <QLabel>    // For title

StatisticsViewWidget::StatisticsViewWidget(QWidget *parent) : QWidget(parent) {
    m_statsRepo = new StatisticsRepository(DatabaseManager::instance());

    QVBoxLayout *layout = new QVBoxLayout(this);
    
    QLabel* titleLabel = new QLabel(tr("User Statistics"), this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(titleFont.pointSize() + 4);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    m_refreshButton = new QPushButton(tr("Refresh Statistics"), this);
    connect(m_refreshButton, &QPushButton::clicked, this, &StatisticsViewWidget::loadAndDisplayStats);
    layout->addWidget(m_refreshButton, 0, Qt::AlignRight);

    // Summary display (optional, can be removed if table is enough)
    m_statsSummaryDisplay = new QTextBrowser(this);
    m_statsSummaryDisplay->setFixedHeight(100); // Example height for summary
    // layout->addWidget(m_statsSummaryDisplay); // Uncomment to add summary view
    
    QLabel* historyLabel = new QLabel(tr("Session History"), this);
    QFont historyFont = historyLabel->font();
    historyFont.setPointSize(historyFont.pointSize() + 2);
    historyLabel->setFont(historyFont);
    layout->addWidget(historyLabel);

    m_sessionTable = new QTableWidget(this);
    m_sessionTable->setColumnCount(8); // Added Time Spent
    m_sessionTable->setHorizontalHeaderLabels({
        tr("Session ID"), tr("Date"), tr("Difficulty"), tr("Total Tasks"), 
        tr("Correct"), tr("Errors"), tr("Score"), tr("Time (s)") // Removed "Success" for brevity, implied by score
    });
    m_sessionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_sessionTable->verticalHeader()->setVisible(false);
    m_sessionTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_sessionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_sessionTable->setAlternatingRowColors(true);
    layout->addWidget(m_sessionTable);

    setLayout(layout);
}

void StatisticsViewWidget::refreshData() {
    loadAndDisplayStats();
}

QString StatisticsViewWidget::mapDifficultyIdToName(int id) {
    if (id == 0) return tr("Easy");
    if (id == 1) return tr("Medium");
    if (id == 2) return tr("Hard");
    return tr("Unknown");
}

void StatisticsViewWidget::loadAndDisplayStats() {
    // m_statsSummaryDisplay->clear(); // If using summary view
    // m_statsSummaryDisplay->append("<h2>Overall Performance</h2>");
    // TODO: Calculate and display overall stats (total exercises, avg score, etc.)

    QList<UserSessionRecord> sessions = m_statsRepo->getAllUserSessions(0); // User ID 0

    m_sessionTable->setRowCount(0); // Clear previous data
    if (sessions.isEmpty()) {
        // m_statsSummaryDisplay->append(tr("<p>No session data found.</p>"));
        QTableWidgetItem *noDataItem = new QTableWidgetItem(tr("No session data available."));
        m_sessionTable->setRowCount(1);
        m_sessionTable->setItem(0,0, noDataItem);
        m_sessionTable->setSpan(0,0,1, m_sessionTable->columnCount()); // Span across all columns
        noDataItem->setTextAlignment(Qt::AlignCenter);
        return;
    }
    m_sessionTable->setRowCount(sessions.size());

    int row = 0;
    for (const auto& session : sessions) {
        m_sessionTable->setItem(row, 0, new QTableWidgetItem(QString::number(session.sessionId)));
        
        QDateTime startTime = QDateTime::fromString(session.sessionStartTime, Qt::ISODate);
        m_sessionTable->setItem(row, 1, new QTableWidgetItem(startTime.toString("yyyy-MM-dd hh:mm:ss")));
        
        m_sessionTable->setItem(row, 2, new QTableWidgetItem(mapDifficultyIdToName(session.difficultyId)));
        m_sessionTable->setItem(row, 3, new QTableWidgetItem(QString::number(session.totalTasksInSession)));
        m_sessionTable->setItem(row, 4, new QTableWidgetItem(QString::number(session.tasksCorrectlyCompleted)));
        m_sessionTable->setItem(row, 5, new QTableWidgetItem(QString::number(session.errorsMade)));
        m_sessionTable->setItem(row, 6, new QTableWidgetItem(QString::number(session.finalScore)));
        m_sessionTable->setItem(row, 7, new QTableWidgetItem(QString::number(session.timeSpentSeconds)));
        
        // Color row based on success (optional)
        if (!session.wasSuccessful) {
            for(int col = 0; col < m_sessionTable->columnCount(); ++col) {
                if(m_sessionTable->item(row, col))
                   m_sessionTable->item(row, col)->setBackground(QColor(255, 220, 220)); // Light red for unsuccessful
            }
        }
        row++;
    }
    m_sessionTable->resizeColumnsToContents(); // Adjust column widths
    m_sessionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // Re-apply stretch after resize
}