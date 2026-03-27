
#include "Schedule.h"

class Utils {
public:

    // write data to JSON
    static void writeScheduleToFile(const QString &filename, const Schedule &schedule);

    // read data from JSON
    static Schedule readScheduleFromFile(const QString &filename);

};
