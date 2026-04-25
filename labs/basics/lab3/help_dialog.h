#ifndef HELP_DIALOG_H
#define HELP_DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QGroupBox>

class HelpDialog : public QDialog {
    Q_OBJECT

public:
    explicit HelpDialog(QWidget *parent = nullptr);

private:
    void setupUI();
    void applyTheme();
    
    // UI Components
    QLabel* titleLabel;
    QTextEdit* helpContent;
    QPushButton* closeButton;
    
    QString getTranslationHelp();
    QString getGrammarHelp();
};

#endif // HELP_DIALOG_H
