#include "../include/duolingo.h"

#include <QApplication>
#include <QAudioOutput>
#include <QComboBox>
#include <QInputDialog>
#include <QKeyEvent>
#include <QListWidget>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QTextEdit>
#include <QVBoxLayout>

// TODO добавить возможность переводить предложения, где надо вставить текст

Duolingo::Duolingo(QWidget* parent)  // NOLINT
    : QMainWindow(parent)
    , player(new QMediaPlayer(this))
    , currentExercise(0)
    , score(0)
    , wrongAttempts(0)
    , currentDifficulty("start") {
    // панель сверху
    QMenu* menu = menuBar()->addMenu("Меню");
    const QAction* difficulty = menu->addAction("Выбрать сложность");
    const QAction* help = menu->addAction("Помощь");

    connect(difficulty, &QAction::triggered, this, &Duolingo::ShowDifficultyDialog);
    connect(help, &QAction::triggered, this, &Duolingo::ShowHelp);
    SetupExercises();

    QWidget* centralWidget = new QWidget(this);            // NOLINT
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);  // NOLINT

    stackedWidget = new QStackedWidget(this);              // NOLINT
    CreatePages();

    QComboBox* pageComboBox = new QComboBox;               // NOLINT
    pageComboBox->addItem(tr("Главная"));
    pageComboBox->addItem(tr("Cлова"));
    pageComboBox->addItem(tr("Перевод"));
    pageComboBox->addItem(tr("Грамматика"));
    pageComboBox->addItem(tr("Рейтинг"));

    connect(
        pageComboBox, QOverload<int>::of(&QComboBox::activated), stackedWidget,
        &QStackedWidget::setCurrentIndex);
    connect(
        pageComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &Duolingo::IsModeChanged);

    layout->addWidget(pageComboBox, 0, Qt::AlignRight);
    layout->addWidget(stackedWidget);

    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    exerciseTimer = new QTimer(this);   // NOLINT
    countdownTimer = new QTimer(this);  // NOLINT
    connect(exerciseTimer, &QTimer::timeout, this, &Duolingo::OnTimeout);
    connect(countdownTimer, &QTimer::timeout, this, &Duolingo::UpdateTimer);

    this->installEventFilter(this);
    learnPageWidget->installEventFilter(this);
    translationPageWidget->installEventFilter(this);
    grammarPageWidget->installEventFilter(this);
}

void Duolingo::CreatePages() {
    QWidget* welcomeScreen = new QWidget();                       // NOLINT
    QVBoxLayout* welcomeLayout = new QVBoxLayout(welcomeScreen);  // NOLINT
    std::string s = "Добро пожаловать в приложение для изучения испанского!";
    if (currentDifficulty == "start") {
        s = "Добро пожаловать в приложение для изучения испанского!\n"
            "Пройдите тест, чтобы определить Ваш уровень владения языком\n(позже этот уровень "
            "можно будет изменить в меню)";
    }
    QLabel* text = new QLabel(s.c_str());  // NOLINT
    welcomeLayout->addWidget(text, 0, Qt::AlignCenter);
    stackedWidget->addWidget(welcomeScreen);

    learnPageWidget = new QWidget();                 // NOLINT
    learnLayout = new QVBoxLayout(learnPageWidget);  // NOLINT
    LearnPage(learnLayout);
    stackedWidget->addWidget(learnPageWidget);

    translationPageWidget = new QWidget();                       // NOLINT
    translationLayout = new QVBoxLayout(translationPageWidget);  // NOLINT
    TranslationPage(translationLayout);
    stackedWidget->addWidget(translationPageWidget);

    grammarPageWidget = new QWidget();                   // NOLINT
    grammarLayout = new QVBoxLayout(grammarPageWidget);  // NOLINT
    GrammarPage(grammarLayout);
    stackedWidget->addWidget(grammarPageWidget);

    QWidget* ratingPageWidget = new QWidget();                          // NOLINT
    QVBoxLayout* ratingLayout = new QVBoxLayout(ratingPageWidget);      // NOLINT
    QLabel* rating = new QLabel("Embajadores de la lengua Española!");  // NOLINT
    ratingList = new QListWidget();
    ratingLayout->addWidget(rating, 0, Qt::AlignCenter);
    ratingLayout->addWidget(ratingList, 0, Qt::AlignCenter);
    stackedWidget->addWidget(ratingPageWidget);
}

