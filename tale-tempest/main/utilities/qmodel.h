#ifndef QModel_H
#define QModel_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <QPair>
#include <QVariant>
#include <QJsonParseError>

enum QPropertyValueType
{
    String = QVariant::String,
    Int = QVariant::Int,
    Float = QVariant::Double,
    Bool = QVariant::Bool,
};

typedef QPair<QString, QVariant> QProperty;

class QModel
{
public:
    QModel();
    QModel(const QJsonObject jo);
    QModel(const QString jstr);
    ~QModel();
    void setId(const QString id);
    void setName(const QString name);
    void setOwner(const QString owner);
    void setTs(const QString ts);
    QString getId();
    QString getName();
    QString getOwner();
    QString getTs();
    bool getLabel(const QString label);
    QVariant getProperty(const QString key);
    QModel getChild(const QString childId);
    void addLabel(const QString label);
    void addProperty(const QString key, const QVariant val);
    void addChild(const QModel child);
    void delLabel(const QString label);
    bool delProperty(const QString key);
    bool delChild(const QString childId);
    QList<QProperty>& getProperties();
    QList<QString>& getLabels();
    QList<QModel>& getChildren();
    QJsonObject toObject() const;
    QString toString() const;

private:
    QString m_id;
    QString m_name;
    QString m_owner;
    QString m_ts;
    QList<QString> m_labels;
    QList<QProperty> m_properties;
    QList<QModel> m_children;
};

#endif // QModel_H
