#pragma once

#include <array>
#include <vector>

#include <QString>
#include <QStringList>

struct CatAiFeatures {
    double difficulty = 0.0;
    double grammarMode = 0.0;
    double progress = 0.0;
    double timePressure = 0.0;
    double mistakePressure = 0.0;
    double streakStrength = 0.0;
    double hintPressure = 0.0;
    double dailyChallenge = 0.0;
};

struct CatAiPrediction {
    double logisticRisk = 0.0;
    double memoryRisk = 0.0;
    double ensembleRisk = 0.0;
    int memoryNeighbors = 0;
    QString recommendation;
};

class CatAiModel {
public:
    double predictMistakeRisk(const CatAiFeatures& features) const;
    CatAiPrediction predictDetailed(const CatAiFeatures& features) const;
    void train(const CatAiFeatures& features, bool mistakeHappened);

    QStringList weightsToStrings() const;
    void setWeightsFromStrings(const QStringList& values);
    QStringList memoryToStrings() const;
    void setMemoryFromStrings(const QStringList& values);
    QStringList explainPrediction(const CatAiFeatures& features, int limit) const;

    int examplesSeen() const;
    void setExamplesSeen(int examplesSeen);
    int memorySize() const;
    QString learningPhase() const;

    static QString riskLabel(double risk);

private:
    static constexpr int kFeatureCount = 9;
    static constexpr int kMaxMemorySamples = 96;

    struct TrainingSample {
        std::array<double, kFeatureCount> features{};
        bool mistake = false;
    };

    static std::array<double, kFeatureCount> vectorize(const CatAiFeatures& features);
    static QString recommendationFor(double risk, const CatAiFeatures& features);
    static double sigmoid(double value);
    double predictLogistic(const std::array<double, kFeatureCount>& x) const;
    double predictMemory(const std::array<double, kFeatureCount>& x, int* neighborsUsed) const;

    std::array<double, kFeatureCount> weights_ = {
        -0.75,  // bias
        0.55,   // difficulty
        0.18,   // grammar mode
        0.20,   // progress through session
        1.15,   // time pressure
        1.35,   // mistake pressure
        -0.85,  // streak reduces risk
        0.55,   // frequent hints indicate uncertainty
        0.25,   // daily challenge
    };
    int examplesSeen_ = 0;
    std::vector<TrainingSample> memory_;
};
