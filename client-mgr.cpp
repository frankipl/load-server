#include "client-mgr.h"
#include "zmienne.h"
#include "app-net-manager.h"

ClientMgr::ClientMgr(QObject* parent, Sql* sql) : QObject(parent)
{
    this->sql = sql;
}

QJsonObject ClientMgr::getClientJSon(QSharedPointer<ClientDataStruct> p) {
    QJsonObject cl_obj;

    cl_obj.insert("cl_id",p->cl_id);
    cl_obj.insert("cl_companyname",p->cl_companyname);
    cl_obj.insert("cl_nip",p->cl_nip);
    cl_obj.insert("cl_email",p->cl_email);
    cl_obj.insert("cl_idcard",p->cl_idcard);
    cl_obj.insert("cl_name1",p->cl_name1);
    cl_obj.insert("cl_name2",p->cl_name2);
    cl_obj.insert("cl_pesel",p->cl_pesel);
    cl_obj.insert("cl_phone1",p->cl_phone1);
    cl_obj.insert("cl_phone2",p->cl_phone2);
    cl_obj.insert("cl_surname",p->cl_surname);
    cl_obj.insert("cl_draft",p->cl_draft);
    cl_obj.insert("cl_login",p->cl_login);
    cl_obj.insert("cl_pass",p->cl_pass);
    cl_obj.insert("cl_updated",p->cl_updated.toMSecsSinceEpoch()/1000);

//    QJsonArray rec_arr;
//    QMapIterator<int,QSharedPointer<RecordDataStruct>> a(p->recordMap);
//    while(a.hasNext()) {
//        a.next();
//        QJsonObject rec_obj=this->getRecordJson(a.value());
//        rec_arr.append(rec_obj);
//    }
//    cl_obj.insert("client_record",rec_arr);
    return cl_obj;
}

QJsonObject ClientMgr::getRecordJson(QSharedPointer<RecordDataStruct> p) {
    QJsonObject cl_obj;

    cl_obj.insert("cr_id",p->cr_id);
    cl_obj.insert("cr_value",p->cr_value);
    cl_obj.insert("cr_date",p->cr_date.toMSecsSinceEpoch()/1000);
    cl_obj.insert("cr_point",p->cr_point);
    cl_obj.insert("cr_name",p->cr_name);

    return cl_obj;
}
QJsonObject ClientMgr::getFileJson(QSharedPointer<FileDataStruct> p) {
    QJsonObject cl_obj;

    cl_obj.insert("f_id",p->f_id);
    cl_obj.insert("f_filename",p->f_filename);
    cl_obj.insert("f_timestamp",p->f_timestamp.toMSecsSinceEpoch()/1000);

    return cl_obj;
}

void ClientMgr::init() {
    this->loadClientFromSql();
    qDebug()<<"ClientMgr::init clientMap.count:"<<clientMap.count();
}

