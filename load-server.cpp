#include "load-server.h"
#include "zmienne.h"

LoadServer::LoadServer() {
    Zm().loadServerObj= (void*)this;
    signal(SIGTERM,terminate_wrapper);
    signal(SIGHUP,reload_wrapper);

    settings = new QSettings(Zm().configPath+Zm().configFile, QSettings::IniFormat);
    qDebug()<<"path:"<<Zm().configPath<<" file:"<<Zm().configFile;

    this->lockFile();
    this->setVariables();
    this->openLogFile();

    qsrand ( QDateTime::currentDateTime().toMSecsSinceEpoch() );

    QStringList list=settings->allKeys();
    qDebug()<<"9 LoadServer::LoadServer printing all settings values";
    for(int i=0;i<list.count();i++) {
        qDebug()<<"9 LoadServer::LoadServer config key:"<<list.at(i)<<" val:"<<settings->value(list.at(i));
    }

    Sql::dbhost=settings->value("dbhost").toString();
    Sql::dbport=settings->value("dbport").toInt();
    Sql::dbuser=settings->value("dbuser").toString();
    Sql::dbpass=settings->value("dbpass").toString();
    Sql::dbname=settings->value("dbname").toString();
    sql=new Sql("main");
    try {
        if(Sql::dbname.isEmpty())
            throw Err("Database name not set, aborting");
        sql->connectDb();
    } catch(Err &e) {
        qWarning()<<QString("LoadServer::LoadServer SQL connection failed:%1").arg(e.getMsg());
        this->terminate();
    }
//    this->convertDb();
//    return;


    localSocket=new LocalSocket(this);

    clientMgr = new ClientMgr(this, sql);
    appNetManager=new AppNetManager(this);

    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");

    //from appNetManager

    connect(appNetManager,SIGNAL(newClientReq(int,int,QJsonObject)),clientMgr,SLOT(newClientReq(int,int,QJsonObject)));
    connect(appNetManager,SIGNAL(fileAdded(int,int,QString,int)),clientMgr,SLOT(fileAdded(int,int,QString,int)));


    //to appNetManager
    connect(clientMgr,SIGNAL(setUserId(int,int)),appNetManager,SLOT(setUserId(int,int)));
    connect(clientMgr,SIGNAL(sendToApp(int,QJsonDocument)),appNetManager,SLOT(sendToClient(int,QJsonDocument)));
    connect(clientMgr,SIGNAL(sendToAll(QJsonDocument)),appNetManager,SLOT(sendToAll(QJsonDocument)));

    appNetManager->init();

    //localSocket->init();

    clientMgr->init();

    workingTimer=new QTimer(this);
    workingTimer->setSingleShot(false);
    workingTimer->setInterval(60*60*1000);
    connect(workingTimer,SIGNAL(timeout()),this,SLOT(workTimerTimeout()));
    startDate=QDateTime::currentDateTime();
    workingTimer->start();

    testTimer=new QTimer(this);
    testTimer->setSingleShot(true);
    testTimer->setInterval(5000);
    connect(testTimer,SIGNAL(timeout()),this,SLOT(testTimerTimeout()));
  //  testTimer->start();

    //smtpMgr->setDisabled(true);
  //  this->fillInDatabase();
}

void LoadServer::testTimerTimeout() {

}
void LoadServer::initializeUserApp(int client_id,int user_id) {
    qDebug()<<"LoadServer::initializeUserApp client_id:"<<client_id<<" user_id:"<<user_id;
//    int acc_id=1; // testowo główne ID konta
//    QList<int> ob_list; // testowo, nie ma teraz obiektu z którego można to pobrać
//    reportMgr->getReportConfig(client_id,user_id,acc_id);
//    documentMgr->getDocumentConfig(client_id,ob_list);

//    emit sendAllDataToClient(client_id, user_id);

   // clientMgr->sendAllClients(client_id, user_id);


    this->settingsCompleted(client_id, user_id);
}
void LoadServer::settingsCompleted(int client_id, int user_id) {
    QJsonObject obj;
    obj.insert("module","app");
    obj.insert("req","settings_completed");
    emit sendToApp(client_id,QJsonDocument(obj));
}
void LoadServer::setVariables() {
    Zm().logFileName=settings->value("logFileName").toString();
    Zm().appListenIp=settings->value("appListenIp").toString();
    Zm().appListenPort=settings->value("appListenPort").toInt();
    Zm().webSocketListenIp=settings->value("webSocketListenIp").toString();
    Zm().webSocketListenPort=settings->value("webSocketListenPort").toInt();
    Zm().logging=settings->value("logging").toInt();
    Zm().logLevel=settings->value("logLevel").toInt();
    Zm().email_server=settings->value("email_server").toString();
    Zm().email_login=settings->value("email_login").toString();
    Zm().email_pass=settings->value("email_pass").toString();
    Zm().email_auth=settings->value("email_auth").toInt();
    Zm().email_port=settings->value("email_port").toInt();
    Zm().email_ssl=settings->value("email_ssl").toInt();
    Zm().email_from_address=settings->value("email_from_address").toString();
    Zm().email_from_name=settings->value("email_from_name").toString();
}
void LoadServer::openLogFile() {
    if(!Zm().logging) {
        if(Zm().logFile.isOpen()) {
            QString msg("Debug: %1 Log closed\n");
            msg=msg.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            Zm().logFile.write(msg.toLatin1().data());
            Zm().logFile.close();
        }
        return;
    }
    if(Zm().logFileName.isEmpty())
        return;
    if(Zm().logFile.isOpen()) {
        Zm().logFile.close();
    }
    Zm().logFile.setFileName(Zm().logFileName);
    Zm().logFile.open(QIODevice::Append | QIODevice::Text | QIODevice::Unbuffered);
    QString msg("Debug: %1 Log opened\n");
    msg=msg.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    Zm().logFile.write(msg.toLatin1().data());
}
void LoadServer::lockFile() {
    char str[10];
    int lfp=open(Zm().lockFile.toLatin1(),O_RDWR|O_CREAT,0640);
    if (lfp<0) {
        qCritical()<<"Can't open lock file "<<Zm().lockFile<<": "<<strerror(errno); /* can not open */
        exit(1);
    }
    if (lockf(lfp,F_TLOCK,0)<0) {
        qCritical()<<"Can't create lock file "<<Zm().lockFile<<": "<<strerror(errno); /* can not lock */
        exit(1);
    }
    QCoreApplication::applicationPid();
    sprintf(str,"%d\n",QCoreApplication::applicationPid());
    write(lfp,str,strlen(str));
}

