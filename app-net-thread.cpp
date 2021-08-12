#include "app-net-thread.h"
#include "zmienne.h"

AppNetThread::AppNetThread(int client_id,int socketDescriptor,QObject *parent): QThread(parent), socketDescriptor(socketDescriptor) {
    this->client_id=client_id;

    qDebug()<<"5 AppNetThread::AppNetThread starting thread for client_id:"<<client_id;
#ifdef use_ssl
    socket=new QSslSocket();
    encrypted=false;
#else
    socket=new QTcpSocket();
    encrypted=true;
#endif
    if(!socket->setSocketDescriptor(socketDescriptor)) {
        qCritical()<<"AppNetThread::AppNetThread client_id:"<<client_id<<" setSocketError:"<<socket->error();
        return;
    }
    requestSize=0;
    user_id=0;
    qDebug()<<"4 AppNetThread::ClientThread client_id:"<<client_id<<" peerAddress:"<<socket->peerAddress().toString();

#ifdef use_ssl
    encrypted=false;
    qDebug()<<"4 AppNetThread::ClientThread client_id:"<<client_id<<" ssl supported:"<<socket->supportsSsl();
    qDebug()<<"4 AppNetThread::ClientThread client_id:"<<client_id<<" peerAddress:"<<socket->peerAddress().toString();
 //   socket->addCaCertificates(":/ssl/cacert.pem");
    socket->setPrivateKey(":/ssl/catchr-server.key",QSsl::Rsa,QSsl::Pem,"otny0uy0hpygad45r2");
    socket->setLocalCertificate(":/ssl/catchr-server.crt");
    socket->setPeerVerifyMode(QSslSocket::VerifyNone);//TEMPORARY VERIFYNONE
    connect(socket,SIGNAL(encrypted()),this,SLOT(connectionEncrypted()));
    connect(socket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(socketStateChanged(QAbstractSocket::SocketState)));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(socketError(QAbstractSocket::SocketError)));
    connect(socket,SIGNAL(readyRead()),this,SLOT(readRequest()));
    connect(socket, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(SSLerrorOccured(const QList<QSslError> &)));
    socket->startServerEncryption();
#else

    connect(socket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(socketStateChanged(QAbstractSocket::SocketState)));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(socketError(QAbstractSocket::SocketError)));
    connect(socket,SIGNAL(readyRead()),this,SLOT(readRequest()));
#endif

}
void AppNetThread::setUserId(int user_id) {
    this->user_id=user_id;
}

int AppNetThread::getUserId() const{
    return this->user_id;
}
void AppNetThread::sendToClient(QJsonDocument doc) {
    this->writeToSocket(doc);
}
void AppNetThread::sendToClient(int client_id, QJsonDocument doc) {
    this->writeToSocket(doc);
}
void AppNetThread::writeToSocket(QJsonDocument doc) {
    QByteArray block;
    QDataStream out(&block, QIODevice::ReadWrite);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint64)0;
    out << QString(doc.toJson());
    out.device()->seek(0);
    out << (quint64)(block.size() - sizeof(quint64));

    if(!socket->write(block)) {
        qWarning()<<"AppNetThread::writeToSocket client_id:"<<client_id<<" failed doc:"<<doc.toJson();
    } else {
      //  qDebug().noquote()<<"8 AppNetThread::sendToClient client_id:"<<client_id<<" doc:"<<doc.toJson();
    }
}
void AppNetThread::writeToSocketBinary(QByteArray data) {
#ifdef use_ssl
    if(!encrypted)
        return;
#endif

    QByteArray block;
    QDataStream out(&block, QIODevice::ReadWrite);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint64)0;
    out << data;
    out.device()->seek(0);
    out << (quint64)(block.size() - sizeof(quint64));

    if(!socket->write(block)) {
        qWarning()<<"AppNetThread::writeToSocket client_id:"<<client_id;
    } else {
       // qDebug().noquote()<<"8 AppNetThread::sendToClient client_id:"<<client_id;
    }
}
void AppNetThread::readRequest() {
#ifdef use_ssl
    if(!encrypted)
        return;
#endif
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_0);
  //  qDebug()<<"8 AppNetThread::readRequest client_id:"<<client_id<<" bytesAvailable:"<<socket->bytesAvailable();
    if (requestSize == 0) {
        if (socket->bytesAvailable() < (int)sizeof(quint64))
            return;

        in >> requestSize;
    }

    if (socket->bytesAvailable() < requestSize)
        return;

    QString request;
    in >> request;
    requestSize=0;
    parseRequest(request);
    if(socket->bytesAvailable())
        this->readRequest();
}
void AppNetThread::parseRequest(QString req) {
  //  qDebug()<<"8 AppNetThread::parseRequest client_id:"<<client_id<<" request:"<<req;
    QJsonDocument doc=QJsonDocument::fromJson(req.toUtf8());

    if(req.isNull() || req.isEmpty()) {
        qCritical()<<"AppNetThread::parseRequest client_id:"<<client_id<<" błąd parsowania req:"<<req;
        return;
    }

   // qDebug().noquote()<<"AppNetThread::parseRequest client_id:"<<client_id<<" req:"<<doc.toJson();
    QString module = doc.object().value("module").toString();


    if(module=="ping") {
        this->newPingReq();
        return;
    }


    if(module=="client") {
        emit newClientReq(client_id,user_id,doc.object());
    }
    else if(module=="file") {
        this->newFileReq(doc.object().value("data").toObject());
    }
    else {
        qWarning()<<"AppNetThread::parseRequest unknown module:"<<module<<" client_id:"<<client_id;
    }
}

