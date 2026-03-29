#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

// NOLINTBEGIN(readability-identifier-naming, cppcoreguidelines-owning-memory)

#include "exercisedata.h" 
#include <QDialog>

QT_BEGIN_NAMESPACE
class QComboBox;
class QPushButton;
QT_END_NAMESPACE

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(Difficulty currentDifficulty, QWidget *parent = nullptr);
    [[nodiscard]] Difficulty selectedDifficulty() const;

private:
    QComboBox *difficultyComboBox;
    QPushButton *okButton;
    QPushButton *cancelButton;

    Difficulty m_currentDifficulty;
};

// NOLINTEND(readability-identifier-naming, cppcoreguidelines-owning-memory)

#endif // SETTINGSDIALOG_H