void ClientMgr::newClientReq(int net_client_id,int user_id,QJsonObject obj) {
    QString req=obj.value("req").toString();
    if (req=="login"){
        this->loginReq(net_client_id, user_id,obj.value("data").toObject());
    }
    else if (req=="get_client") {
        this->getClient( net_client_id, user_id, obj.value("data").toObject());
    }
    else if (req=="put_client") {
        this->putClient( net_client_id, user_id, obj.value("data").toObject());
    }
    else if (req=="post_client") {
        this->postClient( net_client_id, user_id, obj.value("data").toObject());
    }
    else {
        qDebug() << "ClientMgr::newReq unkown req = " << req;
    }
}
void ClientMgr::fileAdded(int net_client_id, int user_id, QString filename, int timestamp) {
    QSharedPointer<ClientDataStruct> p=this->getClientById(user_id,true);
    if(p) {
        FileDataStruct file;
        file.f_filename=filename;
        file.f_timestamp=QDateTime::fromMSecsSinceEpoch(timestamp/1000);
        QString error;
        bool updated=this->addFileInSql(file,p->cl_id,&error);
        if(updated) {
            QSharedPointer<FileDataStruct> fp = QSharedPointer<FileDataStruct>{new FileDataStruct {file}};
            p->fileMap.insert(fp->f_id,fp);
            this->sendFileResponse(net_client_id,updated,error);
        }
        else {
            this->sendErrorResponse(net_client_id,QString("failed to add record for cl_id:%1").arg(user_id));
        }
    }
    else {
        this->sendErrorResponse(net_client_id,QString("client cl_id:%1 not found").arg(user_id));
    }
}
void ClientMgr::postClient(int net_client_id, int user_id, QJsonObject data) {
    QSharedPointer<ClientDataStruct> p=this->getClientById(user_id,true);
    if(p) {

        RecordDataStruct record;
        record.cr_date=QDateTime::fromMSecsSinceEpoch(data.value("cl_date").toInt()/1000);
        record.cr_name=data.value("cr_name").toString();
        record.cr_point=data.value("cr_point").toInt();
        record.cr_value=data.value("cr_value").toInt();

        QString error;
        bool updated=this->addRecordInSql(record,p->cl_id,&error);
        if(updated) {
            QSharedPointer<RecordDataStruct> cr = QSharedPointer<RecordDataStruct>{new RecordDataStruct {record}};
            p->recordMap.insert(cr->cr_id,cr);
            this->sendPostResponse(net_client_id,updated,error);
        }
        else {
            this->sendErrorResponse(net_client_id,QString("failed to add record for cl_id:%1").arg(user_id));
        }
    }
    else {
        this->sendErrorResponse(net_client_id,QString("client cl_id:%1 not found").arg(user_id));
    }
}
void ClientMgr::putClient(int net_client_id, int user_id, QJsonObject data) {
    QSharedPointer<ClientDataStruct> p=this->getClientById(user_id,true);
    if(p) {

        ClientDataStruct cl { *p };
        cl.cl_companyname=(data.contains("cl_companyname") ? data.value("cl_companyname").toString() : cl.cl_companyname);
        cl.cl_nip=(data.contains("cl_nip") ? data.value("cl_nip").toString() : cl.cl_nip);
        cl.cl_email=(data.contains("cl_email") ? data.value("cl_email").toString() :cl.cl_email);
        cl.cl_idcard=(data.contains("cl_idcard") ? data.value("cl_idcard").toString() :cl.cl_idcard);
        cl.cl_name1=(data.contains("cl_name1") ? data.value("cl_name1").toString() :cl.cl_name1);
        cl.cl_name2=(data.contains("cl_name2") ? data.value("cl_name2").toString() :cl.cl_name2);
        cl.cl_pesel=(data.contains("cl_pesel") ? data.value("cl_pesel").toString() : cl.cl_pesel);
        cl.cl_phone1=(data.contains("cl_phone1") ? data.value("cl_phone1").toString() :cl.cl_phone1);
        cl.cl_phone2=(data.contains("cl_phone2") ? data.value("cl_phone2").toString() :cl.cl_phone2);
        cl.cl_surname=(data.contains("cl_surname") ? data.value("cl_surname").toString() :cl.cl_surname);
        cl.cl_draft=(data.contains("cl_draft") ? data.value("cl_draft").toInt() : cl.cl_draft);
        cl.cl_updated=QDateTime::currentDateTime();

        QString error;
        bool updated=this->updateClientInSql(p,cl,&error);
        if(updated) {
            this->sendUpdateResponse(net_client_id,updated,error);
        }
        else {
            this->sendErrorResponse(net_client_id,QString("failed to update cl_id:%1").arg(cl.cl_id));
        }

    }
    else {
        this->sendErrorResponse(net_client_id,QString("client cl_id:%1 not found").arg(user_id));
    }
}
void ClientMgr::getClient(int net_client_id, int user_id, QJsonObject data) {
    QSharedPointer<ClientDataStruct> p=this->getClientById(user_id,true);
    if(p) {
        QJsonObject resp_obj;
        resp_obj.insert("module","client");
        resp_obj.insert("req","get_client");
        QJsonObject data;
        QJsonObject cl_obj=this->getClientJSon(p);
        data.insert("client",cl_obj);
        QJsonArray rec_arr;
        QMapIterator<int,QSharedPointer<RecordDataStruct>> i(p->recordMap);
        i.toBack();
        int k=0;
        while(i.hasPrevious()) {
            i.previous();
            rec_arr.append(this->getRecordJson(i.value()));
            if(k++>5)
                break;
        }
        data.insert("record",rec_arr);
        QJsonArray file_arr;
        QMapIterator<int,QSharedPointer<FileDataStruct>> i2(p->fileMap);
        i2.toBack();
        k=0;
        while(i2.hasPrevious()) {
            i2.previous();
            file_arr.append(this->getFileJson(i2.value()));
            if(k++>5)
                break;
        }
        data.insert("file",file_arr);
        resp_obj.insert("data",data);
        emit sendToApp(net_client_id,QJsonDocument(resp_obj));
    }
    else {
        this->sendErrorResponse(net_client_id,QString("client cl_id:%1 not found").arg(user_id));
    }
}
void ClientMgr::loginReq(int net_client_id, int user_id, QJsonObject data) {
    QString cl_login=data.value("cl_login").toString();
    QString cl_pass=data.value("cl_pass").toString();
    QSharedPointer<ClientDataStruct> p=this->isUserValid(cl_login,cl_pass);
    if(p) {
        emit setUserId(net_client_id,p->cl_id);
    }
    this->sendLoginResponse(net_client_id,p);
}
QSharedPointer<ClientDataStruct> ClientMgr::isUserValid(QString cl_login,QString cl_pass) {
    QMapIterator<int,QSharedPointer<ClientDataStruct>> i(clientMap);
    while(i.hasNext()) {
        i.next();
        if(i.value()->cl_login==cl_login && i.value()->cl_pass==cl_pass)
            return i.value();
    }
    return QSharedPointer<ClientDataStruct>();
}
void ClientMgr::sendLoginResponse(int net_client_id, QSharedPointer<ClientDataStruct> p) {
    QJsonObject obj;
    obj.insert("module","client");
    obj.insert("req","login");
    QJsonObject data;
    data.insert("authenticated",(p ? 1 : 0));
    data.insert("cl_id",(p ? p->cl_id : 0));
    data.insert("client",this->getClientJSon(p));
    obj.insert("data",data);
    emit sendToApp(net_client_id,QJsonDocument(obj));
}
void ClientMgr::sendUpdateResponse(int net_client_id, bool updated, QString error) {
    QJsonObject obj;
    obj.insert("module","client");
    obj.insert("req","put_client");
    QJsonObject data;
    data.insert("updated",updated);
    data.insert("error",error);
    obj.insert("data",data);
    emit sendToApp(net_client_id,QJsonDocument(obj));
}
void ClientMgr::sendPostResponse(int net_client_id, bool updated, QString error) {
    QJsonObject obj;
    obj.insert("module","client");
    obj.insert("req","post_client");
    QJsonObject data;
    data.insert("updated",updated);
    data.insert("error",error);
    obj.insert("data",data);
    emit sendToApp(net_client_id,QJsonDocument(obj));
}
void ClientMgr::sendFileResponse(int net_client_id, bool updated, QString error) {
    QJsonObject obj;
    obj.insert("module","client");
    obj.insert("req","file_client");
    QJsonObject data;
    data.insert("updated",updated);
    data.insert("error",error);
    obj.insert("data",data);
    emit sendToApp(net_client_id,QJsonDocument(obj));
}
void ClientMgr::sendErrorResponse(int net_client_id, QString error) {
    QJsonObject obj;
    obj.insert("module","client");
    obj.insert("req","error_client");
    obj.insert("error",error);
    emit sendToApp(net_client_id,QJsonDocument(obj));
}

