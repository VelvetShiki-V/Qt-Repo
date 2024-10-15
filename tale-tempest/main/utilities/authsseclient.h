#ifndef AUTHSSECLIENT_H
#define AUTHSSECLIENT_H

#include "sseclient.h"

class AuthSseClient : public SseClient
{
public:
    explicit AuthSseClient(QObject *parent, const QString server);
    bool inService();
    bool verify(const QString user, const QString password);
    bool login(const QString user, const QString password);
    bool logout();

protected:
    QString getUser();
    QString getToken();
    bool postJson(const QString url, const QJsonObject obj, QByteArray& bdata, QString& fname);
    bool postForm(const QString url, const QJsonObject obj, const QString doc, QByteArray& bdata, QString& fname);
    bool receive(QNetworkReply *reply, QByteArray& bdata, QString& fname);

protected slots:
    void streamFinished() override;
    void streamReceived() override;
    void streamError(QNetworkReply::NetworkError error) override;

private:
    QString m_user;
    QString m_password;
    QString m_token;
};


#endif // AUTHSSECLIENT_H
