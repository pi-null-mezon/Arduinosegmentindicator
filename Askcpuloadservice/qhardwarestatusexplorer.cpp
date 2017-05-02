#include "qhardwarestatusexplorer.h"

QHardwarestatusexplorer::QHardwarestatusexplorer(QObject *parent, uint _timems) : QObject(parent)
{
    // Initialize serial port communication system
    qInfo("Scan available serial ports...");
    QList<QSerialPortInfo> _ap = QSerialPortInfo::availablePorts();
    int _target = -1;
    for(uint i = 0; i < _ap.size(); ++i) {
        qInfo("%d) %s %s", i, _ap.at(i).portName().toLocal8Bit().constData(), _ap.at(i).description().toLocal8Bit().constData());
        if(_ap.at(i).description().contains("CH340")) {
            _target = static_cast<int>(i);
        }
    }
    if(_target > -1) {
        _sp = new QSerialPort(_ap.at(_target));
        if(_sp->open(QIODevice::ReadWrite) == false) {
            qCritical("Can not open CH340 device!");
        } else {
            _sp->setBaudRate(9600);
            _sp->setDataTerminalReady(true);
            qInfo("CH340 device has been opened for communications");
        }
    } else {
        qCritical("Can not find CH340 device!");
    }

    // Setup process to be read when it is ready
    connect(&_proc, SIGNAL(readyRead()), this, SLOT(readproc()));

    // Setup timer to call askcpuload
    QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(askcpuload()));
    _timer.start(_timems);
}

QHardwarestatusexplorer::~QHardwarestatusexplorer()
{
    if(_proc.state() == QProcess::Running) {
        _proc.waitForFinished();
    }

    if(_sp != NULL) {
        _sp->close();
        delete _sp;
    }
}

void QHardwarestatusexplorer::readproc()
{
    QByteArray _out = _proc.readAll();
    int _t = QString(_out).section('\n',1,1).simplified().toInt();
    writecpuloadtoserialport(_t);
    //qInfo("Cpuload: %d", _t);
}

void QHardwarestatusexplorer::askcpuload()
{
    if(_proc.state() != QProcess::Running) {
        QStringList argsl;
        argsl << "cpu" <<  "get" <<  "loadpercentage";
        _proc.start(QString("wmic"), argsl);
    }
}

void QHardwarestatusexplorer::writecpuloadtoserialport(int _val)
{
    QByteArray _ba = QString::number(_val).toLocal8Bit();
    _sp->write(_ba);
}
