#include <QApplication>
#include <QCheckBox>
#include <QDateTime>
#include <QFile>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLabel>
#include <QListWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QStringList>
#include <QTableWidget>
#include <QTextStream>
#include <QTime>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <random>
#include <vector>

// Предварительное объявление GameEngine
class GameEngine;

// Структуры для хранения данных
struct LevelRecord {
    int level;
    QTime bestTime;  // время, затраченное от старта сессии до достижения уровня
};

struct ScoreRecord {
    QDateTime timestamp;
    int score;
    QString text;
};

// Класс блока (наследуется от QLabel)
class Block : public QLabel {
   public:
    Block(QWidget* parent, double relativeX, double speed, int leftMargin, int rightMargin)
        : QLabel(parent)
        , relativeX(relativeX)
        , yPosition(0.0)
        , speed(speed)
        , active(true)
        , leftMargin(leftMargin)
        , rightMargin(rightMargin) {
        setFixedSize(100, 50);
        setAlignment(Qt::AlignCenter);
        setStyleSheet("background-color: rgb(50, 50, 50); border: 2px solid black;");
        updatePosition();
    }

    // Реализация вынесена за пределы класса (после определения GameEngine)
    void updatePosition();

    void updateTextDisplay(const QString& inputBuffer) {
        QString coloredText;
        for (int i = 0; i < text.size(); i++) {
            if (i < inputBuffer.size()) {
                if (text[i] == inputBuffer[i]) {
                    coloredText += QStringLiteral("<font color='cyan'>") + QString(1, text[i]) +
                                   QStringLiteral("</font>");
                } else {
                    coloredText += QStringLiteral("<font color='red'>") + QString(1, text[i]) +
                                   QStringLiteral("</font>");
                }
            } else {
                coloredText += text[i];
            }
        }
        if (inputBuffer.size() > text.size()) {
            coloredText += "<font color='red'>...</font>";
        }
        QLabel::setText(coloredText);
    }

    bool isActive() const {
        return active;
    }

    void setFocusBlock(bool isFocused) {
        if (isFocused) {
            setStyleSheet("background-color: rgb(150, 0, 0); border: 2px solid darkred;");
            raise();  // Поднимаем блок на верхний слой
        } else {
            setStyleSheet("background-color: rgb(50, 50, 50); border: 2px solid black;");
        }
    }

    QString getText() const {
        return text;
    }

    void setBlockText(const QString& newText) {
        text = newText;
        QLabel::setText(newText);
    }

    double getYPosition() const {
        return yPosition;
    }

   private:
    double relativeX;
    double yPosition;
    double speed;
    bool active;
    int leftMargin;
    int rightMargin;
    QString text;
};

// Класс игрового движка, реализующий игровую логику.
class GameEngine : public QWidget {
   public:
    GameEngine(QWidget* parent = nullptr)
        : QWidget(parent)
        , gameRunning(false)
        , score(0)
        , level(1)
        , nextLevelRequirement(100)
        , speed(0.01)
        , totalCharsTyped(0)
        , playAreaX(0.0)
        , playAreaY(0.0)
        , playAreaW(1.0)
        , playAreaH(1.0)
        , currentWordIndex(0)  // Добавляем индекс текущего слова
    {
        allWords = {
          "that",     "this",   "with",     "from",     "your",    "have",    "more",     "will",
          "home",     "about",  "page",     "search",   "free",    "other",   "time",     "they",
          "site",     "what",   "which",    "their",    "news",    "there",   "only",     "when",
          "contact",  "here",   "business", "also",     "help",    "view",    "online",   "first",
          "been",     "would",  "were",     "services", "some",    "these",   "click",    "like",
          "service",  "than",   "find",     "price",    "date",    "back",    "people",   "list",
          "name",     "just",   "over",     "state",    "year",    "into",    "email",    "health",
          "world",    "next",   "used",     "work",     "last",    "most",    "products", "music",
          "data",     "make",   "them",     "should",   "product", "system",  "post",     "city",
          "policy",   "number", "such",     "please",   "support", "message", "after",    "best",
          "software", "then",   "good",     "video",    "well",    "where",   "info",     "rights",
          "public",   "books",  "high",     "school",   "through", "each",    "links",    "review",
          "years",    "order",  "very",     "privacy"};
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(allWords.begin(), allWords.end(), g);

        setFocusPolicy(Qt::StrongFocus);
        updateTimer = new QTimer(this);
        connect(updateTimer, &QTimer::timeout, this, [this]() { updateBlocks(); });
        spawnTimer = new QTimer(this);
        connect(spawnTimer, &QTimer::timeout, this, [this]() { spawnBlock(); });
    }

