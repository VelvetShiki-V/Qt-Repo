#ifndef MYSSECLIENT_H
#define MYSSECLIENT_H

#include "authsseclient.h"

bool directoryExists(const QString path);
QString toJsonString(const QJsonObject jobj);
QJsonObject toQJsonObject(const QString jstr);


class MySseClient: public AuthSseClient
{
public:
    explicit MySseClient(QObject *parent, const QString server): AuthSseClient(parent, server){};

    bool product(const QString product, QString& user_product);

    bool uploadFile(const QString local_file, QString& mid);
    bool uploadModel(const QString model, QString& mid);
    bool downloadFile(const QString mid, const QString local_dir, QString& fname);
    bool downloadModel(const QString mid, QString& model);
    bool unloadModel(const QString mid);
    bool filterModels(const QString filter, QString& models);

    bool listDbLibs(QString& models);
    bool createDbLib(const QString name, const bool is_public, const QJsonObject obj, QString& mid);
    bool queryDbLib(const QString mid, QString& name, QJsonObject& obj);
    bool queryDbRec(const QString mid, QJsonObject& obj);
};

#endif // MYSSECLIENT_H
