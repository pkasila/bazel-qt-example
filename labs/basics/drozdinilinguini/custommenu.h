#ifndef CUSTOMMENU_H
#define CUSTOMMENU_H

#include <QButtonGroup>
#include <QDialog>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>

class CustomMenu : public QDialog {
    Q_OBJECT
   private:
    int m_mode = 1;

   public:
    CustomMenu(QWidget* parent = nullptr) : QDialog(parent) {
        setWindowTitle("Mode");
        setFixedSize(200, 150);

        QVBoxLayout* layout = new QVBoxLayout(this);

        QLabel* label = new QLabel("Choose the mode!");
        layout->addWidget(label);

        QButtonGroup* buttonGroup = new QButtonGroup(this);

        QRadioButton* radio1 = new QRadioButton("Beginner", this);
        QRadioButton* radio2 = new QRadioButton("Intermediate", this);
        QRadioButton* radio3 = new QRadioButton("Advanced", this);

        buttonGroup->addButton(radio1, 1);
        buttonGroup->addButton(radio2, 2);
        buttonGroup->addButton(radio3, 3);

        layout->addWidget(radio1);
        layout->addWidget(radio2);
        layout->addWidget(radio3);

        radio1->setChecked(true);

        connect(buttonGroup, &QButtonGroup::idClicked, this, &CustomMenu::setMode);
    }

    int mode() const {
        return m_mode;
    }

   public slots:

    void setMode(int id) {
        if (id >= 1 && id <= 3) {
            m_mode = id;
            emit modeChanged(m_mode);
        }
    }

   signals:
    void modeChanged(int newMode);
};

#endif  // CUSTOMMENU_H