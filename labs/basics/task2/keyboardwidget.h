#ifndef KEYBOARDWIDGET_H
#define KEYBOARDWIDGET_H

#include <QMap>
#include <QWidget>
#include <Qt>

class KeyboardWidget : public QWidget {
    Q_OBJECT

   public:
    enum class Layout : int8_t { English, Russian };

    explicit KeyboardWidget(QWidget* parent = nullptr);
    void setLayout(Layout layout);
    void highlightKey(Qt::Key key, bool active);
    Layout currentLayout = Layout::English;

   protected:
    void paintEvent(QPaintEvent* event) override;

   private:
    struct KeyInfo {
        QRect rect;
        QString mainChar;
        QString shiftChar;
    };

    void setupLayout();
    void paintEnglish(QPainter& painter);
    void paintRussian(QPainter& painter);

    QMap<Qt::Key, KeyInfo> keyPositions;
    Qt::Key activeKey = Qt::Key_unknown;
    bool keyActive = false;
};

#endif  // KEYBOARDWIDGET_H