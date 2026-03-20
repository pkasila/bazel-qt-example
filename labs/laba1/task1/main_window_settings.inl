    void saveSettings() {
        QSettings settings("ticket_lab", "procrastination_app");
        settings.setValue("geometry", saveGeometry());
        settings.setValue("count", countSpin->value());
        settings.setValue("currentIndex", currentIndex);

        settings.beginWriteArray("tickets");
        for (int i = 0; i < tickets.size(); ++i) {
            settings.setArrayIndex(i);
            settings.setValue("name", tickets[i].name);
            settings.setValue("status", static_cast<int>(tickets[i].status));
        }
        settings.endArray();
    }

    void loadSettings() {
        QSettings settings("ticket_lab", "procrastination_app");

        const int count = qMax(1, settings.value("count", 20).toInt());
        countSpin->setValue(count);

        tickets.clear();
        tickets.resize(count);

        const int savedSize = settings.beginReadArray("tickets");
        if (savedSize == count) {
            for (int i = 0; i < count; ++i) {
                settings.setArrayIndex(i);
                const QString savedName =
                    settings.value("name", defaultTicketName(i)).toString().trimmed();
                const int savedStatus = settings.value("status", 0).toInt();

                tickets[i].name = savedName.isEmpty() ? defaultTicketName(i) : savedName;
                tickets[i].status = static_cast<TicketStatus>(qBound(0, savedStatus, 2));
            }
        } else {
            for (int i = 0; i < count; ++i) {
                tickets[i].name = defaultTicketName(i);
                tickets[i].status = TicketStatus::Default;
            }
        }
        settings.endArray();

        rebuildView();

        currentIndex = settings.value("currentIndex", 0).toInt();
        if (currentIndex < 0 || currentIndex >= tickets.size()) {
            currentIndex = tickets.isEmpty() ? -1 : 0;
        }

        history.clear();
        historyPos = -1;

        if (currentIndex != -1) {
            selectTicket(currentIndex, true);
        } else {
            updateQuestionView();
        }

        updateProgressBars();

        const QByteArray geometry = settings.value("geometry").toByteArray();
        if (!geometry.isEmpty()) {
            restoreGeometry(geometry);
        }
    }
