#include "appstyle.h"

QString AppStyle::styleSheet() {
    return R"(
        QMainWindow {
            background: #ecf4ff;
        }

        QWidget#AppShell {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #eef9ff, stop:0.48 #f7fbff, stop:1 #eef5ff);
            color: #172033;
            font-family: "Segoe UI", Arial, Helvetica, sans-serif;
            font-size: 15px;
        }

        QMenuBar {
            background: #ffffff;
            border-bottom: 1px solid #e1e9f5;
            padding: 4px 10px;
            color: #22304a;
        }

        QMenuBar::item {
            background: transparent;
            border-radius: 8px;
            padding: 7px 12px;
        }

        QMenuBar::item:selected {
            background: #eef5ff;
        }

        QMenu {
            background: #ffffff;
            color: #22304a;
            border: 1px solid #dbe6f4;
            padding: 8px;
        }

        QMenu::item {
            padding: 8px 28px;
            border-radius: 8px;
        }

        QMenu::item:selected {
            background: #eef5ff;
        }

        QFrame#SideBar {
            background: rgba(255, 255, 255, 238);
            border: 1px solid #dbe7f5;
            border-radius: 26px;
        }

        QLabel#LogoMark {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #25c070, stop:1 #10a8ff);
            color: #ffffff;
            border-radius: 24px;
            font-size: 25px;
            font-weight: 900;
        }

        QLabel#AppTitle {
            font-size: 27px;
            font-weight: 900;
            color: #172033;
        }

        QLabel#MutedText {
            color: #68758c;
            line-height: 135%;
        }

        QLabel#SectionCaption {
            color: #7a879d;
            font-size: 12px;
            font-weight: 800;
            text-transform: uppercase;
        }

        QFrame#SideCard, QFrame#HintCard, QFrame#FeatureCard, QFrame#TopCard, QFrame#Card {
            background: #ffffff;
            border: 1px solid #dce7f4;
            border-radius: 22px;
        }

        QFrame#HintCard {
            background: #f4f8ff;
        }

        QFrame#HeroCard {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #ffffff, stop:0.55 #ffffff, stop:1 #eaf7ff);
            border: 1px solid #dce7f4;
            border-radius: 30px;
        }

        QLabel#ScoreLabel {
            color: #14a65b;
            font-size: 34px;
            font-weight: 900;
        }

        QLabel#HintTitle, QLabel#FeatureTitle {
            color: #172033;
            font-size: 16px;
            font-weight: 800;
        }

        QLabel#PillLabel {
            background: #e9f9f0;
            color: #16a15b;
            border: 1px solid #cbeed8;
            border-radius: 14px;
            padding: 8px 12px;
            font-weight: 800;
        }

        QLabel#HeroTitle {
            color: #172033;
            font-size: 36px;
            font-weight: 900;
        }

        QLabel#HeroBody {
            color: #56647a;
            font-size: 17px;
            line-height: 145%;
        }

        QLabel#ExerciseTitle {
            color: #172033;
            font-size: 29px;
            font-weight: 900;
        }

        QLabel#TimerBadge, QLabel#AttemptsBadge {
            border-radius: 15px;
            padding: 10px 14px;
            font-weight: 900;
        }

        QLabel#TimerBadge {
            background: #eaf4ff;
            color: #0f74c8;
            border: 1px solid #cfe4ff;
        }

        QLabel#AttemptsBadge {
            background: #fff3e7;
            color: #c26100;
            border: 1px solid #ffddb6;
        }

        QLabel#QuestionLabel {
            color: #172033;
            font-size: 28px;
            font-weight: 900;
        }

        QLabel#FeedbackSuccess, QLabel#FeedbackError {
            border-radius: 16px;
            padding: 12px 16px;
            font-weight: 800;
        }

        QLabel#FeedbackSuccess {
            background: #e9f9ef;
            color: #139151;
            border: 1px solid #c8ecd6;
        }

        QLabel#FeedbackError {
            background: #fff0f0;
            color: #cf3b3b;
            border: 1px solid #ffd0d0;
        }

        QPushButton {
            border: none;
            border-radius: 16px;
            padding: 14px 20px;
            font-weight: 900;
            font-size: 15px;
        }

        QPushButton#PrimaryButton {
            background: #27c46b;
            color: #ffffff;
        }

        QPushButton#PrimaryButton:hover {
            background: #20b25f;
        }

        QPushButton#PrimaryButton:pressed {
            background: #189d52;
        }

        QPushButton#BlueButton {
            background: #178bff;
            color: #ffffff;
        }

        QPushButton#BlueButton:hover {
            background: #0f7de7;
        }

        QPushButton#BlueButton:pressed {
            background: #0a67c2;
        }

        QPushButton#SecondaryButton {
            background: #eef3fa;
            color: #22304a;
            border: 1px solid #d9e4f2;
        }

        QPushButton#SecondaryButton:hover {
            background: #e3edf8;
        }

        QPushButton#SecondaryButton:pressed {
            background: #d5e3f3;
        }

        QLineEdit {
            background: #ffffff;
            border: 2px solid #d8e4f2;
            border-radius: 18px;
            padding: 15px 18px;
            font-size: 20px;
            selection-background-color: #27c46b;
        }

        QLineEdit:focus {
            border-color: #27c46b;
            background: #fbfffd;
        }

        QRadioButton {
            background: #f7faff;
            border: 1px solid #dce7f4;
            border-radius: 16px;
            padding: 14px 16px;
            spacing: 14px;
            font-size: 18px;
            color: #22304a;
        }

        QRadioButton:hover {
            background: #eef6ff;
            border-color: #bed8f5;
        }

        QRadioButton::indicator {
            width: 18px;
            height: 18px;
        }

        QProgressBar {
            border: 1px solid #dce7f4;
            border-radius: 12px;
            background: #edf3fb;
            min-height: 22px;
            text-align: center;
            color: #22304a;
            font-weight: 800;
        }

        QProgressBar::chunk {
            border-radius: 12px;
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #27c46b, stop:1 #10a8ff);
        }

        QDialog {
            background: #f7fbff;
            color: #172033;
            font-family: "Segoe UI", Arial, Helvetica, sans-serif;
            font-size: 15px;
        }

        QDialog QLabel {
            color: #172033;
            font-weight: 800;
        }
    )";
}
