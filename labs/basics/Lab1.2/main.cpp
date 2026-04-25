#include <QApplication>
#include "mood_journal.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    MoodJournal journal;
    journal.show();
    
    return app.exec();
}
