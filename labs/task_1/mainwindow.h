#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_CountvalueChanged(int value);
    void on_ViewcellClicked(int row, int column);
    void on_ViewcellDoubleClicked(int row, int column);
    void on_nextclicked();
    void on_previousclicked();
    void on_name_edit_returnPressed();
    void on_randomclicked();
    void on_statuscurrentIndexChanged(int index);
    void on_resetclicked();
    void on_savetriggered();
    void on_loadtriggered();

private:
    void setupUi(QMainWindow *MainWindow);
    void retranslateUi(QMainWindow *MainWindow);
    int currentTicket = -1;
    QVector<int> ticketStatus;
    QVector<QString> ticketNames;
    QVector<QString> ticketContents = {"1. Критерий различия действительных чисел.",
                                           "2. Теорема о плотности множества действительных чисел.",
                                           "3. Несчетность множества действительных чисел.",
                                           "4. Теорема о гранях.",
                                           "5. Определение предела последовательности и M-лемма.",
                                             "6. Единственность предела и ограниченность сходящейся последовательности.",
                                             "7. Сумма, произведение и частное пределов.",
                                           "8. Лемма о сжатой последовательности.",
                                           "9. Сходимость монотонной ограниченной последовательности.",
                                           "10. Существование монотонной подпоследовательности и принцип выбора.",
                                       "11. Число e.",
                                           "12. Критерий Коши для предела последовательности.",
                                           "13. Определение предела функции и M-лемма.",
                                             "14. Критерий Гейне для предела функции.",
                                             "15. Критерий Коши для предела функции.",
                                             "16. Определение непрерывности функции и типы точек разрыва.",
                                             "17. Критерий Гейне непрерывности.",
                                             "18. Критерий непрерывности монотонной функции.",
                                             "19. Непрерывность обратной функции.",
                                             "20. Непрерывность сложной функции.",
                                             "21. Теорема о стабилизации знака.",
                                             "22. Теорема о локальной ограниченности непрерывной функции.",
                                             "23. Непрерывность суммы, произведения, частного функций.",
                                       "24. Теорема о промежуточных значениях непрерывной функции.",
                                       "25. Теорема Вейерштрасса."                                       };

    void displayTicket(int row);
    void updateProgress();
    QWidget *centralwidget;
    QGroupBox *questioview;
    QLabel *number;
    QLabel *name;
    QLineEdit *name_edit;
    QComboBox *status;
    QPushButton *previous;
    QProgressBar *total;
    QProgressBar *green;
    QPushButton *next;
    QSpinBox *Count;
    QPushButton *random;
    QTableWidget *View;
    QMenuBar *menubar;
    QStatusBar *statusbar;
};

#endif
