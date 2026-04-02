#include "mainwindow.h"

#include <QComboBox>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QGraphicsDropShadowEffect>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    SetupUi();
    EnsureStorageFileExists();
    LoadCardsFromFile();
    UpdateCardsList();

    if (!cards_.isEmpty()) {
        SelectCard(0);
    } else {
        UpdateEditor();
    }
}

QString MainWindow::ProjectRootPath() const {
    QString env = qEnvironmentVariable("PROJECT_ROOT");
    if (!env.isEmpty()) {
        return env;
    }

    QDir dir(QDir::currentPath());

    while (true) {
        if (dir.exists("MODULE.bazel")) {
            return dir.absolutePath();
        }

        if (!dir.cdUp()) {
            break;
        }
    }

    return QDir::currentPath();
}

QString MainWindow::StoragePath() const {
    QDir dir(ProjectRootPath());
    dir.mkpath("labs/basics/Homework1_2/data");
    return dir.filePath("labs/basics/Homework1_2/data/cards.json");
}

QString MainWindow::ResolveImagePath(const QString& path) const {
    if (path.isEmpty()) {
        return "";
    }

    QFileInfo info(path);
    if (info.isAbsolute()) {
        return path;
    }

    QDir root(ProjectRootPath());
    return root.filePath(path);
}

void MainWindow::EnsureStorageFileExists() {
    QFile file(StoragePath());
    if (file.exists()) {
        return;
    }

    qDebug() << "Creating initial storage file at:"
             << QFileInfo(StoragePath()).absoluteFilePath();

    if (file.open(QIODevice::WriteOnly)) {
        QJsonObject root;
        root["cards"] = QJsonArray{};
        file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
        file.close();
    }
}

QColor MainWindow::RarityColor(Rarity rarity) const {
    switch (rarity) {
        case Common:    return QColor(170, 175, 185);
        case Rare:      return QColor(80, 155, 255);
        case Epic:      return QColor(180, 95, 255);
        case Mythic:    return QColor(255, 140, 70);
        case Legendary: return QColor(255, 210, 90);
        case Unknown:   return QColor(255, 80, 180);
    }
    return QColor(170, 175, 185);
}

QColor MainWindow::RaritySecondaryColor(Rarity rarity) const {
    switch (rarity) {
        case Common:    return QColor(95, 100, 110);
        case Rare:      return QColor(35, 90, 170);
        case Epic:      return QColor(90, 45, 150);
        case Mythic:    return QColor(170, 80, 30);
        case Legendary: return QColor(170, 120, 30);
        case Unknown:   return QColor(150, 30, 110);
    }
    return QColor(95, 100, 110);
}

QColor MainWindow::RaritySoftBackground(Rarity rarity) const {
    switch (rarity) {
        case Common:    return QColor(60, 66, 74);
        case Rare:      return QColor(36, 55, 84);
        case Epic:      return QColor(58, 38, 84);
        case Mythic:    return QColor(90, 50, 28);
        case Legendary: return QColor(95, 78, 28);
        case Unknown:   return QColor(80, 26, 68);
    }
    return QColor(60, 66, 74);
}

QString MainWindow::RarityName(Rarity rarity) const {
    switch (rarity) {
        case Common:    return "Нормальная";
        case Rare:      return "Редкая";
        case Epic:      return "Эпическая";
        case Mythic:    return "Мифическая";
        case Legendary: return "Легендарная";
        case Unknown:   return "???";
    }
    return "Нормальная";
}

