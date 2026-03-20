#include "text_editor.h"

#include <QtWidgets>
#include <iterator>

//NOLINTBEGIN(cppcoreguidelines-owning-memory, *-unused-return-value)
TextEditor::TextEditor() : path_label_(new QLabel()), menu_bar_(new QMenuBar()), font_spin_box_(new QSpinBox()), font_combo_box_(new QFontComboBox()), auto_save_check_box_(new QCheckBox("Auto save")), tab_widget_(new QTabWidget()) {
    auto* widget = new QWidget();
    setCentralWidget(widget);

    setStyleSheet(R"(
        QWidget {
            background-color: #282c34;
            color: #ffffff;
        }
        QMenuBar {
            background-color: #21252b;
            color: #ffffff;
            padding: 4px;
        }
        QMenuBar::item {
            background: transparent;
            padding: 5px 10px;
        }
        QMenuBar::item:selected {
            background: #3a3f4b;
        }
        QMenu {
            background-color: #282c34;
            color: #ffffff;
            border: 1px solid #3a3f4b;
        }
        QMenu::item:selected {
            background: #3a3f4b;
        }
        QToolBar {
            background: #21252b;
            border: none;
            padding: 5px;
        }
        QToolButton {
            background: transparent;
            border: none;
            padding: 6px;
        }
        QToolButton:hover {
            background: #3a3f4b;
            border-radius: 4px;
        }
        QTabWidget::pane {
            border: 1px solid #3a3f4b;
            background: #21252b;
        }
        QTabBar::tab {
            background: #282c34;
            padding: 8px;
            border: 1px solid #3a3f4b;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
        }
        QTabBar::tab:selected {
            background: #3a3f4b;
            color: #ffffff;
        }
        QTextEdit {
            background: #1e2127;
            color: #ffffff;
            border: none;
            padding: 8px;
        }
        
        
    )");

    path_label_->setText("File isn't opened");

    font_spin_box_->setMinimum(10);
    font_spin_box_->setValue(15);

    file_menu_ = menu_bar_->addMenu("File");
    new_action_ = file_menu_->addAction("New");
    open_action_ = file_menu_->addAction("Open");
    save_action_ = file_menu_->addAction("Save");
    save_as_action_ = file_menu_->addAction("Save as");

    new_action_->setShortcut(QKeySequence("Ctrl+N"));
    open_action_->setShortcut(QKeySequence("Ctrl+O"));
    save_action_->setShortcut(QKeySequence("Ctrl+S"));
    save_as_action_->setShortcut(QKeySequence("Ctrl+Shift+S"));

    setMenuBar(menu_bar_);

    tab_widget_->setTabsClosable(true);

    connect(new_action_, &QAction::triggered, this, &TextEditor::NewFile);
    connect(open_action_, &QAction::triggered, this, &TextEditor::OpenFile);
    connect(save_action_, &QAction::triggered, this, &TextEditor::SaveFile);
    connect(save_as_action_, &QAction::triggered, this, &TextEditor::SaveAsFile);
    connect(font_spin_box_, &QSpinBox::valueChanged, this, &TextEditor::ChangeFontSize);
    connect(tab_widget_, &QTabWidget::currentChanged, this, &TextEditor::CurrentTabChanged);
    connect(tab_widget_, &QTabWidget::tabCloseRequested, this, &TextEditor::TabClosed);
    connect(font_combo_box_, &QFontComboBox::currentFontChanged, this, &TextEditor::CurrentFontChanged);
    connect(auto_save_check_box_, &QCheckBox::checkStateChanged, this, &TextEditor::AutoSaveCheckStateChanged);

    auto* v_layout = new QVBoxLayout();
    auto* h_layout = new QHBoxLayout();

    h_layout->setSpacing(10);

    h_layout->addWidget(new QLabel("Font size:"));
    h_layout->addWidget(font_spin_box_);
    h_layout->addWidget(font_combo_box_);
    h_layout->addWidget(auto_save_check_box_);
    h_layout->addStretch();
    v_layout->addWidget(tab_widget_);
    v_layout->addItem(h_layout);
    v_layout->addWidget(path_label_);

    CreateTab();

    widget->setLayout(v_layout);

    setMinimumSize(kDefaultWidth / 2, kDefaultHeight / 2);
    resize(kDefaultWidth, kDefaultHeight);
}

#ifndef QT_NO_CONTEXTMENU
void TextEditor::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(new_action_);
    menu.addAction(open_action_);
    menu.addAction(save_action_);
    menu.addAction(save_as_action_);
    menu.exec(event->globalPos());
}
#endif // QT_NO_CONTEXTMENU


void TextEditor::NewFile() {
    CreateTab();
}


void TextEditor::OpenFile() {
    const auto path = QFileDialog::getOpenFileName(this, "Open file");
    if (path.isEmpty()) { return; }
    const auto index = tab_widget_->currentIndex();
    auto* edit_text = dynamic_cast<QTextEdit*>(tab_widget_->currentWidget());
    if (paths_[index].isEmpty() && edit_text->toPlainText().isEmpty() && !tab_widget_->tabText(index).startsWith('*')) {
        edit_text->setPlainText(ReadFile(path));
        const auto title = QFileInfo(path).fileName();
        setWindowTitle(title);
        path_label_->setText(path);
        tab_widget_->setTabText(index, title);
        paths_[index] = path;
    } else {
        CreateTab(path);
    }
}

