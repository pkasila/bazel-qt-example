#include "keyboardwidget.h"

#include <QFontMetrics>
#include <QPainter>

KeyboardWidget::KeyboardWidget(QWidget* parent) : QWidget(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFixedSize(700, 300);
    setupLayout();
}

void KeyboardWidget::setLayout(Layout layout) {
    currentLayout = layout;
    setupLayout();
    update();
}

void KeyboardWidget::highlightKey(Qt::Key key, bool active) {
    activeKey = key;
    keyActive = active;
    update();
}

void KeyboardWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    switch (currentLayout) {
        case Layout::English:
            paintEnglish(painter);
            break;
        case Layout::Russian:
            paintRussian(painter);
            break;
    }
}

void KeyboardWidget::setupLayout() {
    keyPositions.clear();

    QStringList rows;
    QStringList englishRows;

    if (currentLayout == Layout::English) {
        rows = {"1234567890", "QWERTYUIOP[]", "ASDFGHJKL;'", "ZXCVBNM,./", " SPACE "};
        englishRows = rows;
    } else if (currentLayout == Layout::Russian) {
        rows = {"1234567890", "ЙЦУКЕНГШЩЗХЪ", "ФЫВАПРОЛДЖЭ", "ЯЧСМИТЬБЮ.", " SPACE "};
        englishRows = {"1234567890", "QWERTYUIOP[]", "ASDFGHJKL;'", "ZXCVBNM,./", " SPACE "};
    }

    const int keyWidth = 50;
    const int keyHeight = 50;
    const int horizontalPadding = 20;
    const int verticalPadding = 10;
    for (int rowIdx = 0; rowIdx < rows.size(); ++rowIdx) {
        const QString& chars = rows[rowIdx];
        const QString& enChars = englishRows[rowIdx];
        int y = verticalPadding + (rowIdx * (keyHeight + 5));
        int xOffset = 0;

        switch (rowIdx) {
            case 0:
                xOffset = keyWidth;
                break;
            case 1:
                xOffset = 0;
                break;
            case 2:
                xOffset = keyWidth / 2;
                break;
            case 3:
                xOffset = keyWidth;
                break;
            case 4:
                xOffset = keyWidth * 4;
                break;
        }

        if (rowIdx == 4) {
            Qt::Key key = Qt::Key_Space;
            KeyInfo info;
            info.rect = QRect(horizontalPadding + xOffset, y, keyWidth * 5, keyHeight);
            info.mainChar = ' ';
            keyPositions[key] = info;
            break;
        }

        for (int i = 0; i < chars.size(); ++i) {
            QChar ruChar = chars[i];
            QChar enChar = enChars[i];
            Qt::Key key = static_cast<Qt::Key>(enChar.toUpper().unicode());

            KeyInfo info;
            info.rect =
                QRect(horizontalPadding + xOffset + (i * (keyWidth + 5)), y, keyWidth, keyHeight);
            info.mainChar = ruChar;
            keyPositions[key] = info;
        }
    }
}

void KeyboardWidget::paintEnglish(QPainter& painter) {
    painter.fillRect(rect(), Qt::transparent);

    QFont font = painter.font();
    font.setPointSize(10);
    painter.setFont(font);

    for (auto it = keyPositions.begin(); it != keyPositions.end(); ++it) {
        const KeyInfo& key = it.value();
        QColor color = (it.key() == activeKey) ? QColor(173, 216, 230) : QColor(200, 200, 200);

        painter.setBrush(color);
        painter.drawRoundedRect(key.rect, 5, 5);

        QRect textRect = key.rect.adjusted(2, 2, -2, -2);
        painter.drawText(textRect, Qt::AlignCenter, key.mainChar);
    }
}

void KeyboardWidget::paintRussian(QPainter& painter) {
    painter.fillRect(rect(), Qt::transparent);

    QFont font = painter.font();
    font.setPointSize(10);
    painter.setFont(font);

    for (auto it = keyPositions.begin(); it != keyPositions.end(); ++it) {
        const KeyInfo& key = it.value();
        QColor color = (it.key() == activeKey) ? QColor(173, 216, 230) : QColor(200, 200, 200);

        painter.setBrush(color);
        painter.drawRoundedRect(key.rect, 5, 5);

        QRect textRect = key.rect.adjusted(2, 2, -2, -2);
        painter.drawText(textRect, Qt::AlignCenter, key.mainChar);
    }
}