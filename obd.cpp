#include "obd.h"

#include <QDebug>

OBD::OBD(QString portName, int baudrate)
{
//    obdSerial.setPortName(portName);

//    if (!obdSerial.open(QIODevice::ReadWrite)) {
//        qDebug() << QObject::tr("Failed to open port %1, error: %2").arg(portName).arg(obdSerial.errorString()) << endl;
//        return 1;
//    }
//    if (!obdSerial.setBaudRate(baudrate)) {
//        qDebug() << QObject::tr("Failed to set %1 baud for port %2, error: %3").arg(baudrate).arg(portName).arg(obdSerial.errorString()) << endl;
//        return 1;
//    }
//    if (!obdSerial.setDataBits(QSerialPort::Data8)) {
//        qDebug() << QObject::tr("Failed to set 8 data bits for port %1, error: %2").arg(portName).arg(obdSerial.errorString()) << endl;
//        return 1;
//    }
//    if (!obdSerial.setParity(QSerialPort::NoParity)) {
//        qDebug() << QObject::tr("Failed to set no parity for port %1, error: %2").arg(portName).arg(obdSerial.errorString()) << endl;
//        return 1;
//    }
//    if (!obdSerial.setStopBits(QSerialPort::OneStop)) {
//        qDebug() << QObject::tr("Failed to set 1 stop bit for port %1, error: %2").arg(portName).arg(obdSerial.errorString()) << endl;
//        return 1;
//    }
//    if (!obdSerial.setFlowControl(QSerialPort::NoFlowControl)) {
//        qDebug() << QObject::tr("Failed to set no flow control for port %1, error: %2").arg(portName).arg(obdSerial.errorString()) << endl;
//        return 1;
//    }

    pidToNumValues[PID_COLLANT_TEMP] = 1;
    pidToReqestStr[PID_COLLANT_TEMP] = "0105";
    pidToLabelStr[PID_COLLANT_TEMP] = "Coolant Temp";
    pidToValueStr[PID_COLLANT_TEMP] = "%1 ºF";

    pidToNumValues[PID_TIMING_ADVANCE] = 1;
    pidToReqestStr[PID_TIMING_ADVANCE] = "010E";
    pidToLabelStr[PID_TIMING_ADVANCE] = "Timing Adv.";
    pidToValueStr[PID_TIMING_ADVANCE] = "%1 º";

    pidToNumValues[PID_LONG_TERM_FUEL_TRIM] = 1;
    pidToReqestStr[PID_LONG_TERM_FUEL_TRIM] = "0107";
    pidToLabelStr[PID_LONG_TERM_FUEL_TRIM] = "Long-Term Fuel Trim";
    pidToValueStr[PID_LONG_TERM_FUEL_TRIM] = "%1 \%";

    pidToNumValues[PID_SHORT_TERM_FUEL_TRIM] = 1;
    pidToReqestStr[PID_SHORT_TERM_FUEL_TRIM] = "0106";
    pidToLabelStr[PID_SHORT_TERM_FUEL_TRIM] = "Short-Term Fuel Trim";
    pidToValueStr[PID_SHORT_TERM_FUEL_TRIM] = "%1 \%";

    pidToNumValues[PID_MAF_AIRFLOW_RATE] = 2;
    pidToReqestStr[PID_MAF_AIRFLOW_RATE] = "0110";
    pidToLabelStr[PID_MAF_AIRFLOW_RATE] = "MAF";
    pidToValueStr[PID_MAF_AIRFLOW_RATE] = "%1 g/s";

    pidToNumValues[PID_ENGINE_RPM] = 2;
    pidToReqestStr[PID_ENGINE_RPM] = "010C";
    pidToLabelStr[PID_ENGINE_RPM] = "Engine RPM";
    pidToValueStr[PID_ENGINE_RPM] = "%1 RPM";

//    pidToNumValues[OBD2_PID::PID_COLLANT_TEMP] = 1;
//    pidToReqestStr[OBD2_PID::PID_COLLANT_TEMP] = "0105";
//    pidToLabelStr[OBD2_PID::PID_COLLANT_TEMP] = "Coolant Temp";
//    pidToValueStr[OBD2_PID::PID_COLLANT_TEMP] = "%1 ºF";

}

OBD::~OBD(){

}

bool OBD::setDataLabel(QLabel* label, OBD2_PID pid) {
    QString labelStr = pidToLabelStr[pid];
    label->setText(labelStr);
    return true;
}

bool OBD::setValueLabel(QLabel* label, OBD2_PID pid) {
    QString response = makeRequest(pidToReqestStr[pid], pid);
    QString str = parseValue(response, pid);
    label->setText(str);
    return true;
}

bool OBD::setValueLabelTest(QLabel* label, OBD2_PID pid, QString response) {
    QString str = parseValue(response, pid);
    label->setText(str);
    return true;
}

