#ifndef LOCALSOCKET_H
#define LOCALSOCKET_H

#include <QtCore>
#include <QtNetwork>
#include "helper.h"
#include "stdlib.h"
#include <sys/types.h>
#include <unistd.h>

class LocalSocket:public QObject {
    Q_OBJECT
public:
    LocalSocket(QObject *parent);
public slots:
    void init();
    void sendData(QJsonDocument doc);
    void sendReport();
    void sendSignal();
    void close();
signals:
    void buttonInstallMode();
private slots:
    void reconnect();
    void socketStateChanged(QLocalSocket::LocalSocketState state);
    void socketError(QLocalSocket::LocalSocketError error);
    void readData();
    void parseData(QString text);
    void signalTimerTimeout();
private:
    QLocalSocket    *localSocket;
    quint32         dataSize;
    QTimer          *reconnectTimer;
    bool            watchdogConnected;
    QTimer          *signalTimer;
  //  bool            ccboxInitializing;
  //  bool            ccboxInitialized;
};

#endif // LOCALSOCKET_H