bool MainWindow::SaveCardsToFile() {
    QJsonArray array;

    for (const auto& card : cards_) {
        QJsonObject obj;
        obj["name"] = card.name;
        obj["age"] = card.age;
        obj["profession"] = card.profession;
        obj["status"] = card.status;
        obj["description"] = card.description;
        obj["image_path"] = card.image_path;
        obj["rarity"] = static_cast<int>(card.rarity);
        array.append(obj);
    }

    QJsonObject root;
    root["cards"] = array;

    QString path = StoragePath();
    qDebug() << "Saving cards to:" << QFileInfo(path).absoluteFilePath();

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "Failed to open file for writing:" << QFileInfo(path).absoluteFilePath();
        return false;
    }

    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool MainWindow::LoadCardsFromFile() {
    QString path = StoragePath();
    qDebug() << "Loading cards from:" << QFileInfo(path).absoluteFilePath();

    QFile file(path);
    if (!file.exists()) {
        qDebug() << "Storage file does not exist:" << QFileInfo(path).absoluteFilePath();
        return false;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file for reading:" << QFileInfo(path).absoluteFilePath();
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isObject()) {
        qDebug() << "Invalid JSON structure in:" << QFileInfo(path).absoluteFilePath();
        return false;
    }

    QJsonObject root = doc.object();
    QJsonArray array = root["cards"].toArray();

    cards_.clear();

    for (const auto& value : array) {
        if (!value.isObject()) {
            continue;
        }

        QJsonObject obj = value.toObject();
        PersonCard card;
        card.name = obj["name"].toString("Без имени");
        card.age = obj["age"].toInt(18);
        card.profession = obj["profession"].toString("");
        card.status = obj["status"].toString("");
        card.description = obj["description"].toString("");
        card.image_path = obj["image_path"].toString("");
        card.rarity = static_cast<Rarity>(obj["rarity"].toInt(0));
        cards_.push_back(card);
    }

    return true;
}

void MainWindow::LoadPhotoIntoLabel(const QString& path, Rarity rarity) {
    QColor c1 = RarityColor(rarity);
    QColor c2 = RaritySecondaryColor(rarity);

    photo_label_->setStyleSheet(QString(
        "QLabel#photoLabel {"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "stop:0 rgb(%1,%2,%3), stop:1 rgb(%4,%5,%6));"
        "border: 3px solid rgb(%1,%2,%3);"
        "border-radius: 14px;"
        "min-width: 220px;"
        "min-height: 220px;"
        "max-width: 220px;"
        "max-height: 220px;"
        "qproperty-alignment: AlignCenter;"
        "font-weight: 700;"
        "color: #f5f7fa;"
        "}"
    )
        .arg(c2.red()).arg(c2.green()).arg(c2.blue())
        .arg(c1.red()).arg(c1.green()).arg(c1.blue()));

    if (path.isEmpty()) {
        photo_label_->setText("Нет фото");
        photo_label_->setPixmap(QPixmap());
        return;
    }

    QString real_path = ResolveImagePath(path);
    qDebug() << "Loading image from:" << real_path;

    QPixmap pix(real_path);
    if (pix.isNull()) {
        photo_label_->setText("Фото\nне найдено");
        photo_label_->setPixmap(QPixmap());
        return;
    }

    photo_label_->setText("");
    photo_label_->setPixmap(pix.scaled(
        220, 220,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
    ));
}

