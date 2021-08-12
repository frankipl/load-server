#ifndef HELPER_H
#define HELPER_H
#include <QObject>
#include <QStringList>
#include "zmienne.h"

enum FileAttachType {FileAttachUnknown=0,FileAttachPicture=1};

enum ReportTypeEnum {ReportUnknownType=0,ReportTypeEurowindActivity=1,ReportTypeBmsActivity=2};
enum ReportDateSign {ReportDateUnknown=0,ReportDateBefore=1,ReportDateEquealOrBefore=2,ReportDateEqueal=3,ReportDateEquealOrAfter=4,
                     ReportDateAfter=5};

class Helper: public QObject
{
	Q_OBJECT
public:
	Helper ();
	~Helper();
    static QString cap(QString s);
    static QString hexFromChar(char c);
    static QString hexFromChar(QChar c);
    static QString printHex(QByteArray data);
    static QString getRandomString(int length);
    static QString getRandomString2(int length);
    static QString getRandomPin(int length);
    static int getIntFromRange(int from,int to);
    static int getRandomValFromList(QList<int> list);
    static QDate getDateFromRange(QDate from,int max_days);
    static int getRandomFileDir();
    static QList<int> getListIntFromStr(QString str);
    static QString getListIntStr(QList<int> list);
    static void appendUnique(QList<int> *dest, QList<int> source);
    static QString timePassed(qint64 from,qint64 to);
    static QList<int> get_ob_main_funct();
    static QList<int> get_ob_funct();
    static QList<int> get_ob_modules();
    static QList<int> get_ob_act_funct();
    static QString get_tns_status(int status);
    static QString get_task_type(int type);
    static int get_task_type_int(QString task);
    static int get_tns_status_int(QString status);
    static QString getDistanceStr(int distance);
    static QString toBase64 (QString string);
    static QString fromBase64 (QString string);
    static QString createSessionId();
    static QJsonObject getWebErrJsonObj(WebErrType err_type,QString err_string);
    static QJsonArray getWebErrJsonArr(WebErrType err_type,QString err_string);
    static double getRandomLat();
    static double getRandomLon();
};
#endif // HELPER_H