    void loadWords(const QString& filename) {
        QFile file("google-6648-english.txt");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning("Could not open words file");
            return;
        }

        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty()) {
                allWords.append(line);
            }
        }
        file.close();

        // Перемешиваем слова один раз при загрузке
        std::random_shuffle(allWords.begin(), allWords.end());
    }

    // Установка относительных координат игровой области
    void setPlayArea(double relX, double relY, double relW, double relH) {
        playAreaX = relX;
        playAreaY = relY;
        playAreaW = relW;
        playAreaH = relH;
    }

    // Вычисление игровой области с учетом размеров родительского окна
    QRect getPlayAreaRect() const {
        QWidget* p = parentWidget();
        if (p) {
            int absX = static_cast<int>(playAreaX * p->width());
            int absY = static_cast<int>(playAreaY * p->height());
            int absW = static_cast<int>(playAreaW * p->width());
            int absH = static_cast<int>(playAreaH * p->height());
            return QRect(absX, absY, absW, absH);
        }
        return QRect(0, 0, width(), height());
    }

    // Callback-функции для обратной связи с UI
    std::function<void(int, int)> onScoreUpdated;  // (score, level)
    std::function<void(int)> onProgressUpdated;    // progress (%)
    // Передается предыдущий уровень и время, прошедшее от старта сессии до достижения уровня
    std::function<void(int, QTime)> onLevelUp;
    std::function<void(const ScoreRecord&)> onGameOver;  // при окончании игры

    // Запуск игры
    void startGame() {
        gameRunning = true;
        clearBlocks();
        resetGameStats();
        totalCharsTyped = 0;
        startTime = QTime::currentTime();
        level = 1;
        nextLevelRequirement = 10 * level * (level + 9);  // Инициализация для уровня 1
        QTime elapsed = QTime::fromMSecsSinceStartOfDay(0);
        if (onLevelUp) {
            onLevelUp(level, elapsed);
        }
        addBlock();
        setFocus();
        updateTimer->start(50);
        // Первый запуск таймера спавна с правильной задержкой
        int textLength = blocks.last()->getText().size();
        int delay = 250 + static_cast<int>(500 * textLength / level);
        spawnTimer->start(delay);
        if (onProgressUpdated) {
            onProgressUpdated(0);
        }
    }

    // Остановка игры
    void stopGame() {
        updateTimer->stop();
        spawnTimer->stop();
        gameRunning = false;
        recordScore();
    }

    bool isGameRunning() const {
        return gameRunning;
    }

    int getTotalCharsTyped() const {
        return totalCharsTyped;
    }

    QTime getStartTime() const {
        return startTime;
    }

   protected:
    void keyPressEvent(QKeyEvent* event) override {
        if (!gameRunning) {
            if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
                event->accept();
                return;
            }
            QWidget::keyPressEvent(event);
            return;
        }
        if (event->key() == Qt::Key_Escape) {
            stopGame();
            return;
        } else if (event->key() == Qt::Key_Backspace) {
            if (!inputBuffer.isEmpty()) {
                inputBuffer.chop(1);
            }
            if (!blocks.isEmpty()) {
                blocks.first()->updateTextDisplay(inputBuffer);
            }
            return;
        } else {
            QString keyText = event->text();
            if (!keyText.isEmpty()) {
                inputBuffer.append(keyText);
                totalCharsTyped += keyText.size();
            }
        }
        if (!blocks.isEmpty()) {
            blocks.first()->updateTextDisplay(inputBuffer);
            if (inputBuffer.trimmed() == blocks.first()->getText()) {
                // Новое начисление очков
                int textLength = blocks.first()->getText().size();
                score += (textLength + 4) * level;
                if (onScoreUpdated) {
                    onScoreUpdated(score, level);
                }
                checkLevelUp();
                delete blocks.takeFirst();
                inputBuffer.clear();
                if (blocks.isEmpty()) {
                    addBlock();
                    spawnTimer->start(2000);
                } else {
                    blocks.first()->setFocusBlock(true);
                }
                updateProgress();
            }
        }
    }

   private:
    void prepareWords() {
        currentWords.clear();
        for (int i = 0; i < 50 && i < allWords.size(); ++i) {
            currentWords.append(allWords[i]);
        }
        currentWordIndex = 0;
    }

    void updateBlocks() {
        bool allStopped = true;
        bool firstBlockReachedBottom = false;
        for (Block* block : blocks) {
            if (block->isActive()) {
                block->updatePosition();
                allStopped = false;
                if (block->getYPosition() >= 1.0 && !firstBlockReachedBottom) {
                    firstBlockReachedBottom = true;
                }
            }
        }
        if (firstBlockReachedBottom) {
            stopGame();
            return;
        }
        if (allStopped) {
            updateTimer->stop();
            spawnTimer->stop();
        }
    }

    void addBlock() {
        if (currentWords.isEmpty()) {
            prepareWords();  // Повторно заполняем слова если закончились
        }

        double relX = getRandomX();
        int leftMargin = 10, rightMargin = 10;
        Block* block = new Block(this, relX, speed, leftMargin, rightMargin);

        // Берем следующее слово из списка
        if (currentWordIndex < currentWords.size()) {
            block->setBlockText(currentWords[currentWordIndex++]);
        } else {
            // Если слова закончились - начинаем сначала
            prepareWords();
            block->setBlockText(currentWords[currentWordIndex++]);
        }

        block->setAlignment(Qt::AlignCenter);
        block->show();
        blocks.append(block);
        if (blocks.size() == 1) {
            block->setFocusBlock(true);
        }
    }

    void spawnBlock() {
        addBlock();
        int textLength = blocks.last()->getText().size();
        int delay = 250 + static_cast<int>(500 * textLength / level);
        spawnTimer->start(delay);
    }

    void clearBlocks() {
        for (Block* block : blocks) {
            delete block;
        }
        blocks.clear();
    }

    double getRandomX() {
        return static_cast<double>(rand()) / RAND_MAX;
    }

    void resetGameStats() {
        score = 0;
        level = 1;
        inputBuffer.clear();
    }

    void checkLevelUp() {
        if (score >= nextLevelRequirement) {
            level++;
            // Обновляем требование для следующего уровня
            nextLevelRequirement += 10 * level * (level + 9);
            QTime currentTime = QTime::currentTime();
            int msecsElapsed = startTime.msecsTo(currentTime);
            QTime elapsed = QTime::fromMSecsSinceStartOfDay(msecsElapsed);
            if (onLevelUp) {
                onLevelUp(level, elapsed);
            }
            updateProgress();
        }
    }

    void updateProgress() {
        int progress = 0;
        if (nextLevelRequirement > 0) {
            int baseScore = nextLevelRequirement - (10 * level * (level + 9));
            int currentProgress = score - baseScore;
            progress = (currentProgress * 100) / (10 * level * (level + 9));
            progress = std::min(progress, 100);  // Ограничиваем прогресс 100%
        }
        if (onProgressUpdated) {
            onProgressUpdated(progress);
        }
    }

    void recordScore() {
        QString timestampStr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        ScoreRecord newRecord;
        newRecord.timestamp = QDateTime::fromString(timestampStr, "yyyy-MM-dd hh:mm:ss");
        newRecord.score = score;
        newRecord.text = timestampStr + " - " + QString::number(score);
        if (onGameOver) {
            onGameOver(newRecord);
        }
    }

    QStringList allWords;
    QStringList currentWords;
    int currentWordIndex;
    int nextLevelRequirement;
    bool gameRunning;
    QList<Block*> blocks;
    QString inputBuffer;
    int score;
    int level;
    double speed;
    int totalCharsTyped;
    QTimer* updateTimer;
    QTimer* spawnTimer;
    QTime startTime;
    double playAreaX, playAreaY, playAreaW, playAreaH;
};