void AppNetThread::setUser(int client_id, int user_id){
    this->user_id = user_id;

    emit sendAllClients(client_id);
}

void AppNetThread::newFileReq(QJsonObject obj) {

    if(!QDir().exists(Zm().filePath))
        QDir().mkpath(Zm().filePath);

    int timestamp=QDateTime::currentDateTime().toMSecsSinceEpoch()/1000;
    QString filename=obj.value("filename").toString();
    QByteArray filedata=QByteArray::fromBase64(QByteArray(obj.value("filedata").toString().toUtf8()));
    QFile file(QString("%1/%2_%3_%4").arg(Zm().filePath).arg(user_id).arg(filename).arg(timestamp));
    if(!file.open(QIODevice::WriteOnly)) {
        qWarning()<<"AppNetThread::newFileReq error "<<file.fileName()<<":"<<file.errorString();
        return;
    }
    file.write(filedata);
    file.close();
    emit fileAdded(client_id,user_id,filename,timestamp);
}

void AppNetThread::newPingReq(){
    QJsonObject obj;
    obj.insert("module","pong");
    sendToClient(QJsonDocument(obj));
}
void AppNetThread::setUserValid(bool valid) {
    if(!valid) {
        socket->disconnectFromHost();
    }
    else {
        QFile file(req_file_name);
        if(file.exists()) {
            qDebug()<<"AppNetThread::newFileReq file_exists sending file_name:"<<req_file_name;
            file.open(QIODevice::ReadOnly);
            this->writeToSocketBinary(file.readAll());
            socket->waitForBytesWritten();
            file.close();
            socket->disconnectFromHost();
            qDebug()<<"AppNetThread::newFileReq file sent, closing";
        }
        else {
            qDebug()<<"AppNetThread::newFileReq file does not exists:"<<req_file_name;
            socket->disconnectFromHost();
        }
    }
}
QString AppNetThread::getPeerAddr() {
    return socket->peerAddress().toString();
}
void AppNetThread::closeClient() {
    qDebug()<<"4 AppNetThread::closeClient client_id:"<<client_id;
    socket->deleteLater();
    emit clientClosed(client_id);
    this->quit();
}

void AppNetThread::socketStateChanged(QAbstractSocket::SocketState state) {
    switch(state) {
    case QAbstractSocket::UnconnectedState:
        qDebug() <<"8 AppNetThread::socketStateChanged state=unconnected client_id:"<<client_id;
        this->closeClient();
        break;
    case QAbstractSocket::HostLookupState:
        qDebug() <<"8 AppNetThread::socketStateChanged state=host lookup client_id:"<<client_id;
        break;
    case QAbstractSocket::ConnectingState:
        qDebug() <<"8 AppNetThread::socketStateChanged state=connecting client_id:"<<client_id;
        break;
    case QAbstractSocket::ConnectedState:
        qDebug() <<"8 AppNetThread::socketStateChanged state=connected client_id:"<<client_id;
        break;
    case QAbstractSocket::ClosingState:
        qDebug() <<"8 AppNetThread::socketStateChanged state=closing connection client_id:"<<client_id;
        break;
    }
}
void AppNetThread::socketError(QAbstractSocket::SocketError error) {
    switch (error) {
    case QAbstractSocket::RemoteHostClosedError:
        qDebug() <<"8 AppNetThread::socketError error=remote host closed connection client_id:"<<client_id;
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug() <<"8 AppNetThread::socketError error=host not found client_id:"<<client_id;
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug() <<"8 AppNetThread::socketError error=connection refused client_id:"<<client_id;
        break;
    default:
        qDebug() <<"8 AppNetThread::socketError client_id:"<<client_id<<" error:"<<socket->errorString();
    }
}
#ifdef use_ssl
void AppNetThread::connectionEncrypted() {
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
        qDebug()<<"8 AppNetThread::connectionEncrypted CERT:"<<lines;
    }
    const QSslCertificate cert=socket->peerCertificate();
    QStringList lines;
    lines << tr("Organization: %1").arg(cert.subjectInfo(QSslCertificate::Organization).join(" "))
          << tr("Subunit: %1").arg(cert.subjectInfo(QSslCertificate::OrganizationalUnitName).join(" "))
          << tr("Country: %1").arg(cert.subjectInfo(QSslCertificate::CountryName).join(" "))
          << tr("Locality: %1").arg(cert.subjectInfo(QSslCertificate::LocalityName).join(" "))
          << tr("State/Province: %1").arg(cert.subjectInfo(QSslCertificate::StateOrProvinceName).join(" "))
          << tr("Common Name: %1").arg(cert.subjectInfo(QSslCertificate::CommonName).join(" "));
    qDebug()<<"8 AppNetThread::connectionEncrypted client_id:"<<client_id<<" CERT:"<<lines;

    encrypted=true;
}
void AppNetThread::SSLerrorOccured(const QList<QSslError> list) {
    for(int i=0;i<list.count();i++) {
        QSslError err=list.at(i);
        qDebug()<<"NetManager::SSLerrorOccured err:"<<err.errorString();
    }
}

#endif
