#ifndef CLIENTMGR_H
#define CLIENTMGR_H

#include <QtCore>
#include <QObject>
#include "sql.h"
#include "helper.h"


struct RecordDataStruct {
    int     cr_id;
    int     cr_value;
    QDateTime cr_date;
    int     cr_point;
    QString cr_name;
};
struct FileDataStruct {
    int     f_id;
    QString f_filename;
    QDateTime f_timestamp;
};

struct ClientDataStruct {
    int     cl_id;
    QString cl_surname; 
    QString cl_name1;
    QString cl_name2;
    QString cl_companyname;
    QString cl_nip;
    QString cl_idcard;
    QString cl_pesel;
    QString cl_email;
    QString cl_phone1;
    QString cl_phone2;
    bool    cl_draft;
    int     co_id;
    QString cl_login;
    QString cl_pass;
    QDateTime cl_updated;

    QMap<int,QSharedPointer<RecordDataStruct>> recordMap;
    QMap<int,QSharedPointer<FileDataStruct>> fileMap;
};


class ClientMgr : public QObject
{
    Q_OBJECT
public:
    ClientMgr(QObject* parent, Sql* sql);
    void init();
public slots:
    void newClientReq(int client_id,int user_id, QJsonObject obj);
    void fileAdded(int client_id,int user_id,QString filename,int timestamp);
signals:
    void sendToApp(int client_id, QJsonDocument doc);
    void sendToAll(QJsonDocument doc);
    void setUserId(int net_client_id,int cl_id);

private slots:
    void loginReq(int net_client_id,int user_id,QJsonObject data);
    void getClient(int net_client_id,int user_id,QJsonObject data);
    void putClient(int net_client_id,int user_id,QJsonObject data);
    void postClient(int net_client_id,int user_id,QJsonObject data);
    void sendLoginResponse(int net_client_id,QSharedPointer<ClientDataStruct> p);
    void sendUpdateResponse(int net_client_id,bool updated,QString error);
    void sendPostResponse(int net_client_id,bool updated,QString error);
    void sendFileResponse(int net_client_id,bool updated,QString error);
    void sendErrorResponse(int net_client_id,QString error);
    // SQL
    void loadClientFromSql();
    void loadRecordFromSql(QSharedPointer<ClientDataStruct> client);
    void loadFileFromSql(QSharedPointer<ClientDataStruct> client);
    bool addRecordInSql(RecordDataStruct& record,int cl_id,QString *error);
    bool updateClientInSql(QSharedPointer<ClientDataStruct> p, ClientDataStruct &client,QString *error);
    bool addFileInSql(FileDataStruct &file,int cl_id,QString *error);
    QSharedPointer<ClientDataStruct> getClientById(int cl_id,bool report_missing);
    int  getMapSize();
    QJsonObject getRecordJson(QSharedPointer<RecordDataStruct> p);
    QJsonObject getFileJson(QSharedPointer<FileDataStruct> p);
    QJsonObject getClientJSon(QSharedPointer<ClientDataStruct> p);
    QSharedPointer<ClientDataStruct> isUserValid(QString cl_login,QString cl_pass);
private:
    Sql             *sql;
    QMap<int, QSharedPointer<ClientDataStruct>> clientMap;
};

#endif // CLIENTMGR_H