void Duolingo::LearnPage(QVBoxLayout* layout) {
    QWidget* statusWidget = new QWidget();                      // NOLINT
    QHBoxLayout* statusLayout = new QHBoxLayout(statusWidget);  // NOLINT

    learnProgressBar = new QProgressBar();                      // NOLINT
    learnProgressBar->setRange(0, 5);
    learnProgressBar->setValue(0);
    learnScoreLabel = new QLabel("Результат: 0");               // NOLINT
    learnTimerLabel = new QLabel("Время: 00:00");               // NOLINT

    statusLayout->addWidget(learnProgressBar);
    statusLayout->addWidget(learnScoreLabel);
    statusLayout->addWidget(learnTimerLabel);

    learnExerciseWidget = new QWidget();                 // NOLINT
    learnExerciseWidget->setLayout(new QVBoxLayout());   // NOLINT

    QPushButton* submit = new QPushButton("Отправить");  // NOLINT
    connect(submit, &QPushButton::clicked, this, &Duolingo::OnSubmitClicked);

    layout->addWidget(statusWidget);
    layout->addWidget(learnExerciseWidget, 1);
    layout->addWidget(submit);
}

void Duolingo::TranslationPage(QVBoxLayout* layout) {
    QWidget* statusWidget = new QWidget();                      // NOLINT
    QHBoxLayout* statusLayout = new QHBoxLayout(statusWidget);  // NOLINT

    translateProgressBar = new QProgressBar();
    translateProgressBar->setRange(0, 5);
    translateProgressBar->setValue(0);
    translateScoreLabel = new QLabel("Результат: 0");
    translateTimerLabel = new QLabel("Время: 00:00");

    statusLayout->addWidget(translateProgressBar);
    statusLayout->addWidget(translateScoreLabel);
    statusLayout->addWidget(translateTimerLabel);

    translationExerciseWidget = new QWidget();
    translationExerciseWidget->setLayout(new QVBoxLayout());

    QPushButton* submit = new QPushButton("Отправить");  // NOLINT
    connect(submit, &QPushButton::clicked, this, &Duolingo::OnSubmitClicked);

    layout->addWidget(statusWidget);
    layout->addWidget(translationExerciseWidget, 1);
    layout->addWidget(submit);
}

void Duolingo::GrammarPage(QVBoxLayout* layout) {
    QWidget* statusWidget = new QWidget();                      // NOLINT
    QHBoxLayout* statusLayout = new QHBoxLayout(statusWidget);  // NOLINT

    grammarProgressBar = new QProgressBar();
    grammarProgressBar->setRange(0, 5);
    grammarProgressBar->setValue(0);
    grammarScoreLabel = new QLabel("Результат: 0");
    grammarTimerLabel = new QLabel("Время: 00:00");

    statusLayout->addWidget(grammarProgressBar);
    statusLayout->addWidget(grammarScoreLabel);
    statusLayout->addWidget(grammarTimerLabel);

    grammarExerciseWidget = new QWidget();
    grammarExerciseWidget->setLayout(new QVBoxLayout());

    QPushButton* submit = new QPushButton("Отправить");  // NOLINT
    connect(submit, &QPushButton::clicked, this, &Duolingo::OnSubmitClicked);

    layout->addWidget(statusWidget);
    layout->addWidget(grammarExerciseWidget, 1);
    layout->addWidget(submit);
}

