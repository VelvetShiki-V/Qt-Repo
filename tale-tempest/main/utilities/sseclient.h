#ifndef SSECLIENT_H
#define SSECLIENT_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>


class SseClient : public QObject
{
    Q_OBJECT
public:
    explicit SseClient(QObject *parent = nullptr, const QString server = "http://localhost");

protected slots:
    virtual void streamFinished();
    virtual void streamReceived();
    virtual void streamError(QNetworkReply::NetworkError error);

protected:
    void connectToServer(const QString &url);
    QString m_server;
    QNetworkAccessManager *m_manager;
    QNetworkReply *m_reply;

};

#endif // SSECLIENT_H
