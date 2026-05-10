#ifndef ADMIN_DIALOG_H
#define ADMIN_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTabWidget>
#include <QTextEdit>

class AdminDialog : public QDialog {
    Q_OBJECT

   public:
    explicit AdminDialog(QWidget* parent = nullptr);
    ~AdminDialog() = default;

   private slots:
    void loadTranslations();
    void loadGrammar();
    void addTranslation();
    void deleteTranslation();
    void addGrammar();
    void deleteGrammar();

   private:
    void setupUi();

    QTabWidget* tab_widget_;

    // Translation Tab
    QListWidget* translation_list_;
    QLineEdit* trans_prompt_edit_;
    QLineEdit* trans_answer_edit_;

    // Grammar Tab
    QListWidget* grammar_list_;
    QLineEdit* grammar_prompt_edit_;
    QLineEdit* grammar_options_edit_;
    QLineEdit* grammar_correct_edit_;
};

#endif                                // ADMIN_DIALOG_H