void TextEditor::SaveFile() {
    if (const auto& path = paths_[tab_widget_->currentIndex()]; WriteToFile(path, dynamic_cast<QTextEdit*>(tab_widget_->currentWidget())->toPlainText())) {
        SetSavedNameForCurrentTab();
    } else {
        SaveAsFile();
    }
}

void TextEditor::SaveAsFile() {
    if(const auto path = QFileDialog::getSaveFileName(this, "Save file"); WriteToFile(path, dynamic_cast<QTextEdit*>(tab_widget_->currentWidget())->toPlainText())) {
        const auto title = QFileInfo(path).fileName();
        path_label_->setText(path);
        tab_widget_->setTabText(tab_widget_->currentIndex(), title);
        setWindowTitle(title);
        paths_[tab_widget_->currentIndex()] = path;
    }
}

void TextEditor::SetFontSize(QTextEdit* text_edit, int size) {
    auto font = text_edit->font();
    font.setPointSize(size);
    text_edit->setFont(font);
}

void TextEditor::SetFontSize(QTextEdit* text_edit) const {
    SetFontSize(text_edit, font_spin_box_->value());
}


void TextEditor::ChangeFontSize(int value) const {
    for (int i = 0; i < tab_widget_->count(); ++i) {
        SetFontSize(dynamic_cast<QTextEdit*>(tab_widget_->widget(i)), value);
    }
}

void TextEditor::TabClosed(int index) {
    if (tab_widget_->count() != 1) {
        paths_.erase(std::next(paths_.begin(), index));
        tab_widget_->removeTab(index);
    }
}

void TextEditor::CurrentFontChanged(const QFont& font) {
    auto font1 = font;
    font1.setPointSize(font_spin_box_->value());
    for (int i = 0; i < tab_widget_->count(); ++i) {
        dynamic_cast<QTextEdit*>(tab_widget_->widget(i))->setFont(font1);
    }
}

void TextEditor::AutoSaveCheckStateChanged(Qt::CheckState state) {
    auto_save_ = (state == Qt::CheckState::Checked);
    if (auto_save_ && !paths_[tab_widget_->currentIndex()].isEmpty()) {
        SaveFile();        
    }
}


void TextEditor::CreateTab(const QString& path) {
    auto* text_edit = new QTextEdit();
    SetFontSize(text_edit);
    paths_.push_back(path);
    if (path.isEmpty()) {
        setWindowTitle("Untitled");
        tab_widget_->addTab(text_edit, "Untitled");
    } else {
        const auto title = QFileInfo(path).fileName();
        setWindowTitle(title);
        tab_widget_->addTab(text_edit, title);
        path_label_->setText(path);
        text_edit->setText(ReadFile(path));
    }
    SetPathToLabel(path);
    connect(text_edit, &QTextEdit::textChanged, [this] {
        if (IsCurrentTabSaved()) {
            SetUnsavedNameForCurrentTab();
        }
        if (auto_save_ && !paths_[tab_widget_->currentIndex()].isEmpty()) {
            SaveFile();
        }
    });
    tab_widget_->setCurrentIndex(tab_widget_->count() - 1);
}


QString TextEditor::ReadFile(const QString& path) {
    if (path.isEmpty()) { return ""; }
    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Unable to read file", file.errorString());
        return "";
    }
    QTextStream in(&file);
    const auto result = in.readAll();
    file.close();
    return result;
}

bool TextEditor::WriteToFile(const QString& path, const QString& data) {
    if (path.isEmpty()) { return false; }
    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Unable to write to file", file.errorString());
        return false;
    }
    QTextStream out(&file);
    out << data;
    file.close();
    return true;
}

bool TextEditor::IsCurrentTabSaved() const {
    return !tab_widget_->tabText(tab_widget_->currentIndex()).startsWith('*');
}

void TextEditor::SetUnsavedNameForCurrentTab() {
    const auto index = tab_widget_->currentIndex();
    const auto name = '*' + tab_widget_->tabText(index);
    tab_widget_->setTabText(index, name);
    setWindowTitle(name);
}

void TextEditor::SetSavedNameForCurrentTab() {
    const auto index = tab_widget_->currentIndex();
    const auto name = tab_widget_->tabText(index).mid(1);
    tab_widget_->setTabText(index, name);
    setWindowTitle(name);
}

void TextEditor::CurrentTabChanged(int index) {
    setWindowTitle(tab_widget_->tabText(index));
    SetPathToLabel(paths_[index]);
}

void TextEditor::SetPathToLabel(const QString& path) {
    path_label_->setText(path.isEmpty() ? "File isn't opened" : path);
}

//NOLINTEND(cppcoreguidelines-owning-memory, *-unused-return-value)