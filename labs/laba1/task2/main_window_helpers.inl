    QString currentCaptain() const {
        const QString trimmed = captainEdit->text().trimmed();
        return trimmed.isEmpty() ? "Капитан Безымянный Кот" : trimmed;
    }

    QString weatherName() const {
        return weatherBox->currentText();
    }

    bool isWeekendSelected() const {
        const int day = calendar->selectedDate().dayOfWeek();
        return day == 6 || day == 7;
    }

    QString calendarMeaningText() const {
        if (isWeekendSelected()) {
            return "📅 Выбраны выходные: календарь добавляет +5 к риску рейса.";
        }
        return "📅 Выбран будний день: календарь не добавляет штраф к риску.";
    }

    QString weatherBadgeStyle() const {
        switch (weatherBox->currentIndex()) {
            case 0:
                return "background:#dff4ff; color:#1f5f8b; border:1px solid #9ad7ff;";
            case 1:
                return "background:#e9fbe5; color:#2f6b2f; border:1px solid #aee1a3;";
            case 2:
                return "background:#fff4cf; color:#8a6200; border:1px solid #efd27a;";
            case 3:
                return "background:#ffe0e6; color:#8b2f4c; border:1px solid #ef9fb4;";
            case 4:
                return "background:#eee4ff; color:#5a3f8a; border:1px solid #c8b2ff;";
            default:
                return "background:#f1f3f7; color:#5d647a; border:1px solid #d7dcea;";
        }
    }

    QString catMoodText() const {
        const int waves = waveSlider->value();

        if (weatherBox->currentIndex() == 3 || waves >= 80) {
            return "😼 Кот-штурман держится лапой за шляпу: впереди серьёзные волны.";
        }
        if (weatherBox->currentIndex() == 4) {
            return "😺 Вечерний рейс обещает красивый закат и спокойное мурчание.";
        }
        if (waves <= 25) {
            return "🐱 Море тихое, капитан довольно щурится на солнце.";
        }
        if (waves <= 60) {
            return "🐾 Корабль слегка покачивает, но коты всё ещё уверены в маршруте.";
        }
        return "🙀 Волны высокие, поэтому маршрут стоит проходить осторожно.";
    }

    QString routeMarker(StopState state) const {
        switch (state) {
            case StopState::Visited:
                return "🐾";
            case StopState::Current:
                return "⭐";
            case StopState::Blocked:
                return "🚧";
            case StopState::Planned:
                return "🐟";
        }
        return "🐟";
    }

    QString stopStateText(StopState state) const {
        switch (state) {
            case StopState::Visited:
                return "уже посещена";
            case StopState::Current:
                return "текущий доступный этап";
            case StopState::Blocked:
                return "этап временно заблокирован";
            case StopState::Planned:
                return "ждёт своей очереди";
        }
        return "неизвестно";
    }

    int firstUnvisitedIndex() const {
        for (int i = 0; i < stops.size(); ++i) {
            if (!stops[i].visited) {
                return i;
            }
        }
        return -1;
    }

    int routeRiskValue() const {
        int risk = waveSlider->value();

        switch (weatherBox->currentIndex()) {
            case 0:
                risk += 0;
                break;
            case 1:
                risk += 10;
                break;
            case 2:
                risk += 22;
                break;
            case 3:
                risk += 38;
                break;
            case 4:
                risk += 14;
                break;
            default:
                break;
        }

        if (isWeekendSelected()) {
            risk += 5;
        }

        if (stops.size() >= 6) {
            risk += 6;
        }

        if (stops.size() <= 4) {
            risk -= 4;
        }

        if (risk < 0) {
            risk = 0;
        }
        if (risk > 100) {
            risk = 100;
        }
        return risk;
    }

    QString riskText(int risk) const {
        if (risk <= 25) {
            return "Лёгкий рейс";
        }
        if (risk <= 50) {
            return "Умеренный рейс";
        }
        if (risk <= 75) {
            return "Сложный рейс";
        }
        return "Опасный рейс";
    }

    bool canVisitStop(int index) const {
        if (index < 0 || index >= stops.size()) {
            return false;
        }

        if (index != firstUnvisitedIndex()) {
            return false;
        }

        const RouteStop& stop = stops[index];
        const int risk = routeRiskValue();

        if (risk > stop.maxSafeRisk) {
            return false;
        }

        if (stop.forbiddenInStorm && weatherBox->currentIndex() == 3) {
            return false;
        }

        return true;
    }

    StopState stateForStop(int index) const {
        if (index < 0 || index >= stops.size()) {
            return StopState::Planned;
        }

        if (stops[index].visited) {
            return StopState::Visited;
        }

        const int nextIndex = firstUnvisitedIndex();
        if (index == nextIndex) {
            return canVisitStop(index) ? StopState::Current : StopState::Blocked;
        }

        return StopState::Planned;
    }

    QString accessRuleText(const RouteStop& stop) const {
        QString rule = QString("Можно пройти, если риск рейса не выше %1.").arg(stop.maxSafeRisk);
        if (stop.forbiddenInStorm) {
            rule += " Во время шторма этот этап недоступен.";
        } else {
            rule += " В шторм этот этап допустим, если общий риск остаётся в пределах нормы.";
        }
        return rule;
    }

    QString blockingReasonForStop(int index) const {
        if (index < 0 || index >= stops.size()) {
            return "Некорректный этап.";
        }

        if (stops[index].visited) {
            return "Этап уже пройден.";
        }

        const int nextIndex = firstUnvisitedIndex();
        if (index != nextIndex) {
            return QString("Маршрут проходится строго по порядку. Сейчас доступен этап %1.")
                .arg(nextIndex + 1);
        }

        const RouteStop& stop = stops[index];
        const int risk = routeRiskValue();

        if (risk > stop.maxSafeRisk) {
            return QString("Сейчас риск равен %1, а для этого этапа допустимо не больше %2.")
                .arg(risk)
                .arg(stop.maxSafeRisk);
        }

        if (stop.forbiddenInStorm && weatherBox->currentIndex() == 3) {
            return "Этот этап запрещён во время шторма.";
        }

        return "Этап доступен.";
    }

    QString stopDescription(const RouteStop& stop, int index) const {
        const StopState state = stateForStop(index);

        return QString(
                   "Дата: %1\n"
                   "Погода: %2\n"
                   "Высота волн: %3 / 100\n"
                   "Точка маршрута: %4\n"
                   "Задача экипажа: %5\n"
                   "Заметка кота-штурмана: %6\n"
                   "Правило этапа: %7\n"
                   "Текущее состояние: %8\n"
                   "Пояснение: %9")
            .arg(calendar->selectedDate().toString("dd.MM.yyyy"))
            .arg(weatherName())
            .arg(waveSlider->value())
            .arg(stop.title)
            .arg(stop.action)
            .arg(stop.hint)
            .arg(accessRuleText(stop))
            .arg(stopStateText(state))
            .arg(blockingReasonForStop(index));
    }