QString OBD::parseValue(QString str, OBD2_PID pid) {
    qDebug() << "OBD::parseValue(\"" << str << "\", OBD2_PID)";
    QString format = pidToValueStr[pid];

    if (str.compare("") == 0) {
        format.arg("-");
    } else {
        QString s;
        int ivalue;
        float fvalue;
        int A, B, C, D;

        switch (pid) {
        case PID_COLLANT_TEMP:
            qDebug() << "PID_COLLANT_TEMP";
            parseResult1(str, &A);
            ivalue = A-40;
            qDebug() << "ivalue: " << QString("%1").arg(ivalue) ;
            s = s.number(ivalue);
//            s.append(QString.number(ivalue));
//            format.arg(ivalue);
            break;
        case PID_ENGINE_RPM:
            qDebug() << "PID_ENGINE_RPM";
            parseResult2(str, &A, &B);
            fvalue = ((float)((A*256)+B))/4.0;
            s = s.number(fvalue);
//            s.append(QString.number(fvalue));
//            format.arg(fvalue);
            break;
        case PID_LONG_TERM_FUEL_TRIM:
            qDebug() << "PID_LONG_TERM_FUEL_TRIM";
            parseResult1(str, &A);
            fvalue = (float)((A-128)*100) / 128.0;
            s = s.number(fvalue);
//            s.append(QString.number(fvalue));
//            format.arg(fvalue);
            break;
        case PID_SHORT_TERM_FUEL_TRIM:
            qDebug() << "PID_SHORT_TERM_FUEL_TRIM";
            parseResult1(str, &A);
            fvalue = (float)((A-128)*100) / 128.0;
            s = s.number(fvalue);
//            s.append(QString.number(fvalue));
//            format.arg(fvalue);
            break;
        case PID_MAF_AIRFLOW_RATE:
            qDebug() << "PID_MAF_AIRFLOW_RATE";
            parseResult2(str, &A, &B);
            fvalue = ((float)((A*256)+B)) / 100.0;
            s = s.number(fvalue);
//            s.append(QString.number(fvalue));
//            format.arg(fvalue);
            break;
        case PID_TIMING_ADVANCE:
            qDebug() << "PID_TIMING_ADVANCE";
            parseResult1(str, &A);
            fvalue = (float)(A-128) / 2.0;
            s = s.number(fvalue);
//            s.append(QString.number(fvalue));
//            format.arg(fvalue);
            break;
        default:
            qDebug() << "default";
            break;
        }
        qDebug() << "s: " << s;
        format = format.arg(s);
    }
    return format;
}

QString OBD::makeRequest(QString req, OBD2_PID pid) {
//    QByteArray readData;
//    obdSerial.clear();
//    qint64 bytesWritten = obdSerial.write(req.toLocal8Bit());

//    if (bytesWritten == -1) {
//        qDebug() << QObject::tr("Failed to write the data to port %1, error: %2").arg(obdSerial.portName()).arg(obdSerial.errorString()) << endl;
//        return "";
//    } else if (bytesWritten != req.length()) {
//        qDebug() << QObject::tr("Failed to write all the data to port %1, error: %2").arg(obdSerial.portName()).arg(obdSerial.errorString()) << endl;
//        return "";
//    } else if (!obdSerial.waitForBytesWritten(5000)) {
//        qDebug() << QObject::tr("Operation timed out or an error occurred for port %1, error: %2").arg(obdSerial.portName()).arg(obdSerial.errorString()) << endl;
//        return "";
//    }

//    //    qDebug() << QObject::tr("Data successfully sent to port %1").arg(serialPortName) << endl;

//    //    obdSerial.writeData();
//    while (obdSerial.bytesAvailable() < pidToNumValues[pid]*3) {}
//    QString r;
//    while (!(r.endsWith("\n"))) {
//        QByteArray readData2 = obdSerial.readAll();
//        r.append(readData2);
//    }
//    QStringList sl = r.split("\n");
////    sl[0];
////    QByteArray readData = serialPort.readAll();
////    while (serialPort.waitForReadyRead(5000))
////        readData.append(serialPort.readAll());

////    obdSerial.re
////    obdSerial.readLine(&readData);
//    QString out = sl[0];//QString(readData);
//    return out;
    return "";
}

void OBD::parseResult1(QString str, int* a) {
    bool ok;
    str.replace(" ", "");
    QString str2 = str.right(1*2);
    qDebug() << "str2: " << str2;
    *a = str2.toInt(&ok, 16);
    qDebug() << "a: " << QString("%1").arg(*a) ;

}

void OBD::parseResult2(QString str, int* a, int* b) {
    bool ok;
    str.replace(" ", "");
    QString str2 = str.right(1*2);
    qDebug() << "str2: " << str2;
    *a = str2.mid(0, 2).toInt(&ok, 16);
    *b = str2.mid(2, 2).toInt(&ok, 16);
    qDebug() << "a: " << QString("%1").arg(*a) ;
    qDebug() << "b: " << QString("%1").arg(*b) ;
}

void OBD::parseResult3(QString str, int* a, int* b, int* c) {
    bool ok;
    str.replace(" ", "");
    QString str2 = str.right(1*2);
    qDebug() << "str2: " << str2;
    *a = str2.mid(0, 2).toInt(&ok, 16);
    *b = str2.mid(2, 2).toInt(&ok, 16);
    *c = str2.mid(4, 2).toInt(&ok, 16);
    qDebug() << "a: " << QString("%1").arg(*a) ;
    qDebug() << "b: " << QString("%1").arg(*b) ;
    qDebug() << "c: " << QString("%1").arg(*c) ;
}

void OBD::parseResult4(QString str, int* a, int* b, int* c, int* d) {
    bool ok;
    str.replace(" ", "");
    QString str2 = str.right(1*2);
    qDebug() << "str2: " << str2;
    *a = str2.mid(0, 2).toInt(&ok, 16);
    *b = str2.mid(2, 2).toInt(&ok, 16);
    *c = str2.mid(4, 2).toInt(&ok, 16);
    *d = str2.mid(6, 2).toInt(&ok, 16);
    qDebug() << "a: " << QString("%1").arg(*a) ;
    qDebug() << "b: " << QString("%1").arg(*b) ;
    qDebug() << "c: " << QString("%1").arg(*c) ;
    qDebug() << "d: " << QString("%1").arg(*d) ;
}