// Реализация метода Block::updatePosition()
// (Теперь GameEngine полностью определён)
void Block::updatePosition() {
    if (!active) {
        return;
    }
    yPosition += speed;
    if (yPosition >= 1.0) {
        yPosition = 1.0;
        active = false;
    }
    QRect playArea = static_cast<GameEngine*>(parentWidget())->getPlayAreaRect();
    int maxWidth = playArea.width() - leftMargin - rightMargin - width();
    int xPos = playArea.x() + leftMargin + relativeX * maxWidth;
    int yPos = playArea.y() + yPosition * (playArea.height() - height());
    move(xPos, yPos);
}

// Виджет таблицы рекордов уровней
class LevelTableWidget : public QTableWidget {
   public:
    LevelTableWidget(QWidget* parent = nullptr) : QTableWidget(0, 3, parent) {
        setHorizontalHeaderLabels(QStringList() << "Lvl" << "Time" << "Diff");
        setFixedWidth(250);
        verticalHeader()->setVisible(false);
        setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    void updateLevel(int level, const QTime& elapsed) {
        QString diffStr;
        QColor diffColor;
        auto it =
            std::find_if(levelRecords.begin(), levelRecords.end(), [level](const LevelRecord& rec) {
                return rec.level == level;
            });
        if (it == levelRecords.end()) {
            LevelRecord newRec = {level, elapsed};
            levelRecords.push_back(newRec);
            diffStr = "Record";
            diffColor = Qt::green;
        } else {
            if (elapsed < it->bestTime) {
                int improvement = it->bestTime.msecsTo(elapsed);
                diffStr = QString("+%1 ms").arg(-improvement);
                diffColor = Qt::green;
                it->bestTime = elapsed;
            } else if (elapsed > it->bestTime) {
                int delay = it->bestTime.msecsTo(elapsed);
                diffStr = QString("-%1 ms").arg(delay);
                diffColor = Qt::red;
            } else {
                diffStr = "Equal";
                diffColor = Qt::black;
            }
        }
        insertRow(0);
        setItem(0, 0, new QTableWidgetItem(QString("Lvl %1").arg(level)));
        setItem(0, 1, new QTableWidgetItem(elapsed.toString("mm:ss.zzz")));
        QTableWidgetItem* diffItem = new QTableWidgetItem(diffStr);
        diffItem->setForeground(diffColor);
        setItem(0, 2, diffItem);
        while (rowCount() > 10) {
            removeRow(rowCount() - 1);
        }
    }

   private:
    std::vector<LevelRecord> levelRecords;
};

// Виджет списка рекордов (ScoreBoard)
class ScoreBoardWidget : public QWidget {
   public:
    ScoreBoardWidget(QWidget* parent = nullptr) : QWidget(parent) {
        QVBoxLayout* layout = new QVBoxLayout(this);
        scoreList = new QListWidget(this);
        scoreList->setFixedWidth(200);
        layout->addWidget(scoreList);
    }

