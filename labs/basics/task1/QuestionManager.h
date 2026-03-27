#pragma once

#include <random>
#include <deque>
#include <chrono>
#include <QDebug>

inline std::mt19937 rnd(time(nullptr));
const int mx_sz = 1000;

inline const int random(int to, int from) {
    return to + rnd() % (from - to + 1);
}

class QuestionManager {
public:
    QuestionManager(const int n) : sz(n), curSize(n) {
        for (int i = 0; i < sz; i++) {
            col.push_back(0);
            questions.push_back(i);
            pos.push_back(i);
        }
    }
    void resize(const int n) {
        sz = n;
        curSize = n;
        col.clear();
        history.clear();
        questions.clear();
        pos.clear();
        for (int i = 0; i < sz; i++) {
            col.push_back(0);
            questions.push_back(i);
            pos.push_back(i);
        }
    }

    int nextQuestion() {
        if (curSize == 0) {
            return -1;
        }
        int ind = random(0, curSize - 1);
        if (history.size() == mx_sz) {
            history.pop_front();
        }
        history.push_back(questions[ind]);
        return history.back();
    }

    int prevQuestion() {
        if (history.size() <= 1) {
            return -1;
        }
        history.pop_back();
        return history.back();
    }

    void addQuestion(const int ind) {
        if (history.size() == mx_sz) {
            history.pop_front();
        }
        history.push_back(ind);
    }

    void changeType(const int ind, const int c) {
        if (col[ind] == 2) {
            if(c != 2) {
                questions.push_back(ind);
                pos[ind] = curSize;
                curSize++;
            }
        } else {
            if(c == 2) {
                pos[questions.back()] = pos[ind];
                std::swap(questions.back(), questions[pos[ind]]);
                questions.pop_back();
                curSize--;
            }
        }
        col[ind] = c;
    }

private:
    int sz;
    int curSize;
    std::vector<int> col;
    std::vector<int> pos;
    std::vector<int> questions;
    std::deque<int> history;
};