/*
void Duolingo::CreateCommonPageElements(QVBoxLayout* layout) {
    QWidget* statusWidget = new QWidget();                      // NOLINT
    QHBoxLayout* statusLayout = new QHBoxLayout(statusWidget);  // NOLINT

    learnProgressBar = new QProgressBar();
    learnProgressBar->setRange(0, 5);
    learnProgressBar->setValue(0);
    learnScoreLabel = new QLabel("Результат: 0");
    learnTimerLabel = new QLabel("Время: 00:00");

    statusLayout->addWidget(learnProgressBar);
    statusLayout->addWidget(learnScoreLabel);
    statusLayout->addWidget(learnTimerLabel);

    currentExerciseWidget = new QWidget();
    currentExerciseWidget->setLayout(new QVBoxLayout());

    QPushButton* submit = new QPushButton("Отправить");  // NOLINT
    connect(submit, &QPushButton::clicked, this, &Duolingo::OnSubmitClicked);

    layout->addWidget(statusWidget);
    layout->addWidget(currentExerciseWidget, 1);
    layout->addWidget(submit);
}
*/

void Duolingo::IsModeChanged(int index) {
    mode = index;
    countdownTimer->stop();
    exerciseTimer->stop();

    if (index == 1) {
        OnLearnClicked();
    } else if (index == 2) {
        OnTranslationClicked();
    } else if (index == 3) {
        OnGrammarClicked();
    }
}

void Duolingo::SetupExercises() {
    translationExercises = {
      {"Привет", "Hola"}, {"Пока", "Adios"}, {"Спасибо", "Gracias"}, {"Пожалуйста", "Por favor"}};

    grammarQuestions = {
      "Yo ___ (comer) una manzana", "Ella ___ (ir) a la escuela", "Nosotros ___ (tener) un gato",
      "Ellos ___ (ser) felices", "Tú ___ (hablar) español"};

    grammarOptions = {
      {"como", "comes", "comemos", "comen"},
      {"voy", "va", "vamos", "van"},
      {"tengo", "tiene", "tenemos", "tienen"},
      {"soy", "es", "somos", "son"},
      {"hablo", "hablas", "hablamos", "hablan"}};

    grammarAnswers = {0, 1, 2, 3, 1};
}

void Duolingo::OnLearnClicked() {
    /*currentExercise = 0;
    wrongAttempts = 0;
    progressBar->setMaximum(translationExercises.size());
    progressBar->setValue(0);

    if (currentDifficulty == "Начинающий") {
        timeLeft = 120;
    } else if (currentDifficulty == "Продвинутый") {
        timeLeft = 90;
    } else {
        timeLeft = 60;
    }

    UpdateTimerDisplay();
    countdownTimer->start(1000);
    exerciseTimer->start(timeLeft * 1000);

    ShowLearnExercise();*/
}

void Duolingo::OnTranslationClicked() {
    currentExercise = 0;
    wrongAttempts = 0;
    translateProgressBar->setMaximum(translationExercises.size());
    translateProgressBar->setValue(0);

    if (currentDifficulty == "Начинающий") {
        timeLeft = 120;
    } else if (currentDifficulty == "Продвинутый") {
        timeLeft = 90;
    } else {
        timeLeft = 60;
    }

    UpdateTimerDisplay();
    countdownTimer->start(1000);
    exerciseTimer->start(timeLeft * 1000);

    ShowTranslationExercise();
}

void Duolingo::OnGrammarClicked() {
    currentExercise = 0;
    wrongAttempts = 0;
    grammarProgressBar->setMaximum(grammarQuestions.size());
    grammarProgressBar->setValue(0);

    if (currentDifficulty == "Начинающий") {
        timeLeft = 150;
    } else if (currentDifficulty == "Продвинутый") {
        timeLeft = 120;
    } else {
        timeLeft = 90;
    }

    UpdateTimerDisplay();
    countdownTimer->start(1000);
    exerciseTimer->start(timeLeft * 1000);

    ShowGrammarExercise();
}

