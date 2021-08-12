#ifndef LOADSERVER_H
#define LOADSERVER_H

#include <QtCore>
#include <QtNetwork>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include "app-net-manager.h"
#include "sql.h"
#include "local-socket.h"
#include "client-mgr.h"



class LoadServer: public QObject {
    Q_OBJECT
public:
    LoadServer();
public slots:
     void terminate(bool fromSignal=false);
     void reload();
signals:
    void sendToApp(int client_id,QJsonDocument doc);
    void initClock();

   // void sendAllDataToClient(int client_id, int user_id);
private slots:
    void setVariables();
    void lockFile();
    void openLogFile();
    void setServerLogging(int logging);
    void setServerLogLevel(int logLevel);
    void initializeUserApp(int client_id,int user_id);
    void debugMsg(QString msg, int code);
    void settingsCompleted(int client_id,int user_id);
    void workTimerTimeout();
    void testTimerTimeout();
    void fillInDatabase();
private:
    QSettings       *settings;
    QSettings       *sslSettings;
    AppNetManager   *appNetManager;
    LocalSocket     *localSocket;
    Sql             *sql;
    QTimer          *workingTimer;
    QDateTime       startDate;
    ClientMgr       *clientMgr;

    QProcess        testProcess;
    QTimer          *testTimer;
};

extern "C" void terminate_wrapper(int dummy);
extern "C" void reload_wrapper(int dummy);

#endif // LOADSERVER_H
