#include "mainwindow.h"

#include "addgrammardialog.h"
#include "addtranslationdialog.h"

#include <QActionGroup>
#include <QApplication>
#include <qevent.h>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    currentTimeGrammar(60),
    currentTimeTranslation(90),
    currentDifficulty(tr("Лёгкий")){

    setWindowTitle(tr("Приложение для изучения языка"));
    resize(900, 700);
    setMinimumSize(640, 480);

    setupUi();
    setupMenu();
    loadData();

    ShowWelcome();
    statusBar()->showMessage(tr("Готово к работе. Выберите упражнение и уровень сложности."), 5000);
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUi() {
    welcomePage = new QWidget(this);
    QVBoxLayout *welcomeLayout = new QVBoxLayout(welcomePage);
    welcomeLayout->setContentsMargins(30, 30, 30, 30);
    welcomeLayout->setSpacing(20);

    QLabel *titleLabel = new QLabel(tr("Добро пожаловать!"), welcomePage);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 28pt; font-weight: bold; color: #2c3e50; margin-bottom: 15px;");

    QLabel *usageTitleLabel = new QLabel(tr("Как пользоваться приложением:"), welcomePage);
    usageTitleLabel->setAlignment(Qt::AlignLeft);
    usageTitleLabel->setStyleSheet("font-size: 16pt; font-weight: bold; color: #2c3e50; margin-bottom: 10px;");

    QLabel *usageInstructionsLabel = new QLabel(
    tr("1.  <b>Запустите приложение.</b> (Вы уже здесь!)<br>"
       "2.  <b>Выберите сложность:</b><br>"
       "    Используйте меню <i>Сложность</i>, чтобы выбрать уровень: <i>Лёгкий</i>, <i>Средний</i> или <i>Сложный</i>.<br>"
       "3.  <b>Выберите упражнение:</b><br>"
       "    В меню <i>Упражнения</i> выберите:<br>"
       "    •  <i>Упражнения для грамматики</i><br>"
       "    •  <i>Упражнения для перевода</i><br>"
       "4.  <b>Начните упражнение:</b><br>"
       "    Нажмите кнопку 'Начать' на странице упражнения.<br>"
       "5.  <b>Выполняйте задания:</b><br>"
       "    •  Для грамматики: выберите вариант и нажмите 'Ответить'.<br>"
       "    •  Для перевода: введите ваш перевод и нажмите 'Отправить перевод'.<br>"
       "6.  <b>Добавляйте свой контент (по желанию):</b><br>"
       "    В меню <i>Редактировать</i>:<br>"
       "    •  <i>Добавить вопрос (грамматика)...</i><br>"
       "    •  <i>Добавить предложение (перевод)...</i><br>"
       "7.  <b>Получите справку:</b><br>"
       "    В меню <i>Справка</i> выберите <i>О программе</i> или <i>О упражнении</i> для дополнительной информации."),
        welcomePage);
    usageInstructionsLabel->setAlignment(Qt::AlignLeft);
    usageInstructionsLabel->setWordWrap(true);
    usageInstructionsLabel->setTextFormat(Qt::RichText);
    usageInstructionsLabel->setStyleSheet("font-size: 12pt; color: #34495e; line-height: 150%;");

    welcomeLayout->addWidget(titleLabel);
    welcomeLayout->addStretch(1);
    welcomeLayout->addWidget(usageTitleLabel);
    welcomeLayout->addWidget(usageInstructionsLabel);
    welcomeLayout->addStretch(2);

    welcomePage->setLayout(welcomeLayout);
    welcomePage->setStyleSheet("background-color: #ecf0f1;");

    grammarPage = new Grammar(this);
    translationPage = new Translation(this);

    stack = new QStackedWidget(this);
    stack->addWidget(welcomePage);
    stack->addWidget(grammarPage);
    stack->addWidget(translationPage);

    setCentralWidget(stack);

    connect(grammarPage, &Grammar::exerciseComplete, this, &MainWindow::OnExerciseDone);
    connect(translationPage, &Translation::exerciseComplete, this, &MainWindow::OnExerciseDone);
}

void MainWindow::setupMenu() {
    exerciseMenu = menuBar()->addMenu(tr("&Упражнения"));

    welcomeAct = new QAction(tr("Главная страница"), this);
    connect(welcomeAct, &QAction::triggered, this, &MainWindow::ShowWelcome);
    exerciseMenu->addAction(welcomeAct);
    exerciseMenu->addSeparator();

    grammarAct = new QAction(tr("Упражнения для &грамматики"), this);
    connect(grammarAct, &QAction::triggered, this, &MainWindow::ShowGrammar);
    exerciseMenu->addAction(grammarAct);

    translationAct = new QAction(tr("Упражнения для &перевода"), this);
    connect(translationAct, &QAction::triggered, this, &MainWindow::ShowTranslation);
    exerciseMenu->addAction(translationAct);

    exerciseMenu->addSeparator();
    exitAct = new QAction(tr("&Выход"), this);
    exitAct->setShortcut(QKeySequence::Quit);
    connect(exitAct, &QAction::triggered, qApp, &QApplication::quit);
    exerciseMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Редактировать"));
    addGrammarAct = new QAction(tr("Добавить &вопрос (грамматика)..."), this);
    connect(addGrammarAct, &QAction::triggered, this, &MainWindow::AddGrammarQ);
    editMenu->addAction(addGrammarAct);

    addTranslationAct = new QAction(tr("Добавить &предложение (перевод)..."), this);
    connect(addTranslationAct, &QAction::triggered, this, &MainWindow::AddTranslationS);
    editMenu->addAction(addTranslationAct);

    difficultyMenu = menuBar()->addMenu(tr("&Сложность"));
    difficultyGroup = new QActionGroup(this);
    difficultyGroup->setExclusive(true);

    easyAct = difficultyGroup->addAction(tr("&Лёгкий"));
    easyAct->setCheckable(true);
    mediumAct = difficultyGroup->addAction(tr("&Средний"));
    mediumAct->setCheckable(true);
    hardAct = difficultyGroup->addAction(tr("&Сложный"));
    hardAct->setCheckable(true);

    difficultyMenu->addAction(easyAct);
    difficultyMenu->addAction(mediumAct);
    difficultyMenu->addAction(hardAct);

    easyAct->setChecked(true);

    connect(difficultyGroup, &QActionGroup::triggered, this, &MainWindow::OnDifficultyAction);

    helpMenu = menuBar()->addMenu(tr("&Справка"));
    helpAct = new QAction("&О программе", this);
    helpExercise = new QAction(tr("Об &упражнении"), this);
    connect(helpExercise, &QAction::triggered, this, &MainWindow::ShowHelpExercise);
    connect(helpAct, &QAction::triggered, this, &MainWindow::ShowHelp);
    helpMenu->addAction(helpAct);
    helpMenu->addAction(helpExercise);
}

void MainWindow::loadData() {
    grammarData.append({tr("The cat ___ on the mat."), {tr("sit"), tr("sits"), tr("sitting"), tr("sat")}, 1});
    grammarData.append({tr("She ___ to the store yesterday."), {tr("go"), tr("goes"), tr("went"), tr("gone")}, 2});
    grammarData.append({tr("___ you like some tea?"), {tr("Do"), tr("Does"), tr("Would"), tr("Will")}, 2});
    grammarData.append({tr("My brother is ___ than me."), {tr("tall"), tr("taller"), tr("tallest"), tr("more tall")}, 1});
    grammarData.append({tr("I haven't seen him ___ last year."), {tr("for"), tr("since"), tr("ago"), tr("from")}, 1});

    translationData.append({tr("Hello, world!"), tr("Привет, мир!"), 1});
    translationData.append({tr("This is an interesting book."), tr("Это интересная книга."), 2});
    translationData.append({tr("The weather is wonderful today."), tr("Сегодня прекрасная погода."), 1});
    translationData.append({tr("Practice makes perfect."), tr("Повторение - мать учения."), 3});
    translationData.append({tr("Could you please help me?"), tr("Не могли бы вы мне помочь, пожалуйста?"), 2});
}

void MainWindow::ShowWelcome() {
    stack->setCurrentWidget(welcomePage);
    updateTitle();
}

void MainWindow::ShowGrammar() {
    grammarPage->updateLevel(currentDifficulty);
    grammarPage->startExercise(currentDifficulty, grammarData);
    stack->setCurrentWidget(grammarPage);
    updateTitle();
}

void MainWindow::ShowTranslation() {
    translationPage->updateLevel(currentDifficulty);
    translationPage->startExercise(currentDifficulty, translationData);
    stack->setCurrentWidget(translationPage);
    updateTitle();
}

void MainWindow::OnDifficultyAction(const QAction *action) {
    if (action == easyAct) {
        currentDifficulty = tr("Лёгкий");
        currentTimeGrammar = 60;
        currentTimeTranslation = 90;
    } else if (action == mediumAct) {
        currentDifficulty = tr("Средний");
        currentTimeGrammar = 30;
        currentTimeTranslation = 60;
    } else if (action == hardAct) {
        currentDifficulty = tr("Сложный");
        currentTimeGrammar = 10;
        currentTimeTranslation = 30;
    }

    statusBar()->showMessage(tr("Уровень сложности изменен на: %1").arg(currentDifficulty), 3000);
    updateTitle();

    QWidget *current_visible = stack->currentWidget();
    if (current_visible == grammarPage) {
        grammarPage->updateLevel(currentDifficulty);
        grammarPage->startExercise(currentDifficulty, grammarData, currentTimeGrammar);
    } else if (current_visible == translationPage) {
        translationPage->updateLevel(currentDifficulty);
        translationPage->startExercise(currentDifficulty, translationData, currentTimeTranslation);
    }

}

void MainWindow::AddGrammarQ() {
    QTimer* targetTimer = StartAndStopTimer();

    AddGrammarDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Question q;
        q.question = dialog.getQuestionText();
        q.options = dialog.getOptions();
        q.correctOption = dialog.getCorrectOptionIndex();
        grammarData.append(q);
        statusBar()->showMessage(tr("Новый вопрос по грамматике добавлен."), 3000);
        if (stack->currentWidget() == grammarPage) {
            if (QMessageBox::question(this, tr("Обновить упражнение?"),
                                      tr("Вопрос добавлен. Хотите перезапустить упражнение по грамматике с новым набором вопросов?"),
                                      QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                grammarPage->startExercise(currentDifficulty, grammarData);
            }
        }
    }
    if (targetTimer != nullptr) {
        targetTimer->start();
    }
}

void MainWindow::AddTranslationS() {
    QTimer* target_timer = StartAndStopTimer();

    AddTranslationDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {


        Sentence s;
        s.original = dialog.getOriginalSentence();
        s.translation = dialog.getTranslatedSentence();
        s.level = dialog.getLevel();
        translationData.append(s);
        statusBar()->showMessage(tr("Новое предложение для перевода добавлено."), 3000);
        if (stack->currentWidget() == translationPage) {
            if (QMessageBox::question(this, tr("Обновить упражнение?"),
                                      tr("Предложение добавлено. Хотите перезапустить упражнение по переводу с новым набором предложений?"),
                                      QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                translationPage->startExercise(currentDifficulty, translationData);
            }
        }
    }
    if (target_timer != nullptr) {
        target_timer->start();
    }
}

void MainWindow::updateTitle() {
    QString title = tr("Приложение для изучения языка");
    if (stack->currentWidget() == grammarPage) {
        title = tr("Грамматика - %1").arg(currentDifficulty);
    } else if (stack->currentWidget() == translationPage) {
        title = tr("Перевод - %1").arg(currentDifficulty);
    }
    setWindowTitle(title);
}

QTimer* MainWindow::StartAndStopTimer() const {
    const QWidget *current_active_widget = stack->currentWidget();
    QTimer *target_timer = nullptr;
    if (current_active_widget == grammarPage) {
        if ((grammarPage != nullptr) && (grammarPage->getTimer() != nullptr)) {
            target_timer = grammarPage->getTimer();
            if (target_timer != nullptr) {
                target_timer->stop();
            }
        }
    } else if (current_active_widget == translationPage) {
        if ((translationPage != nullptr) && (translationPage->getTimer() != nullptr)) {
            target_timer = translationPage->getTimer();
            if (target_timer != nullptr) {
                target_timer->stop();
            }
        }
    }
    return target_timer;
}

void MainWindow::ShowHelp() {
    QTimer* target_timer = StartAndStopTimer();

    QMessageBox::about(this, tr("О программе"),
                       tr("<h2>Приложение для изучения языка</h2>"
                          "<p>Это приложение создано для помощи в изучении грамматики и перевода.</p>"
                          "<p><b>Основные возможности:</b></p>"
                          "<ul>"
                          "<li>Интерактивные упражнения по грамматике с выбором правильного варианта ответа.</li>"
                          "<li>Задания на перевод предложений с исходного языка на целевой.</li>"
                          "<li>Выбор уровня сложности (Лёгкий, Средний, Сложный), влияющий на время выполнения, количество попыток и начисляемые очки.</li>"
                          "<li>Возможность добавлять собственные вопросы по грамматике и предложения для перевода через меню 'Редактировать'.</li>"
                          "<li>Отслеживание прогресса с помощью таймера для каждого задания и подсчёт очков.</li>"
                          "<li>Наглядное отображение текущего вопроса/предложения и общего прогресса в упражнении.</li>"
                          "</ul>"
                          "<p>Используйте меню для навигации, выбора упражнений, настройки сложности и добавления новых заданий.</p>"
                          "<p>Автор: Дикан Данила</p>"));

    if(target_timer != nullptr) {
        target_timer->start();
    }
}

void MainWindow::OnExerciseDone(int score) {
    QString type;
    if (sender() == grammarPage) {
        type = tr("грамматике");
    } else if (sender() == translationPage) {
        type = tr("переводу");
    } else {
        return;
    }

    QMessageBox::information(this, tr("Упражнение завершено"),
                             tr("Поздравляем! Упражнение по %1 завершено.\nВаш итоговый счёт: %2")
                                 .arg(type).arg(score));

    ShowWelcome();
    statusBar()->showMessage(tr("Упражнение завершено. Ваш счёт: %1").arg(score), 5000);
}


void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_H) {
        ShowHelpExercise();
        event->accept();
    }
}

