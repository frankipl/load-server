#include "helper.h"
#include "zmienne.h"

Helper::Helper(): QObject() {

}
QString Helper::cap(QString s) {
        return (s.isEmpty()) ? s : s.left(1).toUpper() + s.mid(1).toLower();
}
QString Helper::hexFromChar(char c) {
    QString str;
    str.setNum(QChar(c).unicode(),16);
    if(str.count()<2) str.prepend("0");
    return str;
}
QString Helper::hexFromChar(QChar c) {
    QString str;
    str.setNum(c.unicode(),16);
    if(str.count()<2) str.prepend("0");
    return str;
}
QString Helper::printHex(QByteArray data) {
    QString msg;
    for (int i = 0; i <data.count(); i++) {
        QString hex;
        hex.setNum(QChar(data.at(i)).unicode(),16);
        if(hex.count()==1) hex.prepend("0");
        msg.append(hex).append(" ");
    }
    msg.chop(1);
    return msg;
}
QString Helper::getRandomString(int length) {

    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    const int randomStringLength = length; // assuming you want random strings of 12 characters

    QString randomString;
    for(int i=0; i<randomStringLength; ++i)
    {
        int index = qrand() % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}
QString Helper::getRandomString2(int length) {

    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    const int randomStringLength = length; // assuming you want random strings of length characters

    QString randomString;
    for(int i=0; i<randomStringLength; ++i)
    {
        int index = qrand() % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}
QString Helper::getRandomPin(int length) {

    const QString possibleCharacters("0123456789");
    const int randomStringLength = length; // assuming you want random strings of length characters

    QString randomString;
    for(int i=0; i<randomStringLength; ++i)
    {
        int index = qrand() % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}
int Helper::getIntFromRange(int from, int to) {
    to=to-from;
    int val=(qrand() % to)+from;
    return val;
  //  return QRandomGenerator::global()->bounded(from,to);
}
int Helper::getRandomValFromList(QList<int> list) {
    int pos=Helper::getIntFromRange(0,list.count()-1);
    return list.at(pos);
}
QDate Helper::getDateFromRange(QDate from, int max_days) {
    int val=(qrand() % max_days);
    return from.addDays(val);
  //  return from.addDays(QRandomGenerator::global()->bounded(6*30));
}
int Helper::getRandomFileDir() {
    int val = qrand() % 50;
    return val;
}
QList<int> Helper::getListIntFromStr(QString str) {
    QList<int> list;
    QStringList pieces = str.split( ",", QString::SkipEmptyParts);
    for (QString piece: pieces) {
        int v = piece.toInt();
        list.append(v);
    }
    return list;
}
QString Helper::getListIntStr(QList<int> list) {
    QString str;
    for (int v: list) {
        if (str.length() != 0) {
            str.append(",");
        }
        str.append(QString::number(v));
    }
    return str;
}
void Helper::appendUnique(QList<int> *dest, QList<int> source) {
    for(int i=0;i<source.count();i++) {
        if(!dest->contains(source.at(i)))
            dest->append(source.at(i));
    }
}
QString Helper::timePassed(qint64 from, qint64 to) {
    qint64 seconds=to-from;
    int hours=seconds/3600;
    int mins=(seconds-(hours*3600))/60;
    int secs=(seconds-(hours*3600)-mins*60);
    int days=hours/24;
    int hour=(hours-days*24);
    return QString("%1 days,%2 hours,%3 minutes,%4 seconds").arg(days).arg(hour).arg(mins).arg(secs);
}
QList<int> Helper::get_ob_main_funct() {
    QList<int> list;
    list<<0<<1<<2<<3<<4;
    return list;
}
QList<int> Helper::get_ob_funct() {
    QList<int> list;
    list<<0<<1<<2<<3<<4;
    return list;
}
QList<int> Helper::get_ob_act_funct() {
    QList<int> list;
    list<<0<<1<<2<<3<<4<<5<<6<<7<<8;
    return list;
}
QList<int> Helper::get_ob_modules() {
    QList<int> list;
    list<<0<<1<<2<<3<<4<<5<<6<<7;
    return list;
}
QString Helper::get_tns_status(int status) {
    switch(status) {
    case 0:
        return QString("Planning");
    case 1:
        return QString("Quote");
    case 2:
        return QString("Open");
    case 3:
        return QString("Completed");
    default:
        return QString("Unknown");
    }
}
QString Helper::get_task_type(int type) {
    switch(type) {
    case 0:
        return QString("Posting");
    case 1:
        return QString("Heading");
    case 2:
        return QString("Total");
    case 3:
        return QString("Begin_Total");
    case 4:
        return QString("End_Total");
    default:
        return QString("Unknown");
    }
}
int Helper::get_task_type_int(QString task) {
    if(task=="Posting") {
        return 0;
    } else if(task=="Heading") {
        return 1;
    } else if(task=="Total") {
        return 2;
    } else if(task=="Begin Total") {
        return 3;
    } else if(task=="End Total") {
        return 4;
    } else
        return 0;
}
int Helper::get_tns_status_int(QString status) {
    if(status==QString("Planning"))
        return 0;
    else if(status==QString("Quote"))
        return 1;
    else if(status==QString("Open"))
        return 2;
    else if(status==QString("Completed"))
        return 3;
    else
        0;
}
QString Helper::getDistanceStr(int distance) {
    double res=0.0;
    if(distance>10000) {
        res=qRound(distance/(double)1000);
        return QString("%1 km").arg(res).replace(".",",");
    }
    else if(distance<=10000 && distance>1000) {
        res=qRound((distance/(double)100))/(double)10;
        return QString("%1 km").arg(res).replace(".",",");
    }
    else if(distance<=1000) {
        return QString("%1 m").arg(distance);
    }
}

QString Helper::toBase64(QString string){
    QByteArray ba;
    ba.append(string.toUtf8());
    return ba.toBase64();
}
QString Helper::createSessionId() {
    QString s;
    for(int i=0;i<4;i++) {
        s.append(Helper::getRandomString(6));
        if(i!=3)
            s.append("-");
    }
    return s;
}

QString Helper::fromBase64(QString string){
    QByteArray ba;
    ba.append(string.toUtf8());
    return QByteArray::fromBase64(ba);
}
QJsonObject Helper::getWebErrJsonObj(WebErrType err_type, QString err_string) {
    QJsonObject obj;
    obj.insert("error",1);
    obj.insert("web_err_type",err_type);
    obj.insert("string",err_string);
    return obj;
}
QJsonArray Helper::getWebErrJsonArr(WebErrType err_type, QString err_string) {
    QJsonArray arr;
    arr.append(Helper::getWebErrJsonObj(err_type,err_string));
    return arr;
}
double Helper::getRandomLat() {
    QString num=QString("52.%1").arg(Helper::getRandomPin(6));
    return num.toDouble();
}
double Helper::getRandomLon() {
    QString num=QString("21.%1").arg(Helper::getRandomPin(6));
    return num.toDouble();
}
Helper::~Helper() {

}
