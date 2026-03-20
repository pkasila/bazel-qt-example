    QLabel* createLegendChip(const QString& text, const QString& objectName) {
        auto* label = new QLabel(text, this);
        label->setObjectName(objectName);
        label->setAlignment(Qt::AlignCenter);
        label->setMinimumHeight(30);
        return label;
    }

    void buildUi() {
        setWindowTitle("Прокрастинация — повторение билетов");
        setMinimumSize(980, 660);
        resize(1220, 760);

        auto* rootLayout = new QVBoxLayout(this);
        rootLayout->setContentsMargins(18, 18, 18, 18);
        rootLayout->setSpacing(14);

        auto* headerFrame = new QFrame(this);
        headerFrame->setObjectName("headerFrame");
        auto* headerLayout = new QHBoxLayout(headerFrame);
        headerLayout->setContentsMargins(18, 16, 18, 16);

        auto* titleLayout = new QVBoxLayout();
        auto* titleLabel = new QLabel("🐱 Прокрастинация", this);
        titleLabel->setObjectName("titleLabel");

        auto* subtitleLabel = new QLabel(
            "Удобное приложение для повторения билетов: статусы, случайный выбор, история и "
            "прогресс",
            this);
        subtitleLabel->setObjectName("subtitleLabel");
        subtitleLabel->setWordWrap(true);

        titleLayout->addWidget(titleLabel);
        titleLayout->addWidget(subtitleLabel);

        auto* headerPaws = new QLabel("🐾🐾", this);
        headerPaws->setObjectName("headerPaws");

        headerLayout->addLayout(titleLayout, 1);
        headerLayout->addWidget(headerPaws);

        rootLayout->addWidget(headerFrame);

        countSpin = new QSpinBox(this);
        countSpin->setRange(1, 500);
        countSpin->setValue(20);
        countSpin->setObjectName("countSpin");

        view = new QListWidget(this);
        view->setViewMode(QListView::IconMode);
        view->setFlow(QListView::LeftToRight);
        view->setWrapping(true);
        view->setResizeMode(QListView::Adjust);
        view->setMovement(QListView::Static);
        view->setSpacing(10);
        view->setWordWrap(true);
        view->setGridSize(QSize(195, 94));
        view->setSelectionMode(QAbstractItemView::SingleSelection);
        view->setUniformItemSizes(true);

        auto createCatPhoto = [this](const QString& resourcePath, const QString& fallbackText) {
            auto* label = new QLabel(this);
            label->setObjectName("catPhotoLabel");
            label->setAlignment(Qt::AlignCenter);
            label->setMinimumHeight(180);
            label->setMaximumHeight(210);
            label->setWordWrap(true);

            const QPixmap photo(resourcePath);
            if (!photo.isNull()) {
                label->setPixmap(
                    photo.scaled(360, 190, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            } else {
                label->setText(fallbackText);
            }

            return label;
        };

        auto createCatThumb = [this](const QString& resourcePath, const QString& fallbackText) {
            auto* label = new QLabel(this);
            label->setObjectName("catThumbLabel");
            label->setAlignment(Qt::AlignCenter);
            label->setMinimumSize(96, 76);
            label->setMaximumHeight(90);
            label->setWordWrap(true);

            const QPixmap photo(resourcePath);
            if (!photo.isNull()) {
                label->setPixmap(photo.scaled(150, 86, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            } else {
                label->setText(fallbackText);
            }

            return label;
        };

        auto* leftGroup = new QGroupBox("Список билетов", this);
        auto* leftLayout = new QVBoxLayout(leftGroup);
        leftLayout->setSpacing(12);

        auto* leftCatBanner = new QLabel("🐱 Котик следит за повторением билетов", this);
        leftCatBanner->setObjectName("leftCatBanner");
        auto* leftCatPhoto =
            createCatPhoto(":/cats/task1/cat1.jpg", "😺 Фото котика временно недоступно");
        auto* leftCatThumbsRow = new QHBoxLayout();
        leftCatThumbsRow->setSpacing(8);
        leftCatThumbsRow->addWidget(
            createCatThumb(":/cats/task1/cat3.jpg", "😺"));
        leftCatThumbsRow->addWidget(
            createCatThumb(":/cats/task1/cat4.jpg", "😺"));

        auto* countRow = new QHBoxLayout();
        auto* countLabel = new QLabel("Количество билетов:", this);
        countLabel->setObjectName("sectionLabel");
        countRow->addWidget(countLabel);
        countRow->addWidget(countSpin);
        countRow->addStretch();

        auto* legendRow = new QHBoxLayout();
        legendRow->setSpacing(8);
        legendRow->addWidget(createLegendChip("⬜ Не повторял", "legendDefault"));
        legendRow->addWidget(createLegendChip("🟨 Нужно ещё раз", "legendYellow"));
        legendRow->addWidget(createLegendChip("🟩 Повторён", "legendGreen"));
        legendRow->addStretch();

        auto* listHint = new QLabel(
            "Один клик — открыть билет. Двойной клик — быстро переключить статус между зелёным и "
            "жёлтым.",
            this);
        listHint->setObjectName("hintLabel");
        listHint->setWordWrap(true);

        leftLayout->addWidget(leftCatBanner);
        leftLayout->addWidget(leftCatPhoto);
        leftLayout->addLayout(leftCatThumbsRow);
        leftLayout->addLayout(countRow);
        leftLayout->addLayout(legendRow);
        leftLayout->addWidget(listHint);
        leftLayout->addWidget(view, 1);

        numberValue = new QLabel("—", this);
        numberValue->setMinimumHeight(24);
        nameValue = new QLabel("—", this);
        nameValue->setMinimumHeight(24);
        nameValue->setWordWrap(true);

        statusBadge = new QLabel("Не выбран", this);
        statusBadge->setObjectName("statusBadge");
        statusBadge->setAlignment(Qt::AlignCenter);
        statusBadge->setFixedHeight(42);
        statusBadge->setMinimumWidth(210);
        statusBadge->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);

        nameEdit = new QLineEdit(this);
        nameEdit->setPlaceholderText("Введите новое имя билета и нажмите Enter");

        statusCombo = new QComboBox(this);
        statusCombo->addItems({"Не повторял", "Нужно ещё раз", "Повторён"});
        statusCombo->setObjectName("statusCombo");

        previousButton = new QPushButton("← Предыдущий", this);
        previousButton->setObjectName("previousButton");

        nextButton = new QPushButton("Следующий случайный →", this);
        nextButton->setObjectName("nextButton");

        totalProgress = new QProgressBar(this);
        totalProgress->setObjectName("totalProgress");

        greenProgress = new QProgressBar(this);
        greenProgress->setObjectName("greenProgress");

        auto* rightGroup = new QGroupBox("Текущий билет", this);
        auto* rightLayout = new QVBoxLayout(rightGroup);
        rightLayout->setSpacing(14);

        auto* ticketCard = new QFrame(this);
        ticketCard->setObjectName("ticketCard");
        auto* ticketCardLayout = new QVBoxLayout(ticketCard);
        ticketCardLayout->setSpacing(10);

        auto* form = new QFormLayout();
        form->setLabelAlignment(Qt::AlignLeft);
        form->setFormAlignment(Qt::AlignTop);
        form->setSpacing(12);
        form->addRow("Номер:", numberValue);
        form->addRow("Имя:", nameValue);

        ticketCardLayout->addLayout(form);

        auto* statusRow = new QHBoxLayout();
        statusRow->setSpacing(10);
        statusRow->setContentsMargins(0, 0, 0, 6);
        auto* statusLabel = new QLabel("Статус:", this);
        statusLabel->setObjectName("sectionLabel");
        statusRow->addWidget(statusLabel);
        statusRow->addWidget(statusBadge);
        statusRow->addStretch();
        ticketCardLayout->addLayout(statusRow);
        ticketCardLayout->addSpacing(6);

        auto* editLabel = new QLabel("Переименовать билет:", this);
        editLabel->setObjectName("sectionLabel");
        ticketCardLayout->addWidget(editLabel);
        ticketCardLayout->addWidget(nameEdit);

        auto* comboLabel = new QLabel("Изменить статус:", this);
        comboLabel->setObjectName("sectionLabel");
        ticketCardLayout->addWidget(comboLabel);
        ticketCardLayout->addWidget(statusCombo);

        rightLayout->addWidget(ticketCard);

        auto* buttonsRow = new QHBoxLayout();
        buttonsRow->setSpacing(10);
        buttonsRow->addWidget(previousButton);
        buttonsRow->addWidget(nextButton);
        rightLayout->addLayout(buttonsRow);

        auto* progressGroup = new QGroupBox("Прогресс подготовки", this);
        auto* progressLayout = new QVBoxLayout(progressGroup);
        progressLayout->setSpacing(10);

        auto* totalLabel = new QLabel("Общий прогресс (жёлтые + зелёные):", this);
        totalLabel->setObjectName("sectionLabel");
        auto* greenLabel = new QLabel("Полностью выученные билеты (зелёные):", this);
        greenLabel->setObjectName("sectionLabel");

        progressLayout->addWidget(totalLabel);
        progressLayout->addWidget(totalProgress);
        progressLayout->addWidget(greenLabel);
        progressLayout->addWidget(greenProgress);

        rightLayout->addWidget(progressGroup);
        rightLayout->addStretch();

        auto* splitter = new QSplitter(Qt::Horizontal, this);
        splitter->addWidget(leftGroup);
        splitter->addWidget(rightGroup);
        splitter->setStretchFactor(0, 3);
        splitter->setStretchFactor(1, 2);
        splitter->setChildrenCollapsible(false);

        rootLayout->addWidget(splitter, 1);
    }

    void applyStyle() {
        setStyleSheet(
            "QWidget {"
            "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
            "    stop:0 #fff8fb, stop:0.5 #f7f8ff, stop:1 #f7fcff);"
            "  font-size: 14px;"
            "  color: #2d3142;"
            "}"

            "QFrame#headerFrame {"
            "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
            "    stop:0 #ffd9e8, stop:0.5 #e6e8ff, stop:1 #d8f3ff);"
            "  border: 1px solid #eadced;"
            "  border-radius: 20px;"
            "}"

            "QLabel#titleLabel {"
            "  font-size: 28px;"
            "  font-weight: 800;"
            "  color: #4b3f72;"
            "  background: transparent;"
            "}"

            "QLabel#subtitleLabel {"
            "  font-size: 14px;"
            "  color: #5e6280;"
            "  background: transparent;"
            "}"

            "QLabel#headerPaws {"
            "  font-size: 34px;"
            "  color: #7f6db0;"
            "  background: transparent;"
            "  padding: 6px 10px;"
            "}"

            "QGroupBox {"
            "  font-weight: 700;"
            "  border: 1px solid #e3e5f1;"
            "  border-radius: 18px;"
            "  margin-top: 14px;"
            "  padding-top: 16px;"
            "  background: rgba(255, 255, 255, 0.88);"
            "}"

            "QGroupBox::title {"
            "  subcontrol-origin: margin;"
            "  left: 14px;"
            "  padding: 0 8px;"
            "  color: #51456f;"
            "}"

            "QLabel#sectionLabel {"
            "  font-weight: 600;"
            "  color: #4e5572;"
            "  background: transparent;"
            "}"

            "QLabel#leftCatBanner {"
            "  background: #fff7d9;"
            "  border: 1px solid #f0df9e;"
            "  border-radius: 14px;"
            "  padding: 8px 12px;"
            "  font-weight: 700;"
            "  color: #7a5c00;"
            "}"

            "QLabel#catPhotoLabel {"
            "  background: rgba(255, 255, 255, 0.95);"
            "  border: 1px solid #d8dfef;"
            "  border-radius: 14px;"
            "  padding: 6px;"
            "  color: #5c6481;"
            "  font-weight: 600;"
            "}"

            "QLabel#catThumbLabel {"
            "  background: rgba(255, 255, 255, 0.96);"
            "  border: 1px solid #dde3f2;"
            "  border-radius: 10px;"
            "  padding: 4px;"
            "  color: #62708f;"
            "  font-size: 12px;"
            "}"

            "QLabel#hintLabel {"
            "  background: #fff8dc;"
            "  border: 1px solid #f1e0a3;"
            "  border-radius: 12px;"
            "  padding: 8px 10px;"
            "  color: #78653f;"
            "}"

            "QLabel#legendDefault {"
            "  background: #f3f5f8;"
            "  border: 1px solid #d7dce8;"
            "  border-radius: 12px;"
            "  padding: 6px 10px;"
            "  color: #5b6177;"
            "  font-weight: 600;"
            "}"

            "QLabel#legendYellow {"
            "  background: #fff1b8;"
            "  border: 1px solid #efd36f;"
            "  border-radius: 12px;"
            "  padding: 6px 10px;"
            "  color: #7a5d00;"
            "  font-weight: 600;"
            "}"

            "QLabel#legendGreen {"
            "  background: #d8f5dd;"
            "  border: 1px solid #99ddb0;"
            "  border-radius: 12px;"
            "  padding: 6px 10px;"
            "  color: #22633b;"
            "  font-weight: 600;"
            "}"

            "QFrame#ticketCard {"
            "  background: rgba(255, 255, 255, 0.96);"
            "  border: 1px solid #ececf5;"
            "  border-radius: 16px;"
            "}"

            "QLineEdit, QComboBox, QSpinBox {"
            "  padding: 8px 10px;"
            "  border-radius: 10px;"
            "  border: 1px solid #d9dced;"
            "  background: white;"
            "}"

            "QLineEdit:focus, QComboBox:focus, QSpinBox:focus {"
            "  border: 2px solid #a58bff;"
            "}"

            "QListWidget {"
            "  border: 1px solid #dde0ef;"
            "  border-radius: 16px;"
            "  padding: 10px;"
            "  background: rgba(255, 255, 255, 0.93);"
            "  selection-background-color: transparent;"
            "  selection-color: #1f2430;"
            "  outline: none;"
            "}"

            "QListWidget::item {"
            "  border-radius: 14px;"
            "  padding: 8px;"
            "  margin: 6px;"
            "  border: 1px solid #d9dceb;"
            "}"

            "QListWidget::item:hover {"
            "  border: 1px solid #bfc5ea;"
            "}"

            "QListWidget::item:selected {"
            "  border: 2px solid #7b6ef6;"
            "}"

            "QPushButton {"
            "  min-height: 40px;"
            "  border-radius: 12px;"
            "  padding: 8px 14px;"
            "  font-weight: 700;"
            "  border: none;"
            "}"

            "QPushButton#previousButton {"
            "  background: #e8e4ff;"
            "  color: #4f4581;"
            "}"

            "QPushButton#previousButton:hover {"
            "  background: #ddd7ff;"
            "}"

            "QPushButton#nextButton {"
            "  background: #c9f7d6;"
            "  color: #1f6a39;"
            "}"

            "QPushButton#nextButton:hover {"
            "  background: #baf0ca;"
            "}"

            "QPushButton:disabled {"
            "  background: #ececf1;"
            "  color: #9b9fb2;"
            "}"

            "QLabel#statusBadge {"
            "  border-radius: 14px;"
            "  font-weight: 700;"
            "  padding: 4px 12px;"
            "}"

            "QProgressBar {"
            "  min-height: 24px;"
            "  border-radius: 10px;"
            "  border: 1px solid #d7dcec;"
            "  text-align: center;"
            "  background: #f5f7fb;"
            "  font-weight: 600;"
            "}"

            "QProgressBar#totalProgress::chunk {"
            "  border-radius: 9px;"
            "  background: #ffd56b;"
            "}"

            "QProgressBar#greenProgress::chunk {"
            "  border-radius: 9px;"
            "  background: #7fd98d;"
            "}"

            "QSplitter::handle {"
            "  background: transparent;"
            "  width: 8px;"
            "}");
    }

    void connectSignals() {
        connect(countSpin, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
            resetTickets(value);
        });

        connect(view, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
            selectTicket(view->row(item), true);
        });

        connect(view, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
            const int index = view->row(item);
            toggleStatusFromView(index);
        });

        connect(
            statusCombo, qOverload<int>(&QComboBox::currentIndexChanged), this,
            [this](int comboIndex) {
                if (currentIndex < 0 || currentIndex >= tickets.size()) {
                    return;
                }
                setTicketStatus(currentIndex, statusFromComboIndex(comboIndex));
            });

        connect(nameEdit, &QLineEdit::returnPressed, this, [this]() { renameCurrentTicket(); });

        connect(nextButton, &QPushButton::clicked, this, [this]() { chooseNextRandomTicket(); });

        connect(previousButton, &QPushButton::clicked, this, [this]() { goToPreviousTicket(); });
    }
