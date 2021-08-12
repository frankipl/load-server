#include "app-net-manager.h"
#include "zmienne.h"

AppNetManager::AppNetManager(QObject *parent): QTcpServer(parent){
    this->clientId=0;

    QString priv_key_path=QString("test_privateKey_nopass.pem");
    QString cert_path=QString("test_cert.pem");
    QByteArray key_pem;
    QFile key_file(priv_key_path);
    if(key_file.open(QIODevice::ReadOnly)) {
        key_pem=key_file.readAll();
    }
    else {
        qWarning()<<"AppNetManager::AppNetManager unable to open key:"<<key_file.errorString();
    }
    QByteArray cert_pem;
    QFile cert_file(cert_path);
    if(cert_file.open(QIODevice::ReadOnly)) {
        cert_pem=cert_file.readAll();
    }
    else {
        qWarning()<<"AppNetManager::AppNetManager unable to open cert:"<<cert_file.errorString();
    }

    websocket_key=QSslKey(key_pem, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);
    websocket_cert=QSslCertificate(cert_pem);

    websocket_ssl_cfg.setPrivateKey(websocket_key);
    websocket_ssl_cfg.setLocalCertificate(websocket_cert);
    websocket_ssl_cfg.setPeerVerifyMode(QSslSocket::VerifyNone);

}
void AppNetManager::init() {
    if(!this->listen((Zm().appListenIp.isEmpty() ?  QHostAddress::Any :  QHostAddress(Zm().appListenIp)),Zm().appListenPort)) {
        qWarning()<<"AppNetManager::initAppNetManager::init cant listen on anyIP port:"<<Zm().appListenPort;
    } else {
        qDebug()<<"AppNetManager::init listening on ip:"<<Zm().appListenIp<<" port:"<<Zm().appListenPort;
    }
    m_pWebSocketServer=new QWebSocketServer(QStringLiteral("Nieruchomosci Server"),QWebSocketServer::NonSecureMode, this);
  // m_pWebSocketServer->setSslConfiguration(websocket_ssl_cfg);
    //qDebug()<<"AppNetManager::init websocket secureMode:"<<m_pWebSocketServer->secureMode();
    if (m_pWebSocketServer->listen((Zm().webSocketListenIp.isEmpty() ?  QHostAddress::Any :  QHostAddress(Zm().webSocketListenIp)), Zm().webSocketListenPort)) {
        qDebug()<<"AppNetManager::init web socket listening on ip:"<<Zm().webSocketListenIp<<" port" << Zm().webSocketListenPort;
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection,this, &AppNetManager::onNewWebConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &AppNetManager::webServerClosed);
    }
    else {
        qWarning()<<"AppNetManager::init cant listen on port:"<<Zm().webSocketListenPort<<" error:"<<m_pWebSocketServer->errorString();
    }
}
void AppNetManager::setUserId(int client_id, int user_id) {
    if(!clientMap.contains(client_id)) {
        qWarning()<<"AppNetManager::setUserId there is no client_id:"<<client_id;
        return;
    }
    if(!clientMap[client_id]->web)
        clientMap[client_id]->thread->setUserId(user_id);
    else
        clientMap[client_id]->webThread->setUserId(user_id);

}

void AppNetManager::userIsValid(int client_id, bool valid) {
    if(!clientMap.contains(client_id)) {
        qWarning()<<"AppNetManager::userIsvalid there is no client_id:"<<client_id;
        return;
    }
    if(!clientMap[client_id]->web)
        clientMap[client_id]->thread->setUserValid(valid);
    else
        clientMap[client_id]->webThread->setUserValid(valid);
}
void AppNetManager::incomingConnection(qintptr socketDescriptor) {
    qDebug()<<"AppNetManager::incomingConnection";
     clientId++;
     AppClientStruct *client=new AppClientStruct;
     client->client_id=clientId;
     client->web=0;
     client->thread=new AppNetThread(client->client_id,socketDescriptor,this);

     connect(client->thread,SIGNAL(clientClosed(int)),this,SLOT(deleteClient(int)));

     connect(client->thread,SIGNAL(newClientReq(int,int,QJsonObject)),this,SIGNAL(newClientReq(int,int,QJsonObject)));
     connect(client->thread,SIGNAL(fileAdded(int,int,QString,int)),this,SIGNAL(fileAdded(int,int,QString,int)));

     connect(client->thread,SIGNAL(sendDebug(int,QString)),this,SIGNAL(sendDebug(int,QString)));
     connect(client->thread,SIGNAL(checkUserValid(int,QString,QString)),this,SIGNAL(checkUserValid(int,QString,QString)));

     //connect(this, SIGNAL(setUser(int, int)), client->thread, SLOT(setUser(int, int)));

     connect(client->thread,SIGNAL(sendAllClients(int)),this,SIGNAL(sendAllClients(int)));


     client->thread->start();
     clientMap.insert(client->client_id,client);
}
void AppNetManager::deleteClient(int client_id) {
    qDebug() << "6 AppNetManager::deleteClient client_id:"<<client_id;
    clientMap[client_id]->thread->exit();
    clientMap[client_id]->thread->wait();
    clientMap[client_id]->thread->deleteLater();
    delete clientMap[client_id];
    clientMap.remove(client_id);
    emit clientClosed(client_id);
}
void AppNetManager::sendToClient(int client_id, QJsonDocument doc) {
    if(!clientMap.contains(client_id)) {
        qWarning()<<"AppNetManager::sendToClient there is no client_id:"<<client_id;
        return;
    }
    if(!clientMap[client_id]->web)
        clientMap[client_id]->thread->sendToClient(doc);
    else
        clientMap[client_id]->webThread->sendToClient(doc);
}

void AppNetManager::sendToAll(QJsonDocument doc) {
    for(auto c : clientMap){
        sendToClient(c->client_id, doc);
    }
}

void AppNetManager::onNewWebConnection() {
    qDebug()<<"AppNetManager::onNewWebConnection";
    clientId++;
    AppClientStruct *client=new AppClientStruct;
    client->client_id=clientId;
    client->web=1;
    client->webSocket=m_pWebSocketServer->nextPendingConnection();
    client->webThread=new AppWebThread(client->client_id,client->webSocket,this);

    connect(client->webThread,SIGNAL(clientClosed(int)),this,SLOT(deleteWebClient(int)));

//    connect(client->webThread,SIGNAL(newReportReq(int,int,QDomNode)),this,SIGNAL(newReportReq(int,int,QDomNode)));
//    connect(client->webThread,SIGNAL(newDocumentReq(int,int,QDomNode)),this,SIGNAL(newDocumentReq(int,int,QDomNode)));
//    connect(client->webThread,SIGNAL(authenticateReq(int,QDomNode)),this,SIGNAL(authenticateReq(int,QDomNode)));
//    connect(client->webThread,SIGNAL(sendDebug(int,QString)),this,SIGNAL(sendDebug(int,QString)));
//    connect(client->webThread,SIGNAL(checkUserValid(int,QString,QString)),this,SIGNAL(checkUserValid(int,QString,QString)));

    //connect(client->webThread, SIGNAL(setUser(int, int)), client->webThread, SLOT(setUser(int, int)));

    clientMap.insert(client->client_id,client);
}
void AppNetManager::deleteWebClient(int client_id) {
    qDebug() << "6 AppNetManager::deleteWebClient client_id:"<<client_id;
    clientMap[client_id]->webSocket->deleteLater();
    clientMap[client_id]->webThread->deleteLater();
    delete clientMap[client_id];
    clientMap.remove(client_id);
    emit clientClosed(client_id);
}
void AppNetManager::webServerClosed() {

}
