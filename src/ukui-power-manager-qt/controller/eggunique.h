#ifndef EGGUNIQUE_H
#define EGGUNIQUE_H

#include <QObject>
#include <QSharedMemory>
#include <QSystemSemaphore>


class EggUnique
{

public:
    EggUnique(const QString& key);
    ~EggUnique();

    bool isAnotherRunning();
    bool tryToRun();
    void release();

private:
    const QString key;
    const QString memLockKey;
    const QString sharedmemKey;

    QSharedMemory sharedMem;
    QSystemSemaphore memLock;

    Q_DISABLE_COPY(EggUnique)
};


#endif // EGGUNIQUE_H
