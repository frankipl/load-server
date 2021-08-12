#include <QCoreApplication>

#include "zmienne.h"
#include "load-server.h"
#include <iostream>

void logMsg(QString msg) {
    if(!Zm().logFileName.isEmpty() && Zm().logFile.isOpen())
        Zm().logFile.write(msg.toLocal8Bit().data());
    else
        fprintf(stderr, msg.toLocal8Bit().data());
}
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    if(!Zm().logging)
        return;
    QString msg2=msg;
    if(type==QtDebugMsg) {
        if(msg2.at(0).isDigit()) {
            int level=msg2.at(0).digitValue();
            if(level>9)
                level=9;
            if(level<1)
                level=1;
            if(level>Zm().logLevel)
                return;
            msg2.remove(0,2);
        }
    }
    if(msg2.at(msg2.count()-2)=='"')
        msg2.chop(2);

    QString string=QString("%1 %2\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(msg2.trimmed());
    switch (type) {
    case QtDebugMsg:
        string.prepend("Debug: ");
        if(Zm().logging)
            logMsg(string);
        break;
    case QtWarningMsg:
        string.prepend("Warning: ");
        if(Zm().logging)
            logMsg(string);
        break;
    case QtCriticalMsg:
        string.prepend("Critical: ");
        if(Zm().logging)
            logMsg(string);
        break;
    case QtFatalMsg:
        string.prepend("Fatal: ");
        if(Zm().logging)
            logMsg(string);
        abort();
    }
}

int main(int argc, char *argv[])
{
    try {
        QLocale::setDefault(QLocale::English);

        qInstallMessageHandler(myMessageOutput);

        QCoreApplication a(argc, argv);


        LoadServer loadServer;
        return a.exec();
    } catch(const std::bad_alloc &) {
        logMsg(QString("out of memory error"));
        std::cout<<"out of memory error";
    }
}