void Duolingo::ShowLearnExercise() {
    if (currentExercise >= translationExercises.size()) {
        FinishExercise(true);
        return;
    }

    QLayoutItem* child;
    while ((child = learnExerciseWidget->layout()->takeAt(0))) {
        delete child->widget();
        delete child;
    }

    QVBoxLayout* exerciseLayout = qobject_cast<QVBoxLayout*>(learnExerciseWidget->layout());
    QLabel* instruction = new QLabel("Запомните это слово:");                      // NOLINT
    QLabel* wordLabel = new QLabel(translationExercises[currentExercise].second);  // NOLINT

    exerciseLayout->addWidget(instruction);
    exerciseLayout->addWidget(wordLabel);
    stackedWidget->setCurrentIndex(1);  // Переключение виджета на страницу "Learn"
}

void Duolingo::ShowTranslationExercise() {
    if (currentExercise >= translationExercises.size()) {
        FinishExercise(true);
        return;
    }

    QLayoutItem* child;
    while ((child = translationExerciseWidget->layout()->takeAt(0))) {
        delete child->widget();
        delete child;
    }

    QVBoxLayout* exerciseLayout = qobject_cast<QVBoxLayout*>(translationExerciseWidget->layout());
    QLabel* instruction = new QLabel("Переведите на испанский:");                 // NOLINT
    QLabel* wordLabel = new QLabel(translationExercises[currentExercise].first);  // NOLINT
    QTextEdit* answerEdit = new QTextEdit();                                      // NOLINT

    exerciseLayout->addWidget(instruction);
    exerciseLayout->addWidget(wordLabel);
    exerciseLayout->addWidget(answerEdit);

    // stackedWidget->setCurrentIndex(2);
}

void Duolingo::ShowGrammarExercise() {
    if (currentExercise >= grammarQuestions.size()) {
        FinishExercise(true);
        return;
    }

    QLayoutItem* child;
    while ((child = grammarExerciseWidget->layout()->takeAt(0))) {
        delete child->widget();
        delete child;
    }

    QVBoxLayout* exerciseLayout = qobject_cast<QVBoxLayout*>(grammarExerciseWidget->layout());
    QLabel* questionLabel = new QLabel(grammarQuestions[currentExercise]);            // NOLINT
    QButtonGroup* optionsGroup = new QButtonGroup(this);                              // NOLINT

    for (int i = 0; i < grammarOptions[currentExercise].size(); ++i) {
        QRadioButton* option = new QRadioButton(grammarOptions[currentExercise][i]);  // NOLINT
        optionsGroup->addButton(option, i);
        exerciseLayout->addWidget(option);
    }

    exerciseLayout->addWidget(questionLabel);

    // stackedWidget->setCurrentIndex(3);
}

void Duolingo::OnSubmitClicked() {
    int currentPage = stackedWidget->currentIndex();

    if (currentPage == 1) {
        currentExercise++;
        learnProgressBar->setValue(learnProgressBar->value() + 1);

        if (currentExercise < translationExercises.size()) {
            ShowLearnExercise();
        } else {
            FinishExercise(true);
        }
    } else if (currentPage == 2) {
        currentExercise++;
        translateProgressBar->setValue(translateProgressBar->value() + 1);

        if (currentExercise < translationExercises.size()) {
            ShowTranslationExercise();
        } else {
            FinishExercise(true);
        }
        CheckTranslationAnswer();
    } else if (currentPage == 3) {
        currentExercise++;
        grammarProgressBar->setValue(grammarProgressBar->value() + 1);

        if (currentExercise < grammarExercises.size()) {
            ShowGrammarExercise();
        } else {
            FinishExercise(true);
        }
        CheckGrammarAnswer();
    }
}

