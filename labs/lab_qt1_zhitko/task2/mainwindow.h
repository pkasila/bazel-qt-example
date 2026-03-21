#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onConvert();
    void onCategoryChanged(int index);
    void onLiveConvert();
    void onThemeChanged(int index);
    void onClear();

private:
    void setupUI();
    void applyStyle(const QString &theme);
    double convertValue(double input, const QString &from, const QString &to, const QString &category);
    void updateUnits();

    QComboBox *comboCategory;
    QComboBox *comboFrom;
    QComboBox *comboTo;
    QDoubleSpinBox *spinInput;
    QPushButton *btnConvert;
    QLabel *labelResult;
    QFrame *panel;
    QLabel *labelStatus;
    QComboBox *comboTheme;
};

#endif