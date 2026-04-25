#include "questionrepository.h"

QList<Question> QuestionRepository::questionsFor(ExerciseType type) {
    if (type == ExerciseType::Translation) {
        return {
            {"Translate: I have a green book", "Tengo un libro verde", {}, "Spanish adjective usually goes after the noun: libro verde."},
            {"Translate: She drinks water", "Ella bebe agua", {}, "Use bebe for he/she/it in present tense."},
            {"Translate: We are students", "Somos estudiantes", {}, "Somos is the form of ser for nosotros."},
            {"Translate: The cat is sleeping", "El gato duerme", {}, "Duerme is a present tense form of dormir."}
        };
    }

    return {
        {"Choose the correct word: I ___ a student.", "am", {"is", "am", "are"}, "Use am with I."},
        {"Choose the correct word: She ___ coffee every morning.", "drinks", {"drink", "drinks", "drinking"}, "Present Simple: add -s for he/she/it."},
        {"Choose the correct word: They ___ playing football now.", "are", {"is", "are", "am"}, "Use are with they."},
        {"Choose the correct word: He has ___ this film before.", "seen", {"saw", "see", "seen"}, "Present Perfect uses the past participle."}
    };
}

QString QuestionRepository::titleFor(ExerciseType type) {
    return type == ExerciseType::Translation ? "Translation" : "Grammar";
}

QString QuestionRepository::startTextFor(ExerciseType type) {
    return type == ExerciseType::Translation
        ? "Type the Spanish translation. Small typos are allowed."
        : "Choose exactly one correct answer.";
}
