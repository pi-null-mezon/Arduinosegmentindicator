#ifndef QHARDWARESTATUSSERVICE_H
#define QHARDWARESTATUSSERVICE_H

#include "qtservice.h"
#include "qhardwarestatusexplorer.h"

class QHardwarestatusservice : public QtService<QCoreApplication>
{
public:
    QHardwarestatusservice(int argc, char **argv);

    ~QHardwarestatusservice();

    void start();

    void stop();

    void pause();

    void resume();

private:
    QHardwarestatusexplorer *_hexp = NULL;
};

#endif // QHARDWARESTATUSSERVICE_H
