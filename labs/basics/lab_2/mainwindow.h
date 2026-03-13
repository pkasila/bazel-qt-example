#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QListView>

class QNetworkAccessManager;
class QLabel;
class QPushButton;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onGuitarSelected(const QModelIndex &index);
    void showOrderForm();
    void imageDownloaded(QNetworkReply* reply);

private:
    void setupUI();
    void loadImage(const QString &url);

    struct GuitarInfo {
        QString title;
        QString price;
        QString description;
        QString imageUrl;
    };

    QListView *guitarListView;
    QStandardItemModel *guitarModel;
    QNetworkAccessManager *networkManager;
    QLabel *guitarImageLabel;
    QLabel *guitarTitle;
    QLabel *guitarPrice;
    QLabel *guitarDescription;
    QPushButton *orderButton;
};

#endif // MAINWINDOW_H
