#include "MainWindow.h"
#include "DifficultyDialog.h"
#include "GrammarExerciseWidget.h"
#include "HelpDialog.h"
#include "StatisticsViewWidget.h"
#include "TranslationExerciseWidget.h"
#include "app/SettingsManager.h"
#include "ui/ExerciseViewBase.h" // Добавлено для qobject_cast

#include <QAction>
#include <QKeyEvent>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStackedWidget>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("Language Learning App");
  setMinimumSize(800, 600);

  createActions();
  createMenus();
  setupCentralWidget();

  openTranslationExercise();
}

MainWindow::~MainWindow() {}

void MainWindow::setupCentralWidget() {
  m_stackedWidget = new QStackedWidget(this);

  m_translationWidget = new TranslationExerciseWidget(this);
  m_grammarWidget = new GrammarExerciseWidget(this);
  m_statisticsWidget = new StatisticsViewWidget(this);

  m_stackedWidget->addWidget(m_translationWidget);
  m_stackedWidget->addWidget(m_grammarWidget);
  m_stackedWidget->addWidget(m_statisticsWidget);

  setCentralWidget(m_stackedWidget);
}

void MainWindow::createActions() {
  m_translationAction = new QAction(tr("&Translation Exercise"), this);
  connect(m_translationAction, &QAction::triggered, this,
          &MainWindow::openTranslationExercise);

  m_grammarAction = new QAction(tr("&Grammar Exercise"), this);
  connect(m_grammarAction, &QAction::triggered, this,
          &MainWindow::openGrammarExercise);

  m_statisticsAction = new QAction(tr("&Statistics"), this);
  connect(m_statisticsAction, &QAction::triggered, this,
          &MainWindow::openStatistics);

  m_difficultyAction = new QAction(tr("Change &Difficulty..."), this);
  connect(m_difficultyAction, &QAction::triggered, this,
          &MainWindow::changeDifficulty);

  m_exitAction = new QAction(tr("E&xit"), this);
  connect(m_exitAction, &QAction::triggered, this, &QWidget::close);

  m_aboutAction = new QAction(tr("&About"), this);
  connect(m_aboutAction, &QAction::triggered, []() {
    QMessageBox::about(nullptr, "About",
                       "Language Learning App v0.1\nInspired by Duolingo");
  });
}

void MainWindow::createMenus() {
  m_exercisesMenu = menuBar()->addMenu(tr("&Exercises"));
  m_exercisesMenu->addAction(m_translationAction);
  m_exercisesMenu->addAction(m_grammarAction);

  menuBar()->addAction(m_statisticsAction);

  m_settingsMenu = menuBar()->addMenu(tr("&Settings"));
  m_settingsMenu->addAction(m_difficultyAction);

  m_helpMenu = menuBar()->addMenu(tr("&Help"));
  m_helpMenu->addAction(m_aboutAction);
}

void MainWindow::openTranslationExercise() {
  if (m_translationWidget) {
    m_stackedWidget->setCurrentWidget(m_translationWidget);
    m_translationWidget->loadNewExerciseSet();
  }
}

void MainWindow::openGrammarExercise() {
  if (m_grammarWidget) {
    m_stackedWidget->setCurrentWidget(m_grammarWidget);
    m_grammarWidget->loadNewExerciseSet();
  }
}

void MainWindow::openStatistics() {
  if (m_statisticsWidget) {
    m_stackedWidget->setCurrentWidget(m_statisticsWidget);
    m_statisticsWidget->refreshData();
  }
}

void MainWindow::changeDifficulty() {
  DifficultyDialog dialog(this);
  dialog.setCurrentDifficulty(
      SettingsManager::instance().getCurrentDifficulty());
  if (dialog.exec() == QDialog::Accepted) {
    SettingsManager::instance().setCurrentDifficulty(
        dialog.getSelectedDifficulty());
    QMessageBox::information(
        this, "Difficulty Changed",
        "Difficulty level updated. New exercises will use this setting.");
    // Обновить текущее упражнение, если оно активно
    if (auto *currentExerciseView = qobject_cast<ExerciseViewBase *>(
            m_stackedWidget->currentWidget())) {
      currentExerciseView->loadNewExerciseSet();
    }
  }
}

void MainWindow::showHelp() {
  QWidget *currentView = m_stackedWidget->currentWidget();
  QString helpText;

  if (auto *exView = qobject_cast<ExerciseViewBase *>(
          currentView)) { // Используем currentView
    helpText = exView->getHelpText();
  } else {
    helpText = "General help: Use the menu to navigate. Press 'H' for "
               "context-specific help if an exercise is active.";
  }

  HelpDialog helpDialog(helpText, this);
  helpDialog.exec();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_H) {
    showHelp();
  } else {
    QMainWindow::keyPressEvent(event);
  }
}