void LoadServer::terminate(bool fromSignal) {
    qDebug()<<"1 LoadServer::terminate from signal:"<<fromSignal;
    localSocket->close();
    qApp->quit();
}
void LoadServer::setServerLogging(int logging) {
    settings->setValue("logging",logging);
    this->reload();
}
void LoadServer::setServerLogLevel(int logLevel) {
    settings->setValue("logLevel",logLevel);
    Zm().logLevel=logLevel;
}
void LoadServer::reload() {
    settings->sync();
    Zm().logging=settings->value("logging").toInt();
    Zm().logLevel=settings->value("logLevel").toInt();
    this->openLogFile();
}
void LoadServer::debugMsg(QString msg, int code) {
    switch(code) {
    case 0:
        qDebug()<<msg;
        break;
    case 1:
        qWarning()<<msg;
        break;
    case 2:
        qCritical()<<msg;
        break;
    default:
        qWarning()<<"LoadServer::debugMsg unknown code:"<<code<<" msg:"<<msg;
    }
}
void LoadServer::workTimerTimeout() {
    QDateTime now=QDateTime::currentDateTime();
    qDebug()<<"working: "<<Helper::timePassed(startDate.toMSecsSinceEpoch()/1000,now.toMSecsSinceEpoch()/1000);
}
void LoadServer::fillInDatabase() {
    qDebug()<<"LoadServer::fillInDatabase started";
    try {

        for(int i=0;i<10000;i++) {
            sql->q.prepare("insert into client (cl_surname,cl_name1,cl_name2,cl_companyname,cl_nip,cl_idcard,cl_pesel,cl_email,cl_phone1,cl_phone2,cl_draft,co_id,cl_login,cl_pass) values (?,?,?,?,?,?,?,?,?,?,?,?,?,?) returning cl_id");
            sql->q.bindValue(0,QString("Kowalski%1").arg(i));
            sql->q.bindValue(1,QString("Jan%1").arg(i));
            sql->q.bindValue(2,QString("Maria%1").arg(i));
            sql->q.bindValue(3,QString("Nazwa firmy%1").arg(i));
            sql->q.bindValue(4,Helper::getRandomPin(10));
            sql->q.bindValue(5,QString("%1%2").arg(Helper::getRandomString2(3)).arg(Helper::getRandomPin(6)));
            sql->q.bindValue(6,Helper::getRandomPin(11));
            sql->q.bindValue(7,QString("jan%1@kowalski%2.com").arg(i).arg(i));
            sql->q.bindValue(8,Helper::getRandomPin(9));
            sql->q.bindValue(9,Helper::getRandomPin(9));
            sql->q.bindValue(10,false);
            sql->q.bindValue(11,1);
            sql->q.bindValue(12,QString("login%1").arg(i));
            sql->q.bindValue(13,QString("pass%1").arg(i));
            sql->my_exec();

            sql->q.next();
            int cl_id=sql->q.value(0).toInt();
            for(int k=0;k<10;k++) {
                sql->q.prepare("insert into client_record (cr_value,cr_date,cr_point,cr_name,cl_id) values (?,?,?,?,?)");
                sql->q.bindValue(0,Helper::getIntFromRange(100,1000));
                sql->q.bindValue(1,QDateTime::currentDateTime());
                sql->q.bindValue(2,Helper::getIntFromRange(100,1000));
                sql->q.bindValue(3,Helper::getRandomString(10));
                sql->q.bindValue(4,cl_id);
                sql->my_exec();
            }
        }
    }
    catch(Err &err) {
        qCritical()<<"LoadServer::fillInDatabase sql err:"<<err.getMsg();
        exit(1);
    }
    qDebug()<<"LoadServer::fillInDatabase done";
}

void terminate_wrapper(int dummy) {
    qDebug()<<"1 closing with signal SIGTERM";
    LoadServer *loadServer=(LoadServer*)Zm().loadServerObj;
    loadServer->terminate(true);
}
void reload_wrapper(int dummy) {
    qDebug()<<"1 reload from SIGHUP";
    LoadServer *loadServer=(LoadServer*)Zm().loadServerObj;
    loadServer->reload();
}
