#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QLabel>
#include <QListView>
#include <QTableWidget>

#pragma GCC target("avx512")

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void resetBoolsRecords();

private slots:
    void humsterClick();
    void showAchievements();
    void showShop();

private:
    void updateBars();

    //Ui::MainWindow *ui;
    QPushButton *imageButton;
    QProgressBar *progressBar;
    QLabel *lab_lvl;
    QLabel *lab_coins;
    int lvl = 0;
    std::vector<__int128_t> nums = {100, 500, 1000};

    QLabel *lab_coins_count;
    __int128_t coins = 0;

    __int128_t taps = 0;
    int64_t taps_copy = 0;

    QTableWidget *table_awards;
    std::vector<QString> vec_records =
        {"Начни тапать!",
         "100 кликов!",
         "1000 кликов!",
         "Скупить всю крипту!",
         "Набрать 2000 мощи!",
         "Набрать 3000 мощи!"};
    QPushButton *awards;
    QWidget *awardsWindow = nullptr;

    QPushButton *records;

    std::vector<bool> isGiven = {0, 0, 0, 0, 0, 0};
    std::vector<int> moneys = {100, 500, 1000, 1200, 1400, 5000};

    QPushButton *shop; // кнопка магазина
    std::vector<QString> farm = {"MemCoins", "Ton", "Bitcoin", "YandexCoin"}; // названия всяких коинов и тп
    std::vector<bool> sales; // покупки в магазине
    QWidget *shopWindow = nullptr; // самок окно

};

#endif // MAINWINDOW_H