QWidget* MainWindow::CreateListItemWidget(int index) const {
    const auto& card = cards_[index];

    QColor c1 = RarityColor(card.rarity);
    QColor c2 = RaritySecondaryColor(card.rarity);

    auto* root = new QWidget();
    auto* layout = new QHBoxLayout(root);
    layout->setContentsMargins(8, 6, 8, 6);
    layout->setSpacing(12);

    auto* avatar = new QLabel(root);
    avatar->setFixedSize(56, 56);
    avatar->setStyleSheet(QString(
        "QLabel {"
        "border: 2px solid rgb(%1,%2,%3);"
        "border-radius: 10px;"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "stop:0 rgb(%4,%5,%6), stop:1 rgb(%1,%2,%3));"
        "qproperty-alignment: AlignCenter;"
        "font-size: 12px;"
        "font-weight: 700;"
        "color: #f5f7fa;"
        "}"
    )
        .arg(c1.red()).arg(c1.green()).arg(c1.blue())
        .arg(c2.red()).arg(c2.green()).arg(c2.blue()));

    if (!card.image_path.isEmpty()) {
        QString real_path = ResolveImagePath(card.image_path);
        QPixmap pix(real_path);
        if (!pix.isNull()) {
            avatar->setPixmap(pix.scaled(56, 56, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            avatar->setText("?");
        }
    } else {
        avatar->setText("?");
    }

    auto* text_layout = new QVBoxLayout();
    text_layout->setSpacing(2);
    text_layout->setContentsMargins(0, 0, 0, 0);

    auto* name_label = new QLabel(card.name.isEmpty() ? "Без имени" : card.name, root);
    name_label->setStyleSheet("font-weight: 700; font-size: 14px; color: #f8fafc; background: transparent;");

    auto* sub_label = new QLabel(
        QString("%1 • %2")
            .arg(card.profession.isEmpty() ? "Без профессии" : card.profession,
                 RarityName(card.rarity)),
        root);
    sub_label->setStyleSheet("font-size: 12px; color: #d0d9e3; background: transparent;");

    text_layout->addWidget(name_label);
    text_layout->addWidget(sub_label);
    text_layout->addStretch();

    layout->addWidget(avatar);
    layout->addLayout(text_layout, 1);

    root->setStyleSheet(QString(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "stop:0 rgb(%1,%2,%3), stop:1 rgb(%4,%5,%6));"
        "border: 1px solid rgb(%4,%5,%6);"
        "border-radius: 12px;"
    )
        .arg(RaritySoftBackground(card.rarity).red())
        .arg(RaritySoftBackground(card.rarity).green())
        .arg(RaritySoftBackground(card.rarity).blue())
        .arg(c1.red()).arg(c1.green()).arg(c1.blue()));

    return root;
}

void MainWindow::SetupUi() {
    auto* central = new QWidget(this);
    setCentralWidget(central);

    setWindowTitle("Редактор карточек людей");
    resize(1240, 780);
    setMinimumSize(1000, 650);

    central->setStyleSheet(R"(
        QWidget {
            background-color: #0b0f14;
            color: #e6edf3;
            font-size: 14px;
        }

        QGroupBox {
            background-color: #141a22;
            border: 1px solid #2f3a46;
            border-radius: 14px;
            margin-top: 14px;
            padding-top: 14px;
            font-weight: 700;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 14px;
            padding: 0 6px 0 6px;
            color: #f2d188;
        }

        QLineEdit, QSpinBox, QComboBox, QTextEdit {
            background: #0f141a;
            border: 1px solid #394656;
            border-radius: 10px;
            padding: 7px 9px;
            color: #f0f4f8;
        }

        QLineEdit:focus, QSpinBox:focus, QComboBox:focus, QTextEdit:focus {
            border: 2px solid #8eb4d8;
            background: #151b22;
        }

        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #6f5128, stop:1 #a78044);
            color: #fff5dc;
            border: 1px solid #d1aa66;
            border-radius: 10px;
            padding: 8px 14px;
            min-height: 36px;
            font-weight: 700;
        }

        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #7e5c2e, stop:1 #b58d4d);
        }

        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #5c4322, stop:1 #8b6a3a);
        }

        QListWidget {
            background: #0f141b;
            border: 1px solid #2f3a46;
            border-radius: 14px;
            padding: 8px;
        }

        QListWidget::item {
            margin: 4px;
            border-radius: 12px;
        }

        QListWidget::item:selected {
            background-color: rgba(255, 215, 120, 40);
            border: 1px solid #d3b074;
        }

        QLabel#namePreview {
            font-size: 26px;
            font-weight: 800;
            color: #f7e4b2;
        }
    )");

    cards_list_ = new QListWidget(this);
    cards_list_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    new_card_button_ = new QPushButton("Новая карточка", this);
    delete_card_button_ = new QPushButton("Удалить карточку", this);
    save_button_ = new QPushButton("Сохранить", this);
    reload_button_ = new QPushButton("Перезагрузить", this);

    auto* left_buttons_layout = new QHBoxLayout();
    left_buttons_layout->addWidget(new_card_button_);
    left_buttons_layout->addWidget(delete_card_button_);

    auto* left_top_layout = new QHBoxLayout();
    left_top_layout->addWidget(save_button_);
    left_top_layout->addWidget(reload_button_);
    left_top_layout->addStretch();

    auto* left_box = new QGroupBox("Коллекция персонажей", this);
    auto* left_layout = new QVBoxLayout();
    left_layout->addLayout(left_top_layout);
    left_layout->addLayout(left_buttons_layout);
    left_layout->addWidget(cards_list_);
    left_box->setLayout(left_layout);

    editor_box_ = new QGroupBox("Редактор карточки", this);

    editor_background_ = new QWidget(this);
    editor_background_->setStyleSheet("background-color: #222933; border-radius: 12px;");

    photo_label_ = new QLabel("Нет фото", this);
    photo_label_->setObjectName("photoLabel");

    load_photo_button_ = new QPushButton("Загрузить фото", this);

    name_preview_label_ = new QLabel("Без имени", this);
    name_preview_label_->setObjectName("namePreview");

    name_edit_ = new QLineEdit(this);

    age_spinbox_ = new QSpinBox(this);
    age_spinbox_->setRange(0, 150);
    age_spinbox_->setButtonSymbols(QAbstractSpinBox::NoButtons);
    age_spinbox_->setKeyboardTracking(false);

    profession_edit_ = new QLineEdit(this);

    status_edit_ = new QLineEdit(this);
    status_edit_->setPlaceholderText("Введите любой статус");

    rarity_combo_ = new QComboBox(this);
    rarity_combo_->addItem("Нормальная");
    rarity_combo_->addItem("Редкая");
    rarity_combo_->addItem("Эпическая");
    rarity_combo_->addItem("Мифическая");
    rarity_combo_->addItem("Легендарная");
    rarity_combo_->addItem("???");

    description_edit_ = new QTextEdit(this);
    description_edit_->setMinimumHeight(150);

    auto* photo_layout = new QVBoxLayout();
    photo_layout->addWidget(photo_label_, 0, Qt::AlignTop);
    photo_layout->addWidget(load_photo_button_, 0, Qt::AlignTop);
    photo_layout->addStretch();

    auto* form_layout = new QFormLayout();
    form_layout->setVerticalSpacing(14);
    form_layout->setHorizontalSpacing(14);
    form_layout->addRow("Имя:", name_edit_);
    form_layout->addRow("Возраст:", age_spinbox_);
    form_layout->addRow("Профессия:", profession_edit_);
    form_layout->addRow("Статус:", status_edit_);
    form_layout->addRow("Редкость:", rarity_combo_);
    form_layout->addRow("Описание:", description_edit_);

    auto* content_layout = new QHBoxLayout();
    content_layout->addLayout(photo_layout);
    content_layout->addSpacing(18);

    auto* right_side_layout = new QVBoxLayout();
    right_side_layout->addWidget(name_preview_label_);
    right_side_layout->addSpacing(10);
    right_side_layout->addLayout(form_layout);
    content_layout->addLayout(right_side_layout, 1);

    auto* background_layout = new QVBoxLayout(editor_background_);
    background_layout->setContentsMargins(16, 16, 16, 16);
    background_layout->addLayout(content_layout);

    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(28);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor(0, 0, 0, 140));
    editor_background_->setGraphicsEffect(shadow);

    auto* editor_layout = new QVBoxLayout();
    editor_layout->addWidget(editor_background_);
    editor_layout->addStretch();

    editor_box_->setLayout(editor_layout);

    auto* main_layout = new QHBoxLayout();
    main_layout->setContentsMargins(16, 16, 16, 16);
    main_layout->setSpacing(16);
    main_layout->addWidget(left_box, 4);
    main_layout->addWidget(editor_box_, 7);

    central->setLayout(main_layout);

    connect(new_card_button_, &QPushButton::clicked,
            this, &MainWindow::OnNewCardClicked);
    connect(delete_card_button_, &QPushButton::clicked,
            this, &MainWindow::OnDeleteCardClicked);
    connect(cards_list_, &QListWidget::itemClicked,
            this, &MainWindow::OnCardClicked);
    connect(name_edit_, &QLineEdit::textChanged,
            this, &MainWindow::OnNameChanged);
    connect(age_spinbox_, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::OnAgeChanged);
    connect(profession_edit_, &QLineEdit::textChanged,
            this, &MainWindow::OnProfessionChanged);
    connect(status_edit_, &QLineEdit::textChanged,
            this, &MainWindow::OnStatusChanged);
    connect(rarity_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::OnRarityChanged);
    connect(description_edit_, &QTextEdit::textChanged,
            this, &MainWindow::OnDescriptionChanged);
    connect(load_photo_button_, &QPushButton::clicked,
            this, &MainWindow::OnLoadPhotoClicked);
    connect(save_button_, &QPushButton::clicked,
            this, &MainWindow::OnSaveClicked);
    connect(reload_button_, &QPushButton::clicked,
            this, &MainWindow::OnReloadClicked);
}

