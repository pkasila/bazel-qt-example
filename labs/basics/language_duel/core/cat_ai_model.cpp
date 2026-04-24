#include "core/cat_ai_model.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace {
double clampUnit(double value) {
    return std::max(0.0, std::min(1.0, value));
}
}  // namespace

double CatAiModel::predictMistakeRisk(const CatAiFeatures& features) const {
    return predictDetailed(features).ensembleRisk;
}

CatAiPrediction CatAiModel::predictDetailed(const CatAiFeatures& features) const {
    const auto x = vectorize(features);
    CatAiPrediction prediction;
    prediction.logisticRisk = predictLogistic(x);
    prediction.memoryRisk = predictMemory(x, &prediction.memoryNeighbors);
    prediction.ensembleRisk = prediction.memoryNeighbors == 0
        ? prediction.logisticRisk
        : clampUnit(prediction.logisticRisk * 0.68 + prediction.memoryRisk * 0.32);
    prediction.recommendation = recommendationFor(prediction.ensembleRisk, features);
    return prediction;
}

void CatAiModel::train(const CatAiFeatures& features, bool mistakeHappened) {
    const auto x = vectorize(features);
    const double prediction = predictLogistic(x);
    const double target = mistakeHappened ? 1.0 : 0.0;
    const double error = target - prediction;
    const double learningRate = 0.16;

    for (int i = 0; i < kFeatureCount; ++i) {
        weights_[i] = std::max(-4.0, std::min(4.0, weights_[i] + learningRate * error * x[i]));
    }

    if (memory_.size() >= kMaxMemorySamples) {
        memory_.erase(memory_.begin());
    }
    memory_.push_back({x, mistakeHappened});
    ++examplesSeen_;
}

QStringList CatAiModel::weightsToStrings() const {
    QStringList values;
    values.reserve(kFeatureCount);
    for (double weight : weights_) {
        values.push_back(QString::number(weight, 'g', 12));
    }
    return values;
}

void CatAiModel::setWeightsFromStrings(const QStringList& values) {
    if (values.size() != kFeatureCount) {
        return;
    }

    std::array<double, kFeatureCount> parsed{};
    for (int i = 0; i < kFeatureCount; ++i) {
        bool ok = false;
        parsed[i] = values[i].toDouble(&ok);
        if (!ok) {
            return;
        }
    }

    weights_ = parsed;
}

QStringList CatAiModel::memoryToStrings() const {
    QStringList values;
    values.reserve(static_cast<int>(memory_.size()));

    for (const auto& sample : memory_) {
        QStringList fields;
        fields.reserve(kFeatureCount + 1);
        for (double feature : sample.features) {
            fields.push_back(QString::number(feature, 'g', 12));
        }
        fields.push_back(sample.mistake ? "1" : "0");
        values.push_back(fields.join(','));
    }

    return values;
}

void CatAiModel::setMemoryFromStrings(const QStringList& values) {
    std::vector<TrainingSample> parsed;
    parsed.reserve(std::min(static_cast<int>(values.size()), kMaxMemorySamples));

    for (const QString& row : values) {
        const QStringList fields = row.split(',');
        if (fields.size() != kFeatureCount + 1) {
            continue;
        }

        TrainingSample sample;
        bool ok = true;
        for (int i = 0; i < kFeatureCount; ++i) {
            bool fieldOk = false;
            sample.features[i] = fields[i].toDouble(&fieldOk);
            ok = ok && fieldOk;
        }

        if (!ok) {
            continue;
        }

        sample.mistake = fields[kFeatureCount] == "1";
        parsed.push_back(sample);
        if (static_cast<int>(parsed.size()) >= kMaxMemorySamples) {
            break;
        }
    }

    memory_ = parsed;
}

QStringList CatAiModel::explainPrediction(const CatAiFeatures& features, int limit) const {
    struct Contribution {
        QString name;
        double value = 0.0;
        double weight = 0.0;
        double contribution = 0.0;
    };

    const auto x = vectorize(features);
    const QStringList names = {
        "difficulty",
        "grammar mode",
        "session progress",
        "time pressure",
        "mistake pressure",
        "streak strength",
        "hint pressure",
        "daily challenge",
    };

    std::vector<Contribution> contributions;
    contributions.reserve(kFeatureCount - 1);
    for (int i = 1; i < kFeatureCount; ++i) {
        contributions.push_back({names[i - 1], x[i], weights_[i], x[i] * weights_[i]});
    }

    std::sort(contributions.begin(), contributions.end(), [](const Contribution& lhs, const Contribution& rhs) {
        return std::abs(lhs.contribution) > std::abs(rhs.contribution);
    });

    QStringList lines;
    const int count = std::min(std::max(0, limit), static_cast<int>(contributions.size()));
    for (int i = 0; i < count; ++i) {
        const Contribution& item = contributions[i];
        const QString direction = item.contribution >= 0.0 ? "raises risk" : "lowers risk";
        lines.push_back(QString("%1 %2 by %3 (value %4, weight %5)")
            .arg(item.name)
            .arg(direction)
            .arg(QString::number(std::abs(item.contribution), 'f', 2))
            .arg(QString::number(item.value, 'f', 2))
            .arg(QString::number(item.weight, 'f', 2)));
    }

    return lines;
}

