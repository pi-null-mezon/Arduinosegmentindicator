#include "qhardwarestatusservice.h"
#include "qhardwarestatusexplorer.h"

int main(int argc, char *argv[])
{
    QHardwarestatusservice _service(argc, argv);
    return _service.exec();
}
