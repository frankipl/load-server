#ifndef APPNETTHREAD_H
#define APPNETTHREAD_H

#include <QThread>
#include <QAbstractSocket>
#include <qabstractsocket.h>
#include <QSslSocket>
#include <QHostAddress>
#include <QDateTime>
#include <QtXml/QDomDocument>
#include <QQueue>
#include "helper.h"

//#define use_ssl 1

class AppNetThread : public QThread
{
    Q_OBJECT

public:
    AppNetThread(int client_id,int socketDescriptor,QObject *parent);
public slots:
    QString getPeerAddr();
    void sendToClient(int client_id,QJsonDocument doc);
    void sendToClient(QJsonDocument doc);
    void setUserId(int user_id);
    int getUserId() const;
    void setUserValid(bool valid);
signals:
    void newClientReq(int client_id, int user_id, QJsonObject obj);
    void fileAdded(int client_id,int user_id,QString filename,int timestamp);
    void clientClosed(int client_id);
    void sendDebug(int code,QString msg);
    void checkUserValid(int client_id,QString login,QString pass);

    void sendAllClients(int net_client_id);
private slots:
#ifdef use_ssl
    void connectionEncrypted();
    void SSLerrorOccured(const QList<QSslError> list);
#endif

    void newFileReq(QJsonObject obj);
    void newPingReq();
    void writeToSocket(QJsonDocument doc);
    void writeToSocketBinary(QByteArray data);
    void readRequest();
    void parseRequest(QString req);
    void setUser(int client_id, int user_id);
    void socketStateChanged(QAbstractSocket::SocketState state);
    void socketError(QAbstractSocket::SocketError error);
    void closeClient();
private:
    int                 client_id;
    int                 socketDescriptor;
#ifdef use_ssl
    QSslSocket          *socket;
#else
    QTcpSocket          *socket;
#endif
    quint64             requestSize;
    int                 user_id = 0;
    QString             req_file_name;
    bool                encrypted;
};

#endif // APPNETTHREAD_H
