#ifndef QUESTION_HPP
#define QUESTION_HPP

#include <wasm_export.h>
#include <QObject>
#include <QStringList>
#include "checkbox.hpp"
#include "variant.hpp"

class Question : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString text MEMBER text NOTIFY textChanged)
    Q_PROPERTY(QString image MEMBER image NOTIFY imageChanged)
    Q_PROPERTY(QObjectList variants MEMBER variants NOTIFY variantsChanged)
    Q_PROPERTY(QObjectList checkboxes MEMBER checkboxes NOTIFY checkboxesChanged)
    Q_PROPERTY(QList<int> scores MEMBER scores NOTIFY scoresChanged)
    Q_PROPERTY(QString correct MEMBER correct NOTIFY correctChanged)
    Q_PROPERTY(int score MEMBER score NOTIFY scoreChanged)
    Q_PROPERTY(bool checked MEMBER checked NOTIFY checkedChanged)
    Q_PROPERTY(bool textOnly MEMBER textOnly NOTIFY textOnlyChanged)
    Q_PROPERTY(QString checkerPath MEMBER checkerPath NOTIFY checkerPathChanged)
    Q_PROPERTY(QString checkerUserdata MEMBER checkerUserdata NOTIFY checkerUserdataChanged)
    Q_PROPERTY(int timeLimit MEMBER timeLimit NOTIFY timeLimitChanged)

public:
    Question(QObject* parent = nullptr) : QObject(parent) {}
    void addVariant(Variant* variant);
    void addCheckbox(Checkbox* checkbox);
    void addScore(int score);
    void setCorrect(const QString& correct);
    void setCheckerPath(const QString& path);

    Q_INVOKABLE int answerScore(const QString& answer);
    Q_INVOKABLE bool answerValid(const QString& answer);

signals:
    void textChanged();
    void imageChanged();
    void variantsChanged();
    void checkboxesChanged();
    void scoresChanged();
    void correctChanged();
    void scoreChanged();
    void checkedChanged();
    void textOnlyChanged();
    void checkerPathChanged();
    void checkerUserdataChanged();
    void timeLimitChanged();

private:
    void loadChecker();

    QString text;
    QString image;
    QObjectList variants;
    QObjectList checkboxes;
    QList<int> scores;

    QString correct;
    int score = 0;
    int timeLimit = 0;

    bool checked = false;
    bool textOnly = true;

    QString checkerPath;
    QString checkerUserdata;
    bool checkerLoaded = false;

    wasm_module_t module;
    wasm_module_inst_t moduleInstance;
    wasm_function_inst_t scoreFunction;
    wasm_function_inst_t validFunction;
    wasm_exec_env_t execEnv;
};

#endif // QUESTION_HPP