void MainWindow::UpdateCardsList() {
    cards_list_->clear();

    for (int i = 0; i < cards_.size(); ++i) {
        auto* item = new QListWidgetItem();
        item->setData(Qt::UserRole, i);
        item->setSizeHint(QSize(100, 72));
        cards_list_->addItem(item);
        cards_list_->setItemWidget(item, CreateListItemWidget(i));
    }

    if (current_index_ >= 0 && current_index_ < cards_list_->count()) {
        cards_list_->setCurrentRow(current_index_);
    }
}

void MainWindow::UpdateSingleCardInList(int index) {
    if (index < 0 || index >= cards_.size() || index >= cards_list_->count()) {
        return;
    }

    QListWidgetItem* item = cards_list_->item(index);
    item->setSizeHint(QSize(100, 72));
    cards_list_->setItemWidget(item, CreateListItemWidget(index));
}

void MainWindow::UpdateEditor() {
    bool valid = current_index_ >= 0 && current_index_ < cards_.size();
    editor_box_->setEnabled(valid);
    delete_card_button_->setEnabled(valid);

    if (!valid) {
        name_preview_label_->setText("Без имени");
        name_edit_->clear();
        age_spinbox_->setValue(18);
        profession_edit_->clear();
        status_edit_->clear();
        rarity_combo_->setCurrentIndex(0);
        description_edit_->clear();
        editor_background_->setStyleSheet("background-color: #222933; border-radius: 12px;");
        LoadPhotoIntoLabel("", Common);
        return;
    }

    const auto& card = cards_[current_index_];

    name_preview_label_->setText(card.name.trimmed().isEmpty() ? "Без имени" : card.name);

    name_edit_->blockSignals(true);
    age_spinbox_->blockSignals(true);
    profession_edit_->blockSignals(true);
    status_edit_->blockSignals(true);
    rarity_combo_->blockSignals(true);
    description_edit_->blockSignals(true);

    name_edit_->setText(card.name);
    age_spinbox_->setValue(card.age);
    profession_edit_->setText(card.profession);
    status_edit_->setText(card.status);
    rarity_combo_->setCurrentIndex(static_cast<int>(card.rarity));
    description_edit_->setPlainText(card.description);

    name_edit_->blockSignals(false);
    age_spinbox_->blockSignals(false);
    profession_edit_->blockSignals(false);
    status_edit_->blockSignals(false);
    rarity_combo_->blockSignals(false);
    description_edit_->blockSignals(false);

    QColor c1 = RarityColor(card.rarity);
    QColor c2 = RaritySecondaryColor(card.rarity);

    editor_background_->setStyleSheet(QString(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "stop:0 rgb(%1,%2,%3), stop:1 rgb(%4,%5,%6));"
        "border: 2px solid rgb(%7,%8,%9);"
        "border-radius: 12px;"
    )
        .arg(RaritySoftBackground(card.rarity).red())
        .arg(RaritySoftBackground(card.rarity).green())
        .arg(RaritySoftBackground(card.rarity).blue())
        .arg(c2.red()).arg(c2.green()).arg(c2.blue())
        .arg(c1.red()).arg(c1.green()).arg(c1.blue()));

    LoadPhotoIntoLabel(card.image_path, card.rarity);
}

