#include "sql.h"
#include "error.h"

QString Sql::dbhost="";
int Sql::dbport=0;
QString Sql::dbuser="";
QString Sql::dbpass="";
QString Sql::dbname="";

Sql::Sql(QString id) {
    conn_id=id;
    qDebug()<<"5 Sql::Sql available drivers:"<<QSqlDatabase::drivers();
    db = QSqlDatabase::addDatabase("QPSQL",conn_id);

    if(dbhost!="localhost")
        db.setHostName(dbhost);
    if(Sql::dbport!=0)
        db.setPort(Sql::dbport);
    db.setDatabaseName(Sql::dbname);
    db.setUserName(Sql::dbuser);
    if(!Sql::dbpass.isEmpty())
        db.setPassword(Sql::dbpass);
}
void Sql::connectDb() {
    bool ok = db.open();
    if(!ok) {
        qDebug() << "5 Sql::connectDb conn_id:"<<conn_id<<" FAILED";
        throw Err("conn_id:"+conn_id+" "+db.lastError().text());
        return;
    }
    else
        qDebug() << "2 Sql::connectDb conn_id:"<<conn_id<<" opened";
    q=QSqlQuery(db);
    q1=QSqlQuery(db);
    rq=QSqlQuery(db);
    tq=QSqlQuery(db);
    //      this->dumpObjectTree();
    //      QObject::dumpObjectInfo();
    //      showErr("testowy komu");
}

void Sql::my_execq1(bool rollback) {
    if(!db.isOpen())
        db.open();

    if(db.isOpen())
        q1.exec();

    if(q1.lastError().isValid()) {
        //      qDebug() << "sql error:"+q1.lastError().text();
        //     qDebug() << "sql error:"+q1.lastError().text();
        QString a=q1.lastError().text();
        if(rollback)
            q1.exec("rollback");
        throw Err(a);
    }

}
void Sql::my_exec(bool rollback) {
    if(!db.isOpen())
        db.open();
    if(db.isOpen())
        q.exec();

    if(q.lastError().isValid()) {
        //      qDebug() << "sql error:"<<q.lastError();
        //      qDebug() << "sql error:"<<q.lastError().driverText();
        //      qDebug() << "sql error:"<<q.lastError().databaseText();
        QString a=q.lastError().text();
        if(rollback)
            q.exec("rollback");
        throw Err(a);
    }
}
void Sql::rq_exec(bool rollback) {
    if(!db.isOpen())
        db.open();
    if(db.isOpen())
        rq.exec();

    if(rq.lastError().isValid()) {
        //       qDebug() << "sql error:"<<rq.lastError();
        //       qDebug() << "sql error:"<<rq.lastError().driverText();
        //       qDebug() << "sql error:"<<rq.lastError().databaseText();
        QString a=rq.lastError().text();
        if(rollback)
            rq.exec("rollback");
        throw Err(a);
    }
}
void Sql::tq_exec(bool rollback) {
    if(!db.isOpen())
        db.open();
    if(db.isOpen())
        tq.exec();

    if(tq.lastError().isValid()) {
        //       qDebug() << "sql error:"<<rq.lastError();
        //       qDebug() << "sql error:"<<rq.lastError().driverText();
        //       qDebug() << "sql error:"<<rq.lastError().databaseText();
        QString a=tq.lastError().text();
        if(rollback)
            tq.exec("rollback");
        throw Err(a);
    }
}
QString Sql::error() {
    if(db.lastError().isValid())
        return db.lastError().text();
    return QString();
}

void Sql::close() {
    if(db.isOpen()) {
        if(!db.rollback()) {
            qCritical()<<"Sql::close rollback returened false error:"<<db.lastError().text();
        }
        db.close();
    }
}
