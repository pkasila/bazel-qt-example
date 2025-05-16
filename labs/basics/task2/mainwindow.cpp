#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMessageBox>
#include <QAction>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), isXTurn(true), movesLeft(9) {
    ui->setupUi(this);

    setWindowTitle("Крестики-нолики");

    this->resize(500,600);

    QStringList symbols = {"X", "O", "♥", "★", "♦", "◆", "▲", "●", "◯"};

    ui->symbolChoice1->addItems(symbols);
    ui->symbolChoice2->addItems(symbols);

    ui->symbolChoice1->setCurrentIndex(0);
    ui->symbolChoice2->setCurrentIndex(1);

    ui->fontSizeSlider1->setValue(24);
    ui->fontSizeSlider2->setValue(24);

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            QString buttonName = QString("button_%1_%2").arg(row).arg(col);
            buttons[row][col] = findChild<QPushButton*>(buttonName);
            if (buttons[row][col]) {
                connect(buttons[row][col], &QPushButton::clicked, this, &MainWindow::handleButtonClick);
                buttons[row][col]->setFont(QFont("Arial", 24));
            }
        }
    }

    statusLabel = findChild<QLabel*>("statusLabel");

    connect(ui->actionDarkTheme, &QAction::triggered, this, &MainWindow::setDarkTheme);
    connect(ui->actionLightTheme, &QAction::triggered, this, &MainWindow::setLightTheme);

    connect(ui->symbolChoice1, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::synchronizeSymbols);
    connect(ui->symbolChoice2, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::synchronizeSymbols);

    setLightTheme();

    resetGame();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::handleButtonClick() {
    QPushButton *clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton || !clickedButton->text().isEmpty())
        return;

    QString currentSymbol = isXTurn ? ui->symbolChoice1->currentText() : ui->symbolChoice2->currentText();
    clickedButton->setText(currentSymbol);

    int fontSize = isXTurn ? ui->fontSizeSlider1->value() : ui->fontSizeSlider2->value();
    clickedButton->setFont(QFont("Arial", fontSize));

    movesLeft--;
    checkWinner();
    isXTurn = !isXTurn;
    updateGameStatus();
}

void MainWindow::checkWinner() {
    QString board[3][3];
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (buttons[row][col]) {
                board[row][col] = buttons[row][col]->text();
            }
        }
    }

    QStringList symbols = {ui->symbolChoice1->currentText(), ui->symbolChoice2->currentText()};
    QStringList playerNames = {
        ui->playerNameInput2->text().isEmpty() ? "Игрок 2" : ui->playerNameInput2->text(),
        ui->playerNameInput1->text().isEmpty() ? "Игрок 1" : ui->playerNameInput1->text()
    };

    for (int i = 0; i < symbols.size(); ++i) {
        QString symbol = symbols[i];
        QString playerName = playerNames[i];

        for (int j = 0; j < 3; ++j) {
            if ((board[j][0] == symbol && board[j][1] == symbol && board[j][2] == symbol) ||
                (board[0][j] == symbol && board[1][j] == symbol && board[2][j] == symbol)) {
                if (statusLabel) statusLabel->setText(QString("Победил %1!").arg(playerName));
                disableButtons();
                showWinnerDialog(playerName);
                return;
            }
        }

        if ((board[0][0] == symbol && board[1][1] == symbol && board[2][2] == symbol) ||
            (board[0][2] == symbol && board[1][1] == symbol && board[2][0] == symbol)) {
            if (statusLabel) statusLabel->setText(QString("Победил %1!").arg(playerName));
            disableButtons();
            showWinnerDialog(playerName);
            return;
        }
    }

    if (movesLeft == 0 && statusLabel) {
        statusLabel->setText("Ничья!");
        showDrawDialog();
    }
}

void MainWindow::showWinnerDialog(const QString &playerName) {
    QMessageBox messageBox;
    messageBox.setWindowTitle("Игра окончена");
    messageBox.setText(QString("Поздравляем! Победил %1!").arg(playerName));
    messageBox.setStandardButtons(QMessageBox::Ok);
    messageBox.setDefaultButton(QMessageBox::Ok);

    if (messageBox.exec() == QMessageBox::Ok) {
        resetGame();
    }
}

