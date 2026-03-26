#ifndef REVISER_HPP
#define REVISER_HPP

#include <QObject>
#include <random>

class Reviser : public QObject {
    Q_OBJECT
    Q_PROPERTY(QObjectList list MEMBER list NOTIFY listChanged)
    Q_PROPERTY(int index MEMBER index NOTIFY indexChanged)

    Q_PROPERTY(QString curName READ getCurName WRITE setCurName NOTIFY curNameChanged)
    Q_PROPERTY(int curState READ getCurState WRITE setCurState NOTIFY curStateChanged)
    Q_PROPERTY(double progress READ getProgress NOTIFY progressChanged)

    Q_PROPERTY(bool canPrevious READ canPrevious NOTIFY canPreviousChanged)
    Q_PROPERTY(bool canNext READ canNext NOTIFY canNextChanged)
    Q_PROPERTY(bool canRemove READ canRemove NOTIFY canRemoveChanged)

public:
    Reviser();
    Q_INVOKABLE void previous();
    Q_INVOKABLE void next();
    Q_INVOKABLE void addTicket();
    Q_INVOKABLE void removeTicket();

    QString getCurName();
    void setCurName(QString name);
    int getCurState();
    void setCurState(int state);
    double getProgress();

    bool canPrevious();
    bool canNext();
    bool canRemove();

signals:
    void listChanged();
    void indexChanged();

    void curNameChanged();
    void curStateChanged();
    void progressChanged();

    void canPreviousChanged();
    void canNextChanged();
    void canRemoveChanged();

private slots:
    void refresh();

private:
    std::mt19937_64 gen;
    QObjectList list;
    std::vector<int> history;
    int index = 0;
    int newTicketNumber = 0;
};

#endif // REVISER_HPP
