#ifndef ZMIENNE_H
#define ZMIENNE_H

#include <QtCore>
#include <QObject>
#include <qnamespace.h>
#include <qobjectdefs.h>
//#include "helper.h"

enum WebErrType {
    WebErrUnknown=0,
    WebErrAccess=1,
    WebErrNotAvail=2,
    WebErrDbErr=3,
    WebErrNotExists=4
};



class Singleton {
// definicja klasy o nazwie Singleton - jest
// to nazwa wlasna :)
public:
        QString mainPath;
        QString configPath;
        QString configFile;
        QString appListenIp;
        int     appListenPort;
        QString webSocketListenIp;
        int     webSocketListenPort;
        void    *loadServerObj;
        int     logging;
        int     logLevel;
        QFile   logFile;
        QString logFileName;
        QString lockFile;
        QString watchdogSocketName;
        QLocale locale;

        //reports
        QString report_dir;
        QString report_template_path;
        QString report_html_file;
        QString report_pdf_file;
        QString report_doc_file;
        QString report_map_template;
        QString htmlToPdfCmd;
        QString exifToolCmd;
        QString mapbox_access_token;
        QString report_domain;
        QString email_report;
        QString email_report_www;
        QString pdfs_dir;

        QString invoices_dir;

        //email sending
        QString email_server;
        QString email_login;
        QString email_pass;
        int     email_auth;
        int     email_port;
        int     email_ssl;
        QString email_from_address;
        QString email_from_name;
        QString email_invite_regular;
        QString email_invite_admin;
        QString email_revoke;
        QString email_change_pass;
        QString email_delete_account;

        //documents
        QString documentsPath;

        //payments
        QString payPosId;
        QString payCardDeleteUrl;
        QString payCardUrl;
        QString payApiLogin;
        QString payApiPass;
        QString payApiUrl;
        QString payRegisterOrderUrl;
        QString payApiPIN;
        QString payTrnRegisterUrl;
        QString payTrnExecuteUrl;
        QString payTrnVerifyUrl;
        QString payTrnRejectUrl;
        QString payTrnRetrieveCardUrl;

        QString payContinueUrl;
        QString payNotifyUrl;
        QString payNotifyCardUrl;
        QString trnReturnUrl;

        QString payCRC;

        QString SmsApiLogin;
        QString SMSApiPass;

        //REST API
        void    *httpServer;
        QString restApiIp;
        int     restApiPort;
        int     webSessionDuration;
        bool    webUseSsl;

        QString filePath;

signals:
     //   void testSignal();
private:
        Singleton();
        Singleton(const Singleton&);
        // konstruktor klasy

        friend Singleton& Zm();
};

Singleton& Zm();

#endif // ZMIENNE_H
