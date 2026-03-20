    void buildUi() {
        setWindowTitle("Свободное плавание — Кошачья гавань");
        setMinimumSize(960, 650);
        resize(1200, 780);

        auto* rootLayout = new QVBoxLayout(this);
        rootLayout->setContentsMargins(12, 12, 12, 12);
        rootLayout->setSpacing(10);

        auto* headerFrame = new QFrame(this);
        headerFrame->setObjectName("headerFrame");
        auto* headerLayout = new QHBoxLayout(headerFrame);
        headerLayout->setContentsMargins(16, 12, 16, 12);

        auto* headerTextLayout = new QVBoxLayout();
        headerTitleLabel = new QLabel("🐱 Свободное плавание", this);
        headerTitleLabel->setObjectName("headerTitleLabel");

        headerSubtitleLabel = new QLabel(
            "Кото-морской пульт: выбери дату, оцени риск и проведи капитана по этапам маршрута.",
            this);
        headerSubtitleLabel->setObjectName("headerSubtitleLabel");
        headerSubtitleLabel->setWordWrap(true);

        headerTextLayout->addWidget(headerTitleLabel);
        headerTextLayout->addWidget(headerSubtitleLabel);

        auto* headerCats = new QLabel("⛵🐾", this);
        headerCats->setObjectName("headerCats");

        headerLayout->addLayout(headerTextLayout, 1);
        headerLayout->addWidget(headerCats);

        rootLayout->addWidget(headerFrame);

        auto createCatPhoto = [this](const QString& resourcePath, const QString& fallbackText) {
            auto* label = new QLabel(this);
            label->setObjectName("catPhotoLabel");
            label->setAlignment(Qt::AlignCenter);
            label->setMinimumHeight(170);
            label->setMaximumHeight(205);
            label->setWordWrap(true);

            const QPixmap photo(resourcePath);
            if (!photo.isNull()) {
                label->setPixmap(
                    photo.scaled(340, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            } else {
                label->setText(fallbackText);
            }

            return label;
        };

        auto createCatThumb = [this](const QString& resourcePath, const QString& fallbackText) {
            auto* label = new QLabel(this);
            label->setObjectName("catThumbLabel");
            label->setAlignment(Qt::AlignCenter);
            label->setMinimumSize(94, 74);
            label->setMaximumHeight(88);
            label->setWordWrap(true);

            const QPixmap photo(resourcePath);
            if (!photo.isNull()) {
                label->setPixmap(photo.scaled(146, 84, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            } else {
                label->setText(fallbackText);
            }

            return label;
        };

        auto* leftContentWidget = new QWidget(this);
        auto* leftContentLayout = new QVBoxLayout(leftContentWidget);
        leftContentLayout->setContentsMargins(0, 0, 0, 0);

        auto* leftGroup = new QGroupBox("Пульт котокапитана", this);
        auto* leftLayout = new QVBoxLayout(leftGroup);
        leftLayout->setSpacing(10);

        controlCatLabel = new QLabel("🐱 Штурман ждёт имя капитана и параметры рейса", this);
        controlCatLabel->setObjectName("controlCatLabel");
        controlCatLabel->setWordWrap(true);
        auto* controlCatPhoto =
            createCatPhoto(":/cats/task2/cat1.jpg", "😺 Штурман временно вне кадра");
        auto* controlCatThumbsRow = new QHBoxLayout();
        controlCatThumbsRow->setSpacing(8);
        controlCatThumbsRow->addWidget(
            createCatThumb(":/cats/task2/cat3.jpg", "😺"));
        controlCatThumbsRow->addWidget(
            createCatThumb(":/cats/task2/cat4.jpg", "😺"));

        auto* captainLabel = new QLabel("Имя капитана-кота:", this);
        captainLabel->setObjectName("sectionLabel");
        captainEdit = new QLineEdit(this);
        captainEdit->setPlaceholderText("Например: Барсик Мореплаватель");

        auto* weatherLabel = new QLabel("Погода:", this);
        weatherLabel->setObjectName("sectionLabel");
        weatherBox = new QComboBox(this);
        weatherBox->addItems({"Штиль", "Лёгкий бриз", "Волны", "Шторм", "Закатное плавание"});

        weatherBadge = new QLabel("Погода: Штиль", this);
        weatherBadge->setObjectName("weatherBadge");
        weatherBadge->setAlignment(Qt::AlignCenter);
        weatherBadge->setMinimumHeight(28);

        auto* waveLabel = new QLabel("Высота волн:", this);
        waveLabel->setObjectName("sectionLabel");
        waveSlider = new QSlider(Qt::Horizontal, this);
        waveSlider->setRange(0, 100);
        waveSlider->setValue(35);

        waveValueLabel = new QLabel("35 / 100", this);
        waveValueLabel->setObjectName("waveValueLabel");
        waveValueLabel->setAlignment(Qt::AlignCenter);

        auto* waveRow = new QHBoxLayout();
        waveRow->addWidget(waveSlider, 1);
        waveRow->addWidget(waveValueLabel);

        auto* calendarLabel = new QLabel("Дата отплытия:", this);
        calendarLabel->setObjectName("sectionLabel");
        calendar = new QCalendarWidget(this);
        calendar->setGridVisible(true);
        calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
        calendar->setHorizontalHeaderFormat(QCalendarWidget::ShortDayNames);
        calendar->setFirstDayOfWeek(Qt::Monday);
        calendar->setMinimumHeight(210);
        calendar->setMaximumHeight(230);
        calendar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        calendarInfoLabel = new QLabel("📅 Дата влияет на риск рейса.", this);
        calendarInfoLabel->setObjectName("calendarInfoLabel");
        calendarInfoLabel->setWordWrap(true);

        moodLabel = new QLabel("🐾 Настроение маршрута появится здесь", this);
        moodLabel->setObjectName("moodLabel");
        moodLabel->setWordWrap(true);

        auto* riskCard = new QFrame(this);
        riskCard->setObjectName("riskCard");
        auto* riskCardLayout = new QVBoxLayout(riskCard);
        riskCardLayout->setContentsMargins(10, 10, 10, 10);

        auto* riskTitle = new QLabel("Сложность рейса", this);
        riskTitle->setObjectName("sectionLabel");

        riskLabel = new QLabel("Уровень риска: 0", this);
        riskLabel->setObjectName("riskLabel");
        riskLabel->setWordWrap(true);

        riskBar = new QProgressBar(this);
        riskBar->setObjectName("riskBar");
        riskBar->setRange(0, 100);
        riskBar->setValue(0);
        riskBar->setFormat("Риск: 0%");

        riskCardLayout->addWidget(riskTitle);
        riskCardLayout->addWidget(riskLabel);
        riskCardLayout->addWidget(riskBar);

        generateButton = new QPushButton("Построить маршрут 🧭", this);
        generateButton->setObjectName("generateButton");

        resetVisitsButton = new QPushButton("Сбросить посещения", this);
        resetVisitsButton->setObjectName("resetButton");

        exportButton = new QPushButton("Экспорт рейса в файл", this);
        exportButton->setObjectName("exportButton");

        auto* buttonsRow1 = new QHBoxLayout();
        buttonsRow1->addWidget(generateButton);
        buttonsRow1->addWidget(resetVisitsButton);

        auto* buttonsRow2 = new QHBoxLayout();
        buttonsRow2->addWidget(exportButton);
        buttonsRow2->addStretch();

        shortcutHint = new QLabel(
            "⌨️ Горячие клавиши: Ctrl+G — новый маршрут, Ctrl+R — сброс посещений, Ctrl+E — экспорт",
            this);
        shortcutHint->setObjectName("shortcutHint");
        shortcutHint->setWordWrap(true);

        leftLayout->addWidget(controlCatLabel);
        leftLayout->addWidget(controlCatPhoto);
        leftLayout->addLayout(controlCatThumbsRow);
        leftLayout->addWidget(captainLabel);
        leftLayout->addWidget(captainEdit);
        leftLayout->addWidget(weatherLabel);
        leftLayout->addWidget(weatherBox);
        leftLayout->addWidget(weatherBadge);
        leftLayout->addWidget(waveLabel);
        leftLayout->addLayout(waveRow);
        leftLayout->addWidget(calendarLabel);
        leftLayout->addWidget(calendar);
        leftLayout->addWidget(calendarInfoLabel);
        leftLayout->addWidget(moodLabel);
        leftLayout->addWidget(riskCard);
        leftLayout->addLayout(buttonsRow1);
        leftLayout->addLayout(buttonsRow2);
        leftLayout->addWidget(shortcutHint);

        leftContentLayout->addWidget(leftGroup);
        leftContentLayout->addStretch();

        auto* leftScroll = new QScrollArea(this);
        leftScroll->setWidgetResizable(true);
        leftScroll->setFrameShape(QFrame::NoFrame);
        leftScroll->setObjectName("leftScroll");
        leftScroll->setWidget(leftContentWidget);

        auto* rightGroup = new QGroupBox("Маршрут плавания", this);
        auto* rightLayout = new QVBoxLayout(rightGroup);
        rightLayout->setSpacing(10);

        routeBannerLabel = new QLabel("🐾 Кот-боцман уже разложил карту маршрута", this);
        routeBannerLabel->setObjectName("routeBannerLabel");
        routeBannerLabel->setWordWrap(true);
        auto* routeCatPhoto =
            createCatPhoto(":/cats/task2/cat2.jpg", "😺 Фото боцмана недоступно");
        auto* routeCatThumbsRow = new QHBoxLayout();
        routeCatThumbsRow->setSpacing(8);
        routeCatThumbsRow->addWidget(
            createCatThumb(":/cats/task2/cat5.png", "😺"));
        routeCatThumbsRow->addWidget(
            createCatThumb(":/cats/task2/cat6.jpg", "😺"));

        auto* summaryCard = new QFrame(this);
        summaryCard->setObjectName("summaryCard");
        auto* summaryCardLayout = new QVBoxLayout(summaryCard);
        summaryCardLayout->setContentsMargins(10, 10, 10, 10);

        auto* summaryTitle = new QLabel("Сводка рейса", this);
        summaryTitle->setObjectName("sectionLabel");

        summaryLabel = new QLabel("Здесь появится краткая информация о рейсе.", this);
        summaryLabel->setObjectName("summaryLabel");
        summaryLabel->setWordWrap(true);

        readinessBar = new QProgressBar(this);
        readinessBar->setObjectName("readinessBar");
        readinessBar->setRange(0, 100);
        readinessBar->setValue(0);
        readinessBar->setFormat("Готовность рейса: 0%");

        summaryCardLayout->addWidget(summaryTitle);
        summaryCardLayout->addWidget(summaryLabel);
        summaryCardLayout->addWidget(readinessBar);

        auto* filterCard = new QFrame(this);
        filterCard->setObjectName("filterCard");
        auto* filterLayout = new QVBoxLayout(filterCard);
        filterLayout->setContentsMargins(10, 10, 10, 10);
        filterLayout->setSpacing(8);

        auto* filterTitle = new QLabel("Фильтры маршрута", this);
        filterTitle->setObjectName("sectionLabel");

        searchEdit = new QLineEdit(this);
        searchEdit->setPlaceholderText("Поиск по названию, задаче или заметке...");

        onlyUnvisitedBox = new QCheckBox("Показывать только непосещённые точки", this);
        onlyUnvisitedBox->setObjectName("onlyUnvisitedBox");

        filterLayout->addWidget(filterTitle);
        filterLayout->addWidget(searchEdit);
        filterLayout->addWidget(onlyUnvisitedBox);

        routeList = new QListWidget(this);
        routeList->setSelectionMode(QAbstractItemView::SingleSelection);
        routeList->setSpacing(6);
        routeList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        routeList->setMinimumHeight(220);

        auto* detailsCard = new QFrame(this);
        detailsCard->setObjectName("detailsCard");
        auto* detailsLayout = new QVBoxLayout(detailsCard);
        detailsLayout->setContentsMargins(10, 10, 10, 10);

        auto* detailsTitle = new QLabel("Текущая точка маршрута", this);
        detailsTitle->setObjectName("sectionLabel");

        selectedStopTitle = new QLabel("Маршрут ещё не выбран", this);
        selectedStopTitle->setObjectName("selectedStopTitle");
        selectedStopTitle->setWordWrap(true);

        selectedStopDescription = new QTextEdit(this);
        selectedStopDescription->setObjectName("selectedStopDescription");
        selectedStopDescription->setReadOnly(true);
        selectedStopDescription->setMinimumHeight(150);
        selectedStopDescription->setPlaceholderText(
            "Нажми на точку маршрута слева, чтобы увидеть детали.");

        detailsLayout->addWidget(detailsTitle);
        detailsLayout->addWidget(selectedStopTitle);
        detailsLayout->addWidget(selectedStopDescription, 1);

        auto* rightSplitter = new QSplitter(Qt::Vertical, this);
        rightSplitter->addWidget(routeList);
        rightSplitter->addWidget(detailsCard);
        rightSplitter->setStretchFactor(0, 3);
        rightSplitter->setStretchFactor(1, 2);
        rightSplitter->setChildrenCollapsible(false);

        rightLayout->addWidget(routeBannerLabel);
        rightLayout->addWidget(routeCatPhoto);
        rightLayout->addLayout(routeCatThumbsRow);
        rightLayout->addWidget(summaryCard);
        rightLayout->addWidget(filterCard);
        rightLayout->addWidget(rightSplitter, 1);

        auto* mainSplitter = new QSplitter(Qt::Horizontal, this);
        mainSplitter->addWidget(leftScroll);
        mainSplitter->addWidget(rightGroup);
        mainSplitter->setStretchFactor(0, 2);
        mainSplitter->setStretchFactor(1, 3);
        mainSplitter->setChildrenCollapsible(false);

        rootLayout->addWidget(mainSplitter, 1);
    }

    void applyStyle() {
        setStyleSheet(
            "QWidget {"
            "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
            "    stop:0 #fff9fc, stop:0.45 #f6f9ff, stop:1 #f0fbff);"
            "  color: #283042;"
            "  font-size: 13px;"
            "}"

            "QFrame#headerFrame {"
            "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
            "    stop:0 #ffd9ea, stop:0.5 #dff1ff, stop:1 #d9fff3);"
            "  border: 1px solid #e7dceb;"
            "  border-radius: 20px;"
            "}"

            "QLabel#headerTitleLabel {"
            "  font-size: 25px;"
            "  font-weight: 800;"
            "  color: #4d3f75;"
            "  background: transparent;"
            "}"

            "QLabel#headerSubtitleLabel {"
            "  color: #5e6681;"
            "  background: transparent;"
            "}"

            "QLabel#headerCats {"
            "  font-size: 30px;"
            "  background: transparent;"
            "  color: #6c5ea8;"
            "  padding: 4px;"
            "}"

            "QGroupBox {"
            "  font-weight: 700;"
            "  border: 1px solid #e1e5f0;"
            "  border-radius: 16px;"
            "  margin-top: 12px;"
            "  padding-top: 14px;"
            "  background: rgba(255, 255, 255, 0.9);"
            "}"

            "QGroupBox::title {"
            "  subcontrol-origin: margin;"
            "  left: 12px;"
            "  padding: 0 8px;"
            "  color: #4f5575;"
            "}"

            "QLabel#sectionLabel {"
            "  font-weight: 700;"
            "  color: #4c5470;"
            "  background: transparent;"
            "}"

            "QLabel#controlCatLabel {"
            "  background: #fff6d8;"
            "  border: 1px solid #f0dc97;"
            "  border-radius: 12px;"
            "  padding: 8px 10px;"
            "  color: #7a5a00;"
            "  font-weight: 700;"
            "}"

            "QLabel#routeBannerLabel {"
            "  background: #eaf6ff;"
            "  border: 1px solid #b5daf5;"
            "  border-radius: 12px;"
            "  padding: 8px 10px;"
            "  color: #255a7b;"
            "  font-weight: 700;"
            "}"

            "QLabel#catPhotoLabel {"
            "  background: rgba(255, 255, 255, 0.97);"
            "  border: 1px solid #d3def1;"
            "  border-radius: 14px;"
            "  padding: 6px;"
            "  color: #5a6684;"
            "  font-weight: 600;"
            "}"

            "QLabel#catThumbLabel {"
            "  background: rgba(255, 255, 255, 0.97);"
            "  border: 1px solid #d7e0f0;"
            "  border-radius: 10px;"
            "  padding: 4px;"
            "  color: #5e6d8a;"
            "  font-size: 12px;"
            "}"

            "QLabel#weatherBadge {"
            "  border-radius: 12px;"
            "  padding: 5px 10px;"
            "  font-weight: 700;"
            "}"

            "QLabel#calendarInfoLabel {"
            "  background: #eef8ea;"
            "  border: 1px solid #bddfb0;"
            "  border-radius: 12px;"
            "  padding: 8px 10px;"
            "  color: #2e6a36;"
            "}"

            "QLabel#moodLabel {"
            "  background: #f7f1ff;"
            "  border: 1px solid #dac9ff;"
            "  border-radius: 12px;"
            "  padding: 8px 10px;"
            "  color: #60459a;"
            "}"

            "QLabel#shortcutHint {"
            "  background: #eef8ea;"
            "  border: 1px solid #bddfb0;"
            "  border-radius: 12px;"
            "  padding: 8px 10px;"
            "  color: #2e6a36;"
            "}"

            "QFrame#summaryCard, QFrame#detailsCard, QFrame#filterCard, QFrame#riskCard {"
            "  background: rgba(255, 255, 255, 0.97);"
            "  border: 1px solid #e8eaf2;"
            "  border-radius: 14px;"
            "}"

            "QLineEdit, QComboBox, QSlider, QCalendarWidget, QListWidget, QProgressBar, QTextEdit {"
            "  border-radius: 10px;"
            "}"

            "QLineEdit, QComboBox, QTextEdit {"
            "  background: white;"
            "  border: 1px solid #d7dced;"
            "  padding: 7px 9px;"
            "}"

            "QLineEdit:focus, QComboBox:focus, QTextEdit:focus {"
            "  border: 2px solid #9a89ff;"
            "}"

            "QSlider::groove:horizontal {"
            "  height: 8px;"
            "  background: #e7ebf6;"
            "  border-radius: 4px;"
            "}"

            "QSlider::handle:horizontal {"
            "  width: 16px;"
            "  margin: -5px 0;"
            "  border-radius: 8px;"
            "  background: #8d7cf7;"
            "}"

            "QLabel#waveValueLabel {"
            "  min-width: 78px;"
            "  background: #f2edff;"
            "  border: 1px solid #d7ceff;"
            "  border-radius: 10px;"
            "  padding: 5px 7px;"
            "  color: #5f4f99;"
            "  font-weight: 700;"
            "}"

            "QLabel#riskLabel {"
            "  color: #4d5875;"
            "  font-weight: 700;"
            "}"

            "QCalendarWidget QWidget {"
            "  alternate-background-color: #f7fbff;"
            "}"

            "QCalendarWidget QToolButton {"
            "  color: #4c5470;"
            "  font-weight: 700;"
            "}"

            "QCalendarWidget QAbstractItemView:enabled {"
            "  background: white;"
            "  selection-background-color: #bfe8ff;"
            "  selection-color: #243042;"
            "}"

            "QListWidget {"
            "  background: rgba(255, 255, 255, 0.96);"
            "  border: 1px solid #dfe4f0;"
            "  padding: 8px;"
            "  outline: none;"
            "  selection-background-color: transparent;"
            "  selection-color: #222a3a;"
            "}"

            "QListWidget::item {"
            "  border: 1px solid #d9dfed;"
            "  border-radius: 12px;"
            "  padding: 8px;"
            "  margin: 3px;"
            "}"

            "QListWidget::item:selected {"
            "  border: 2px solid #7c6ef7;"
            "}"

            "QPushButton {"
            "  min-height: 38px;"
            "  border-radius: 12px;"
            "  padding: 7px 12px;"
            "  font-weight: 800;"
            "  border: none;"
            "}"

            "QPushButton#generateButton {"
            "  background: #c9f3df;"
            "  color: #1c6b3a;"
            "}"

            "QPushButton#generateButton:hover {"
            "  background: #b6ebd0;"
            "}"

            "QPushButton#resetButton {"
            "  background: #ece6ff;"
            "  color: #57458f;"
            "}"

            "QPushButton#resetButton:hover {"
            "  background: #dfd6ff;"
            "}"

            "QPushButton#exportButton {"
            "  background: #ffe7cb;"
            "  color: #8a5615;"
            "}"

            "QPushButton#exportButton:hover {"
            "  background: #ffdcb1;"
            "}"

            "QCheckBox#onlyUnvisitedBox {"
            "  color: #43506b;"
            "  font-weight: 600;"
            "}"

            "QLabel#selectedStopTitle {"
            "  font-size: 17px;"
            "  font-weight: 800;"
            "  color: #35405e;"
            "}"

            "QTextEdit#selectedStopDescription, QLabel#summaryLabel {"
            "  color: #4f5874;"
            "}"

            "QProgressBar {"
            "  min-height: 22px;"
            "  text-align: center;"
            "  background: #f4f7fb;"
            "  border: 1px solid #d8ddea;"
            "  font-weight: 700;"
            "}"

            "QProgressBar#readinessBar::chunk {"
            "  border-radius: 9px;"
            "  background: #7dd6a2;"
            "}"

            "QProgressBar#riskBar::chunk {"
            "  border-radius: 9px;"
            "  background: #8fc6ff;"
            "}"

            "QScrollArea#leftScroll {"
            "  border: none;"
            "  background: transparent;"
            "}"

            "QSplitter::handle {"
            "  background: transparent;"
            "  width: 6px;"
            "  height: 6px;"
            "}");
    }

    void configureCalendarAppearance() {
        QTextCharFormat weekdayFormat;
        weekdayFormat.setForeground(QColor("#4c5470"));

        QTextCharFormat weekendFormat;
        weekendFormat.setForeground(QColor("#69748f"));

        calendar->setWeekdayTextFormat(Qt::Monday, weekdayFormat);
        calendar->setWeekdayTextFormat(Qt::Tuesday, weekdayFormat);
        calendar->setWeekdayTextFormat(Qt::Wednesday, weekdayFormat);
        calendar->setWeekdayTextFormat(Qt::Thursday, weekdayFormat);
        calendar->setWeekdayTextFormat(Qt::Friday, weekdayFormat);
        calendar->setWeekdayTextFormat(Qt::Saturday, weekendFormat);
        calendar->setWeekdayTextFormat(Qt::Sunday, weekendFormat);

        QTextCharFormat todayFormat;
        todayFormat.setForeground(QColor("#4d3f75"));
        todayFormat.setBackground(QColor("#fff1b8"));
        todayFormat.setFontWeight(QFont::Bold);
        calendar->setDateTextFormat(QDate::currentDate(), todayFormat);
    }

    void setupShortcuts() {
        auto* generateShortcut = new QShortcut(QKeySequence("Ctrl+G"), this);
        generateShortcut->setContext(Qt::ApplicationShortcut);
        connect(generateShortcut, &QShortcut::activated, this, [this]() { generateRoute(); });

        auto* resetShortcut = new QShortcut(QKeySequence("Ctrl+R"), this);
        resetShortcut->setContext(Qt::ApplicationShortcut);
        connect(resetShortcut, &QShortcut::activated, this, [this]() { resetVisitedStops(); });

        auto* exportShortcut = new QShortcut(QKeySequence("Ctrl+E"), this);
        exportShortcut->setContext(Qt::ApplicationShortcut);
        connect(exportShortcut, &QShortcut::activated, this, [this]() { exportRoute(); });
    }

    void connectSignals() {
        connect(captainEdit, &QLineEdit::textChanged, this, [this]() {
            updateCaptainHeader();
            updateSummary();
        });

        connect(weatherBox, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int) {
            updateWeatherBadge();
            updateRiskStatus();
            recomputeRouteLogic();
        });

        connect(waveSlider, &QSlider::valueChanged, this, [this](int value) {
            updateWaveLabel(value);
            updateRiskStatus();
            recomputeRouteLogic();
        });

        connect(calendar, &QCalendarWidget::selectionChanged, this, [this]() {
            updateCalendarInfo();
            updateRiskStatus();
            recomputeRouteLogic();
        });

        connect(generateButton, &QPushButton::clicked, this, [this]() { generateRoute(); });

        connect(resetVisitsButton, &QPushButton::clicked, this, [this]() { resetVisitedStops(); });

        connect(exportButton, &QPushButton::clicked, this, [this]() { exportRoute(); });

        connect(routeList, &QListWidget::currentRowChanged, this, [this](int) {
            refreshAllRouteItems();
            updateDetailsPanel();
        });

        connect(routeList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
            tryVisitStop(routeList->row(item));
        });

        connect(searchEdit, &QLineEdit::textChanged, this, [this]() { applyRouteFilter(); });

        connect(onlyUnvisitedBox, &QCheckBox::toggled, this, [this](bool) { applyRouteFilter(); });
    }
