    void saveSettings() {
        QSettings settings("qt_labs", "cat_free_sailing");

        settings.setValue("geometry", saveGeometry());
        settings.setValue("captain", captainEdit->text());
        settings.setValue("search", searchEdit->text());
        settings.setValue("onlyUnvisited", onlyUnvisitedBox->isChecked());
        settings.setValue("weatherIndex", weatherBox->currentIndex());
        settings.setValue("waves", waveSlider->value());
        settings.setValue("date", calendar->selectedDate());
        settings.setValue("selectedRow", routeList->currentRow());

        settings.beginWriteArray("stops");
        for (int i = 0; i < stops.size(); ++i) {
            settings.setArrayIndex(i);
            settings.setValue("title", stops[i].title);
            settings.setValue("action", stops[i].action);
            settings.setValue("hint", stops[i].hint);
            settings.setValue("visited", stops[i].visited);
            settings.setValue("maxSafeRisk", stops[i].maxSafeRisk);
            settings.setValue("forbiddenInStorm", stops[i].forbiddenInStorm);
        }
        settings.endArray();
    }

    void loadSettings() {
        QSettings settings("qt_labs", "cat_free_sailing");

        const QByteArray geometry = settings.value("geometry").toByteArray();
        if (!geometry.isEmpty()) {
            restoreGeometry(geometry);
        }

        captainEdit->setText(settings.value("captain", "Барсик Мореплаватель").toString());
        searchEdit->setText(settings.value("search", "").toString());
        onlyUnvisitedBox->setChecked(settings.value("onlyUnvisited", false).toBool());
        weatherBox->setCurrentIndex(settings.value("weatherIndex", 0).toInt());
        waveSlider->setValue(settings.value("waves", 35).toInt());
        calendar->setSelectedDate(settings.value("date", QDate::currentDate()).toDate());

        stops.clear();
        const int stopCount = settings.beginReadArray("stops");
        for (int i = 0; i < stopCount; ++i) {
            settings.setArrayIndex(i);

            RouteStop stop;
            stop.title = settings.value("title").toString();
            stop.action = settings.value("action").toString();
            stop.hint = settings.value("hint").toString();
            stop.visited = settings.value("visited", false).toBool();
            stop.maxSafeRisk = settings.value("maxSafeRisk", 60).toInt();
            stop.forbiddenInStorm = settings.value("forbiddenInStorm", false).toBool();

            if (!stop.title.isEmpty()) {
                stops.push_back(stop);
            }
        }
        settings.endArray();

        if (!stops.isEmpty()) {
            rebuildRouteList();

            const int savedRow = settings.value("selectedRow", 0).toInt();
            if (savedRow >= 0 && savedRow < routeList->count()) {
                routeList->setCurrentRow(savedRow);
            }
        }
    }
