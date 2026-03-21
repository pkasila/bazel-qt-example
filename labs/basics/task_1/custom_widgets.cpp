#include <QtWidgets>

#include "custom_widgets.h"

void CustomLineEdit::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Return && this->hasFocus() && !this->text().isEmpty()) {
        int index = this->parent()->objectName().slice(10).toInt();
        emit hitEnter(index, this->text());
    }

    QLineEdit::keyPressEvent(event);
}

StartupMenu::StartupMenu() {
    setMinimumSize(500, 400);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowTitle("Procrastination");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(20);
    layout->setContentsMargins(20,20,20,20);


    QLabel* imageLabel = new QLabel(this);
    QPixmap pixmap(":/images/catgirl.png");
    pixmap = pixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imageLabel->setPixmap(pixmap);
    imageLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(imageLabel);

    QLabel* welcome = new QLabel(tr("Давайте поработаем"), this);
    welcome->setAlignment(Qt::AlignCenter);
    welcome->setFont(QFont("Segoe UI", 14, QFont::DemiBold));
    layout->addWidget(welcome);

    QPushButton* startButton = new QPushButton(tr("Начать"), this);
    startButton->setMinimumHeight(40);
    layout->addWidget(startButton, 0, Qt::AlignCenter);

    connect(startButton, &QPushButton::clicked, this, [this]() {
        emit startClicked();
    });
}