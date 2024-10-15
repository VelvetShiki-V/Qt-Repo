#include "authsseclient.h"
#include "qmodel.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QtNetwork/QHttpMultiPart>
#include <QCryptographicHash>

QString sha256_encrypt(const QString& data);

AuthSseClient::AuthSseClient(QObject *parent, const QString server)
    : SseClient{parent, server}
{
    m_token = "";
}

void AuthSseClient::streamReceived()
{
    qDebug() << "AuthSseClient::streamReceived";
    QString recvData =  m_reply->readAll().simplified();
    QStringList recvDataList = recvData.split(' ');
    if (recvDataList[1] == "session/token") {
        m_token = recvDataList[3];
    }
}

void AuthSseClient::streamFinished() {
    qDebug() << "AuthSseClient::streamFinished";
    // if (m_reply->request().url().path() == "/user/login"){
    //     m_token = "";
    // }
    m_token = "";
    m_reply->deleteLater();
}

void AuthSseClient::streamError(QNetworkReply::NetworkError error) {
    qDebug() << "AuthSseClient::streamError" << error << m_reply->errorString();
}


bool AuthSseClient::verify(const QString user, const QString password)
{
    QString request_url = QString("%1/admin/user/verify").arg(m_server);
    QJsonObject data;
    data.insert("user", m_user);
    data.insert("password", sha256_encrypt(password));
    QByteArray bdata;
    QString fname;
    return postJson(request_url, data, bdata, fname);
}

bool AuthSseClient::login(const QString user, const QString password)
{
    m_user = user;
    if (password.length() == 64){
        m_password = password;
    }
    else {
        m_password = sha256_encrypt(password);
    }
    QString url = QString("%1/admin/user/login?user=%2&password=%3").arg(m_server).arg(m_user).arg(m_password);

    connectToServer(url);

    while (!inService()){
        QApplication::processEvents();
    }
    return true;
}

bool AuthSseClient::logout()
{
    QString request_url = QString("%1/admin/user/logout").arg(m_server);
    QJsonObject data;
    data.insert("user", m_user);
    data.insert("password", m_password);
    QByteArray bdata;
    QString fname;
    bool res = postJson(request_url, data, bdata, fname);
    return res;
}

bool AuthSseClient::inService(){
    return !m_token.isEmpty();
}

QString AuthSseClient::getUser()
{
    return m_user;
}

QString AuthSseClient::getToken()
{
    if (inService()){
        return m_token;
    }else{
        return "";
    }
}

bool AuthSseClient::postJson(const QString url, const QJsonObject obj, QByteArray& bdata, QString& fname)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QString jsonStr = QJsonDocument(obj).toJson(QJsonDocument::Compact);

    QNetworkReply *reply = m_manager->post(request,jsonStr.toUtf8());

    return receive(reply, bdata, fname);
}

bool AuthSseClient::postForm(const QString url, const QJsonObject obj, const QString doc, QByteArray& bdata, QString& fname)
{
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType,this);

    if (!doc.isEmpty()){
        QFileInfo fileInfo(doc);
        if (fileInfo.isFile() && fileInfo.exists()){
            // transfer a document
            QFile *file = new QFile(doc, this);
            if (file->open(QIODevice::ReadOnly)){
                QHttpPart dbFilePart;
                QString requestFormat = QString::fromUtf8("form-data;name=%1;filename=%2;type=application/octet-stream")
                                            .arg("file")
                                            .arg(fileInfo.fileName());
                dbFilePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(requestFormat));
                dbFilePart.setBodyDevice(file);
                multiPart->append(dbFilePart);
            }
        }
        else{
            // transfer text content
            QHttpPart dbFilePart;
            QString requestFormat = QString::fromUtf8("form-data;name=file;filename=");
            dbFilePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(requestFormat));
            dbFilePart.setBody(doc.toUtf8());
            multiPart->append(dbFilePart);
        }
    }

    for (const QString &key : obj.keys()) {
        QJsonValue value = obj[key];
        QHttpPart onePart;
        onePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data;name="+key));
        onePart.setBody(value.toString().toUtf8());
        multiPart->append(onePart);
    }

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    // request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data");

    QNetworkReply *reply = m_manager->post(request, multiPart);
    multiPart->setParent(reply);

    return receive(reply, bdata, fname);
}

bool AuthSseClient::receive(QNetworkReply *reply, QByteArray& bdata, QString& fname){
    bool succ = false;

    while (!reply->isFinished())
    {
        qApp->processEvents();
    }

    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (statusCode.isValid()){
        int status = statusCode.toInt();
        if ( status == 200 )
        {
            bdata =  reply->readAll();

            QVariant contentTypeHeader = reply->header(QNetworkRequest::ContentTypeHeader);
            QString contentType = contentTypeHeader.toString();
            if (contentType == "application/json"){

                QString resStr = QString::fromLatin1(bdata);
                succ = resStr != "false";
            }
            if (contentType == "application/octet-stream"){
                QString contentDisposition = reply->header(QNetworkRequest::ContentDispositionHeader).toString();
                fname = contentDisposition.split("=")[1];
                succ = !fname.isEmpty();
            }
        }
    }

    reply->deleteLater();
    return succ;
}

QString sha256_encrypt(const QString& data)
{
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(data.toLocal8Bit());
    QByteArray result = hash.result();
    return QString(result.toHex());
}
