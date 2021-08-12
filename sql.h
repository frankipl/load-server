#ifndef SQL_H
#define SQL_H

#include <QtSql>
#include <QObject>
#include <QSqlDatabase>
#include "error.h"

class Sql: public QObject
{
    Q_OBJECT
public:
    Sql(QString id);
    QSqlDatabase db;
    QSqlQuery q;
    QSqlQuery q1;
    QSqlQuery rq;
    QSqlQuery tq;
    static QString dbhost;
    static int dbport;
    static QString dbuser;
    static QString dbpass;
    static QString dbname;
public slots:
 //   void setHost(QString host,int port);
 //   void setDb(QString db);
//    void setUser(QString user,QString pass);
    void connectDb();
    void my_exec(bool rollback=false);
    void my_execq1(bool rollback=false);
    void rq_exec(bool rollback=false);
    void tq_exec(bool rollback=false);
    void close();
    QString error();
signals:
private:
    QString conn_id;
};

#endif // SQL_H