// user is needed for previlages checks
//void ClientMgr::sendAllDataToClient(int net_client_id, int user_id){
//    sendAllClients(net_client_id);
//    sendAllAddresses(net_client_id);
//}


void ClientMgr::loadClientFromSql() {
    try {
        sql->q.prepare("select cl_id, cl_surname, cl_name1, cl_name2, cl_companyname, cl_nip, cl_idcard, cl_pesel, cl_email, cl_phone1, cl_phone2, cl_draft, co_id,cl_login,cl_pass,cl_updated from client");
        sql->my_exec();
        while(sql->q.next()) {
            QSharedPointer<ClientDataStruct> p = QSharedPointer<ClientDataStruct>{new ClientDataStruct};
            p->cl_id =          sql->q.value(0).toInt();
            p->cl_surname =     sql->q.value(1).toString();
            p->cl_name1 =       sql->q.value(2).toString();
            p->cl_name2 =       sql->q.value(3).toString();
            p->cl_companyname = sql->q.value(4).toString();
            p->cl_nip =         sql->q.value(5).toString();
            p->cl_idcard =      sql->q.value(6).toString();
            p->cl_pesel =       sql->q.value(7).toString();
            p->cl_email =       sql->q.value(8).toString();
            p->cl_phone1 =      sql->q.value(9).toString();
            p->cl_phone2 =      sql->q.value(10).toString();
            p->cl_draft =       sql->q.value(11).toInt();
            p->co_id =          sql->q.value(12).toInt();
            p->cl_login=        sql->q.value(13).toString();
            p->cl_pass=         sql->q.value(14).toString();
            p->cl_updated=      sql->q.value(15).toDateTime();

            clientMap.insert(p->cl_id,p);

            this->loadRecordFromSql(p);
            this->loadFileFromSql(p);

        }
    }
    catch(Err &err) {
        qCritical()<<"ClientMgr::loadFromSql sql err:"<<err.getMsg();
        return;
    }
}

