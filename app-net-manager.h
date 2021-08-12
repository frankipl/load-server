#ifndef APPNETMANAGER_H
#define APPNETMANAGER_H

#include <QtNetwork>
#include <QWebSocket>
#include "QtWebSockets/qwebsocketserver.h"
#include "QtWebSockets/qwebsocket.h"
#include "app-net-thread.h"
#include "app-web-thread.h"


struct AppClientStruct {
    int client_id;
    AppNetThread *thread;
    QWebSocket *webSocket;
    AppWebThread *webThread;
    int     web;
};

class AppNetManager : public QTcpServer
 {
     Q_OBJECT

 public:
     AppNetManager(QObject *parent);
 public slots:
     void init();
     void sendToClient(int client_id,QJsonDocument doc);
     void sendToAll(QJsonDocument doc);
     void setUserId(int client_id,int user_id);
     void userIsValid(int client_id,bool valid);
 protected:
     void incomingConnection(qintptr socketDescriptor);
 signals:

     void newClientReq(int client_id, int user_id, QJsonObject obj);
     void fileAdded(int client_id,int user_id,QString filename,int timestamp);

     void clientClosed(int client_id);
     void sendDebug(int code,QString msg);
     void checkUserValid(int client_id,QString login,QString pass);
     void sendAllClients(int net_client_id);

     void sendAllDataToClient(int client_id, int user_id);
 private slots:
     void deleteClient(int client_id);
     void deleteWebClient(int client_id);
     void onNewWebConnection();
     void webServerClosed();
 private:
    QMap<int,AppClientStruct*> clientMap;
    int     clientId;
    QWebSocketServer *m_pWebSocketServer;
    QSslKey websocket_key;
    QSslCertificate websocket_cert;
    QSslConfiguration websocket_ssl_cfg;

 public:
    QMap<int,AppClientStruct*> * getClientMap(){
        return &this->clientMap;
    };
 };

#endif // APPNETMANAGER_H
