#ifndef APPWEBTHREAD_H
#define APPWEBTHREAD_H

#include <QThread>
#include <QtWebSockets>
#include <QtWebSockets/qwebsocket.h>
#include <QHostAddress>
#include <QDateTime>
#include <QtXml/QDomDocument>
#include <QQueue>
#include "helper.h"

//#define use_ssl 1

class AppWebThread : public QObject
{
    Q_OBJECT

public:
    AppWebThread(int client_id,QWebSocket *socket,QObject *parent);
public slots:
    QString getPeerAddr();
    void sendToClient(int client_id,QJsonDocument doc);
    void sendToClient(QJsonDocument doc);
    void setUserId(int user_id);
    void setUserValid(bool valid);
signals:
    void newClientReq(int client_id, int user_id, QJsonObject obj);
    void clientClosed(int client_id);
    void sendDebug(int code,QString msg);
    void checkUserValid(int client_id,QString login,QString pass);
private slots:
#ifdef use_ssl
    void connectionEncrypted();
    void SSLerrorOccured(const QList<QSslError> list);
#endif
    void newFileReq(QJsonObject obj);
    void writeToSocket(QJsonDocument doc);
    void writeToSocketBinary(QByteArray data);

    void parseRequest(QString req);
    void socketStateChanged(QAbstractSocket::SocketState state);
    void socketError(QAbstractSocket::SocketError error);
    void closeClient();
    void socketDisconnected();
private:
    int                 client_id;
    int                 socketDescriptor;
#ifdef use_ssl
    QSslSocket          *socket;
#else
    QWebSocket          *socket;
#endif
    quint64             requestSize;
    int                 user_id;
    QString             req_file_name;
    bool                encrypted;
};

#endif // APPWEBTHREAD_H
