#ifndef LT_SETTINGS_DIALOG_H
#define LT_SETTINGS_DIALOG_H

#include <QCheckBox>
#include <QDialog>
#include <QPushButton>

class LTSettingsDialog : public QDialog {
    Q_OBJECT
   public:
    explicit LTSettingsDialog(QWidget* parent = nullptr);

   private slots:
    void onSaveClicked();

   private:
    QCheckBox* grammar_cb_;
    QCheckBox* syntax_cb_;
    QCheckBox* typos_cb_;
    QCheckBox* punctuation_cb_;
    QCheckBox* style_cb_;
    QPushButton* save_btn_;
};

#endif  // LT_SETTINGS_DIALOG_H
