#include "TicketApp.h"

TicketApp::TicketApp(QWidget *parent) : QMainWindow(parent),
    prevTicket(nullptr), currentTicket(nullptr),
    isTicketViewActivated(false) {
    setUpUi();
    setUpConnections();
}

void TicketApp::setUpUi() {
    setWindowTitle(WindowTitles::mainWindowTitle);

    resize(Geometry::ticketAppMainWindowSize);
    setMinimumSize(Geometry::minTicketAppMainWindowSize);

    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    //--- Ticket control unit ---
    QVBoxLayout *ticketManagementLayout = new QVBoxLayout();

    //Progress bars
    totalProgressBar = new TicketsProgressBar(this);
    totalProgressBar->setFormat(Formats::totalProgressBarFormat);
    totalProgressBar->setStyleSheet(Styles::totalProgressBarStyle);
    totalProgressBar->hide();

    greenProgressBar = new TicketsProgressBar(this);
    greenProgressBar->setFormat(Formats::greenProgressBarFormat);
    greenProgressBar->setStyleSheet(Styles::greenProgressBarStyle);
    greenProgressBar->hide();

    ticketManagementLayout->addWidget(totalProgressBar);
    ticketManagementLayout->addWidget(greenProgressBar);
    //Number of tickets input
    QHBoxLayout *inputLayout = new QHBoxLayout();
    numberOfTickets = new QLabel(WindowTexts::ticketsInputText, this);
    numberOfTickets->setAlignment(Qt::AlignCenter);

    ticketCountInput = new QSpinBox(this);
    submitButton = new CursorChangingButton(ButtonTexts::submit, this);

    inputLayout->addWidget(numberOfTickets);
    inputLayout->addWidget(ticketCountInput);
    inputLayout->addWidget(submitButton);
    ticketManagementLayout->addLayout(inputLayout);

    //Тable of tickets
    ticketTable = new TicketTableWidget(0, this);
    ticketTable->setMouseTracking(true);
    ticketTable->installEventFilter(this);
    ticketManagementLayout->addWidget(ticketTable);

    //Add tickets button
    addTicketsButton = new CursorChangingButton(ButtonTexts::addTicketsButtonText, this);
    addTicketsButton->hide();
    ticketManagementLayout->addWidget(addTicketsButton);

    // --- Блок отображения билетов ---
    ticketView = new QGroupBox(WindowTexts::groupBoxHeader, this);
    ticketView->hide();
    ticketView->setMinimumWidth(Geometry::ticketViewMinWidth);
    ticketView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *groupLayout = new QVBoxLayout();

    QHBoxLayout *ticketInfoLayout = new QHBoxLayout();

    currentTicketNumber = new QLabel("0", this);
    currentTicketNumber->hide();

    currentTicketName = new EditableLabel(WindowTexts::defaultEditableLabelText, this);
    currentTicketName->setAlignment(Qt::AlignCenter);
    currentTicketName->hide();

    currentTicketStatusBox = new StatusComboBox(this);
    currentTicketStatusBox->setFixedWidth(Geometry::ticketStatusBoxWidth);
    currentTicketStatusBox->hide();

    ticketInfoLayout->addWidget(currentTicketStatusBox);
    ticketInfoLayout->addWidget(currentTicketNumber);
    ticketInfoLayout->addWidget(currentTicketName);
    groupLayout->addLayout(ticketInfoLayout);

    currentTicketText = new TicketTextEdit(this);
    currentTicketText->setHtml(WindowTexts::textEditUnableText);
    currentTicketText->setEnabled(false);
    groupLayout->addWidget(currentTicketText);

    QHBoxLayout *scrollButtonsLayout = new QHBoxLayout();

    prevTicketButton = new CursorChangingButton(ButtonTexts::prevTicketButtonText, this);
    prevTicketButton->setEnabled(false);
    prevTicketButton->hide();

    randomTicketButton = new CursorChangingButton(ButtonTexts::randomTicketButtonText, this);
    randomTicketButton->hide();

    scrollButtonsLayout->addWidget(prevTicketButton);
    scrollButtonsLayout->addWidget(randomTicketButton);
    groupLayout->addLayout(scrollButtonsLayout);

    ticketView->setLayout(groupLayout);

    //--- Добавляем оба блока в основной макет ---
    mainLayout->addLayout(ticketManagementLayout);
    mainLayout->addWidget(ticketView);

    setCentralWidget(centralWidget);
}

