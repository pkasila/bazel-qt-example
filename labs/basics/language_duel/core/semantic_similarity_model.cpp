#include "core/semantic_similarity_model.h"

#include <algorithm>

#include <QChar>
#include <QMap>
#include <QSet>

namespace {
int consumeOverlap(QMap<QString, int>* left, QMap<QString, int>* right) {
    int overlap = 0;
    for (auto it = left->begin(); it != left->end(); ++it) {
        auto rightIt = right->find(it.key());
        if (rightIt == right->end()) {
            continue;
        }

        const int matched = std::min(it.value(), rightIt.value());
        overlap += matched;
        it.value() -= matched;
        rightIt.value() -= matched;
    }
    return overlap;
}

QMap<QString, int> countsFor(const QStringList& concepts) {
    QMap<QString, int> counts;
    for (const QString& concept : concepts) {
        counts[concept] += 1;
    }
    return counts;
}

QStringList positiveKeys(const QMap<QString, int>& counts) {
    QStringList keys;
    for (auto it = counts.begin(); it != counts.end(); ++it) {
        if (it.value() > 0) {
            keys.push_back(it.key());
        }
    }
    return keys;
}

bool hasPrefix(const QString& token, std::initializer_list<const char*> prefixes) {
    for (const char* prefix : prefixes) {
        if (token.startsWith(QString::fromUtf8(prefix))) {
            return true;
        }
    }
    return false;
}

QString normalizeSemanticText(const QString& text) {
    QString normalized = text.normalized(QString::NormalizationForm_D);
    QString stripped;
    stripped.reserve(normalized.size());

    for (const QChar ch : normalized) {
        if (ch.category() == QChar::Mark_NonSpacing ||
            ch.category() == QChar::Mark_SpacingCombining ||
            ch.category() == QChar::Mark_Enclosing) {
            continue;
        }

        if (ch.isLetterOrNumber() || ch.isSpace()) {
            stripped.push_back(ch.toLower());
            continue;
        }

        if (QString("'-").contains(ch)) {
            stripped.push_back(' ');
        }
    }

    stripped.replace(QChar(0x0451), QChar(0x0435));
    stripped.replace(QChar(0x0401), QChar(0x0435));
    return stripped.simplified();
}

bool isCyrillic(QChar ch) {
    const ushort code = ch.unicode();
    return code >= 0x0400 && code <= 0x04FF;
}

QString repairMixedCyrillicToken(const QString& token) {
    bool hasCyrillicLetter = false;
    for (const QChar ch : token) {
        hasCyrillicLetter = hasCyrillicLetter || isCyrillic(ch);
    }
    if (!hasCyrillicLetter) {
        return token;
    }

    QString repaired;
    repaired.reserve(token.size());
    for (const QChar ch : token) {
        switch (ch.toLatin1()) {
            case 'a':
                repaired.push_back(QChar(0x0430));  // а
                break;
            case 'c':
                repaired.push_back(QChar(0x0441));  // с
                break;
            case 'e':
                repaired.push_back(QChar(0x0435));  // е
                break;
            case 'h':
                repaired.push_back(QChar(0x043D));  // н
                break;
            case 'k':
                repaired.push_back(QChar(0x043A));  // к
                break;
            case 'm':
                repaired.push_back(QChar(0x043C));  // м
                break;
            case 'o':
                repaired.push_back(QChar(0x043E));  // о
                break;
            case 'p':
                repaired.push_back(QChar(0x0440));  // р
                break;
            case 't':
                repaired.push_back(QChar(0x0442));  // т
                break;
            case 'x':
                repaired.push_back(QChar(0x0445));  // х
                break;
            case 'y':
                repaired.push_back(QChar(0x0443));  // у
                break;
            default:
                repaired.push_back(ch);
                break;
        }
    }
    return repaired;
}

QStringList repairedTokensFor(const QString& text) {
    QStringList repaired;
    const QStringList tokens = normalizeSemanticText(text).split(' ', Qt::SkipEmptyParts);
    repaired.reserve(tokens.size());
    for (const QString& token : tokens) {
        repaired.push_back(repairMixedCyrillicToken(token));
    }
    return repaired;
}

bool tokenHasPrefix(const QString& token, std::initializer_list<const char*> prefixes) {
    return hasPrefix(token, prefixes);
}

bool isPoliteNegative(const QStringList& tokens, int index) {
    if (index < 0 || index >= tokens.size()) {
        return false;
    }
    const QString token = tokens[index];
    if (token != "не" && token != "ne") {
        return false;
    }

    for (int i = index + 1; i < tokens.size() && i <= index + 3; ++i) {
        if (tokenHasPrefix(tokens[i], {"мог", "мож", "mog", "mozh", "could", "would"})) {
            return true;
        }
    }
    return false;
}

bool hasPoliteRequestShape(const QStringList& tokens) {
    bool hasSoftModal = false;
    bool hasBy = false;
    for (const QString& token : tokens) {
        hasSoftModal = hasSoftModal || tokenHasPrefix(token, {"мог", "мож", "mog", "mozh", "could", "would"});
        hasBy = hasBy || token == "бы" || token == "by";
    }
    return hasSoftModal && hasBy;
}

bool isPoliteRequestFiller(const QStringList& tokens, int index) {
    if (!hasPoliteRequestShape(tokens)) {
        return false;
    }

    const QString token = tokens[index];
    return tokenHasPrefix(token, {"мог", "мож", "mog", "mozh", "could", "would"}) ||
        token == "бы" || token == "by" ||
        token == "ты" || token == "тебе" || token == "тебя" ||
        token == "you" || token == "ty" || token == "tebe" || token == "tebya";
}

bool containsConcept(const QStringList& concepts, const QString& concept) {
    return concepts.contains(concept);
}

bool hasAnyConcept(const QStringList& concepts, std::initializer_list<const char*> candidates) {
    for (const char* candidate : candidates) {
        if (concepts.contains(QString::fromUtf8(candidate))) {
            return true;
        }
    }
    return false;
}

QString guardMessage(const QStringList& inputConcepts, const QStringList& expectedConcepts) {
    const bool inputFemale = containsConcept(inputConcepts, "person:female");
    const bool inputMale = containsConcept(inputConcepts, "person:male");
    const bool expectedFemale = containsConcept(expectedConcepts, "person:female");
    const bool expectedMale = containsConcept(expectedConcepts, "person:male");
    if ((inputFemale && expectedMale) || (inputMale && expectedFemale)) {
        return "guard blocked: subject gender changed";
    }

    const bool inputNegation = containsConcept(inputConcepts, "logic:negation");
    const bool expectedNegation = containsConcept(expectedConcepts, "logic:negation");
    if (inputNegation != expectedNegation) {
        return "guard blocked: negation changed";
    }

    const bool inputPerson = hasAnyConcept(inputConcepts, {
        "person:female", "person:male", "person:first", "person:we", "person:they", "person:you"
    });
    const bool expectedPerson = hasAnyConcept(expectedConcepts, {
        "person:female", "person:male", "person:first", "person:we", "person:they", "person:you"
    });
    if (inputPerson && expectedPerson) {
        const bool sharedPerson =
            (inputFemale && expectedFemale) ||
            (inputMale && expectedMale) ||
            (containsConcept(inputConcepts, "person:first") && containsConcept(expectedConcepts, "person:first")) ||
            (containsConcept(inputConcepts, "person:we") && containsConcept(expectedConcepts, "person:we")) ||
            (containsConcept(inputConcepts, "person:they") && containsConcept(expectedConcepts, "person:they")) ||
            (containsConcept(inputConcepts, "person:you") && containsConcept(expectedConcepts, "person:you"));
        if (!sharedPerson) {
            return "guard blocked: grammatical person changed";
        }
    }

    return {};
}

QString confidenceFor(double similarity, double coverage, double precision, bool accepted) {
    if (!accepted) {
        return "rejected";
    }
    if (similarity >= 0.93 && coverage >= 0.92 && precision >= 0.85) {
        return "high";
    }
    if (similarity >= 0.87 && coverage >= 0.84) {
        return "medium";
    }
    return "borderline";
}
}  // namespace

