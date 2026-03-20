    void updateCaptainHeader() {
        headerTitleLabel->setText(QString("🐱 Свободное плавание — %1").arg(currentCaptain()));
        controlCatLabel->setText(
            QString("⛵ Кот-штурман готовит маршрут для капитана «%1»").arg(currentCaptain()));
    }

    void updateWeatherBadge() {
        weatherBadge->setText(QString("Погода: %1").arg(weatherName()));
        weatherBadge->setStyleSheet(weatherBadgeStyle());
        moodLabel->setText(catMoodText());
    }

    void updateWaveLabel(int value) {
        waveValueLabel->setText(QString("%1 / 100").arg(value));
    }

    void updateCalendarInfo() {
        calendarInfoLabel->setText(calendarMeaningText());
    }

    void updateRiskStatus() {
        const int risk = routeRiskValue();

        riskBar->setValue(risk);
        riskBar->setFormat(QString("Риск: %1%").arg(risk));
        riskLabel->setText(QString("%1 — %2").arg(riskText(risk)).arg(risk));

        QString chunkColor = "#8fc6ff";
        QString labelColor = "#2e5f91";

        if (risk <= 25) {
            chunkColor = "#7fd8a0";
            labelColor = "#2b6a3e";
        } else if (risk <= 50) {
            chunkColor = "#ffd56b";
            labelColor = "#8a6500";
        } else if (risk <= 75) {
            chunkColor = "#ffb26b";
            labelColor = "#9a5518";
        } else {
            chunkColor = "#ff8d96";
            labelColor = "#8a2f46";
        }

        riskBar->setStyleSheet(
            QString(
                "QProgressBar {"
                "  min-height: 22px;"
                "  text-align: center;"
                "  background: #f4f7fb;"
                "  border: 1px solid #d8ddea;"
                "  border-radius: 10px;"
                "  font-weight: 700;"
                "}"
                "QProgressBar::chunk {"
                "  border-radius: 9px;"
                "  background: %1;"
                "}")
                .arg(chunkColor));

        riskLabel->setStyleSheet(QString("color:%1; font-weight:800;").arg(labelColor));
    }

    void updateSummary() {
        const int nextIndex = firstUnvisitedIndex();
        QString nextText = "маршрут завершён";

        if (nextIndex != -1) {
            const StopState nextState = stateForStop(nextIndex);
            nextText = QString("этап %1 — %2 (%3)")
                           .arg(nextIndex + 1)
                           .arg(stops[nextIndex].title)
                           .arg(stopStateText(nextState));
        }

        summaryLabel->setText(
            QString(
                "Капитан: %1\n"
                "Дата отплытия: %2\n"
                "Погода: %3\n"
                "Высота волн: %4 / 100\n"
                "Точек маршрута: %5\n"
                "Следующая цель: %6")
                .arg(currentCaptain())
                .arg(calendar->selectedDate().toString("dd.MM.yyyy"))
                .arg(weatherName())
                .arg(waveSlider->value())
                .arg(stops.size())
                .arg(nextText));
    }

    void updateRouteBanner() {
        const int nextIndex = firstUnvisitedIndex();

        if (nextIndex == -1) {
            routeBannerLabel->setText(
                "✅ Все этапы пройдены по порядку. Котокорабль вернулся в гавань!");
            return;
        }

        const RouteStop& stop = stops[nextIndex];
        const StopState state = stateForStop(nextIndex);

        if (state == StopState::Current) {
            routeBannerLabel->setText(
                QString("⭐ Сейчас активен этап %1: %2. Его можно проходить прямо сейчас.")
                    .arg(nextIndex + 1)
                    .arg(stop.title));
        } else {
            routeBannerLabel->setText(
                QString(
                    "🚧 Следующий этап %1 (%2) временно заблокирован. "
                    "Нужно подстроить рейс под правило: %3")
                    .arg(nextIndex + 1)
                    .arg(stop.title)
                    .arg(accessRuleText(stop)));
        }
    }

    void refreshRouteItem(int index) {
        if (index < 0 || index >= stops.size()) {
            return;
        }

        QListWidgetItem* item = routeList->item(index);
        if (!item) {
            return;
        }

        const RouteStop& stop = stops[index];
        const StopState state = stateForStop(index);

        item->setText(
            QString("%1 Этап %2\n%3").arg(routeMarker(state)).arg(index + 1).arg(stop.title));

        item->setToolTip(stopDescription(stop, index));
        item->setForeground(QColor("#1f2430"));

        switch (state) {
            case StopState::Visited:
                item->setBackground(QColor("#d8f6df"));
                break;
            case StopState::Current:
                item->setBackground(QColor("#fff2b8"));
                break;
            case StopState::Blocked:
                item->setBackground(QColor("#ffdfe5"));
                break;
            case StopState::Planned:
                item->setBackground(QColor("#eef4ff"));
                break;
        }

        QFont font = item->font();
        font.setBold(index == routeList->currentRow() || state == StopState::Current);
        item->setFont(font);
        item->setSizeHint(QSize(220, 56));
    }

    void refreshAllRouteItems() {
        for (int i = 0; i < stops.size(); ++i) {
            refreshRouteItem(i);
        }
    }

    void rebuildRouteList() {
        routeList->clear();

        for (int i = 0; i < stops.size(); ++i) {
            auto* item = new QListWidgetItem();
            routeList->addItem(item);
            refreshRouteItem(i);
        }
    }

    void updateDetailsPanel() {
        const int row = routeList->currentRow();

        if (row < 0 || row >= stops.size()) {
            selectedStopTitle->setText("Маршрут ещё не выбран");
            selectedStopDescription->setPlainText(
                "Нажми на точку маршрута слева, чтобы увидеть детали. "
                "Двойной клик работает только для текущего доступного этапа.");
            return;
        }

        const RouteStop& stop = stops[row];
        const StopState state = stateForStop(row);

        selectedStopTitle->setText(QString("%1 %2").arg(routeMarker(state)).arg(stop.title));

        selectedStopDescription->setPlainText(
            QString("%1\n\nСтатус: %2").arg(stopDescription(stop, row)).arg(stopStateText(state)));
    }

    void updateProgress() {
        if (stops.isEmpty()) {
            readinessBar->setValue(0);
            readinessBar->setFormat("Готовность рейса: 0%");
            return;
        }

        int visitedCount = 0;
        for (const RouteStop& stop : stops) {
            if (stop.visited) {
                ++visitedCount;
            }
        }

        const int percent = (visitedCount * 100) / stops.size();
        readinessBar->setValue(percent);
        readinessBar->setFormat(QString("Посещено этапов: %1/%2 — %3%")
                                    .arg(visitedCount)
                                    .arg(stops.size())
                                    .arg(percent));
    }

    void recomputeRouteLogic() {
        updateSummary();
        updateRouteBanner();
        refreshAllRouteItems();
        updateDetailsPanel();
        updateProgress();
        applyRouteFilter();
    }

    void applyRouteFilter() {
        const QString needle = searchEdit->text().trimmed().toLower();
        const bool onlyUnvisited = onlyUnvisitedBox->isChecked();

        for (int i = 0; i < stops.size(); ++i) {
            if (auto* item = routeList->item(i)) {
                const RouteStop& stop = stops[i];

                const bool matchesText =
                    needle.isEmpty() || stop.title.toLower().contains(needle) ||
                    stop.action.toLower().contains(needle) || stop.hint.toLower().contains(needle);

                const bool matchesVisited = !onlyUnvisited || !stop.visited;
                item->setHidden(!(matchesText && matchesVisited));
            }
        }

        ensureVisibleSelection();
    }

    void ensureVisibleSelection() {
        const int current = routeList->currentRow();
        if (current >= 0 && current < routeList->count()) {
            QListWidgetItem* currentItem = routeList->item(current);
            if (currentItem && !currentItem->isHidden()) {
                updateDetailsPanel();
                refreshAllRouteItems();
                return;
            }
        }

        for (int i = 0; i < routeList->count(); ++i) {
            QListWidgetItem* item = routeList->item(i);
            if (item && !item->isHidden()) {
                routeList->setCurrentRow(i);
                return;
            }
        }

        routeList->setCurrentItem(nullptr);
        refreshAllRouteItems();
        updateDetailsPanel();
    }

    void generateRoute() {
        const QStringList titles = {
          "Маяк с тунцом",    "Остров Клубка",        "Бухта Дневного Сна",
          "Причал Сметаны",   "Залив Ласковых Лап",   "Лунная пристань",
          "Риф Мурчания",     "Порт Шпрот",           "Мостик Хвостатого Ветра",
          "Коралловая Каюта", "Берег Мягких Подушек", "Гавань Полосатых Парусов"};

        const QStringList actions = {
          "проверить запасы рыбы",
          "обновить маршрутную карту",
          "сделать фотопаузу для команды",
          "проверить якорь и верёвки",
          "собрать морские сувениры",
          "передать привет чайкам",
          "устроить короткий привал на палубе",
          "поймать удачный ветер"};

        const QStringList hints = {
          "держать хвост трубой и не бояться волн",
          "не забыть про запасной клубок ниток",
          "следить, чтобы капитан не уснул на солнце",
          "доверять компасу и кошачьей интуиции",
          "не спорить с чайками без необходимости",
          "в шторм держаться ближе к безопасной воде",
          "в закат обязательно сделать красивую остановку",
          "по пути считать встреченные лапки на песке"};

        const QVector<int> safeThresholds = {35, 45, 55, 65, 75, 85};

        stops.clear();

        int stopCount = 5;
        if (weatherBox->currentIndex() == 0 && waveSlider->value() < 30) {
            stopCount = 6;
        } else if (weatherBox->currentIndex() == 3 || waveSlider->value() > 75) {
            stopCount = 4;
        }

        QVector<int> chosen;
        while (chosen.size() < stopCount) {
            const int idx = QRandomGenerator::global()->bounded(titles.size());
            if (!chosen.contains(idx)) {
                chosen.push_back(idx);
            }
        }

        for (int i = 0; i < chosen.size(); ++i) {
            const int titleIndex = chosen[i];
            const int actionIndex = QRandomGenerator::global()->bounded(actions.size());
            const int hintIndex = QRandomGenerator::global()->bounded(hints.size());
            const int thresholdIndex = QRandomGenerator::global()->bounded(safeThresholds.size());

            RouteStop stop;
            stop.title = titles[titleIndex];
            stop.action = actions[actionIndex];
            stop.hint = hints[hintIndex];
            stop.visited = false;
            stop.maxSafeRisk = safeThresholds[thresholdIndex];
            stop.forbiddenInStorm = (QRandomGenerator::global()->bounded(100) < 45);

            stops.push_back(stop);
        }

        rebuildRouteList();
        updateRiskStatus();
        recomputeRouteLogic();
    }

    void tryVisitStop(int row) {
        if (row < 0 || row >= stops.size()) {
            return;
        }

        if (stops[row].visited) {
            QMessageBox::information(
                this, "Этап уже пройден",
                "Эта точка уже посещена. Двигайся дальше по маршруту, кот-капитан 🐾");
            return;
        }

        const int nextIndex = firstUnvisitedIndex();

        if (row != nextIndex) {
            QMessageBox::warning(
                this, "Нельзя перепрыгнуть этап",
                QString(
                    "Маршрут нужно проходить по порядку.\n"
                    "Сейчас доступен этап %1: %2.")
                    .arg(nextIndex + 1)
                    .arg(stops[nextIndex].title));
            return;
        }

        if (!canVisitStop(row)) {
            QMessageBox::warning(
                this, "Этап заблокирован",
                QString("Сейчас пройти этот этап нельзя.\n\n%1").arg(blockingReasonForStop(row)));
            return;
        }

        stops[row].visited = true;
        recomputeRouteLogic();

        if (firstUnvisitedIndex() == -1) {
            QMessageBox::information(
                this, "Рейс завершён",
                "Все этапы маршрута успешно пройдены по порядку. Котокорабль вернулся в гавань! "
                "🐱⛵");
        }
    }

    void resetVisitedStops() {
        for (RouteStop& stop : stops) {
            stop.visited = false;
        }

        recomputeRouteLogic();
    }

    void exportRoute() {
        QString suggestedName =
            QString("cat_route_%1.txt").arg(calendar->selectedDate().toString("yyyyMMdd"));

        const QString path = QFileDialog::getSaveFileName(
            this, "Экспорт рейса", suggestedName, "Text Files (*.txt);;All Files (*)");

        if (path.isEmpty()) {
            return;
        }

        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для записи.");
            return;
        }

        QTextStream out(&file);
        out << "Кошачья гавань — экспорт рейса\n";
        out << "==============================\n\n";
        out << "Капитан: " << currentCaptain() << "\n";
        out << "Дата: " << calendar->selectedDate().toString("dd.MM.yyyy") << "\n";
        out << "Погода: " << weatherName() << "\n";
        out << "Высота волн: " << waveSlider->value() << " / 100\n";
        out << "Сложность рейса: " << riskText(routeRiskValue()) << " (" << routeRiskValue()
            << "%)\n";
        out << "Календарь: " << calendarMeaningText() << "\n\n";

        for (int i = 0; i < stops.size(); ++i) {
            const RouteStop& stop = stops[i];
            out << "Этап " << (i + 1) << ": " << stop.title << "\n";
            out << "Статус: " << stopStateText(stateForStop(i)) << "\n";
            out << stopDescription(stop, i) << "\n";
            out << "----------------------------------------\n";
        }

        file.close();

        QMessageBox::information(this, "Готово", "Рейс успешно экспортирован в файл. 🐾");
    }
