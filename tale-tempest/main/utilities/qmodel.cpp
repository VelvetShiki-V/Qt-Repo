#include "qmodel.h"
#include <QObject>

QModel::QModel(){
    QModel(QJsonObject());
}

QModel::QModel(const QJsonObject jo){
    if (!jo.value("Id").isNull()){
        m_id = jo.value("Id").toString();
    }
    if (!jo.value("Name").isNull()){
        m_name = jo.value("Name").toString();
    }
    if (!jo.value("Owner").isNull()){
        m_owner = jo.value("Owner").toString();
    }
    if (!jo.value("Ts").isNull()){
        m_ts = jo.value("Ts").toString();
    }
    if (!jo.value("Labels").isNull()){
        QJsonArray labels = jo.value("Labels").toArray();
        foreach(QJsonValue label, labels){
            m_labels.append(label.toString());
        }
    }
    if (!jo.value("Properties").isNull()){
        QJsonArray properties = jo.value("Properties").toArray();
        foreach(QJsonValue property, properties){
            m_properties.append(
                QProperty(
                    property.toObject().value("K").toString(),
                    property.toObject().value("V").toVariant()
                )
            );
        }
    }
    if (!jo.value("Children").isNull()){
        QJsonArray children = jo.value("Children").toArray();
        foreach(QJsonValue child, children){
            m_children.append(
                QModel(child.toObject())
            );
        }
    }
}

QModel::QModel(const QString jstr){
    QJsonParseError json_error;
    QJsonDocument jdoc = QJsonDocument::fromJson(jstr.toUtf8(), &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {
        if(jdoc.isObject())
        {
            QModel(jdoc.object());
        }
    }
}

QModel::~QModel(){
    m_children.clear();
    m_properties.clear();
    m_labels.clear();
}

void QModel::setId(const QString id){
    m_id = id;
}

QString QModel::getId(){
    return m_id;
}

void QModel::setName(const QString name){
    m_name = name;
}

QString QModel::getName(){
    return m_name;
}

void QModel::setOwner(const QString owner){
    m_owner = owner;
}

QString QModel::getOwner(){
    return m_owner;
}

void QModel::setTs(const QString ts){
    m_ts = ts;
}

QString QModel::getTs(){
    return m_ts;
}

void QModel::addLabel(const QString label){
    m_labels.append(label);
}

void QModel::delLabel(const QString label){
    m_labels.removeAll(label);
}

QList<QProperty>& QModel::getProperties(){
    return m_properties;
}

bool QModel::getLabel(const QString label){
    foreach(QString _label, m_labels){
        if (_label.compare(label)){
            return true;
        }
    }
    return false;
}

QVariant QModel::getProperty(const QString key){
    foreach(QProperty _property, m_properties){
        if (_property.first.compare(key)){
            return _property.second;
        }
    }
    return NULL;
}

QModel QModel::getChild(const QString childId){
    foreach(QModel _child, m_children){
        if (_child.getId().compare(childId)){
            return _child;
        }
    }
    return QModel();
}

QList<QString>& QModel::getLabels(){
    return m_labels;
}

void QModel::addProperty(const QString key, const QVariant val){
    m_properties.append(QProperty(key, val));
}

bool QModel::delProperty(const QString key){
    foreach(QProperty _property, m_properties) {
        if (_property.first.compare(key)){
            m_properties.removeOne(_property);
            return true;
        }
    }
    return false;
}

void QModel::addChild(const QModel child){
    m_children.append(child);
}

bool QModel::delChild(const QString childId){
    for(QList<QModel>::iterator it = m_children.end(); it != m_children.begin(); --it) {
        if (it->m_id.compare(childId)){
            m_children.erase(it);
            return true;
        }
    }
    return false;
}

QList<QModel>& QModel::getChildren(){
    return m_children;
}

QJsonObject QModel::toObject() const {
    QJsonObject jo = QJsonObject();
    if (!m_id.isEmpty())
        jo["Id"] = m_id;

    if (!m_name.isEmpty())
        jo["Name"] = m_name;

    if (!m_owner.isEmpty())
        jo["Owner"] = m_owner;

    if (!m_ts.isEmpty())
        jo["Ts"] = m_ts;

    if (!m_labels.isEmpty()){
        QJsonArray ja = QJsonArray();
        for (int i = 0; i < m_labels.count(); ++i) {
            ja.append(QJsonValue(m_labels[i]));
        }
        jo["Labels"] = ja;
    }

    if (! m_properties.isEmpty()){
        QJsonArray ja = QJsonArray();
        for (int i = 0; i <  m_properties.count(); ++i) {
            QJsonObject property = QJsonObject();
            property["K"] = m_properties[i].first;
            property["V"] = m_properties[i].second.toJsonValue();
            ja.append(property);
        }
        jo["Properties"] = ja;
    }

    if (!m_children.isEmpty()){
        QJsonArray ja = QJsonArray();
        for (int i = 0; i < m_children.count(); ++i) {
            QJsonObject child = m_children[i].toObject();
            ja.append(child);
        }
        jo["Children"] = ja;
    }
    return jo;
}

QString QModel::toString() const{
    QJsonDocument jdoc;
    jdoc.setObject(toObject());
    return jdoc.toJson(QJsonDocument::Compact);
}