void ClientMgr::loadRecordFromSql( QSharedPointer<ClientDataStruct> client ) {
    try {
        sql->q1.prepare("select  cr_id,cr_value,cr_date,cr_point,cr_name from client_record where cl_id=?");
        sql->q1.bindValue(0, client->cl_id);
        sql->my_execq1();
        while(sql->q1.next()) {
           // qDebug()<<"next record";
            QSharedPointer<RecordDataStruct> p = QSharedPointer<RecordDataStruct>{new RecordDataStruct};
            p->cr_id = sql->q1.value(0).toInt();
            p->cr_value = sql->q1.value(1).toInt();
            p->cr_date = sql->q1.value(2).toDateTime();
            p->cr_point =  sql->q1.value(3).toInt();
            p->cr_name = sql->q1.value(4).toString();

            client->recordMap.insert(p->cr_id,p);


        }
    }
    catch(Err &err) {
        qCritical()<<"ClientMgr::loadRecordFromSql sql err:"<<err.getMsg();
        return;
    }
}
void ClientMgr::loadFileFromSql( QSharedPointer<ClientDataStruct> client ) {
    try {
        sql->q1.prepare("select f_id,f_filename,f_timestamp from client_file where cl_id=?");
        sql->q1.bindValue(0, client->cl_id);
        sql->my_execq1();
        while(sql->q1.next()) {
           // qDebug()<<"next record";
            QSharedPointer<FileDataStruct> p = QSharedPointer<FileDataStruct>{new FileDataStruct};
            p->f_id = sql->q1.value(0).toInt();
            p->f_filename = sql->q1.value(1).toString();
            p->f_timestamp = sql->q1.value(2).toDateTime();

            client->fileMap.insert(p->f_id,p);


        }
    }
    catch(Err &err) {
        qCritical()<<"ClientMgr::loadFileFromSql sql err:"<<err.getMsg();
        return;
    }
}
bool ClientMgr::updateClientInSql(QSharedPointer<ClientDataStruct> p, ClientDataStruct &client,QString *error) {
    try {
        sql->q.prepare("update client set cl_surname=?, cl_name1=?, cl_name2=?,cl_companyname=?, cl_nip=?, cl_idcard=?, cl_pesel=?, cl_email=?, cl_phone1=?, cl_phone2=?, cl_draft=?, co_id=?,cl_updated=? where cl_id=?");
        sql->q.bindValue(0, client.cl_surname);
        sql->q.bindValue(1, client.cl_name1);
        sql->q.bindValue(2, client.cl_name2);
        sql->q.bindValue(3, client.cl_companyname);
        sql->q.bindValue(4, client.cl_nip);
        sql->q.bindValue(5, client.cl_idcard);
        sql->q.bindValue(6, client.cl_pesel);
        sql->q.bindValue(7, client.cl_email);
        sql->q.bindValue(8, client.cl_phone1);
        sql->q.bindValue(9, client.cl_phone2);
        sql->q.bindValue(10, client.cl_draft);
        sql->q.bindValue(11, client.co_id);
        sql->q.bindValue(12, client.cl_updated);
        sql->q.bindValue(13, client.cl_id);
        sql->my_exec(true);

        *p=client;

        return true;
    }
    catch(Err &err) {
        qCritical()<<"ClientMgr::updateClientInSql sql err:"<<err.getMsg();
        *error=err.getMsg();
        return false;
    }
}