void MainWindow::SelectCard(int index) {
    if (index < 0 || index >= cards_.size()) {
        return;
    }

    current_index_ = index;
    cards_list_->setCurrentRow(index);
    UpdateEditor();
}

void MainWindow::OnNewCardClicked() {
    PersonCard card;
    card.name = "Новый человек";
    card.age = 18;
    card.profession = "";
    card.status = "";
    card.description = "";
    card.image_path = "";
    card.rarity = Common;

    cards_.push_back(card);
    UpdateCardsList();
    SelectCard(cards_.size() - 1);
}

void MainWindow::OnDeleteCardClicked() {
    if (current_index_ < 0 || current_index_ >= cards_.size()) {
        return;
    }

    cards_.remove(current_index_);

    if (cards_.isEmpty()) {
        current_index_ = -1;
        UpdateCardsList();
        UpdateEditor();
        return;
    }

    if (current_index_ >= cards_.size()) {
        current_index_ = cards_.size() - 1;
    }

    UpdateCardsList();
    SelectCard(current_index_);
}

void MainWindow::OnCardClicked(QListWidgetItem* item) {
    if (!item) {
        return;
    }

    int index = item->data(Qt::UserRole).toInt();
    SelectCard(index);
}

void MainWindow::OnNameChanged(const QString& text) {
    if (current_index_ < 0 || current_index_ >= cards_.size()) {
        return;
    }

    cards_[current_index_].name = text;
    name_preview_label_->setText(text.trimmed().isEmpty() ? "Без имени" : text);
    UpdateSingleCardInList(current_index_);
}

