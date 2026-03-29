#include <QApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <algorithm>
#include <deque>
#include <functional>
#include <random>
#include <vector>

namespace Constants {
constexpr int TotalBarHeight = 30;
constexpr int TableButtonSize = 30;
constexpr int TableButtonMargin = 5;
constexpr int LeftContainerMinWidth = 91;
constexpr int LeftContainerMaxWidth = 200;
constexpr int RightContainerMinWidth = 91;
constexpr int RightContainerMaxWidth = 200;
constexpr int Button2_MaxWidth = 200;
constexpr int Button2_MaxHeight = 200;
constexpr int Button5_MaxWidth = 200;
constexpr int Button5_MaxHeight = 200;
constexpr int TableWidget_MaxWidth = 200;
constexpr int TableWidget_MaxHeight = 300;
constexpr int TicketWidget_MinWidth = 150;
constexpr int TicketWidget_MinHeight = 150;
constexpr int DefaultButton_MinSize = 50;
}  // namespace Constants

class DynamicTotalBar : public QWidget {
   public:
    explicit DynamicTotalBar(QWidget* parent = nullptr)
        : QWidget(parent), greenCount(0), yellowCount(0), greyCount(0) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setFixedHeight(Constants::TotalBarHeight);
    }

    void updateCounts(int green, int yellow, int grey) {
        greenCount = green;
        yellowCount = yellow;
        greyCount = grey;
        update();
    }

   protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        int w = width();
        int total = greenCount + yellowCount + greyCount;
        if (total == 0) {
            total = 1;
        }
        int greenWidth = w * greenCount / total;
        int yellowWidth = w * yellowCount / total;
        int greyWidth = w - greenWidth - yellowWidth;
        painter.fillRect(0, 0, greenWidth, Constants::TotalBarHeight, Qt::green);
        painter.fillRect(greenWidth, 0, yellowWidth, Constants::TotalBarHeight, Qt::yellow);
        painter.fillRect(
            greenWidth + yellowWidth, 0, greyWidth, Constants::TotalBarHeight, Qt::gray);
        int percGreen = greenCount * 100 / total;
        int percYellow = yellowCount * 100 / total;
        int percGrey = greyCount * 100 / total;
        QString text = QString("%1%% / %2%% / %3%%").arg(percGreen).arg(percYellow).arg(percGrey);
        painter.drawText(rect(), Qt::AlignCenter, text);
    }

   private:
    int greenCount, yellowCount, greyCount;
};

class TicketButton : public QPushButton {
   public:
    enum State { Grey, Yellow, Green };

    explicit TicketButton(const QString& text, QWidget* parent = nullptr)
        : QPushButton(text, parent), state(Grey) {
        updateStyle();
    }

    State getState() const {
        return state;
    }

    void setState(State newState) {
        state = newState;
        updateStyle();
    }

    void setOnStateChanged(std::function<void()> callback) {
        onStateChanged = callback;
    }

   protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override {
        if (state == Grey) {
            setState(Green);
        } else if (state == Green) {
            setState(Yellow);
        } else if (state == Yellow) {
            setState(Green);
        }

        if (onStateChanged) {
            onStateChanged();
        }
        event->accept();
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        QPushButton::mouseReleaseEvent(event);
    }

   private:
    void updateStyle() {
        switch (state) {
            case Grey:
                setStyleSheet("background-color: grey;");
                break;
            case Yellow:
                setStyleSheet("background-color: yellow;");
                break;
            case Green:
                setStyleSheet("background-color: green;");
                break;
        }
    }

    State state;
    std::function<void()> onStateChanged;
};

class TableWidget : public QScrollArea {
   public:
    explicit TableWidget(QWidget* parent = nullptr) : QScrollArea(parent) {
        container = new QWidget;
        gridLayout = new QGridLayout(container);
        gridLayout->setContentsMargins(
            Constants::TableButtonMargin, Constants::TableButtonMargin,
            Constants::TableButtonMargin, Constants::TableButtonMargin);
        gridLayout->setSpacing(5);
        container->setLayout(gridLayout);
        setWidget(container);
        setWidgetResizable(true);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }

