#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QDateEdit>
#include <QCoreApplication>
#include <QResizeEvent>
#include <QCalendarWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

   public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

   private slots:
    void handleSingleClick(QListWidgetItem *item);
    void handleDoubleClick(QListWidgetItem *item); // Обработка двойного нажатия
    void openTicketWindow(QListWidgetItem *item); // Открытие окна покупки билетов
    void openMerchWindow(QListWidgetItem *item); // Открытие окна заказа мерча
    void switchView(); // Переключение между пилотами и трассами
    void showDriverImage(QListWidgetItem *item); // Показ изображения гонщика
    void updateTrackInfo(int index); // Обновление информации о трассе

   private:
    void updateLeftView(const QString &itemName); // Обновление левой части (карта/мерч)
    void setupPilots(); // Заполнение списка пилотов
    void setupTracks(); // Заполнение списка трасс
    void setupSchedule(); // Заполнение расписания

    QLabel *backgroundLabel; // Для фонового изображения
    QPixmap backgroundPixmap; // Для хранения фонового изображения
    QListWidget *listWidget; // Список пилотов/трасс
    QComboBox *comboBox;     // Список трасс
    QPushButton *switchButton; // Кнопка переключения
    QPushButton *merchButton; // Кнопка заказа мерча
    QLabel *trackInfoLabel;  // Информация о трассе
    QLabel *imageLabel;      // Изображение трассы/гонщика
    QLabel *scheduleLabel;  // Расписание заездов
    QCalendarWidget *calendarWidget;

    bool isPilotsView;       // Флаг для переключения между пилотами и трассами
   protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // MAINWINDOW_H
