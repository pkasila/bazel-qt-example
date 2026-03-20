    QString defaultTicketName(int index) const {
        return QString("Билет %1").arg(index + 1);
    }

    QString statusMarker(TicketStatus status) const {
        switch (status) {
            case TicketStatus::Default:
                return "⬜";
            case TicketStatus::Yellow:
                return "🟨";
            case TicketStatus::Green:
                return "🟩";
        }
        return "⬜";
    }

    QString viewTextForTicket(int index) const {
        if (index < 0 || index >= tickets.size()) {
            return "";
        }

        const Ticket& ticket = tickets[index];
        if (ticket.name == defaultTicketName(index)) {
            return QString("%1 %2").arg(statusMarker(ticket.status)).arg(ticket.name);
        }

        return QString("%1 №%2\n%3")
            .arg(statusMarker(ticket.status))
            .arg(index + 1)
            .arg(ticket.name);
    }

    QColor colorForStatus(TicketStatus status) const {
        switch (status) {
            case TicketStatus::Default:
                return QColor("#f3f5f8");
            case TicketStatus::Yellow:
                return QColor("#ffe48f");
            case TicketStatus::Green:
                return QColor("#bce9c2");
        }
        return QColor("#f3f5f8");
    }

    QString statusText(TicketStatus status) const {
        switch (status) {
            case TicketStatus::Default:
                return "Не повторял";
            case TicketStatus::Yellow:
                return "Нужно ещё раз";
            case TicketStatus::Green:
                return "Повторён";
        }
        return "Не повторял";
    }

    QString statusBadgeStyle(TicketStatus status) const {
        const QString baseStyle = "padding:4px 12px; border-radius:14px; font-weight:700;";
        switch (status) {
            case TicketStatus::Default:
                return baseStyle + "background:#eceff5; color:#596079; border:1px solid #d8dde9;";
            case TicketStatus::Yellow:
                return baseStyle + "background:#fff1b8; color:#7a5d00; border:1px solid #efd36f;";
            case TicketStatus::Green:
                return baseStyle + "background:#d6f5db; color:#22633b; border:1px solid #98dbab;";
        }
        return baseStyle + "background:#eceff5; color:#596079; border:1px solid #d8dde9;";
    }

    QString comboStyleForStatus(TicketStatus status) const {
        switch (status) {
            case TicketStatus::Default:
                return "QComboBox { background:#f6f7fb; color:#4d536c; border:1px solid #d9dced; "
                       "padding:8px 10px; border-radius:10px; }";
            case TicketStatus::Yellow:
                return "QComboBox { background:#fff6d8; color:#7a6200; border:1px solid #efd36f; "
                       "padding:8px 10px; border-radius:10px; }";
            case TicketStatus::Green:
                return "QComboBox { background:#e6f8ea; color:#1f6a39; border:1px solid #99ddb0; "
                       "padding:8px 10px; border-radius:10px; }";
        }
        return "";
    }

    TicketStatus statusFromComboIndex(int comboIndex) const {
        switch (comboIndex) {
            case 1:
                return TicketStatus::Yellow;
            case 2:
                return TicketStatus::Green;
            default:
                return TicketStatus::Default;
        }
    }

    int comboIndexFromStatus(TicketStatus status) const {
        switch (status) {
            case TicketStatus::Default:
                return 0;
            case TicketStatus::Yellow:
                return 1;
            case TicketStatus::Green:
                return 2;
        }
        return 0;
    }

    void resetTickets(int count) {
        tickets.clear();
        tickets.resize(count);

        for (int i = 0; i < count; ++i) {
            tickets[i].name = defaultTicketName(i);
            tickets[i].status = TicketStatus::Default;
        }

        history.clear();
        historyPos = -1;
        currentIndex = -1;

        rebuildView();
        updateProgressBars();

        if (!tickets.isEmpty()) {
            selectTicket(0, true);
        } else {
            updateQuestionView();
        }
    }

    void rebuildView() {
        view->clear();

        for (int i = 0; i < tickets.size(); ++i) {
            auto* item = new QListWidgetItem();
            item->setTextAlignment(Qt::AlignCenter);
            item->setSizeHint(QSize(170, 72));
            view->addItem(item);
            refreshViewItem(i);
        }
    }

    void refreshViewItem(int index) {
        if (index < 0 || index >= tickets.size()) {
            return;
        }

        auto* item = view->item(index);
        if (!item) {
            return;
        }

        const Ticket& ticket = tickets[index];

        item->setText(viewTextForTicket(index));
        item->setBackground(colorForStatus(ticket.status));
        item->setForeground(Qt::black);

        QFont font = item->font();
        font.setBold(index == currentIndex);
        item->setFont(font);

        item->setToolTip(QString("Билет %1").arg(index + 1));
    }

    void selectTicket(int index, bool addToHistory) {
        if (index < 0 || index >= tickets.size()) {
            return;
        }

        const int previousIndex = currentIndex;
        currentIndex = index;

        if (previousIndex != -1) {
            refreshViewItem(previousIndex);
        }

        view->setCurrentRow(index);
        view->scrollToItem(view->item(index), QAbstractItemView::PositionAtCenter);
        refreshViewItem(index);

        if (addToHistory) {
            if (historyPos + 1 < history.size()) {
                history.resize(historyPos + 1);
            }

            if (history.isEmpty() || history.last() != index) {
                history.push_back(index);
            }
            historyPos = history.size() - 1;
        }

        updateQuestionView();
    }

    void updateQuestionView() {
        const bool hasCurrent = currentIndex >= 0 && currentIndex < tickets.size();

        numberValue->setText(hasCurrent ? QString::number(currentIndex + 1) : "—");
        nameValue->setText(hasCurrent ? tickets[currentIndex].name : "—");

        if (hasCurrent) {
            nameEdit->setText(tickets[currentIndex].name);
            statusBadge->setText(statusText(tickets[currentIndex].status));
            statusBadge->setStyleSheet(statusBadgeStyle(tickets[currentIndex].status));
            statusCombo->setStyleSheet(comboStyleForStatus(tickets[currentIndex].status));
        } else {
            nameEdit->clear();
            statusBadge->setText("Не выбран");
            statusBadge->setStyleSheet(statusBadgeStyle(TicketStatus::Default));
            statusCombo->setStyleSheet(comboStyleForStatus(TicketStatus::Default));
        }

        {
            QSignalBlocker blocker(statusCombo);
            statusCombo->setCurrentIndex(
                hasCurrent ? comboIndexFromStatus(tickets[currentIndex].status) : 0);
        }

        nameEdit->setEnabled(hasCurrent);
        statusCombo->setEnabled(hasCurrent);
        previousButton->setEnabled(historyPos > 0);
        nextButton->setEnabled(hasQuestionForNext());
    }

    void renameCurrentTicket() {
        if (currentIndex < 0 || currentIndex >= tickets.size()) {
            return;
        }

        if (!nameEdit->hasFocus()) {
            return;
        }

        const QString newName = nameEdit->text().trimmed();
        if (newName.isEmpty()) {
            return;
        }

        tickets[currentIndex].name = newName;
        nameValue->setText(newName);
        refreshViewItem(currentIndex);
    }

    void setTicketStatus(int index, TicketStatus newStatus) {
        if (index < 0 || index >= tickets.size()) {
            return;
        }

        tickets[index].status = newStatus;
        refreshViewItem(index);
        updateProgressBars();

        if (index == currentIndex) {
            updateQuestionView();
        }
    }

    void toggleStatusFromView(int index) {
        if (index < 0 || index >= tickets.size()) {
            return;
        }

        const TicketStatus currentStatus = tickets[index].status;
        if (currentStatus == TicketStatus::Green) {
            setTicketStatus(index, TicketStatus::Yellow);
        } else {
            setTicketStatus(index, TicketStatus::Green);
        }

        selectTicket(index, true);
    }

    bool hasQuestionForNext() const {
        for (const Ticket& ticket : tickets) {
            if (ticket.status == TicketStatus::Default || ticket.status == TicketStatus::Yellow) {
                return true;
            }
        }
        return false;
    }

    void chooseNextRandomTicket() {
        QVector<int> candidates;
        for (int i = 0; i < tickets.size(); ++i) {
            if (tickets[i].status == TicketStatus::Default ||
                tickets[i].status == TicketStatus::Yellow) {
                candidates.push_back(i);
            }
        }

        if (candidates.isEmpty()) {
            QMessageBox::information(this, "Готово", "Все билеты уже зелёные. Котики довольны 🐱");
            updateQuestionView();
            return;
        }

        if (candidates.size() > 1 && currentIndex >= 0) {
            candidates.removeAll(currentIndex);
            if (candidates.isEmpty()) {
                candidates.push_back(currentIndex);
            }
        }

        const int randomPos = QRandomGenerator::global()->bounded(candidates.size());
        selectTicket(candidates[randomPos], true);
    }

    void goToPreviousTicket() {
        if (historyPos <= 0 || history.isEmpty()) {
            return;
        }

        --historyPos;
        const int newIndex = history[historyPos];
        selectTicket(newIndex, false);
    }

    void updateProgressBars() {
        const int totalCount = tickets.size();
        int totalDone = 0;
        int greenDone = 0;

        for (const Ticket& ticket : tickets) {
            if (ticket.status != TicketStatus::Default) {
                ++totalDone;
            }
            if (ticket.status == TicketStatus::Green) {
                ++greenDone;
            }
        }

        totalProgress->setRange(0, totalCount);
        greenProgress->setRange(0, totalCount);

        totalProgress->setFormat(QString("Повторено: %1/%2").arg(totalDone).arg(totalCount));
        greenProgress->setFormat(QString("Зелёных: %1/%2").arg(greenDone).arg(totalCount));

        totalProgress->setValue(totalDone);
        greenProgress->setValue(greenDone);

        nextButton->setEnabled(hasQuestionForNext());
    }
