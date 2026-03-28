#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <QSlider>
#include <QCheckBox>

QT_BEGIN_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void convertValue();
    void toggleTheme(bool enabled);

private:
    QComboBox *velichbox1;
    QComboBox *velichbox2;
    QLineEdit *textline;
    QLabel *resultLabel;
    QSlider *slide1;
    QCheckBox *themeToggle;

};

#endif // MAINWINDOW_H