bool ClientMgr::addRecordInSql(RecordDataStruct& record,int cl_id,QString *error) {
    try {
        sql->q.prepare("insert into client_record ( cr_value, cr_date, cr_point, cr_name, cl_id ) values (?,?,?,?,?) returning cr_id");
        sql->q.bindValue(0, record.cr_value);
        sql->q.bindValue(1, record.cr_date);
        sql->q.bindValue(2, record.cr_point);
        sql->q.bindValue(3, record.cr_name);
        sql->q.bindValue(4, cl_id);

        sql->my_exec(true);
        sql->q.next();

        record.cr_id = sql->q.value(0).toInt();
        return true;
    }
    catch(Err &err) {
        qCritical()<<"ClientMgr::addRecordInSql sql err:"<<err.getMsg();
        *error=err.getMsg();
        return false;
    }
}
bool ClientMgr::addFileInSql(FileDataStruct& file,int cl_id,QString *error) {
    try {
        sql->q.prepare("insert into client_file ( f_filename, f_timestamp, cl_id ) values (?,?,?) returning f_id");
        sql->q.bindValue(0, file.f_filename);
        sql->q.bindValue(1, file.f_timestamp);
        sql->q.bindValue(2, cl_id);

        sql->my_exec(true);
        sql->q.next();

        file.f_id = sql->q.value(0).toInt();
        return true;
    }
    catch(Err &err) {
        qCritical()<<"ClientMgr::addFileInSql sql err:"<<err.getMsg();
        *error=err.getMsg();
        return false;
    }
}
//void ClientMgr::sendToAll(QDomDocument xml, QList<int> list) {
//    for(int i=0;i<list.count();i++) {
//        emit sendToApp(list.at(i),xml);
//    }
//}



int ClientMgr::getMapSize() {
    return clientMap.count();
}
QSharedPointer<ClientDataStruct> ClientMgr::getClientById(int cl_id,bool report_missing) {
    if(!clientMap.contains(cl_id)) {
        if(report_missing)
            qWarning()<<"ClientMgr::getClientById there is no cl_id:"<<cl_id;
        return QSharedPointer<ClientDataStruct>();
    }
    return clientMap[cl_id];
}
