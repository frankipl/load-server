#include "app-web-thread.h"
#include "zmienne.h"

AppWebThread::AppWebThread(int client_id,QWebSocket *socket,QObject *parent): QObject(parent) {
    this->client_id=client_id;
    this->socket=socket;

    qDebug()<<"5 AppWebThread::AppWebThread starting thread for client_id:"<<client_id;

    requestSize=0;
    user_id=0;
    qDebug()<<"4 AppWebThread::ClientThread client_id:"<<client_id<<" peerAddress:"<<socket->peerAddress().toString();


    connect(socket, &QWebSocket::textMessageReceived, this, &AppWebThread::parseRequest);
    //connect(pSocket, &QWebSocket::binaryMessageReceived, this, &AppWebThread::processBinaryMessage);
    connect(socket, &QWebSocket::disconnected, this, &AppWebThread::socketDisconnected);
//    connect(socket,&QWebSocket::stateChanged, this, &AppWebThread::socketStateChanged);
//    connect(socket,&QWebSocket::error,this,&AppWebThread::socketError);
    connect(socket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(socketStateChanged(QAbstractSocket::SocketState)));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(socketError(QAbstractSocket::SocketError)));


}
void AppWebThread::setUserId(int user_id) {
    this->user_id=user_id;
}
void AppWebThread::sendToClient(QJsonDocument doc) {
    this->writeToSocket(doc);
}
void AppWebThread::sendToClient(int client_id, QJsonDocument doc) {
    this->writeToSocket(doc);
}
void AppWebThread::writeToSocket(QJsonDocument doc) {
    qint64 bytes=socket->sendTextMessage(doc.toJson());
    qDebug()<<"AppWebThread::writeToSocket data:"<<doc.toJson();

    qDebug()<<"AppWebThread::writeToSocket bytes send:"<<bytes;

//    QByteArray block;
//    QDataStream out(&block, QIODevice::ReadWrite);
//    out.setVersion(QDataStream::Qt_4_0);
//    out << (quint64)0;
//    out << xml.toString();
//    out.device()->seek(0);
//    out << (quint64)(block.size() - sizeof(quint64));

//    if(!socket->write(block)) {
//        qWarning()<<"AppWebThread::writeToSocket client_id:"<<client_id<<" failed xml:"<<xml.toString();
//    } else {
//        qDebug().noquote()<<"8 AppWebThread::sendToClient client_id:"<<client_id<<" xml:"<<xml.toString();
//    }
}
void AppWebThread::writeToSocketBinary(QByteArray data) {
    socket->sendBinaryMessage(data);
//#ifdef use_ssl
//    if(!encrypted)
//        return;
//#endif

//    QByteArray block;
//    QDataStream out(&block, QIODevice::ReadWrite);
//    out.setVersion(QDataStream::Qt_4_0);
//    out << (quint64)0;
//    out << data;
//    out.device()->seek(0);
//    out << (quint64)(block.size() - sizeof(quint64));

//    if(!socket->write(block)) {
//        qWarning()<<"AppWebThread::writeToSocket client_id:"<<client_id;
//    } else {
//        qDebug().noquote()<<"8 AppWebThread::sendToClient client_id:"<<client_id;
//    }
}
void AppWebThread::parseRequest(QString req) {
    qDebug()<<"8 AppWebThread::parseRequest client_id:"<<client_id<<" request:"<<req;
    QJsonDocument doc=QJsonDocument::fromJson(req.toUtf8());
 //   qDebug()<<"AppWebThread::parseRequest client_id:"<<client_id<<" req:"<<xml.toString();
    QString module=doc.object().value("module").toString();


    if(module=="client") {
        emit newClientReq(client_id,user_id,doc.object());
    }
    else {
        qWarning()<<"AppWebThread::parseRequest unknown module:"<<module<<" client_id:"<<client_id;
    }
}
void AppWebThread::newFileReq(QJsonObject obj) {
//    QDomElement e=node.firstChild().toElement();
//    int f_dir=e.attribute("f_dir").toInt();
//    int f_id=e.attribute("f_id").toInt();
//    QString f_suffix=e.attribute("f_suffix");
////    QString login=e.attribute("login");
////    QString pass=e.attribute("pass");
//    req_file_name=QString("%1%2/%3.%4").arg(Zm().filePath).arg(f_dir).arg(f_id).arg(f_suffix);
// //   emit checkUserValid(client_id,login,pass);

//    QFile file(req_file_name);
//    if(file.exists()) {
//        qDebug()<<"AppWebThread::newFileReq file_exists sending file_name:"<<req_file_name;
//        file.open(QIODevice::ReadOnly);
//        this->writeToSocketBinary(file.readAll());
////            socket->waitForBytesWritten();
//        file.close();
////            socket->disconnectFromHost();
//        qDebug()<<"AppWebThread::newFileReq file sent, closing";
//    }
//    else {
//        qDebug()<<"AppWebThread::newFileReq file does not exists:"<<req_file_name;
////        socket->disconnectFromHost();
//    }
}
void AppWebThread::setUserValid(bool valid) {
//    if(!valid) {
//        socket->close(QWebSocketProtocol::CloseCodeNormal,QString());
//    }
//    else {
//        QFile file(req_file_name);
//        if(file.exists()) {
//            qDebug()<<"AppWebThread::newFileReq file_exists sending file_name:"<<req_file_name;
//            file.open(QIODevice::ReadOnly);
//            this->writeToSocketBinary(file.readAll());
////            socket->waitForBytesWritten();
//            file.close();
////            socket->disconnectFromHost();
//            qDebug()<<"AppWebThread::newFileReq file sent, closing";
//        }
//        else {
//            qDebug()<<"AppWebThread::newFileReq file does not exists:"<<req_file_name;
//            socket->disconnectFromHost();
//        }
//    }
}
QString AppWebThread::getPeerAddr() {
    return socket->peerAddress().toString();
}
void AppWebThread::closeClient() {
    qDebug()<<"4 AppWebThread::closeClient client_id:"<<client_id;
    socket->close(QWebSocketProtocol::CloseCodeNormal,QString());
  //  socket->deleteLater();
    emit clientClosed(client_id);
//    this->quit();
}
void AppWebThread::socketDisconnected() {

}
void AppWebThread::socketStateChanged(QAbstractSocket::SocketState state) {
    switch(state) {
    case QAbstractSocket::UnconnectedState:
        qDebug() <<"8 AppWebThread::socketStateChanged state=unconnected client_id:"<<client_id;
        this->closeClient();
        break;
    case QAbstractSocket::HostLookupState:
        qDebug() <<"8 AppWebThread::socketStateChanged state=host lookup client_id:"<<client_id;
        break;
    case QAbstractSocket::ConnectingState:
        qDebug() <<"8 AppWebThread::socketStateChanged state=connecting client_id:"<<client_id;
        break;
    case QAbstractSocket::ConnectedState:
        qDebug() <<"8 AppWebThread::socketStateChanged state=connected client_id:"<<client_id;
        break;
    case QAbstractSocket::ClosingState:
        qDebug() <<"8 AppWebThread::socketStateChanged state=closing connection client_id:"<<client_id;
        break;
    }
}
void AppWebThread::socketError(QAbstractSocket::SocketError error) {
    switch (error) {
    case QAbstractSocket::RemoteHostClosedError:
        qDebug() <<"8 AppWebThread::socketError error=remote host closed connection client_id:"<<client_id;
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug() <<"8 AppWebThread::socketError error=host not found client_id:"<<client_id;
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug() <<"8 AppWebThread::socketError error=connection refused client_id:"<<client_id;
        break;
    default:
        qDebug() <<"8 AppWebThread::socketError client_id:"<<client_id<<" error:"<<socket->errorString();
    }
}
#ifdef use_ssl
void AppWebThread::connectionEncrypted() {
    for(int i=0;i<socket->peerCertificateChain().count();i++) {
        const QSslCertificate cert = socket->peerCertificateChain().at(i);
        QStringList lines;
        lines << tr("Organization: %1").arg(cert.subjectInfo(QSslCertificate::Organization).join(" "))
              << tr("Subunit: %1").arg(cert.subjectInfo(QSslCertificate::OrganizationalUnitName).join(" "))
              << tr("Country: %1").arg(cert.subjectInfo(QSslCertificate::CountryName).join(" "))
              << tr("Locality: %1").arg(cert.subjectInfo(QSslCertificate::LocalityName).join(" "))
              << tr("State/Province: %1").arg(cert.subjectInfo(QSslCertificate::StateOrProvinceName).join(" "))
              << tr("Common Name: %1").arg(cert.subjectInfo(QSslCertificate::CommonName).join(" "))
              << QString()
              << tr("Issuer Organization: %1").arg(cert.issuerInfo(QSslCertificate::Organization).join(" "))
              << tr("Issuer Unit Name: %1").arg(cert.issuerInfo(QSslCertificate::OrganizationalUnitName).join(" "))
              << tr("Issuer Country: %1").arg(cert.issuerInfo(QSslCertificate::CountryName).join(" "))
              << tr("Issuer Locality: %1").arg(cert.issuerInfo(QSslCertificate::LocalityName).join(" "))
              << tr("Issuer State/Province: %1").arg(cert.issuerInfo(QSslCertificate::StateOrProvinceName).join(" "))
              << tr("Issuer Common Name: %1").arg(cert.issuerInfo(QSslCertificate::CommonName).join(" "));
        qDebug()<<"8 AppWebThread::connectionEncrypted CERT:"<<lines;
    }
    const QSslCertificate cert=socket->peerCertificate();
    QStringList lines;
    lines << tr("Organization: %1").arg(cert.subjectInfo(QSslCertificate::Organization).join(" "))
          << tr("Subunit: %1").arg(cert.subjectInfo(QSslCertificate::OrganizationalUnitName).join(" "))
          << tr("Country: %1").arg(cert.subjectInfo(QSslCertificate::CountryName).join(" "))
          << tr("Locality: %1").arg(cert.subjectInfo(QSslCertificate::LocalityName).join(" "))
          << tr("State/Province: %1").arg(cert.subjectInfo(QSslCertificate::StateOrProvinceName).join(" "))
          << tr("Common Name: %1").arg(cert.subjectInfo(QSslCertificate::CommonName).join(" "));
    qDebug()<<"8 AppWebThread::connectionEncrypted client_id:"<<client_id<<" CERT:"<<lines;

    encrypted=true;
}
void AppWebThread::SSLerrorOccured(const QList<QSslError> list) {
    for(int i=0;i<list.count();i++) {
        QSslError err=list.at(i);
        qDebug()<<"NetManager::SSLerrorOccured err:"<<err.errorString();
    }
}

#endif
