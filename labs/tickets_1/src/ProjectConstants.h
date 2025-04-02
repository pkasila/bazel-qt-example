#ifndef PROJECTCONSTANTS_H
#define PROJECTCONSTANTS_H
#include <QString>
#include <QMap>
#include <QSize>

namespace WindowTitles {
    inline const QString mainWindowTitle = "Повторение билетов";
    inline const QString addTicketsDialogWindowTitle = "Добавление билетов";
    inline const QString warningWindowTitle = "Предупреждение";
    inline const QString validateInputWarningTitle = "Ошибка ввода";
}

namespace WindowTexts {
    inline const QString addTicketsDialogQuestion = "Сколько билетов добавить?";
    inline const QString lableWarningText = "Имя не может быть пустым";
    inline const QString statusComboBoxText = "";
    inline const QString ticketsInputText = "Количество билетов:";
    inline const QString groupBoxHeader = "Информация о билете";
    inline const QString defaultEditableLabelText = "Билет №";
    inline const QString textEditUnableText = "Выберите билет для просмотра...";
    inline const QString textEditPlaceholderText = "Добавьте заметки...";
    inline const QString validateInputWarningMessage = "Введите положительное целое число";
}

namespace Formats {
    inline const QString totalProgressBarFormat = "Общий прогресс: %p%";
    inline const QString greenProgressBarFormat = "Билетов пройдено: %v/%m";
    inline const QString ticketNumberFormat = "№";
    inline const QString defaultTicketNameFormat = "Билет №%1";
}

namespace ButtonTexts {
    inline const QString ok = "Ок";
    inline const QString cancel = "Отмена";
    inline const QString submit = "Ввод";
    inline const QString addTicketsButtonText = "Добавить билеты...";
    inline const QString prevTicketButtonText = "Предыдущий билет";
    inline const QString randomTicketButtonText = "Случайный билет";
}

namespace Geometry {
    inline const QSize addTicketsDialogWindowSize = QSize(200, 100);
    inline const QSize ticketAppMainWindowSize = QSize(325, 300);
    inline const QSize minTicketAppMainWindowSize = QSize(300, 250);
    inline const QSize ticketAppMainWindowWithTicketViewActivatedSize = QSize(600, 300);
    inline const QSize minTicketAppMainWindowWithTicketViewActivatedSize = QSize(550, 250);
    inline const QSize statusFlagSize = QSize(20, 20);
    constexpr int ticketViewMinWidth = 250;
    constexpr int ticketStatusBoxWidth = 50;
}

namespace Parameters {
    constexpr int MaxNumberOfSymbolsInEditableLabel = 20;
}

namespace Status {
    constexpr int Red = 0;
    constexpr int Yellow = 1;
    constexpr int Green = 2;
}

namespace ColorMaps {
    inline const QMap<int, QString> statusStyleSheets = {
        {0, "background-color: red; "},
        {1, "background-color: yellow; "},
        {2, "background-color: green; "}
    };

    inline const QMap<int, QString> statusColors = {
        {0, "red"},
        {1, "yellow"},
        {2, "green"}
    };
}

namespace Styles {
    inline const QString editableLabelStyle = "background-color: white; ";
    inline const QString statusComboBoxStyle = "QComboBox { background-color: %1; border: 1px solid gray; }";
    inline const QString ticketTableStyle = "QTableView::item:hover { background: none; }";
    inline const QString totalProgressBarStyle = R"(
    QProgressBar {
        border: 2px solid black;
        border-radius: 8px;
        background-color: #ecf0f1;
        text-align: center;
        font-weight: bold;
        color: #2c3e50;
        height: 20px;
    }
    QProgressBar::chunk {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #1abc9c, stop:1 #16a085);
        border-radius: 6px;
    }
    )";
    inline const QString greenProgressBarStyle = R"(
    QProgressBar {
        border: 2px solid black;
        border-radius: 8px;
        background-color: #ecf0f1;
        text-align: center;
        font-weight: bold;
        color: #2c3e50;
        height: 20px;
    }
    QProgressBar::chunk {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #1abc9c, stop:1 #16a085);
        border-radius: 6px;
    }
    )";
}

#endif // PROJECTCONSTANTS_H
