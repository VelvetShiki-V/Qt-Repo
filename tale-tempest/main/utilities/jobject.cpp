#include "jobject.h"
#include <QJsonParseError>
#include <QJsonArray>

JObject::JObject()
{
    m_jobject = QJsonObject();
}

JObject::JObject(const QString jstr)
{
    m_jobject = load_json(jstr);
}

JObject::JObject(const QJsonObject jo)
{
    m_jobject = jo;
}

QJsonObject JObject::object(){
    return m_jobject;
}

QString JObject::text(){
    QJsonDocument jdoc;
    jdoc.setObject(m_jobject);
    return QString::fromLatin1(jdoc.toJson());
}

QJsonValue::Type JObject::check(QString key)
{
    QJsonValue val;
    if (read_jsonobject(m_jobject, key, val)){
        return val.type();
    }
    return QJsonValue::Undefined;
}

QJsonValue JObject::parse(QString key)
{
    QJsonValue val;
    if (read_jsonobject(m_jobject, key, val)){
        return val;
    }else{
        return QJsonValue::Null;
    }
}

int JObject::count(QString key)
{
    QJsonValue val = parse(key);

    if(val != QJsonValue::Null){
        if (val.type() == QJsonValue::Array){
            return val.toArray().count();
        }
    }
    return -1;
}

bool JObject::remove(QString key)
{
    return upsert(key, QJsonValue::Undefined);
}

bool JObject::append(QString key, QJsonValue val)
{
    if (check(key) == QJsonValue::Array){
        return upsert(QString("%1.%2").arg(key).arg(count(key)), val);
    }
    return false;
}

///////////////////////////////////////////////////////////////////

void seperate_key(QString key, QString& top_key, QString& rem_key)
{
    QStringList sub_keys = key.split(".");
    top_key = sub_keys.at(0);
    rem_key = key.right(key.length()-top_key.length()-1);
}

bool isNum(QString str)
{
    bool isNum;
    str.toDouble(&isNum);
    return isNum;
}

bool read_jsonobject(QJsonObject jo, QString key, QJsonValue& val)
{
    if (key.contains(".")){
        QString top_key, rem_key;
        seperate_key(key, top_key,rem_key);
        if(jo.value(top_key).isObject()){
            QJsonObject jo_top = jo.value(top_key).toObject();
            return read_jsonobject(jo_top, rem_key, val);
        }
        if(jo.value(top_key).isArray()){
            QJsonArray jo_top = jo.value(top_key).toArray();
            return read_jsonarray(jo_top, rem_key, val);
        }
        return false;
    }

    if (jo.contains(key)){
        val = jo.value(key);
        return true;
    }
    return false;
}


bool read_jsonarray(QJsonArray jo, QString key, QJsonValue& val)
{
    if (key.contains(".")){
        QString top_key, rem_key;
        seperate_key(key, top_key,rem_key);
        if (isNum(top_key)){
            int idx = top_key.toInt();
            if (jo.at(idx).isArray()) {
                QJsonArray jo_top = jo.at(idx).toArray();
                return read_jsonarray(jo_top, rem_key, val);
            }
            if (jo.at(idx).isObject()) {
                QJsonObject jo_top = jo.at(idx).toObject();
                return read_jsonobject(jo_top, rem_key, val);
            }
        }else{
            return false;
        }
    }

    if (isNum(key)){
        if (jo.size() > key.toInt()){
            val = jo.at(key.toInt());
            return true;
        }
    }
    return false;
}

bool check_jsonobject(QJsonObject jo, QString key, QJsonValue::Type& typ)
{
    QJsonValue val;
    if (read_jsonobject(jo, key, val)){
        typ = val.type();
        return true;
    }
    return false;
}

QString dump_json(QJsonObject& jo)
{
    QJsonDocument jdoc;
    jdoc.setObject(jo);
    return QString::fromLatin1(jdoc.toJson());
}

QJsonObject load_json(QString jstr)
{
    QJsonParseError json_error;
    QJsonDocument jdoc = QJsonDocument::fromJson(jstr.toUtf8(), &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {
        if(jdoc.isObject())
        {
            return jdoc.object();
        }
    }
    return QJsonObject();
}

///////////////////////////////////////////////////////////////////
bool upsert_jsonobject(QJsonObject& jo, QString key, QJsonValue val)
{
    if (key.contains(".")){
        QString top_key, rem_key;
        seperate_key(key, top_key,rem_key);
        if (jo.value(top_key).isObject()){
            QJsonObject jo_top;
            jo_top = jo.value(top_key).toObject();
            bool rt = upsert_jsonobject(jo_top, rem_key, val);
            jo[top_key] = jo_top;
            return rt;
        }
        if (jo.value(top_key).isArray()){
            QJsonArray jo_top = jo.value(top_key).toArray();
            bool rt = upsert_jsonarray(jo_top, rem_key, val);
            jo[top_key] = jo_top;
            return rt;
        }
        return false;
    }
    if (val.isUndefined()){
        jo.remove(key);
    }else{
        jo[key] = val;
    }
    return true;
}

bool upsert_jsonarray(QJsonArray& jo, QString key, QJsonValue val)
{
    if (key.contains(".")){
        QString top_key, rem_key;
        seperate_key(key, top_key,rem_key);
        if (isNum(top_key)){
            if (jo.size() > top_key.toInt() && top_key.toInt() >= 0){
                if (jo.at(top_key.toInt()).isObject()) {
                    QJsonObject jo_top = jo.at(top_key.toInt()).toObject();
                    bool rt = upsert_jsonobject(jo_top, rem_key, val);
                    jo.replace(top_key.toInt(), jo_top);
                    return rt;
                }
                if (jo.at(top_key.toInt()).isArray()) {
                    QJsonArray jo_top = jo.at(top_key.toInt()).toArray();
                    bool rt = upsert_jsonarray(jo_top, rem_key, val);
                    jo.replace(top_key.toInt(), jo_top);
                    return rt;
                }
            }
        }
        return false;
    }
    if (isNum(key)){
        if (val.isUndefined()){
            jo.removeAt(key.toInt());
            return true;
        }else{
            if(jo.size() > key.toInt() && key.toInt() >= 0){
                jo.replace(key.toInt(), val);
                return true;
            }
            if(jo.size() == key.toInt()){
                jo.append(val);
                return true;
            }
        }
    }
    return false;
}