SemanticMatchResult SemanticSimilarityModel::compare(const QString& userInput, const QString& expected) {
    SemanticMatchResult result;
    result.inputConcepts = conceptsForText(userInput);
    result.expectedConcepts = conceptsForText(expected);

    if (result.expectedConcepts.isEmpty()) {
        result.accepted = result.inputConcepts.isEmpty();
        result.similarity = result.accepted ? 1.0 : 0.0;
        result.conceptCoverage = result.similarity;
        result.precision = result.similarity;
        result.confidence = result.accepted ? "high" : "rejected";
        result.explanation = result.accepted
            ? "Semantic model: both answers are empty after normalization."
            : "Semantic model: expected sentence has no concepts to compare.";
        return result;
    }

    QMap<QString, int> inputCounts = countsFor(result.inputConcepts);
    QMap<QString, int> expectedCounts = countsFor(result.expectedConcepts);
    QMap<QString, int> inputRemaining = inputCounts;
    QMap<QString, int> expectedRemaining = expectedCounts;
    const int overlap = consumeOverlap(&inputRemaining, &expectedRemaining);
    result.matchedConcepts = positiveKeys(inputCounts);
    for (int i = result.matchedConcepts.size() - 1; i >= 0; --i) {
        if (!expectedCounts.contains(result.matchedConcepts[i])) {
            result.matchedConcepts.removeAt(i);
        }
    }

    const int inputTotal = result.inputConcepts.size();
    const int expectedTotal = result.expectedConcepts.size();
    const int unionTotal = inputTotal + expectedTotal - overlap;
    const double coverage = static_cast<double>(overlap) / static_cast<double>(expectedTotal);
    const double precision = inputTotal == 0 ? 0.0 : static_cast<double>(overlap) / static_cast<double>(inputTotal);
    const double jaccard = unionTotal == 0 ? 1.0 : static_cast<double>(overlap) / static_cast<double>(unionTotal);

    result.conceptCoverage = coverage;
    result.precision = precision;
    result.similarity = 0.50 * coverage + 0.30 * precision + 0.20 * jaccard;
    result.accepted = result.similarity >= 0.84 && coverage >= 0.80 && precision >= 0.74;
    result.missingConcepts = positiveKeys(expectedRemaining);
    result.extraConcepts = positiveKeys(inputRemaining);

    const QString guard = guardMessage(result.inputConcepts, result.expectedConcepts);
    if (!guard.isEmpty()) {
        result.accepted = false;
        result.blockedByGuard = true;
    }
    result.confidence = confidenceFor(result.similarity, coverage, precision, result.accepted);

    result.explanation = QString("Semantic model: %1% meaning match, %2% expected concepts covered")
        .arg(static_cast<int>(result.similarity * 100.0))
        .arg(static_cast<int>(coverage * 100.0));
    if (!guard.isEmpty()) {
        result.explanation += ". " + guard;
    }
    if (!result.missingConcepts.isEmpty()) {
        result.explanation += ". Missing concept(s): " + result.missingConcepts.join(", ");
    }

    return result;
}

