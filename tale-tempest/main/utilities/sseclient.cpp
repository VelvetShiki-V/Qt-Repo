#include "sseclient.h"


SseClient::SseClient(QObject *parent, const QString server)
    : QObject{parent}, m_server{server}
{
    m_manager = new QNetworkAccessManager(this);
}

void SseClient::connectToServer(const QString &url) {
    QNetworkRequest request(url);
    m_reply = m_manager->get(request);
    connect(m_reply,
            &QNetworkReply::readyRead,
            this,
            &SseClient::streamReceived
            );
    connect(m_reply,
            &QNetworkReply::finished,
            this,
            &SseClient::streamFinished
            );
    connect(m_reply,
            &QNetworkReply::errorOccurred,
            this,
            &SseClient::streamError
            );
    request.setTransferTimeout(5000);
}

void SseClient::streamReceived()
{
    qDebug() << "SseClient::streamReceived";
}

void SseClient::streamFinished() {
    qDebug() << "SseClient::streamFinished";
}

void SseClient::streamError(QNetworkReply::NetworkError error) {
    qDebug() << "SseClient::streamError" << error;
}
