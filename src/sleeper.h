#ifndef SLEEPER_H
#define SLEEPER_H

#include <QThread>

class Sleeper : public QThread {
public:
    static void sleep(unsigned long l){
        QThread::sleep(l);
    }
};

#endif // SLEEPER_H
