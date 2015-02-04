#ifndef SLEEPERTHREAD_H
#define SLEEPERTHREAD_H
#include <QThread>

class SleeperThread : public QThread{
    public:
    static void msleep(unsigned long msecs)
    {
        QThread::msleep(msecs);
    }
};

#endif // SLEEPERTHREAD_H
