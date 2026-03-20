//
// Created by blalex on 3/14/25.
//

#include "main_window.h"

#include "calculator_controller.h"

#include <QComboBox>
#include <QFile>
#include <QLineEdit>
#include <QMainWindow>
#include <QVBoxLayout>
#include <cstddef>
#include <qdir.h>
#include <qlayoutitem.h>
#include <qmenubar.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qwidget.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , main_layout_(new QVBoxLayout())
    , controller_(new CalculatorController(this))
    , button_(new QPushButton("Add new unit", this))
    , main_layout1_(new QVBoxLayout())
    , menu_bar_(new QMenuBar(this)) {
    main_layout_->addWidget(button_);

    setMenuBar(menu_bar_);

    QMenu* fileMenu = menu_bar_->addMenu("&File");

    QAction* saveAction = new QAction("&Save", this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::SaveSettings);
    fileMenu->addAction(saveAction);

    QAction* openAction = new QAction("&Open", this);
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::LoadSettings);
    fileMenu->addAction(openAction);

    auto* central_widget = new QWidget(this);
    central_widget->setLayout(main_layout1_);
    setCentralWidget(central_widget);
    setWindowTitle("Distance unit converter");

    AddField();
    AddField();
    AddField();

    QSpacerItem* spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    main_layout1_->addLayout(main_layout_);
    main_layout1_->addSpacerItem(spacer);
    connect(button_, &QPushButton::clicked, this, &MainWindow::AddField);
}

void MainWindow::AddField() {
    double base_value;
    if (std::size(controller_->fields_) == 0) {
        base_value = 0;
    } else {
        base_value = controller_->fields_.front().value_->text().toDouble() *
                     to_base_[controller_->fields_.front().unit_->currentText()];
    }

    std::unordered_set<QString> fields;
    for (auto& field : controller_->fields_) {
        fields.insert(field.unit_->currentText());
    }

    QString necessary_unit = "m";
    for (const auto& unit_ : units_) {
        if (fields.find(unit_) == fields.end()) {
            necessary_unit = unit_;
            break;
        }
    }

    base_value = base_value * from_base_[necessary_unit];

    QHBoxLayout* fieldLayout = new QHBoxLayout;
    QLineEdit* value = new QLineEdit(QString::number(base_value));
    QComboBox* unit = new QComboBox;

    unit->addItem("Metrical:");
    unit->addItems({"m", "cm", "km", "dm", "mm"});
    unit->insertSeparator(unit->count());

    unit->addItem("British/American:");
    unit->addItems({"mi", "yd", "ft", "in"});
    unit->insertSeparator(unit->count());

    unit->addItem("Chinese:");
    unit->addItems({"li", "zhang", "chi", "cun"});
    unit->insertSeparator(unit->count());

    unit->addItem("Astronomical:");
    unit->addItems({"au", "ly", "pc"});
    unit->insertSeparator(unit->count());

    unit->addItem("Sea:");
    unit->addItems({"nmi"});

    unit->setCurrentText(necessary_unit);

    fieldLayout->addWidget(value);
    fieldLayout->addWidget(unit);
    main_layout_->addLayout(fieldLayout);

    controller_->fields_.push_back({value, unit, necessary_unit});
    connect(
        value, &QLineEdit::textEdited, this, [this, value] { controller_->UpdateValues(value); });
    connect(unit, &QComboBox::currentIndexChanged, this, [this, value, unit] {
        controller_->UpdateUnit(value, unit->currentText());
    });
}

void MainWindow::SaveSettings() {
    QFile file(filepath_);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);

    if (!controller_->fields_.empty()) {
        out << controller_->fields_.front().value_->text().toDouble() *
                   to_base_[controller_->fields_.front().unit_->currentText()]
            << Qt::endl;
    }

    for (const auto& field : controller_->fields_) {
        out << field.prev_value_ << Qt::endl;
    }
}

void MainWindow::LoadSettings() {
    СlearAllFields();

    QFile file(filepath_);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    std::vector<QString> values;

    while (!in.atEnd()) {
        values.push_back(in.readLine());
    }

    if (values.empty()) {
        return;
    }

    double first_value = values.front().toDouble();

    controller_->fields_.front().value_->setText(QString::number(first_value));
    controller_->fields_.front().unit_->setCurrentText(values[1]);

    for (size_t i = 2; i < std::size(values); ++i) {
        AddField();
        controller_->fields_[i - 1].unit_->setCurrentText(values[i]);
    }
}

void MainWindow::СlearAllFields() {
    while (controller_->fields_.size() > 1) {
        auto& lastField = controller_->fields_.back();
        delete lastField.value_;
        delete lastField.unit_;
        controller_->fields_.pop_back();
    }

    if (!controller_->fields_.empty()) {
        controller_->fields_.front().value_->clear();
        controller_->fields_.front().unit_->setCurrentIndex(1);
    }
}