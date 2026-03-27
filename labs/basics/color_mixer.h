#ifndef COLOR_MIXER_H
#define COLOR_MIXER_H

#include <QtWidgets>

class ColorMixer : public QMainWindow {
    Q_OBJECT

   public:
    ColorMixer(QWidget* parent = nullptr);

   private slots:
    void onRedSliderChanged(int value);
    void onGreenSliderChanged(int value);
    void onBlueSliderChanged(int value);
    void onAlphaSliderChanged(int value);
    void onCopyColor();
    void onPasteColor();
    void onRandomColor();
    void onColorNameChanged(const QString& text);
    void updateColorDisplay();

   private:
    void createWidgets();
    void createLayouts();
    void createConnections();

    QSlider* redSlider;
    QSlider* greenSlider;
    QSlider* blueSlider;
    QSlider* alphaSlider;
    QLabel* colorLabel;
    QLineEdit* colorNameEdit;
    QPushButton* copyButton;
    QPushButton* pasteButton;
    QPushButton* randomButton;

    // Other widgets (2 required - QLCDNumber, QProgressBar)
    QLCDNumber* redLcd;
    QLCDNumber* greenLcd;
    QLCDNumber* blueLcd;
    QLCDNumber* alphaLcd;
    QProgressBar* opacityBar;

    QWidget* centralWidget;
    QVBoxLayout* mainLayout;
    QGridLayout* sliderLayout;
    QHBoxLayout* buttonLayout;

    QColor currentColor;
};

#endif
