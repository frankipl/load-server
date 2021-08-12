#include "local-socket.h"
#include "zmienne.h"

LocalSocket::LocalSocket(QObject *parent):QObject(parent) {
    dataSize=0;
    watchdogConnected=false;
 //   ccboxInitializing=false;
 //   ccboxInitialized=false;

    signalTimer=new QTimer(this);
    signalTimer->setSingleShot(false);
    signalTimer->setInterval(10*1000);
    connect(signalTimer,SIGNAL(timeout()),this,SLOT(signalTimerTimeout()));
}
void LocalSocket::init() {
    localSocket=new QLocalSocket(this);
    connect(localSocket,SIGNAL(readyRead()),this,SLOT(readData()));
    connect(localSocket,SIGNAL(stateChanged(QLocalSocket::LocalSocketState)),this,SLOT(socketStateChanged(QLocalSocket::LocalSocketState)));
    connect(localSocket,SIGNAL(error(QLocalSocket::LocalSocketError)),this,SLOT(socketError(QLocalSocket::LocalSocketError)));
    connect(localSocket,SIGNAL(connected()),this,SLOT(sendReport()));

    reconnectTimer=new QTimer(this);
    reconnectTimer->setSingleShot(true);
    reconnectTimer->setInterval(10*1000);
    connect(reconnectTimer,SIGNAL(timeout()),this,SLOT(reconnect()));

    localSocket->abort();
    localSocket->connectToServer(Zm().watchdogSocketName);
}
void LocalSocket::close() {
    localSocket->disconnectFromServer();
}
void LocalSocket::reconnect() {
    localSocket->abort();
    localSocket->connectToServer(Zm().watchdogSocketName);
}
void LocalSocket::sendReport() {
    QJsonObject obj;
    obj.insert("module","report");
    obj.insert("pid",getpid());
    this->sendData(QJsonDocument(obj));
}
void LocalSocket::signalTimerTimeout() {
    this->sendSignal();
}
void LocalSocket::sendSignal() {
    QJsonObject obj;
    obj.insert("module","signal");
    this->sendData(QJsonDocument(obj));
}
void LocalSocket::parseData(QString text) {
    QJsonDocument doc=QJsonDocument::fromJson(text.toUtf8());

    QString module=doc.object().value("module").toString();
    if(module=="buttonInstallMode") {
        emit buttonInstallMode();
    }
    else {
        qWarning()<<"LocalSocket::parseData unknown data from watchdog module:"<<module;
    }
}
void LocalSocket::readData() {
    qDebug() << "8 LocalSocket::readData bytes:"<<localSocket->bytesAvailable();
    QDataStream in(localSocket);
    in.setVersion(QDataStream::Qt_4_0);
    if (dataSize == 0) {
        if (localSocket->bytesAvailable() < (int)sizeof(quint32))
            return;
        in >> dataSize;
    }
    if(localSocket->bytesAvailable() < dataSize)
             return;

    QString text;
    in >> text;

    dataSize=0;
    this->parseData(text);
    if(this->localSocket->bytesAvailable()) {
        this->readData();
    }
}
void LocalSocket::sendData(QJsonDocument doc) {
    if(!watchdogConnected)
        return;
//    qDebug() << "LocalSocket::sendData xmlReq:"<<xmlReq.toString();
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint32)0;
    out << doc.toJson();
    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));

    if(localSocket->write(block)==-1) {
 //       qDebug()<<"LocalSocket::sendData write failed xml:";//<<xmlReq.toString()<<" error:"<<localSocket->errorString();
    } else {
        localSocket->flush();
    }
 //   qDebug()<<"LocalSocket::sendData address:"<<localSocket->serverName();
}
void LocalSocket::socketStateChanged(QLocalSocket::LocalSocketState state) {
    switch(state) {
    case QLocalSocket::UnconnectedState:
        watchdogConnected=false;
        reconnectTimer->start();
        signalTimer->stop();
        qDebug() <<"9 LocalSocket::socketStateChanged state=unconnected";
        break;
    case QLocalSocket::ConnectingState:
        watchdogConnected=false;
        qDebug() << "9 LocalSocket::socketStateChanged state=connecting";
        break;
    case QLocalSocket::ConnectedState:
        watchdogConnected=true;
        signalTimer->start();
        qDebug() << "9 LocalSocket::socketStateChanged state=connected";
        break;
    case QLocalSocket::ClosingState:
        watchdogConnected=false;
        qDebug() << "9 LocalSocket::socketStateChanged state=closing connection";
        break;
    default:
        watchdogConnected=false;
        qDebug()<<"9 LocalSocket::socketStateChanged unknown state:"<<state;
    }
}
void LocalSocket::socketError(QLocalSocket::LocalSocketError error) {
    watchdogConnected=false;
    switch (error) {
    case QLocalSocket::PeerClosedError:
        qDebug() << "8 LocalSocket::socketError error=peer host closed connection";
        break;
    case QLocalSocket::ServerNotFoundError:
        qDebug() << "8 LocalSocket::socketError error=peer not found";
        break;
    case QLocalSocket::ConnectionRefusedError:
        qDebug() << "8 LocalSocket::socketError error=connection refused";
        break;
    case QLocalSocket::ConnectionError:
        qDebug() <<"8 LocalSocket::socketError connection error";
        break;
    case QLocalSocket::SocketAccessError:
        qDebug() <<"8 LocalSocket::socketError socket access error";
        break;
    case QLocalSocket::SocketResourceError:
        qDebug() <<"8 LocalSocket::socketError socket resource error";
        break;
    case QLocalSocket::SocketTimeoutError:
        qDebug() <<"8 LocalSocket::socketError socket timeout error";
        break;
    case QLocalSocket::DatagramTooLargeError:
        qDebug()<<"8 LocalSocket::socketError datagram too large";
        break;
    case QLocalSocket::UnsupportedSocketOperationError:
        qDebug()<<"8 LocalSocket::socketError unsupported operation";
        break;
    case QLocalSocket::UnknownSocketError:
        qDebug()<<"8 LocalSocket::socketError unknown error";
        break;
    default:
        qDebug() << "8 LocalSocket::socketError error="<<error;
    }
}
