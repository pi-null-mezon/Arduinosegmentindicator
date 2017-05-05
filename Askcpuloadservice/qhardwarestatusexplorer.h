#ifndef QHARDWARESTATUSEXPLORER_H
#define QHARDWARESTATUSEXPLORER_H

#include <QSerialPortInfo>
#include <QSerialPort>
#include <QEventLoop>
#include <QProcess>
#include <QTimer>

class QHardwarestatusexplorer : public QObject
{
    Q_OBJECT
public:
    explicit QHardwarestatusexplorer(QObject *parent = 0, uint _timems=4000);
    ~QHardwarestatusexplorer();
signals:

private slots:
    void readproc();
    void askcpuload();
    void writecpuloadtoserialport(int _val);
    void searchcomdevice();

private:

    QTimer _timer;
    QProcess _proc;
    QSerialPort *_sp = NULL;
};

#endif // QHARDWARESTATUSEXPLORER_H
