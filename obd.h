#ifndef OBD_H
#define OBD_H

#include <QtSerialPort/QSerialPort>
#include <QString>
#include <QWidget>
#include <QLabel>

#include <map>

//class QLabel;

//union pid_t {
//    quint16 qi;
//    unsigned short i;
//    struct {
//        unsigned char mode;
//        unsigned char id;
//    };
//    struct {
//        quint8 qmode;
//        quint8 qid;
//    };
//};


class OBD
{
public:
    enum OBD2_PID {PID_SHORT_TERM_FUEL_TRIM, PID_LONG_TERM_FUEL_TRIM, PID_ENGINE_RPM, PID_MAF_AIRFLOW_RATE, PID_COLLANT_TEMP, PID_TIMING_ADVANCE};

    OBD(QString portName = "", int baudrate = QSerialPort::Baud9600);
    ~OBD();

    bool setDataLabel(QLabel* label, OBD2_PID pid);
    bool setValueLabel(QLabel* label, OBD2_PID pid);
    bool setValueLabelTest(QLabel* label, OBD2_PID pid, QString response);

private:
    QSerialPort *obdSerial;
//    std::map<OBD2_PID, QLabel*> pidToValueLabel;
    std::map<OBD2_PID, char> pidToNumValues;
    std::map<OBD2_PID, QString> pidToReqestStr;
    std::map<OBD2_PID, QString> pidToLabelStr;
    std::map<OBD2_PID, QString> pidToValueStr;

    QString makeRequest(QString req, OBD2_PID pid);

//    void clearLabelMap();
//    void associatePidAndLabel(OBD2_PID pid, QLabel* label);

    QString parseValue(QString str, OBD2_PID pid);

    void parseResult1(QString str, int* a);
    void parseResult2(QString str, int* a, int* b);
    void parseResult3(QString str, int* a, int* b, int* c);
    void parseResult4(QString str, int* a, int* b, int* c, int* d);

    pid_t strToPid(QString str);
};

#endif // OBD_H
