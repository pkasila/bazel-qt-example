#ifndef STYLES_H
#define STYLES_H

#include <QString>

namespace Styles {

const QString MAIN_WINDOW = R"(
    QMainWindow {
        background-color: #050508;
        color: #f0f0f0;
    }
    QMenuBar {
        background-color: #0a0a0f;
        color: #f0f0f0;
        border-bottom: 2px solid #1a1a2e;
        padding: 5px;
        font-weight: bold;
    }
    QMenuBar::item:selected {
        background-color: #1a1a2e;
        color: #00f2ff;
        border-radius: 4px;
    }
    QMenu {
        background-color: #0a0a0f;
        color: #f0f0f0;
        border: 2px solid #1a1a2e;
        border-radius: 8px;
        padding: 5px;
    }
    QMenu::item:selected {
        background-color: #1a1a2e;
        color: #00f2ff;
    }
)";

const QString SIDEBAR = R"(
    QWidget#sidebar {
        background-color: #08080c;
        border-right: 3px solid #bc13fe;
        min-width: 260px;
    }
    QLabel#sidebar_title {
        font-size: 28px;
        font-weight: 800;
        color: #00f2ff;
        padding: 40px 20px;
        text-transform: uppercase;
        letter-spacing: 3px;
    }
    QLabel#stat_label {
        font-size: 16px;
        color: #a0a0b0;
        font-weight: 800;
        padding: 8px 20px;
        text-transform: uppercase;
    }
)";

const QString SIDEBAR_BUTTON = R"(
    QPushButton {
        text-align: left;
        padding: 16px 28px;
        border: none;
        border-radius: 12px;
        font-size: 16px;
        font-weight: bold;
        color: #707080;
        background-color: transparent;
        margin: 6px 18px;
    }
    QPushButton:hover {
        background-color: rgba(188, 19, 254, 0.15);
        color: #ffffff;
    }
    QPushButton[active="true"] {
        color: #00f2ff;
        background-color: rgba(0, 242, 255, 0.12);
        border: 1px solid rgba(0, 242, 255, 0.5);
        padding-left: 27px;
    }
)";

const QString PROGRESS_BAR = R"(
    QProgressBar {
        border: 2px solid #1a1a2e;
        border-radius: 12px;
        background-color: #0d0d16;
        height: 14px;
        text-align: center;
        margin: 15px 30px;
    }
    QProgressBar::chunk {
        background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #00ff88, stop:0.5 #00f2ff, stop:1 #bc13fe);
        border-radius: 10px;
    }
)";

const QString OPTION_BUTTON = R"(
    QPushButton {
        border: 2px solid #1a1a2e;
        border-radius: 16px;
        background-color: #0f0f1a;
        color: #d0d0e0;
        font-size: 17px;
        font-weight: 600;
        padding: 18px;
        min-width: 240px;
        margin: 5px;
    }
    QPushButton:hover {
        background-color: #16162a;
        border: 2px solid #3a3a5e;
        color: #ffffff;
    }
    QPushButton:pressed {
        background-color: #0a0a14;
        margin-top: 2px;
    }
    QPushButton[selected="true"] {
        background-color: rgba(188, 19, 254, 0.1);
        border: 2px solid #bc13fe;
        color: #ce42ff;
    }
)";

const QString RADIO_BUTTON = R"(
    QRadioButton {
        font-size: 18px;
        color: #d0d0e0;
        spacing: 15px;
        padding: 18px;
        border-radius: 16px;
        background-color: #0f0f1a;
        border: 2px solid #1a1a2e;
        margin: 8px 0px;
    }
    QRadioButton:hover {
        border: 2px solid #bc13fe;
        background-color: #16162a;
    }
    QRadioButton::indicator {
        width: 24px;
        height: 24px;
        border-radius: 12px;
        border: 2px solid #333344;
        background-color: #050508;
    }
    QRadioButton::indicator:checked {
        border: 2px solid #bc13fe;
        background-color: #bc13fe;
    }
)";

const QString TEXT_EDIT = R"(
    QTextEdit {
        background-color: #0f0f1a;
        border: 2px solid #1a1a2e;
        border-radius: 16px;
        font-size: 19px;
        padding: 20px;
        color: #ffffff;
        selection-background-color: #bc13fe;
    }
    QTextEdit:focus {
        border: 2px solid #00f2ff;
        background-color: #131326;
    }
)";

const QString CHECK_BUTTON = R"(
    QPushButton {
        background-color: #00f2ff;
        border: none;
        border-radius: 14px;
        color: #050508;
        font-size: 19px;
        font-weight: 800;
        padding: 18px 60px;
        text-transform: uppercase;
        letter-spacing: 2px;
    }
    QPushButton:hover {
        background-color: #5effff;
        border: 2px solid #ffffff;
    }
    QPushButton:pressed {
        background-color: #00c2cc;
        margin-top: 3px;
    }
    QPushButton:disabled {
        background-color: #1a1a2e;
        color: #404050;
        border: 2px solid #0f0f1a;
    }
)";

const QString QUESTION_TEXT =
    "font-size: 32px; font-weight: 800; color: #ffffff; margin: 40px 10px; letter-spacing: 1px;";
const QString SUB_TEXT =
    "font-size: 20px; color: #707080; margin-bottom: 20px; text-transform: uppercase; font-weight: "
    "bold;";
const QString TIMER_TEXT =
    "font-size: 24px; font-weight: 900; color: #ff0077; padding: 20px; font-family: 'Courier New';";

}  // namespace Styles

#endif  // STYLES_H