int CatAiModel::examplesSeen() const {
    return examplesSeen_;
}

void CatAiModel::setExamplesSeen(int examplesSeen) {
    examplesSeen_ = std::max(0, examplesSeen);
}

int CatAiModel::memorySize() const {
    return static_cast<int>(memory_.size());
}

QString CatAiModel::learningPhase() const {
    if (examplesSeen_ < 8) {
        return "cold start: heuristic weights are active, personalization has just begun";
    }
    if (memory_.size() < 15) {
        return "calibrating: logistic model is learning, kNN memory is collecting examples";
    }
    if (examplesSeen_ < 35) {
        return "personalizing: ensemble mixes learned weights with similar-answer memory";
    }
    return "personalized: enough local data for stable risk explanations";
}

QString CatAiModel::riskLabel(double risk) {
    if (risk >= 0.70) {
        return "high";
    }
    if (risk >= 0.42) {
        return "medium";
    }
    return "low";
}

std::array<double, CatAiModel::kFeatureCount> CatAiModel::vectorize(const CatAiFeatures& features) {
    return {
        1.0,
        clampUnit(features.difficulty),
        clampUnit(features.grammarMode),
        clampUnit(features.progress),
        clampUnit(features.timePressure),
        clampUnit(features.mistakePressure),
        clampUnit(features.streakStrength),
        clampUnit(features.hintPressure),
        clampUnit(features.dailyChallenge),
    };
}

QString CatAiModel::recommendationFor(double risk, const CatAiFeatures& features) {
    if (risk >= 0.76 && features.timePressure >= 0.75) {
        return "High risk plus low time: answer the core meaning first, then polish only if seconds remain.";
    }
    if (risk >= 0.70 && features.mistakePressure >= 0.50) {
        return "High risk near the mistake limit: use a hint before submitting.";
    }
    if (risk >= 0.60 && features.grammarMode >= 0.5) {
        return "Grammar risk is elevated: look for tense markers and agreement traps.";
    }
    if (risk >= 0.60) {
        return "Translation risk is elevated: keep nouns and verbs, punctuation can wait.";
    }
    if (risk <= 0.28 && features.streakStrength >= 0.35) {
        return "Low risk with a streak: push for combo points.";
    }
    return "Balanced risk: read once for meaning and once for form.";
}

double CatAiModel::sigmoid(double value) {
    if (value >= 0.0) {
        const double z = std::exp(-value);
        return 1.0 / (1.0 + z);
    }

    const double z = std::exp(value);
    return z / (1.0 + z);
}

double CatAiModel::predictLogistic(const std::array<double, kFeatureCount>& x) const {
    double score = 0.0;
    for (int i = 0; i < kFeatureCount; ++i) {
        score += weights_[i] * x[i];
    }

    return sigmoid(score);
}

double CatAiModel::predictMemory(const std::array<double, kFeatureCount>& x, int* neighborsUsed) const {
    if (neighborsUsed) {
        *neighborsUsed = 0;
    }
    if (memory_.empty()) {
        return predictLogistic(x);
    }

    struct Neighbor {
        double distance = std::numeric_limits<double>::max();
        bool mistake = false;
    };

    std::vector<Neighbor> neighbors;
    neighbors.reserve(memory_.size());
    for (const auto& sample : memory_) {
        double distanceSquared = 0.0;
        for (int i = 1; i < kFeatureCount; ++i) {
            const double delta = x[i] - sample.features[i];
            distanceSquared += delta * delta;
        }
        neighbors.push_back({std::sqrt(distanceSquared), sample.mistake});
    }

    std::sort(neighbors.begin(), neighbors.end(), [](const Neighbor& lhs, const Neighbor& rhs) {
        return lhs.distance < rhs.distance;
    });

    const int k = std::min(7, static_cast<int>(neighbors.size()));
    double weightedMistakes = 0.0;
    double totalWeight = 0.0;
    for (int i = 0; i < k; ++i) {
        const double weight = 1.0 / (0.08 + neighbors[i].distance);
        weightedMistakes += neighbors[i].mistake ? weight : 0.0;
        totalWeight += weight;
    }

    if (neighborsUsed) {
        *neighborsUsed = k;
    }
    if (totalWeight <= 0.0) {
        return predictLogistic(x);
    }
    return clampUnit(weightedMistakes / totalWeight);
}