QString SemanticSimilarityModel::modelVersion() {
    return "local-hybrid-semantic-v3.0";
}

QStringList SemanticSimilarityModel::capabilities() {
    return {
        "Unicode normalization and mixed Latin/Cyrillic repair",
        "Russian/English/translit concept mapping",
        "Synonym and paraphrase graph for lab vocabulary",
        "Polite request normalization",
        "Negation, gender and grammatical-person guards",
        "Coverage/precision/Jaccard confidence scoring",
    };
}

QString SemanticSimilarityModel::normalize(const QString& text) {
    return normalizeSemanticText(text);
}

QStringList SemanticSimilarityModel::conceptsForText(const QString& text) {
    QStringList concepts;
    const QStringList tokens = repairedTokensFor(text);
    for (int i = 0; i < tokens.size(); ++i) {
        const QString& token = tokens[i];
        if (isPoliteNegative(tokens, i) || isPoliteRequestFiller(tokens, i)) {
            continue;
        }
        if (isStopWord(token)) {
            continue;
        }

        const QString concept = conceptForToken(token);
        if (!concept.isEmpty()) {
            concepts.push_back(concept);
        }
    }

    if ((tokens.contains("не") || tokens.contains("ne")) &&
        (tokens.contains("мог") || tokens.contains("могли") || tokens.contains("могла") ||
         tokens.contains("mog") || tokens.contains("could") || tokens.contains("would")) &&
        (tokens.contains("бы") || tokens.contains("by"))) {
        concepts.removeAll("logic:negation");
    }

    if (containsConcept(concepts, "time:never")) {
        concepts.removeAll("logic:negation");
    }
    if (containsConcept(concepts, "logic:unless")) {
        concepts.removeAll("logic:negation");
    }
    if (containsConcept(concepts, "quality:robust") && containsConcept(concepts, "object:typo")) {
        concepts.removeAll("action:handle");
    }

    concepts.removeDuplicates();
    std::sort(concepts.begin(), concepts.end());
    return concepts;
}

