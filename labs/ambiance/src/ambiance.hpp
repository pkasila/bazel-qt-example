#ifndef AMBIANCE_HPP
#define AMBIANCE_HPP

#include <QQuickItem>
#include <QUrl>

namespace amb {
    class Ambiance;
}

class amb::Ambiance : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(QObjectList sounds MEMBER sounds NOTIFY soundsChanged)
    Q_PROPERTY(int theme MEMBER theme NOTIFY themeChanged)
    Q_PROPERTY(int accent MEMBER accent NOTIFY accentChanged)
    Q_PROPERTY(bool windowBorder MEMBER windowBorder NOTIFY windowBorderChanged)
    Q_PROPERTY(bool controls MEMBER controls NOTIFY controlsChanged)

public:
    Ambiance(QQuickItem* parent = nullptr);
    ~Ambiance();

    Q_INVOKABLE void add(QString name, QUrl path, double volume = 1);
    Q_INVOKABLE void addCustom(QUrl url);
    Q_INVOKABLE void remove(int index);

signals:
    void soundsChanged();
    void themeChanged();
    void accentChanged();
    void windowBorderChanged();
    void controlsChanged();

private slots:
    void updateWindowBorder();

private:
    void loadConfig();
    void saveConfig();

    QObjectList sounds;
    int theme = 0;
    int accent = 0;
    bool windowBorder = false;
    bool controls = true;
};

#endif // AMBIANCE_HPP