void MainWindow::showDrawDialog() {
    QMessageBox messageBox;
    messageBox.setWindowTitle("Игра окончена");
    messageBox.setText("Ничья! Никто не победил.");
    messageBox.setStandardButtons(QMessageBox::Ok);
    messageBox.setDefaultButton(QMessageBox::Ok);

    if (messageBox.exec() == QMessageBox::Ok) {
        resetGame();
    }
}

void MainWindow::updateGameStatus() {
    QString currentPlayer = isXTurn ? ui->playerNameInput2->text() : ui->playerNameInput1->text();
    QString currentSymbol = isXTurn ? ui->symbolChoice1->currentText() : ui->symbolChoice2->currentText();

    if (currentPlayer.isEmpty())
        currentPlayer = isXTurn ? "Игрок 2" : "Игрок 1";

    if (statusLabel) {
        statusLabel->setText(QString("Ходит: %1 (%2)").arg(currentPlayer).arg(currentSymbol));
    }
}

void MainWindow::resetGame() {
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (buttons[row][col]) {
                buttons[row][col]->setText("");
                buttons[row][col]->setEnabled(true);
            }
        }
    }

    isXTurn = true;
    movesLeft = 9;
    updateGameStatus();
}

void MainWindow::updatePlayerName(const QString &name) {
    Q_UNUSED(name);
    updateGameStatus();
}

void MainWindow::synchronizeSymbols(int index) {
    QComboBox *senderComboBox = qobject_cast<QComboBox*>(sender());
    if (!senderComboBox) return;

    QString selectedSymbol = senderComboBox->currentText();
    QComboBox *otherComboBox = (senderComboBox == ui->symbolChoice1) ?
                                   ui->symbolChoice2 : ui->symbolChoice1;

    if (otherComboBox->currentText() == selectedSymbol) {
        QMessageBox warningBox;
        warningBox.setWindowTitle("Внимание");
        warningBox.setText("Каждый игрок должен иметь уникальный символ.");
        warningBox.setInformativeText("Пожалуйста, выберите разные символы для игроков.");
        warningBox.setIcon(QMessageBox::Warning);
        warningBox.setStandardButtons(QMessageBox::Ok);
        warningBox.exec();

        senderComboBox->blockSignals(true);

        int newIndex = 0;
        while (newIndex < senderComboBox->count() &&
               senderComboBox->itemText(newIndex) == otherComboBox->currentText()) {
            newIndex++;
        }

        if (newIndex < senderComboBox->count()) {
            senderComboBox->setCurrentIndex(newIndex);
        } else {
            senderComboBox->setCurrentIndex((index + 1) % senderComboBox->count());
        }

        senderComboBox->blockSignals(false);
    }

    updateGameStatus();
}

void MainWindow::updateFontSize(int value) {
    QSlider *senderSlider = qobject_cast<QSlider*>(sender());
    if (!senderSlider) return;

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (buttons[row][col] && !buttons[row][col]->text().isEmpty()) {
                if ((senderSlider == ui->fontSizeSlider1 &&
                     buttons[row][col]->text() == ui->symbolChoice1->currentText()) ||
                    (senderSlider == ui->fontSizeSlider2 &&
                     buttons[row][col]->text() == ui->symbolChoice2->currentText())) {
                    buttons[row][col]->setFont(QFont("Arial", value));
                }
            }
        }
    }
}

void MainWindow::setLightTheme() {
    setStyleSheet(
        "QMainWindow { background-color: white; } "
        "QPushButton { background-color: #f0f0f0; border: 1px solid #ddd; border-radius: 3px; } "
        "QLabel, QLineEdit, QComboBox { color: black; } "
        "QLineEdit, QComboBox { border: 1px solid #aaa; border-radius: 3px; padding: 2px; }"
        );
}

void MainWindow::setDarkTheme() {
    setStyleSheet(
        "QMainWindow { background-color: #333; } "
        "QPushButton { background-color: #444; border: 1px solid #555; color: white; border-radius: 3px; } "
        "QLabel, QLineEdit, QComboBox { color: white; } "
        "QLineEdit, QComboBox { border: 1px solid #555; border-radius: 3px; padding: 2px; background-color: #444; }"
        );
}

void MainWindow::disableButtons() {
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (buttons[row][col]) {
                buttons[row][col]->setEnabled(false);
            }
        }
    }
}
