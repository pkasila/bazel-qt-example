#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget widget;
    widget.setWindowTitle("Первый проект");
    widget.setMinimumHeight(180);
    widget.setMinimumWidth(300);

    QLabel label{&widget};
    label.setText("Hello, world!");

    QPushButton button("Hell nahhhh, world!", &widget);
    button.resize(200, 100);

    widget.show();
    return app.exec();
}