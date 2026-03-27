#ifndef EXERCISE_H
#define EXERCISE_H

#include "models/ExerciseData.h" // Include ExerciseData

// This could be an interface or a base class if common logic for all exercises exists
// For now, specific exercise classes can manage their own logic based on ExerciseData
class Exercise {
public:
    virtual ~Exercise() = default;
    virtual bool checkAnswer(const QString& userAnswer) = 0;
    virtual const ExerciseData& getCurrentExerciseData() const = 0;
    // virtual void loadNext() = 0; // Logic for loading next within an exercise instance
};

#endif // EXERCISE_H