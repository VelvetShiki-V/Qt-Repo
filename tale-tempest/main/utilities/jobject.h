#ifndef JOBJECT_H
#define JOBJECT_H

#include <QString>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>

#define UNVAL QJsonValue::Undefined

QString dump_json(QJsonObject& jo);
QJsonObject load_json(QString jstr);
bool read_jsonobject(QJsonObject jo, QString key, QJsonValue& val);
bool read_jsonarray(QJsonArray jo, QString key, QJsonValue& val);
bool upsert_jsonobject(QJsonObject& jo, QString key, QJsonValue val);
bool upsert_jsonarray(QJsonArray& jo, QString key, QJsonValue val);

class JObject
{
public:
    JObject();
    JObject(const QString jstr);
    JObject(const QJsonObject jo);
    QJsonObject object();
    QString text();
    QJsonValue::Type check(QString key);
    QJsonValue parse(QString key);
    int count(QString key);
    bool remove(QString key);
    bool append(QString key, QJsonValue val);

    template<typename T>
    bool upsert(QString key, T val){
        return upsert_jsonobject(m_jobject, key, QJsonValue(val));
    }

protected:
    QJsonObject m_jobject;
};

#endif // JOBJECT_H
