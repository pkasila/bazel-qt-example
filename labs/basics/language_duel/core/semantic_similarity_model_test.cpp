#include "core/text_matcher.h"

#include <iostream>

namespace {
bool expectAccepted(const QString& input, const QString& expected) {
    const MatchResult result = TextMatcher::compare(input, expected);
    if (!result.accepted) {
        std::cerr << "Expected accepted, got rejected: "
                  << result.feedback.toStdString() << '\n';
        return false;
    }
    return true;
}

bool expectRejected(const QString& input, const QString& expected) {
    const MatchResult result = TextMatcher::compare(input, expected);
    if (result.accepted) {
        std::cerr << "Expected rejected, got accepted: "
                  << result.feedback.toStdString() << '\n';
        return false;
    }
    return true;
}
}  // namespace

int main() {
    const QString expected = QString::fromUtf8("Ей нравится зеленый чай");

    bool ok = true;
    ok = expectAccepted(QString::fromUtf8("Oна любит зеленый чай"), expected) && ok;
    ok = expectAccepted(QString::fromUtf8("Она любит зеленый чай"), expected) && ok;
    ok = expectAccepted(QString::fromUtf8("ona lyubit zelyony chay"), expected) && ok;
    ok = expectRejected(QString::fromUtf8("Он любит зеленый чай"), expected) && ok;
    ok = expectAccepted(QString::fromUtf8("Каждый день я читаю книжки"), QString::fromUtf8("Я читаю книги каждый день")) && ok;
    ok = expectAccepted(QString::fromUtf8("Кошка находится под столом"), QString::fromUtf8("Кот под столом")) && ok;
    ok = expectAccepted(QString::fromUtf8("Мы изучаем английский язык"), QString::fromUtf8("Мы учим английский")) && ok;
    ok = expectRejected(QString::fromUtf8("Мы не изучаем английский язык"), QString::fromUtf8("Мы учим английский")) && ok;
    ok = expectAccepted(QString::fromUtf8("Мой приятель проживает в Минске"), QString::fromUtf8("Мой друг живет в Минске")) && ok;
    ok = expectAccepted(QString::fromUtf8("Они прибыли очень поздно"), QString::fromUtf8("Они приехали очень поздно")) && ok;
    ok = expectAccepted(QString::fromUtf8("Он вчера забыл выслать email"), QString::fromUtf8("Он забыл отправить письмо вчера")) && ok;
    ok = expectAccepted(QString::fromUtf8("Открой окно пожалуйста"), QString::fromUtf8("Не мог бы ты открыть окно, пожалуйста")) && ok;
    ok = expectAccepted(QString::fromUtf8("Занятие было труднее, чем я думал"), QString::fromUtf8("Урок был сложнее, чем я ожидал")) && ok;
    ok = expectAccepted(QString::fromUtf8("Этот метод устойчив к небольшим опечаткам"), QString::fromUtf8("Этот подход достаточно надежен, чтобы обрабатывать небольшие опечатки")) && ok;

    return ok ? 0 : 1;
}