void Duolingo::CheckTranslationAnswer() {
    QTextEdit* answerEdit = translationExerciseWidget->findChild<QTextEdit*>();
    if (!answerEdit) {
        return;
    }

    QString userAnswer = answerEdit->toPlainText().trimmed();
    QString correctAnswer = translationExercises[currentExercise].second;

    if (IsAnswerCorrect(userAnswer, correctAnswer)) {
        PlaySound(true);
        currentExercise++;
        translateProgressBar->setValue(translateProgressBar->value() + 1);
        wrongAttempts = 0;

        if (currentExercise < translationExercises.size()) {
            ShowTranslationExercise();
        } else {
            FinishExercise(true);
        }
    } else {
        PlaySound(false);
        wrongAttempts++;
        if (wrongAttempts >= 3) {
            FinishExercise(false);
        } else {
            QMessageBox::warning(this, "Неверно", "Попробуйте еще раз!");
        }
    }
}

void Duolingo::CheckGrammarAnswer() {
    QButtonGroup* optionsGroup = grammarExerciseWidget->findChild<QButtonGroup*>();
    if (!optionsGroup) {
        return;
    }

    int selectedId = optionsGroup->checkedId();
    if (selectedId == -1) {
        QMessageBox::warning(this, "Не выбрано", "Пожалуйста, выберите вариант ответа!");
        return;
    }

    if (selectedId == grammarAnswers[currentExercise]) {
        PlaySound(true);
        currentExercise++;
        learnProgressBar->setValue(learnProgressBar->value() + 1);
        wrongAttempts = 0;

        if (currentExercise < grammarQuestions.size()) {
            ShowGrammarExercise();
        } else {
            FinishExercise(true);
        }
    } else {
        PlaySound(false);
        wrongAttempts++;
        if (wrongAttempts >= 3) {
            FinishExercise(false);
        } else {
            QMessageBox::warning(this, "Неверно", "Попробуйте еще раз!");
        }
    }
}

bool Duolingo::IsAnswerCorrect(const QString& userAnswer, const QString& correctAnswer) {
    return userAnswer == correctAnswer;
}

void Duolingo::FinishExercise(bool success) {
    exerciseTimer->stop();
    countdownTimer->stop();

    if (success) {
        int pointsEarned = 10 * learnProgressBar->maximum();
        score += pointsEarned;
        learnScoreLabel->setText(QString("Результат: %1").arg(score));

        PlaySound(true);
        QMessageBox::information(
            this, "Поздравляем!",
            QString("Вы завершили упражнение!\nНабрано очков: %1").arg(pointsEarned));
    } else {
        PlaySound(false);
        QMessageBox::information(
            this, "Упражнение завершено", "Слишком много ошибок. Попробуйте еще раз!");
    }

    stackedWidget->setCurrentIndex(0);
    learnProgressBar->setValue(0);
}

void Duolingo::OnTimeout() {
    FinishExercise(false);
    QMessageBox::information(this, "Время вышло", "Время на выполнение упражнения истекло.");
}

void Duolingo::UpdateTimer() {
    timeLeft--;
    if (timeLeft <= 0) {
        countdownTimer->stop();
        OnTimeout();
        return;
    }

    UpdateTimerDisplay();
}

void Duolingo::UpdateTimerDisplay() const {
    switch (mode) {
        case 1:
            learnTimerLabel->setText(QString("Время: %1:%2")
                                         .arg(timeLeft / 60, 2, 10, QLatin1Char('0'))
                                         .arg(timeLeft % 60, 2, 10, QLatin1Char('0')));
            break;
        case 2:
            translateTimerLabel->setText(QString("Время: %1:%2")
                                             .arg(timeLeft / 60, 2, 10, QLatin1Char('0'))
                                             .arg(timeLeft % 60, 2, 10, QLatin1Char('0')));
            break;
        case 3:
            grammarTimerLabel->setText(QString("Время: %1:%2")
                                           .arg(timeLeft / 60, 2, 10, QLatin1Char('0'))
                                           .arg(timeLeft % 60, 2, 10, QLatin1Char('0')));
            break;
        default:
            break;
    }
}