void MainWindow::ShowHelpExercise() {
    QWidget *current_active_widget = stack->currentWidget();
    QTimer* exerciseTimer = nullptr;
    bool timerWasActive = false;

    if (current_active_widget == grammarPage) {
        exerciseTimer = grammarPage->getTimer();
    } else if (current_active_widget == translationPage) {
        exerciseTimer = translationPage->getTimer();
    }

    if (exerciseTimer && exerciseTimer->isActive()) {
        exerciseTimer->stop();
        timerWasActive = true;
    }

    if (current_active_widget == grammarPage) {
        QMessageBox::information(this, tr("Подсказка: Упражнение по грамматике"),
                                 tr("<h3>Упражнение по грамматике</h3>"
                                    "<p>Вам будет предложен вопрос (обычно предложение с пропуском) и несколько вариантов ответа.</p>"
                                    "<p><b>Ваша задача:</b> выбрать единственно верный вариант, который грамматически правильно дополняет предложение.</p>"
                                    "<h4>Как это работает:</h4>"
                                    "<ul>"
                                    "<li>Прочитайте внимательно предложение.</li>"
                                    "<li>Рассмотрите все предложенные варианты ответа.</li>"
                                    "<li>Выберите тот вариант, который, по вашему мнению, является правильным, кликнув по нему.</li>"
                                    "<li>Нажмите кнопку 'Ответить' для проверки.</li>"
                                    "</ul>"
                                    "<p><b>Уровни сложности влияют на:</b></p>"
                                    "<ul>"
                                    "<li>Время, отведённое на ответ.</li>"
                                    "<li>Количество допустимых ошибок на один вопрос.</li>"
                                    "<li>Количество очков за правильный ответ.</li>"
                                    "</ul>"));
    } else if (current_active_widget == translationPage) {
        QMessageBox::information(this, tr("Подсказка: Упражнение по переводу"),
                                 tr("<h3>Упражнение по переводу</h3>"
                                    "<p>Вам будет дано предложение на исходном языке.</p>"
                                    "<p><b>Ваша задача:</b> как можно точнее перевести это предложение на целевой язык и ввести ваш перевод в текстовое поле.</p>"
                                    "<h4>Как это работает:</h4>"
                                    "<ul>"
                                    "<li>Внимательно прочитайте исходное предложение.</li>"
                                    "<li>Сформулируйте его перевод на целевом языке.</li>"
                                    "<li>Введите ваш вариант перевода в поле ввода. Обращайте внимание на знаки препинания и регистр, если это важно для языка.</li>"
                                    "<li>Нажмите кнопку 'Отправить перевод' для проверки.</li>"
                                    "</ul>"
                                    "<p><b>Уровни сложности влияют на:</b></p>"
                                    "<ul>"
                                    "<li>Время, отведённое на перевод.</li>"
                                    "<li>Количество допустимых ошибок на одно предложение.</li>"
                                    "<li>Количество очков за правильный перевод.</li>"
                                    "</ul>"));
    } else {
        QMessageBox::information(this, tr("Информация"),
                                 tr("Чтобы получить подсказку по конкретному упражнению, пожалуйста, сначала перейдите к этому упражнению через меню 'Упражнения', а затем нажмите клавишу 'H' (или выберите соответствующий пункт меню, если он есть)."));
    }

    if (timerWasActive && exerciseTimer) {
        exerciseTimer->start();
    }
}