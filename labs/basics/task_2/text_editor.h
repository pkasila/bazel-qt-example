#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <vector>

constexpr auto kDefaultWidth = 1200;
constexpr auto kDefaultHeight = 600;

constexpr auto kDefaultTicketsCount = 30;

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QTextEdit;
class QMenuBar;
class QMenu;
class QSpinBox;
class QTabWidget;
class QFontComboBox;
class QCheckBox;
class QString;
QT_END_NAMESPACE

class TextEditor : public QMainWindow {
    Q_OBJECT

public:
    TextEditor();

private slots:
    void NewFile();
    void OpenFile();
    void SaveFile();
    void SaveAsFile();
    void ChangeFontSize(int value) const;
    void CurrentTabChanged(int index);
    void TabClosed(int index);
    void CurrentFontChanged(const QFont& font);
    void AutoSaveCheckStateChanged(Qt::CheckState state);


protected:
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif // QT_NO_CONTEXTMENU


private:
    QLabel* path_label_;
    QMenu* file_menu_;
    QMenuBar* menu_bar_;
    QAction* new_action_;
    QAction* open_action_;
    QAction* save_action_;
    QAction* save_as_action_;
    QSpinBox* font_spin_box_;
    QFontComboBox* font_combo_box_;
    QCheckBox* auto_save_check_box_;
    QTabWidget* tab_widget_;

    std::vector<QString> paths_;
    int current_tab_ = 0;
    bool auto_save_ = false;

    [[nodiscard]] QString ReadFile(const QString& path);
    bool WriteToFile(const QString& path, const QString& data);
    void CreateTab(const QString& path = "");
    void SetFontSize(QTextEdit* text_edit) const;
    [[nodiscard]] bool IsCurrentTabSaved() const;
    void SetUnsavedNameForCurrentTab();
    void SetSavedNameForCurrentTab();
    void SetPathToLabel(const QString& path);
    static void SetFontSize(QTextEdit* text_edit, int size);

};

#endif