QString SemanticSimilarityModel::conceptForToken(const QString& token) {
    if (token == "она" || token == "ей" || token == "еи" || token == "ее" || token == "her" || token == "she" ||
        token == "ona" || token == "ey" || token == "ei" || token == "eyo" || token == "ee") {
        return "person:female";
    }
    if (token == "он" || token == "ему" || token == "его" || token == "he" || token == "him" || token == "his" ||
        token == "on" || token == "emu" || token == "ego") {
        return "person:male";
    }
    if (token == "я" || token == "мне" || token == "меня" || token == "i" || token == "me" ||
        token == "ya" || token == "mne" || token == "menya") {
        return "person:first";
    }
    if (token == "мы" || token == "нам" || token == "нас" || token == "we" || token == "us" ||
        token == "my" || token == "nam" || token == "nas") {
        return "person:we";
    }
    if (token == "они" || token == "им" || token == "их" || token == "they" || token == "them" ||
        token == "oni" || token == "im" || token == "ih" || token == "ikh") {
        return "person:they";
    }
    if (token == "ты" || token == "тебе" || token == "тебя" || token == "you" ||
        token == "ty" || token == "tebe" || token == "tebya") {
        return "person:you";
    }

    if (hasPrefix(token, {"люб", "нрав", "обожа", "предпоч", "lyub", "lub", "nrav", "obozh", "predpoch"})) {
        return "meaning:like";
    }
    if (hasPrefix(token, {"зелен", "zelen", "zelyon", "zelion", "green"})) {
        return "color:green";
    }
    if (hasPrefix(token, {"чаи", "чай", "чая", "чаю"}) || token == "tea" ||
        token == "chai" || token == "chay") {
        return "drink:tea";
    }
    if (hasPrefix(token, {"добро", "добры", "хорош", "привет", "здрав", "good", "hello"})) {
        return "meaning:good";
    }
    if (hasPrefix(token, {"утр"}) || token == "morning") {
        return "time:morning";
    }
    if (hasPrefix(token, {"чита", "прочит", "смотр", "read"})) {
        return "action:read";
    }
    if (hasPrefix(token, {"книг", "книж", "book"})) {
        return "object:book";
    }
    if (hasPrefix(token, {"кажд", "every", "daily"})) {
        return "time:every";
    }
    if (hasPrefix(token, {"день", "day"})) {
        return "time:day";
    }
    if (hasPrefix(token, {"кот", "кош", "котик", "cat"})) {
        return "animal:cat";
    }
    if (hasPrefix(token, {"подход", "метод", "способ", "approach"})) {
        return "object:approach";
    }
    if (token == "под" || token == "under") {
        return "place:under";
    }
    if (hasPrefix(token, {"стол", "table"})) {
        return "object:table";
    }
    if (hasPrefix(token, {"уч", "изуч", "занима", "learn", "study"})) {
        return "action:learn";
    }
    if (hasPrefix(token, {"англи", "english"})) {
        return "language:english";
    }
    if (hasPrefix(token, {"друг", "приятел", "товарищ", "friend"})) {
        return "person:friend";
    }
    if (hasPrefix(token, {"жив", "прожива", "обита", "live"})) {
        return "action:live";
    }
    if (hasPrefix(token, {"минск", "minsk"})) {
        return "place:minsk";
    }
    if (hasPrefix(token, {"приех", "прибы", "добра", "яви", "arriv"})) {
        return "action:arrive";
    }
    if (hasPrefix(token, {"позд", "late"})) {
        return "time:late";
    }
    if (hasPrefix(token, {"дожд", "лив", "rain"})) {
        return "weather:rain";
    }
    if (hasPrefix(token, {"завтра", "tomorrow"})) {
        return "time:tomorrow";
    }
    if (hasPrefix(token, {"остан", "посид", "stay"})) {
        return "action:stay";
    }
    if (hasPrefix(token, {"дом", "home"})) {
        return "place:home";
    }
    if (hasPrefix(token, {"никогда", "never"})) {
        return "time:never";
    }
    if (hasPrefix(token, {"раньш", "before"})) {
        return "time:before";
    }
    if (hasPrefix(token, {"вид", "смотр", "seen", "saw", "watch"})) {
        return "action:see";
    }
    if (hasPrefix(token, {"фильм", "movie", "film"})) {
        return "object:movie";
    }
    if (hasPrefix(token, {"готов", "вар", "cook"})) {
        return "action:cook";
    }
    if (hasPrefix(token, {"звон", "позвон", "набрал", "call"})) {
        return "action:call";
    }
    if (hasPrefix(token, {"откр", "распах", "open"})) {
        return "action:open";
    }
    if (hasPrefix(token, {"окн", "window"})) {
        return "object:window";
    }
    if (hasPrefix(token, {"урок", "занят", "lesson", "class"})) {
        return "object:lesson";
    }
    if (hasPrefix(token, {"слож", "трудн", "непрост", "difficult", "hard"})) {
        return "quality:difficult";
    }
    if (hasPrefix(token, {"ожид", "думал", "предполаг", "expect"})) {
        return "action:expect";
    }
    if (hasPrefix(token, {"законч", "заверш", "додел", "finish"})) {
        return "action:finish";
    }
    if (hasPrefix(token, {"проект", "project"})) {
        return "object:project";
    }
    if (hasPrefix(token, {"недел", "week"})) {
        return "time:week";
    }
    if (hasPrefix(token, {"забыл", "забы", "forgot"})) {
        return "action:forget";
    }
    if (hasPrefix(token, {"отправ", "высл", "пересл", "send"})) {
        return "action:send";
    }
    if (hasPrefix(token, {"письм", "сообщ", "email", "mail"})) {
        return "object:email";
    }
    if (hasPrefix(token, {"вчера", "yesterday"})) {
        return "time:yesterday";
    }
    if (hasPrefix(token, {"горж", "рад", "довол", "proud"})) {
        return "feeling:proud";
    }
    if (hasPrefix(token, {"прогресс", "успех", "progress"})) {
        return "object:progress";
    }
    if (hasPrefix(token, {"хотя", "однако", "несмотря", "although"})) {
        return "logic:contrast";
    }
    if (hasPrefix(token, {"задач", "task"})) {
        return "object:task";
    }
    if (hasPrefix(token, {"прост", "легк", "simple", "easy"})) {
        return "quality:simple";
    }
    if (hasPrefix(token, {"треб", "нужд", "понадоб", "require", "need"})) {
        return "action:require";
    }
    if (hasPrefix(token, {"тщател", "внимател", "аккурат", "careful"})) {
        return "quality:careful";
    }
    if (hasPrefix(token, {"рассужд", "обдум", "reason"})) {
        return "action:reason";
    }
    if (hasPrefix(token, {"лекц", "lecture"})) {
        return "object:lecture";
    }
    if (hasPrefix(token, {"начал", "старт", "пошл", "start", "begin"})) {
        return "action:start";
    }
    if (hasPrefix(token, {"теор", "материал", "theory"})) {
        return "object:theory";
    }
    if (hasPrefix(token, {"решен", "решать", "делать", "solv"})) {
        return "action:solve";
    }
    if (hasPrefix(token, {"упражн", "exercise"})) {
        return "object:exercise";
    }
    if (hasPrefix(token, {"интерф", "ui", "interface"})) {
        return "object:interface";
    }
    if (hasPrefix(token, {"интуитив", "понят", "удоб", "intuitive"})) {
        return "quality:intuitive";
    }
    if (hasPrefix(token, {"размер", "растяг", "измен", "resize"})) {
        return "ui:resize";
    }
    if (hasPrefix(token, {"объяснен", "пояснен", "explanation"})) {
        return "object:explanation";
    }
    if (hasPrefix(token, {"крат", "лаконич", "корот", "concise"})) {
        return "quality:concise";
    }
    if (hasPrefix(token, {"содерж", "глубок", "умн", "insight"})) {
        return "quality:insightful";
    }
    if (hasPrefix(token, {"регуляр", "постоян", "част", "regular"})) {
        return "time:regular";
    }
    if (hasPrefix(token, {"медлен", "slow"})) {
        return "quality:slow";
    }
    if (hasPrefix(token, {"надеж", "устойчив", "устоич", "креп", "robust"})) {
        return "quality:robust";
    }
    if (hasPrefix(token, {"обраб", "справ", "перевар", "handle"})) {
        return "action:handle";
    }
    if (hasPrefix(token, {"опечат", "mistake", "typo"})) {
        return "object:typo";
    }
    if (hasPrefix(token, {"небольш", "мал", "minor", "small"})) {
        return "quality:small";
    }
    if (hasPrefix(token, {"результ", "итог", "result"})) {
        return "object:result";
    }
    if (hasPrefix(token, {"превзош", "превыс", "exceed"})) {
        return "action:exceed";
    }
    if (hasPrefix(token, {"ожидан", "expectation"})) {
        return "object:expectation";
    }
    if (hasPrefix(token, {"наста", "insist"})) {
        return "action:insist";
    }
    if (hasPrefix(token, {"детал", "мелоч", "detail"})) {
        return "object:detail";
    }
    if (hasPrefix(token, {"провер", "свер", "check"})) {
        return "action:check";
    }
    if (hasPrefix(token, {"дваж", "twice"})) {
        return "quantity:twice";
    }
    if (hasPrefix(token, {"долж", "обязан", "should", "must"})) {
        return "modality:obligation";
    }
    if (hasPrefix(token, {"если", "if"})) {
        return "logic:condition";
    }
    if (hasPrefix(token, {"unless"})) {
        return "logic:unless";
    }
    if (token == "не" || token == "ne" || token == "not" || token == "no" || token == "without") {
        return "logic:negation";
    }

    return token;
}

bool SemanticSimilarityModel::isStopWord(const QString& token) {
    static const QSet<QString> stopWords = {
        "a", "an", "the", "to", "of", "in", "on", "at", "by", "with",
        "and", "or", "but", "is", "are", "was", "were", "am", "be", "been",
        "это", "в", "во", "на", "по", "с", "со", "к", "ко", "от", "до",
        "за", "из", "у", "для", "при", "о", "об", "а", "и", "но", "или",
        "же", "ли", "то", "том", "как", "что", "чем", "чтобы", "бы",
        "пожалуйста", "очень", "уже", "был", "была", "было", "были",
        "есть", "является", "этот", "эта", "это", "эти", "достаточно",
        "мой", "моя", "мое", "мои", "твой", "твоя", "your", "my", "enough"
    };
    return stopWords.contains(token);
}