    void updateTickets(
        int count, const std::vector<TicketButton::State>& states,
        std::function<void(int)> callbackSelected, std::function<void(int)> callbackStateChange) {
        QLayoutItem* item;
        while ((item = gridLayout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
        buttons.clear();

        for (int i = 0; i < count; ++i) {
            TicketButton* btn = new TicketButton(QString::number(i + 1), container);
            btn->setFixedSize(Constants::TableButtonSize, Constants::TableButtonSize);
            btn->setState(states[i]);
            btn->setOnStateChanged([i, callbackStateChange]() { callbackStateChange(i); });
            connect(btn, &QPushButton::clicked, [i, callbackSelected]() { callbackSelected(i); });
            buttons.push_back(btn);
            gridLayout->addWidget(btn, i / 2, i % 2);
        }
        container->update();
    }

    const std::vector<QPushButton*>& getButtons() const {
        return buttons;
    }

   private:
    QWidget* container;
    QGridLayout* gridLayout;
    std::vector<QPushButton*> buttons;
};

struct TicketData {
    int number;
    TicketButton::State state;
    QString name;
    QString question;
    QString hint;
};

class TicketWidget : public QGroupBox {
   public:
    explicit TicketWidget(QWidget* parent = nullptr) : QGroupBox(parent), editingMode(false) {
        ticketData = {0, TicketButton::Grey, "Билет 1", "", ""};
        setTitle("");
        setStyleSheet(
            "QGroupBox { border: 1px solid gray; border-radius: 5px; margin-top: 20px; }"
            "QGroupBox::title { subcontrol-origin: margin; left: 50%; padding: 0 3px; }");

        QVBoxLayout* mainLayout = new QVBoxLayout;
        mainLayout->setContentsMargins(10, 30, 10, 10);
        mainLayout->setSpacing(10);

        QHBoxLayout* topLayout = new QHBoxLayout;
        topLayout->setContentsMargins(0, 0, 0, 0);

        ticketNumberLabel = new QLabel;
        QFont numFont = ticketNumberLabel->font();
        numFont.setPointSize(14);
        numFont.setBold(true);
        ticketNumberLabel->setFont(numFont);
        ticketNumberLabel->setAlignment(Qt::AlignCenter);

        modeToggleButton = new QPushButton("✎");
        modeToggleButton->setFixedSize(24, 24);
        connect(modeToggleButton, &QPushButton::clicked, [this]() { toggleMode(); });

        topLayout->addStretch();
        topLayout->addWidget(ticketNumberLabel);
        topLayout->addStretch();
        topLayout->addWidget(modeToggleButton);

        ticketNameLabel = new QLabel;
        QFont nameFont = ticketNameLabel->font();
        nameFont.setPointSize(18);
        nameFont.setBold(true);
        ticketNameLabel->setFont(nameFont);
        ticketNameLabel->setAlignment(Qt::AlignCenter);
        ticketNameLabel->setStyleSheet("border: 1px solid black;");

        ticketNameEdit = new QLineEdit;
        ticketNameEdit->setFont(nameFont);
        ticketNameEdit->setAlignment(Qt::AlignCenter);
        ticketNameEdit->hide();

        questionLabel = new QLabel;
        QFont questionFont = questionLabel->font();
        questionFont.setPointSize(12);
        questionLabel->setFont(questionFont);
        questionLabel->setAlignment(Qt::AlignCenter);
        questionLabel->setWordWrap(true);
        questionLabel->setStyleSheet("border: 1px solid black;");

        questionEdit = new QTextEdit;
        questionEdit->setFont(questionFont);
        questionEdit->hide();
        questionEdit->setFixedHeight(80);

        QHBoxLayout* bottomLayout = new QHBoxLayout;
        bottomLayout->setContentsMargins(0, 0, 0, 0);

        showHintButton = new QPushButton("Показать подсказку");
        showHintButton->setFixedSize(120, 30);
        connect(showHintButton, &QPushButton::clicked, [this]() { toggleHintVisibility(); });

        hintLabel = new QLabel;
        hintLabel->setFont(questionFont);
        hintLabel->setAlignment(Qt::AlignCenter);
        hintLabel->setWordWrap(true);
        hintLabel->setStyleSheet("border: 1px solid black;");
        hintLabel->hide();

        hintEdit = new QTextEdit;
        hintEdit->setFont(questionFont);
        hintEdit->setFixedHeight(60);
        hintEdit->hide();

        bottomLayout->addStretch();
        bottomLayout->addWidget(showHintButton);

        mainLayout->addLayout(topLayout);
        mainLayout->addWidget(ticketNameLabel);
        mainLayout->addWidget(ticketNameEdit);
        mainLayout->addWidget(questionLabel);
        mainLayout->addWidget(questionEdit);
        mainLayout->addWidget(hintLabel);
        mainLayout->addWidget(hintEdit);
        mainLayout->addLayout(bottomLayout);

        setLayout(mainLayout);
    }

    TicketData getTicketData() const {
        return ticketData;
    }

    void setTicketData(const TicketData& data) {
        ticketData = data;
        ticketNumberLabel->setText(QString::number(ticketData.number));
        ticketNameLabel->setText(ticketData.name);
        questionLabel->setText(ticketData.question);
        hintLabel->setText(ticketData.hint);
        ticketNameEdit->setText(ticketData.name);
        questionEdit->setText(ticketData.question);
        hintEdit->setText(ticketData.hint);
        hintLabel->hide();
        showHintButton->setVisible(editingMode || !ticketData.hint.isEmpty());
    }

   private:
    void toggleMode() {
        if (editingMode) {
            ticketData.name = ticketNameEdit->text();
            ticketData.question = questionEdit->toPlainText();
            ticketData.hint = hintEdit->toPlainText();
            ticketNameLabel->setText(ticketData.name);
            questionLabel->setText(ticketData.question);
            hintLabel->setText(ticketData.hint);
            ticketNameEdit->hide();
            questionEdit->hide();
            hintEdit->hide();
            ticketNameLabel->show();
            questionLabel->show();
            hintLabel->setVisible(!ticketData.hint.isEmpty());
            showHintButton->setVisible(!ticketData.hint.isEmpty());
            modeToggleButton->setText("✎");
            editingMode = false;
        } else {
            ticketNameEdit->setText(ticketData.name);
            questionEdit->setText(ticketData.question);
            hintEdit->setText(ticketData.hint);
            ticketNameLabel->hide();
            questionLabel->hide();
            hintLabel->hide();
            ticketNameEdit->show();
            questionEdit->show();
            showHintButton->show();
            hintEdit->show();
            modeToggleButton->setText("✔");
            editingMode = true;
        }
    }

    void toggleHintVisibility() {
        if (editingMode) {
            hintEdit->setVisible(true);
        } else {
            hintLabel->setVisible(!hintLabel->isVisible());
        }
    }

    bool editingMode;
    TicketData ticketData;
    QLabel* ticketNumberLabel;
    QPushButton* modeToggleButton;
    QLabel* ticketNameLabel;
    QLineEdit* ticketNameEdit;
    QLabel* questionLabel;
    QTextEdit* questionEdit;
    QLabel* hintLabel;
    QTextEdit* hintEdit;
    QPushButton* showHintButton;
};

class FrontMain : public QWidget {
   public:
    explicit FrontMain(QWidget* parent = nullptr) : QWidget(parent) {
        setWindowTitle("Прокрастинация");
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(5, 5, 5, 5);
        mainLayout->setSpacing(5);

        totalBar = new DynamicTotalBar;
        mainLayout->addWidget(totalBar);

        QHBoxLayout* bottomLayout = new QHBoxLayout;
        bottomLayout->setSpacing(5);

        QWidget* leftContainer = new QWidget;
        leftContainer->setMinimumWidth(Constants::LeftContainerMinWidth);
        leftContainer->setMaximumWidth(Constants::LeftContainerMaxWidth);
        QGridLayout* leftLayout = new QGridLayout(leftContainer);
        leftLayout->setContentsMargins(0, 0, 0, 0);

        btnPrev = new QPushButton("← Назад");
        btnPrev->setMaximumSize(Constants::Button2_MaxWidth, Constants::Button2_MaxHeight);
        btnPrev->setStyleSheet(
            "QPushButton {"
            "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f6f7fa, stop:1 #dadbde);"
            "  border: 1px solid #8f8f91;"
            "  border-radius: 5px;"
            "  padding: 5px;"
            "  font-weight: bold;"
            "}"
            "QPushButton:pressed {"
            "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #dadbde, stop:1 #f6f7fa);"
            "}");

        ticketCountSpinBox = new QSpinBox;
        ticketCountSpinBox->setMinimum(1);
        ticketCountSpinBox->setMaximum(100);
        ticketCountSpinBox->setValue(1);

        leftLayout->addWidget(btnPrev, 0, 0);
        leftLayout->addWidget(ticketCountSpinBox, 2, 0);
        leftLayout->setRowStretch(0, 1);
        leftLayout->setRowStretch(1, 1);
        leftLayout->setRowStretch(2, 1);

        ticketWidget = new TicketWidget;
        ticketWidget->setMinimumSize(
            Constants::TicketWidget_MinWidth, Constants::TicketWidget_MinHeight);
        ticketWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        QWidget* rightContainer = new QWidget;
        rightContainer->setMinimumWidth(Constants::RightContainerMinWidth);
        rightContainer->setMaximumWidth(Constants::RightContainerMaxWidth);
        QGridLayout* rightLayout = new QGridLayout(rightContainer);
        rightLayout->setContentsMargins(0, 0, 0, 0);

        btnNext = new QPushButton("Вперед →");
        btnNext->setMaximumSize(Constants::Button5_MaxWidth, Constants::Button5_MaxHeight);
        btnNext->setStyleSheet(
            "QPushButton {"
            "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f6f7fa, stop:1 #dadbde);"
            "  border: 1px solid #8f8f91;"
            "  border-radius: 5px;"
            "  padding: 5px;"
            "  font-weight: bold;"
            "}"
            "QPushButton:pressed {"
            "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #dadbde, stop:1 #f6f7fa);"
            "}");

        tableWidget = new TableWidget;
        tableWidget->setMaximumSize(
            Constants::TableWidget_MaxWidth, Constants::TableWidget_MaxHeight);
        tableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        rightLayout->addWidget(btnNext, 0, 0);
        rightLayout->addWidget(tableWidget, 2, 0);
        rightLayout->setRowStretch(0, 1);
        rightLayout->setRowStretch(1, 1);
        rightLayout->setRowStretch(2, 1);

        bottomLayout->addWidget(leftContainer);
        bottomLayout->addWidget(ticketWidget, 2);
        bottomLayout->addWidget(rightContainer);
        mainLayout->addLayout(bottomLayout, 2);

        tickets.resize(1);
        tickets[0] = {1, TicketButton::Grey, "Билет 1", "", ""};
        currentTicketIndex = 0;
        ticketWidget->setTicketData(tickets[0]);

        historyQueue.push_back(0);
        updateButtonLabels();
        updateTableAndTotalBar();

        connect(
            ticketCountSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this](int newCount) {
                tickets.clear();
                tickets.resize(newCount);
                for (int i = 0; i < newCount; ++i) {
                    tickets[i] = {
                      i + 1, TicketButton::Grey, QString("Билет %1").arg(i + 1), "", ""};
                }
                currentTicketIndex = 0;
                if (!tickets.empty()) {
                    ticketWidget->setTicketData(tickets[0]);
                }

                historyQueue.clear();
                historyQueue.push_back(0);
                currentHistoryPos = 0;
                updateButtonLabels();
                updateTableAndTotalBar();
            });

        connect(btnPrev, &QPushButton::clicked, [this]() { navigateBack(); });
        connect(btnNext, &QPushButton::clicked, [this]() { navigateForward(); });
    }

   private:
    void updateTableAndTotalBar() {
        std::vector<TicketButton::State> states;
        for (const auto& t : tickets) {
            states.push_back(t.state);
        }

        tableWidget->updateTickets(
            static_cast<int>(tickets.size()), states,
            [this](int index) {
                if (currentTicketIndex >= 0 && currentTicketIndex < tickets.size()) {
                    tickets[currentTicketIndex] = ticketWidget->getTicketData();
                }
                currentTicketIndex = index;
                ticketWidget->setTicketData(tickets[index]);
                updateButtonLabels();
            },
            [this](int index) {
                TicketButton* btn = static_cast<TicketButton*>(tableWidget->getButtons()[index]);
                tickets[index].state = btn->getState();
                updateTotalBar();
            });
        updateTotalBar();
    }

    void updateTotalBar() {
        int green = 0, yellow = 0, grey = 0;
        for (const auto& t : tickets) {
            switch (t.state) {
                case TicketButton::Green:
                    green++;
                    break;
                case TicketButton::Yellow:
                    yellow++;
                    break;
                case TicketButton::Grey:
                    grey++;
                    break;
            }
        }
        totalBar->updateCounts(green, yellow, grey);
    }

    void navigateBack() {
        if (currentHistoryPos > 0) {
            currentHistoryPos--;
            loadTicketFromHistory();
            updateButtonLabels();
        } else {
            QMessageBox::information(this, "Информация", "Вы в начале истории навигации");
        }
    }

    void navigateForward() {
        if (currentHistoryPos < historyQueue.size() - 1) {
            currentHistoryPos++;
            loadTicketFromHistory();
            updateButtonLabels();
        } else {
            int nextTicket = findNextRandomTicket();
            if (nextTicket == -1) {
                QMessageBox::information(this, "Информация", "Все билеты завершены!");
                return;
            }

            if (historyQueue.size() >= 50) {
                historyQueue.pop_front();
                currentHistoryPos--;
            }

            historyQueue.push_back(nextTicket);
            currentHistoryPos++;
            loadTicketFromHistory();
            updateButtonLabels();
        }
    }

    int findNextRandomTicket() {
        std::vector<int> candidates;
        for (int i = 0; i < tickets.size(); ++i) {
            if (i != currentTicketIndex && tickets[i].state != TicketButton::Green) {
                candidates.push_back(i);
            }
        }

        if (candidates.empty()) {
            return -1;
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distr(0, candidates.size() - 1);
        return candidates[distr(gen)];
    }

    void loadTicketFromHistory() {
        currentTicketIndex = historyQueue[currentHistoryPos];
        ticketWidget->setTicketData(tickets[currentTicketIndex]);
    }

    void updateButtonLabels() {
        QString prevText = "← Назад";
        if (currentHistoryPos > 0) {
            int prevIdx = historyQueue[currentHistoryPos - 1];
            prevText = QString("← %1").arg(tickets[prevIdx].name);
        }
        btnPrev->setText(prevText);

        QString nextText = "Вперед →";
        if (currentHistoryPos < historyQueue.size() - 1) {
            int nextIdx = historyQueue[currentHistoryPos + 1];
            nextText = QString("%1 →").arg(tickets[nextIdx].name);
        }
        btnNext->setText(nextText);
    }

    DynamicTotalBar* totalBar;
    TicketWidget* ticketWidget;
    TableWidget* tableWidget;
    QSpinBox* ticketCountSpinBox;
    QPushButton* btnPrev;
    QPushButton* btnNext;
    std::vector<TicketData> tickets;
    std::deque<int> historyQueue;
    size_t currentHistoryPos = 0;
    int currentTicketIndex;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    FrontMain window;
    window.resize(800, 600);
    window.show();
    return app.exec();
}
