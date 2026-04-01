#include "question.hpp"
#include "error.hpp"
#include "bh_read_file.h"

void Question::addVariant(Variant* variant) {
    variants.push_back(variant);
    emit variantsChanged();
    if(textOnly) {
        textOnly = false;
        emit textOnlyChanged();
    }
}

void Question::addCheckbox(Checkbox* checkbox) {
    checkboxes.push_back(checkbox);
    emit checkboxesChanged();
    if(textOnly) {
        textOnly = false;
        emit textOnlyChanged();
    }
}

void Question::addScore(int score) {
    scores.push_back(score);
    emit scoresChanged();
}

void Question::setCorrect(const QString& correct) {
    this->correct = correct;
    emit correctChanged();
    if(textOnly) {
        textOnly = false;
        emit textOnlyChanged();
    }
}

void Question::setCheckerPath(const QString& path) {
    checkerPath = path;
    emit checkerPathChanged();
    if(textOnly) {
        textOnly = false;
        emit textOnlyChanged();
    }
}

int Question::answerScore(const QString& answer) {
    loadChecker();
    std::string answerStr = answer.toStdString();
    std::string userdataStr = checkerUserdata.toStdString();

    void* answerPtr;
    void* userdataPtr;
    uint32_t answerOffset = wasm_runtime_module_malloc(moduleInstance, answerStr.size() + 1, &answerPtr);
    uint32_t userdataOffset = wasm_runtime_module_malloc(moduleInstance, userdataStr.size() + 1, &userdataPtr);

    memcpy(answerPtr, answerStr.c_str(), answerStr.size() + 1);
    memcpy(userdataPtr, userdataStr.c_str(), userdataStr.size() + 1);

    uint32_t argv[2];
    argv[0] = answerOffset;
    argv[1] = userdataOffset;
    wasm_runtime_call_wasm(execEnv, scoreFunction, 4, argv);

    int res;
    memcpy(&res, &argv[0], sizeof(res));
    return res;
}

bool Question::answerValid(const QString& answer) {
    loadChecker();
    std::string answerStr = answer.toStdString();
    std::string userdataStr = checkerUserdata.toStdString().c_str();

    void* answerPtr;
    void* userdataPtr;
    uint64_t answerOffset = wasm_runtime_module_malloc(moduleInstance, answerStr.size() + 1, &answerPtr);
    uint64_t userdataOffset = wasm_runtime_module_malloc(moduleInstance, userdataStr.size() + 1, &userdataPtr);

    memcpy(answerPtr, answerStr.c_str(), answerStr.size() + 1);
    memcpy(userdataPtr, userdataStr.c_str(), userdataStr.size() + 1);

    uint32_t argv[4];
    memcpy(&argv[0], &answerOffset, sizeof(answerOffset));
    memcpy(&argv[2], &userdataOffset, sizeof(userdataOffset));
    wasm_runtime_call_wasm(execEnv, validFunction, 4, argv);

    bool res;
    memcpy(&res, &argv[0], sizeof(res));

    wasm_runtime_module_free(moduleInstance, answerOffset);
    wasm_runtime_module_free(moduleInstance, userdataOffset);

    return res;
}

void Question::loadChecker() {
    if(checkerLoaded) {
        return;
    }

    uint32_t size;
    uint32_t stackSize = 8092;
    uint32_t heapSize = 8092;

    uint8_t* buffer = reinterpret_cast<uint8_t*>(bh_read_file_to_buffer(checkerPath.toStdString().c_str(), &size));
    char errorBuf[128];

    module = wasm_runtime_load(buffer, size, errorBuf, sizeof(errorBuf));
    moduleInstance = wasm_runtime_instantiate(module, stackSize, heapSize, errorBuf, sizeof(errorBuf));
    execEnv = wasm_runtime_create_exec_env(moduleInstance, stackSize);

    scoreFunction = wasm_runtime_lookup_function(moduleInstance, "answer_score");
    validFunction = wasm_runtime_lookup_function(moduleInstance, "answer_valid");
    checkerLoaded = true;
}
