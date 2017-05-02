#include "qhardwarestatusservice.h"

QHardwarestatusservice::QHardwarestatusservice(int argc, char **argv) : QtService<QCoreApplication>(argc, argv, "HardwareService")
{
    try {
        setServiceDescription("This is the service that sends hardware status to the Arduino's COM port");
        setServiceFlags(QtServiceBase::CanBeSuspended);
    }
    catch(...) {
        qCritical("An unknown error in the service constructor!");
    }
}

QHardwarestatusservice::~QHardwarestatusservice()
{
    if(_hexp) {
        delete _hexp;
    }
}

void QHardwarestatusservice::start()
{
    QCoreApplication *app = application();
    Q_UNUSED(app);

    _hexp = new QHardwarestatusexplorer;
}

void QHardwarestatusservice::stop()
{

}

void QHardwarestatusservice::pause()
{

}

void QHardwarestatusservice::resume()
{

}