    void addScoreRecord(const ScoreRecord& record) {
        records.push_back(record);
        if (records.size() > 10) {
            auto worstIt = std::min_element(
                records.begin(), records.end(), [](const ScoreRecord& a, const ScoreRecord& b) {
                    if (a.score == b.score) {
                        return a.timestamp > b.timestamp;
                    }
                    return a.score < b.score;
                });
            if (worstIt != records.end()) {
                records.erase(worstIt);
            }
        }
        std::sort(records.begin(), records.end(), [](const ScoreRecord& a, const ScoreRecord& b) {
            if (a.score == b.score) {
                return a.timestamp < b.timestamp;
            }
            return a.score > b.score;
        });
        scoreList->clear();
        for (const ScoreRecord& r : records) {
            scoreList->addItem(r.text);
        }
    }

   private:
    QListWidget* scoreList;
    std::vector<ScoreRecord> records;
};

// Главное окно приложения
class MainWindow : public QWidget {
   public:
    MainWindow(QWidget* parent = nullptr) : QWidget(parent), extendedMode(false) {
        resize(800, 600);
        QGridLayout* mainLayout = new QGridLayout(this);
        mainLayout->setContentsMargins(5, 5, 5, 5);
        mainLayout->setSpacing(5);

        progressBar = new QProgressBar(this);
        progressBar->setRange(0, 100);
        progressBar->setValue(0);
        mainLayout->addWidget(progressBar, 0, 0, 1, 3);

        levelTable = new LevelTableWidget(this);
        levelTable->setColumnWidth(0, 50);
        levelTable->setColumnWidth(1, 50);
        levelTable->setColumnWidth(2, 50);
        levelTable->setFixedSize(166, 200);
        mainLayout->addWidget(levelTable, 1, 0, Qt::AlignTop);

        startButton = new QPushButton("Start Game", this);
        startButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        startButton->setStyleSheet(
            "background-color: rgba(0, 0, 0, 150); color: white; border: none;");
        mainLayout->addWidget(startButton, 1, 1);

        scoreBoard = new ScoreBoardWidget(this);
        scoreBoard->setFixedSize(196, 200);
        mainLayout->addWidget(scoreBoard, 1, 2, Qt::AlignTop);

        scoreLabel = new QLabel("Score: 0", this);
        mainLayout->addWidget(scoreLabel, 2, 0, 1, 2);

        // Кнопка переключения режима статистики
        toggleStatsButton = new QPushButton("Show Extended Stats", this);
        mainLayout->addWidget(toggleStatsButton, 2, 2);

        // Прогресс-бар в виде QProgressBar уже используется (обновляется через GameEngine)

        // Создаём игровой движок; игровая область берётся из позиции startButton
        gameEngine = new GameEngine(this);
        gameEngine->setGeometry(0, 0, width(), height());
        gameEngine->lower();

        // Обратные вызовы от игрового движка
        gameEngine->onScoreUpdated = [this](int score, int level) {
            updateScoreDisplay(score, level);
        };
        gameEngine->onProgressUpdated = [this](int progress) { progressBar->setValue(progress); };
        gameEngine->onLevelUp = [this](int lvl, QTime elapsed) {
            levelTable->updateLevel(lvl, elapsed);
        };
        gameEngine->onGameOver = [this](const ScoreRecord& rec) {
            scoreBoard->addScoreRecord(rec);
            startButton->show();
            gameEngine->clearFocus();
        };

        connect(startButton, &QPushButton::clicked, this, [this]() {
            // Вычисляем игровую область на основе позиции startButton
            double relX = static_cast<double>(startButton->x()) / width();
            double relY = static_cast<double>(startButton->y()) / height();
            double relW = static_cast<double>(startButton->width()) / width();
            double relH = static_cast<double>(startButton->height()) / height();
            gameEngine->setPlayArea(relX, relY, relW, relH);
            startButton->hide();
            gameEngine->startGame();
        });

        connect(toggleStatsButton, &QPushButton::clicked, this, [this]() {
            extendedMode = !extendedMode;
            if (extendedMode) {
                toggleStatsButton->setText("Show Minimal Stats");
            } else {
                toggleStatsButton->setText("Show Extended Stats");
            }
            updateScoreDisplay(lastScore, lastLevel);
        });
    }