void Duolingo::ShowHelp() {
    QString helpText;
    switch (stackedWidget->currentIndex()) {
        case 0:  // Главная
            helpText =
                "Добро пожаловать в приложение для изучения испанского языка!\n\n"
                "Выберите упражнение, чтобы начать:\n"
                "- Новые слова: Появляется новые слова появляются по 5 штук, для перехода к "
                "следующим "
                "нужно нажать 'Отправить'\n"
                "- Перевод: Переведите русские слова на испанский\n"
                "- Грамматика: Выберите правильное слово или форму слова\n\n"
                "Также вы можете изменить сложность в меню.";
            break;
        case 1:  // Слова
            helpText = "No tengo ni idea, y tiene que seguir así\n🤖🛀🛌";
            break;
        case 2:  // Перевод
            helpText =
                "Помощь в упражнении на перевод:\n\n"
                "Напишите испанский перевод слова, которое дано вам на русском.\n"
                "Ударения над гласными и уникальные буквы для испанского важны, приложение вам их "
                "не "
                "простит.\n"
                "Пример: 'Дерево' должно переводиться как 'Árbol'";
            break;
        case 3:  // Грамматика
            helpText =
                "Помощь в упражнении на грамматику:\n\n"
                "Выберите правильное слово или форму слова, чтобы дополнить предложение.\n"
                "Обращайте внимание на лицо и время.\n"
                "Пример: 'Las ventanas ___ (soy) grandes' должно быть дополнено с помощь 'son'";
            break;
        default:
            helpText = "EEEEEEL PRIMOOOOOO";
            break;
    }

    QMessageBox::information(this, "Help", helpText);
}

void Duolingo::ShowDifficultyDialog() {
    QStringList difficulties = {"Начинающий", "Продвинутый", "Эксперт", "Носитель"};
    bool ok;
    QString newDifficulty = QInputDialog::getItem(
        this, "Выбор сложности", "Выберите уровень сложности:", difficulties,
        difficulties.indexOf(currentDifficulty), false, &ok);

    if (ok) {
        currentDifficulty = newDifficulty;
        QMessageBox::information(
            this, "Сложность изменена",
            QString("Установлена сложность: %1").arg(currentDifficulty));
    }
}

void Duolingo::PlaySound(const bool correct) {
    QAudioOutput* audioOutput = new QAudioOutput(this);  // NOLINT
    player->setAudioOutput(audioOutput);

    if (correct) {
        player->setSource(QUrl("qrc:/sounds/correct.wav"));
    } else {
        player->setSource(QUrl("qrc:/sounds/wrong.wav"));
    }
    player->play();
}

/*void Duolingo::keyPressEvent(QKeyEvent* event) {
    qDebug() << "here";
    if (event->key() == Qt::Key_H) {
        QWidget* focusedWidget = QApplication::focusWidget();
        if (!focusedWidget || focusedWidget == this) {
            if (!qobject_cast<QTextEdit*>(focusedWidget)) {
                ShowHelp();
            }
        }
    } else {
        QMainWindow::keyPressEvent(event);
    }
}*/

bool Duolingo::eventFilter(QObject* obj, QEvent* event) {
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
    if (keyEvent->key() == Qt::Key_H) {
        QWidget* focusedWidget = QApplication::focusWidget();
        if (!qobject_cast<QTextEdit*>(focusedWidget) && !qobject_cast<QLineEdit*>(focusedWidget)) {
            ShowHelp();
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void Duolingo::ShowRating() {
    QMessageBox::information(this, "Рейтинг", "Функция рейтинга пока в разработке.");
}

Duolingo::~Duolingo() {
    delete player;
    delete exerciseTimer;
    delete countdownTimer;
}