void MainWindow::OnAgeChanged(int value) {
    if (current_index_ < 0 || current_index_ >= cards_.size()) {
        return;
    }

    cards_[current_index_].age = value;
}

void MainWindow::OnProfessionChanged(const QString& text) {
    if (current_index_ < 0 || current_index_ >= cards_.size()) {
        return;
    }

    cards_[current_index_].profession = text;
    UpdateSingleCardInList(current_index_);
}

void MainWindow::OnStatusChanged(const QString& text) {
    if (current_index_ < 0 || current_index_ >= cards_.size()) {
        return;
    }

    cards_[current_index_].status = text;
}

void MainWindow::OnRarityChanged(int index) {
    if (current_index_ < 0 || current_index_ >= cards_.size()) {
        return;
    }

    cards_[current_index_].rarity = static_cast<Rarity>(index);
    UpdateSingleCardInList(current_index_);
    UpdateEditor();
}

void MainWindow::OnDescriptionChanged() {
    if (current_index_ < 0 || current_index_ >= cards_.size()) {
        return;
    }

    cards_[current_index_].description = description_edit_->toPlainText();
}

void MainWindow::OnLoadPhotoClicked() {
    if (current_index_ < 0 || current_index_ >= cards_.size()) {
        return;
    }

    QString path = QFileDialog::getOpenFileName(
        this,
        "Выбрать фотографию",
        "",
        "Images (*.png *.jpg *.jpeg *.bmp)"
    );

    if (path.isEmpty()) {
        return;
    }

    cards_[current_index_].image_path = path;
    LoadPhotoIntoLabel(path, cards_[current_index_].rarity);
    UpdateSingleCardInList(current_index_);
}

void MainWindow::OnSaveClicked() {
    if (SaveCardsToFile()) {
        QMessageBox::information(this, "Сохранение", "Карточки успешно сохранены.");
    } else {
        QMessageBox::warning(this, "Сохранение", "Не удалось сохранить карточки.");
    }
}

void MainWindow::OnReloadClicked() {
    if (LoadCardsFromFile()) {
        UpdateCardsList();
        if (!cards_.isEmpty()) {
            if (current_index_ < 0 || current_index_ >= cards_.size()) {
                current_index_ = 0;
            }
            SelectCard(current_index_);
        } else {
            current_index_ = -1;
            UpdateEditor();
        }
        QMessageBox::information(this, "Загрузка", "Карточки успешно загружены.");
    } else {
        QMessageBox::warning(this, "Загрузка", "Не удалось загрузить карточки.");
    }
}