    void keyPressEvent(QKeyEvent* event) override {
        if (!gameEngine->isGameRunning() &&
            (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)) {
            startButton->click();
            return;
        }
        QWidget::keyPressEvent(event);
    }

    // Обновление отображения статистики. Если extendedMode включён, показываем расширенную
    // статистику.
    void updateScoreDisplay(int score, int level) {
        lastScore = score;
        lastLevel = level;
        if (!extendedMode) {
            scoreLabel->setText(QString("Score: %1").arg(score));
        } else {
            // Вычисляем WPM: стандартная формула: (chars/5) / (minutes elapsed)
            QTime start = gameEngine->getStartTime();
            int msecs = start.msecsTo(QTime::currentTime());
            double minutes = msecs / 60000.0;
            double wpm = (minutes > 0) ? ((gameEngine->getTotalCharsTyped() / 5.0) / minutes) : 0.0;
            int words = gameEngine->getTotalCharsTyped() / 5;
            scoreLabel->setText(QString("Score: %1 | Lvl: %2 | Words: %3 | WPM: %4")
                                    .arg(score)
                                    .arg(level)
                                    .arg(words)
                                    .arg(QString::number(wpm, 'f', 2)));
        }
    }

   private:
    GameEngine* gameEngine;
    LevelTableWidget* levelTable;
    ScoreBoardWidget* scoreBoard;
    QProgressBar* progressBar;
    QLabel* scoreLabel;
    QPushButton* startButton;
    QPushButton* toggleStatsButton;
    int lastScore = 0;
    int lastLevel = 1;
    bool extendedMode;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