void TicketApp::setUpConnections() {
    connect(ticketTable, &QTableWidget::cellDoubleClicked, this, &TicketApp::onTableCellDoubleClicked);

    connect(submitButton, &QPushButton::clicked, this, &TicketApp::initTickets);

    connect(addTicketsButton, &QPushButton::clicked, this, &TicketApp::showAddTicketsDialog);

    connect(ticketTable, &QTableWidget::cellClicked, this, &TicketApp::onTableCellClicked);

    connect(prevTicketButton, &CursorChangingButton::clicked, this, &TicketApp::selectPrevTicket);

    connect(randomTicketButton, &CursorChangingButton::clicked, this, &TicketApp::selectRandomTicket);

    connect(ticketTable, &TicketTableWidget::possibleScoreIsIncreased, totalProgressBar,
            &TicketsProgressBar::incPossibleScore);

    connect(ticketTable, &TicketTableWidget::possibleGreenTicketsNumberIsIncreased,
            greenProgressBar, &TicketsProgressBar::incPossibleScore);

    connect(ticketTable, &TicketTableWidget::signalAdditionalPointsScored, totalProgressBar,
            &TicketsProgressBar::incCurrentScore);

    connect(ticketTable, &TicketTableWidget::signalAdditionalGreenTicketAppeared,
            greenProgressBar, &TicketsProgressBar::incCurrentScore);
}

void TicketApp::connectCurrentTicketNameToCurrentTicket() {
    if (currentTicket) {
        if (prevTicket != nullptr){
            disconnect(currentTicketName, &EditableLabel::nameIsChanged,
                       prevTicket, &TicketCell::setTicketName);
        }
        connect(currentTicketName, &EditableLabel::nameIsChanged,
                currentTicket, &TicketCell::setTicketName);
    }
}

void TicketApp::connectCurrentTicketStatusBoxToCurrentTicket() {
    if (currentTicket) {
        if (prevTicket != nullptr) {
            disconnect(currentTicketStatusBox, &StatusComboBox::statusIsChanged,
                       prevTicket, &TicketCell::setTicketStatus);
        }
        connect(currentTicketStatusBox, &StatusComboBox::statusIsChanged,
                currentTicket, &TicketCell::setTicketStatus);
    }
}

void TicketApp::connectCurrentTicketTextToCurrentTicket() {
    if (currentTicket) {
        if (prevTicket != nullptr) {
            disconnect(currentTicketText, &TicketTextEdit::editingFinished,
                       prevTicket, &TicketCell::setTicketText);
        }
        connect(currentTicketText, &TicketTextEdit::editingFinished,
                       currentTicket, &TicketCell::setTicketText);
    }
}

void TicketApp::initTickets() {
    if (!InputUtils::validateInput(this,
        ticketCountInput->text())) {
        return;
    }
    int ticketsCount = ticketCountInput->text().toInt();
    ticketTable->addTickets(ticketsCount);
    //
    numberOfTickets->hide();
    ticketCountInput->hide();
    submitButton->hide();
    initTicketView();
    initProgressBars();
    ticketView->show();
}

void TicketApp::initTicketView() {
    //Resize main window to place ticketView
    resize(Geometry::ticketAppMainWindowWithTicketViewActivatedSize);
    setMinimumSize(Geometry::minTicketAppMainWindowWithTicketViewActivatedSize);
    addTicketsButton->show();
    prevTicketButton->show();
    randomTicketButton->show();
}

void TicketApp::showAddTicketsDialog() {
    AddTicketsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        int additionalTickets = dialog.getTicketCount();
        ticketTable->addTickets(additionalTickets);
    }
}

void TicketApp::selectTicket(int row) {
    if (currentTicket != ticketTable->getTicket(row)) {
        ticketTable->setCurrentCell(row, 0);
        ticketTable->setFocus();

        if (currentTicket != nullptr) {
            prevTicket = currentTicket;
        }
        currentTicket = ticketTable->getTicket(row);

        if (isTicketViewActivated == false) {
            activateTicketView();
        }

        currentTicketText->clear();
        if (currentTicket->getTicketText().isNull()) {
            currentTicketText->setPlaceholderText(WindowTexts::textEditPlaceholderText);
        } else {
            currentTicketText->setHtml(currentTicket->getTicketText());
        }

        connectCurrentTicketTextToCurrentTicket();

        currentTicketNumber->setText(Formats::ticketNumberFormat +
                                     QString::number(currentTicket->getTicketNumber()));

        connectCurrentTicketStatusBoxToCurrentTicket();
        currentTicketStatusBox->setCurrentIndex(currentTicket->getTicketStatus());
        currentTicketStatusBox->updateComboBoxColorWithSignal(currentTicket->getTicketStatus());

        connectCurrentTicketNameToCurrentTicket();
        currentTicketName->setText(currentTicket->getTicketName());
    }
}

void TicketApp::activateTicketView() {
    currentTicketText->setEnabled(true);
    currentTicketNumber->show();
    currentTicketStatusBox->show();
    currentTicketName->show();
    isTicketViewActivated = true;
}

void TicketApp::initProgressBars() {
    totalProgressBar->show();
    greenProgressBar->show();
}

void TicketApp::onTableCellDoubleClicked(int row, int) {
    TicketCell* ticket = ticketTable->getTicket(row);
    int ticketStatus = ticket->getTicketStatus();
    //Changing ticket status
    if (ticketStatus == Status::Red || ticketStatus == Status::Yellow) {
        ticketStatus = Status::Green;
    } else if (ticketStatus == Status::Green) {
        ticketStatus = Status::Yellow;
    }
    ticket->setTicketStatus(ticketStatus);
    currentTicketStatusBox->updateComboBoxColor(ticketStatus);
    currentTicketStatusBox->setCurrentIndex(ticketStatus);
}

void TicketApp::onTableCellClicked(int row, int) {
    if (currentTicket != ticketTable->getTicket(row)) {
        selectTicket(row);
        addCurrentTicketToUndoTicketStack();
    }
}

void TicketApp::addCurrentTicketToUndoTicketStack() {
    if (prevTicket != nullptr && currentTicket != nullptr) {
        if (currentTicket->getTicketNumber() != prevTicket->getTicketNumber()) {
            undoTicketStack.push(currentTicket); //stack for button "prev ticket"
            if (undoTicketStack.size() > 1) {
                prevTicketButton->setEnabled(true);
            }
        }
    } else {
        undoTicketStack.push(currentTicket); //stack for button "prev ticket"
        if (undoTicketStack.size() > 1) {
            prevTicketButton->setEnabled(true);
        }
    }
}

void TicketApp::selectPrevTicket() {
    if (undoTicketStack.size() > 1) {
        undoTicketStack.pop();
        TicketCell *prevTicket = undoTicketStack.top();
        int prevTicketRow = prevTicket->getTicketNumber() - 1;
        selectTicket(prevTicketRow);
    }
    if (undoTicketStack.size() == 1) {
        prevTicketButton->setEnabled(false);
    }
}

void TicketApp::selectRandomTicket() {
    QVector<int> unrevisedTicketsRows = ticketTable->getUnrevisedTicketsRows();
    if (unrevisedTicketsRows.size() == 0) {
        int randomTicketRow = QRandomGenerator::global()->bounded(0,
                                                ticketTable->getNumberOfTickets());
        if (currentTicket != nullptr) {
            while (randomTicketRow == currentTicket->getTicketNumber() - 1) {
                randomTicketRow = QRandomGenerator::global()->bounded(0,
                                                    ticketTable->getNumberOfTickets());
            }
        }
        selectTicket(randomTicketRow);
    }
    else if (unrevisedTicketsRows.size() == 1) {
        selectTicket(unrevisedTicketsRows[0]);
    }
    else if (unrevisedTicketsRows.size() > 1) {
        int randomTicketRow = unrevisedTicketsRows[QRandomGenerator::global()->bounded(0,
                                                    unrevisedTicketsRows.size())];
        if (currentTicket != nullptr) {
            while (randomTicketRow == currentTicket->getTicketNumber() - 1) {
                randomTicketRow = unrevisedTicketsRows[QRandomGenerator::global()->bounded(0,
                                                        unrevisedTicketsRows.size())];
            }
        }
        selectTicket(randomTicketRow);
    }
    addCurrentTicketToUndoTicketStack();
}
