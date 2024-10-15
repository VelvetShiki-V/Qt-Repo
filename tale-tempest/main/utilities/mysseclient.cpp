#include "mysseclient.h"
#include "qmodel.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QtNetwork/QHttpMultiPart>
#include <QCryptographicHash>

// uploadFile可以上传一个本地文件（路径local_file）。同时为其创建1个model，其Id保存在mid。
bool MySseClient::uploadFile(const QString local_file, QString& mid)
{
    QString request_url = QString("%1/store/upload").arg(m_server);
    QJsonObject data;
    data.insert("user", getUser());
    data.insert("token", getToken());
    if (!mid.isEmpty()){
        data.insert("mid", mid);
    }
    QByteArray bdata;
    QString fname;
    bool res = postForm(request_url, data, local_file, bdata, fname);
    if (res) {
        QString resStr = QString::fromLatin1(bdata);
        mid = toQJsonObject(resStr)["Id"].toString();
    }
    return res;
}

// uploadModel可以上传并创建完整的模型数据，模型Id存在mid；也可以更新已有的模型数据；
// uploadModel还可以将一个已有的模型添加到另一个模型的Children。
bool MySseClient::uploadModel(const QString model, QString& mid)
{
    QString request_url = QString("%1/store/upload").arg(m_server);
    QJsonObject data;
    data.insert("user", getUser());
    data.insert("token", getToken());

    QByteArray bdata;
    QString fname;
    bool res = postForm(request_url, data, model, bdata, fname);
    if (res) {
        QString resStr = QString::fromLatin1(bdata);
        mid = toQJsonObject(resStr)["Id"].toString();
    }
    return res;
}

// downloadFile按照模型的Id下载一个文件到本地目录local_dir，文件名存在fname。
bool MySseClient::downloadFile(const QString mid, const QString local_dir, QString& fname)
{
    if (!directoryExists(local_dir)){
        return false;
    }

    QString request_url = QString("%1/store/download").arg(m_server);
    QJsonObject data;
    data.insert("user", getUser());
    data.insert("token", getToken());
    data.insert("mid", mid);
    QByteArray bdata;
    bool res = postJson(request_url, data, bdata, fname);
    if (res){
        QString fn = QString("%1/%2").arg(local_dir, fname);
        QFile file(fn);
        bool ok = file.open(QFile::WriteOnly | QFile::Text);
        if (ok){
            QTextStream out(&file);
            out << bdata;
        }
        file.close();
    }
    return res;
}

// downloadModel按照模型Id下载一个模型数据
bool MySseClient::downloadModel(const QString mid, QString& model)
{
    QString request_url = QString("%1/store/download").arg(m_server);
    QJsonObject data;
    data.insert("user", getUser());
    data.insert("token", getToken());
    data.insert("mid", mid);
    QByteArray bdata;
    QString fname;
    bool res = postJson(request_url, data, bdata, fname);
    model = QString::fromLatin1(bdata);
    return res;
}


// filterModels 按照过滤条件查询模型数据，过滤条件表达式形如
// {"Name": Name,"Labels": [Labels], "Owner": Owner, "Ts": [DateTimeFrom, DateTimeTo]}
// 你可以根据需要选择全部或部分条件
bool MySseClient::filterModels(const QString filter, QString& models)
{
    QString request_url = QString("%1/store/filter").arg(m_server);
    QJsonObject data = toQJsonObject(filter);
    data.insert("user", getUser());
    data.insert("token", getToken());
    QByteArray bdata;
    QString fname;
    bool res = postJson(request_url, data, bdata, fname);
    models = QString::fromLatin1(bdata);
    return res;
}

// unloadModel按照模型的Id删除一个模型
bool MySseClient::unloadModel(const QString mid)
{
    QString request_url = QString("%1/store/unload").arg(m_server);
    QJsonObject data;
    data.insert("user", getUser());
    data.insert("token", getToken());
    data.insert("mid", mid);
    QByteArray bdata;
    QString fname;
    bool res = postJson(request_url, data, bdata, fname);
    return res;
}

// listDbLibs返回所有公开的或自建的模型
bool MySseClient::listDbLibs(QString& models)
{
    QString request_url = QString("%1/store/list").arg(m_server);
    QJsonObject data;
    data.insert("user", getUser());
    data.insert("token", getToken());
    QByteArray bdata;
    QString fname;
    bool res = postJson(request_url, data, bdata, fname);
    models = QString::fromLatin1(bdata);
    return res;
}

// createDbLib上传器件数据库到网络服务器，传入参数包括器件数据库的名称name和器件数据库的数据集obj
//
// obj数据集形如下列形式，包括不同的数据表以及数据表下各个器件数据（各个字段中应有一个对外标识字段Name）
// {
//     "T5": [
//         {"Name": "A32", "F29":1086, "F13":104.2, "F40": true},
//         {"Name": "A19", "F29":1267, "F13":118.9, "F40": false}
//     ],
//     "T2": [
//         {"Name": "A06", "F29":1528, "F13":127.4, "F40": true},
//         {"Name": "A15", "F29":2003, "F13":165.1, "F40": true}
//     ]
// }

bool MySseClient::createDbLib(const QString name, const bool is_public, const QJsonObject obj, QString& mid)
{
    QModel dblib_model;
    dblib_model.setName(name);
    dblib_model.setOwner(getUser());
    dblib_model.addLabel("DbLib");
    if (is_public){
        dblib_model.addLabel("Public");
    }

    for (const QString &key : obj.keys()) {
        QString tabname = key;
        QModel dbtab_model;
        dbtab_model.setName(tabname);
        dbtab_model.setOwner(getUser());
        dbtab_model.addLabel("DbTab");
        QJsonValue tabvalue = obj.value(key);
        if (tabvalue.isArray()) {
            QJsonArray records = tabvalue.toArray();
            for (int i = 0; i < records.size(); ++i) {
                const QJsonValue &elm = records.at(i);
                if (elm.isObject()) {
                    QJsonObject record = elm.toObject();

                    QModel dbrec_model;
                    dbrec_model.setOwner(getUser());
                    dbrec_model.addLabel("DbRec");
                    for (const QString& fkey : record.keys()) {
                        QString fieldkey = fkey;
                        if (!record.value(fieldkey).isUndefined()){
                            dbrec_model.addProperty(fieldkey, record.value(fieldkey));
                        }
                        if (fieldkey == "Name"){
                            dbrec_model.setName(record[fieldkey].toString());
                        }
                    }
                    dbtab_model.addChild(dbrec_model);
                } else {
                    qDebug() << "Element" << i << "is of unknown type";
                }
            }
            dblib_model.addChild(dbtab_model);
        } else {
            qDebug() << "Key:" << key << "is of unknown type";
        }
    }
    return uploadModel(dblib_model.toString(), mid);
}

// queryDbLib函数根据器件数据库模型的Id标识(mid)，查询其所附元器件数据。
// 查询返回的器件数据库数据集，形如下列形式，存在obj中。它包括各个数据表以及表下各个器件的Id标识和Name，
// {
//     "T5": [
//         {"Id":"6253789e8cfb4a0fbe8aceab50dd205e", "Name": "A32"}，
//         {"Id":"fe4bd5c84b86404a9eced4730da7b560", "Name": "A19"}
//     ],
//     "T2": [
//         {"Id":"a1c85c9234ca4f6a9ec7ab306dbea168", "Name": "A06"},
//         {"Id":"980d4e8256d74f5a84a8cf6ea115a45b", "Name": "A15"}
//     ]
// }
bool MySseClient::queryDbLib(const QString mid, QString& name, QJsonObject& obj)
{
    QString model;

    if (downloadModel(mid, model)){

        QJsonObject dblib = toQJsonObject(model);

        if (dblib.value("Labels").toArray().contains("DbLib")) {
            name = dblib.value("Name").toString();

            foreach(QJsonValue dbtab, dblib.value("Children").toArray()) {
                QString dbtab_mid = dbtab.toObject()["Id"].toString();

                QString dbtab_model;
                downloadModel(dbtab_mid, dbtab_model);

                QJsonObject dbtab_obj = toQJsonObject(dbtab_model);
                QString dbtab_name = dbtab_obj["Name"].toString();

                QJsonArray dbrec_names;
                foreach(QJsonValue dbrec, dbtab_obj.value("Children").toArray()){

                    QString dbrec_mid = dbrec.toObject().value("Id").toString();
                    QString dbrec_model;
                    downloadModel(dbrec_mid, dbrec_model);
                    QJsonObject dbrec_obj = toQJsonObject(dbrec_model);
                    QString dbrec_name = dbrec_obj.value("Name").toString();
                    QJsonObject dbrec2;
                    dbrec2["Name"] = dbrec_name;
                    dbrec2["Id"] = dbrec_mid;
                    QJsonValue val = dbrec2;
                    dbrec_names.append(val);
                    obj[dbtab_name] = dbrec_names;

                }
            }
            return true;
        }
    }
    return false;
}

// queryDbRec根据器件模型的Id标识(mid)，查询器件数据（包括了各个字段，其中有Name）
// 查询返回的器件模型数据集，形如{"Name": "A32", "F29":1086, "F13":104.2, "F40": true}，包含了期间模型的各个属性值（属性顺序无关，其中有Name）
bool MySseClient::queryDbRec(const QString mid, QJsonObject& obj)
{
    QString model;
    if (downloadModel(mid, model)){
        QJsonObject mod = toQJsonObject(model);
        foreach (QJsonValue kv, mod["Properties"].toArray()) {
            QString k = kv.toObject()["K"].toString();
            QJsonValue v = kv.toObject()["V"];
            obj.insert(k, v);
        }

        return true;
    }
    return false;
}

// product根据产品名称，返回用户所购的产品信息
bool MySseClient::product(const QString product, QString& user_product)
{
    QString request_url = QString("%1/admin/user/product").arg(m_server);
    QJsonObject data;
    data.insert("user", getUser());
    data.insert("token", getToken());
    data.insert("product", product);
    QByteArray bdata;
    QString fname;
    bool res = postJson(request_url, data, bdata, fname);
    user_product = QString::fromLatin1(bdata);
    return res;
}


bool directoryExists(const QString path) {
    QDir dir(path);
    return dir.exists();
}

QJsonObject toQJsonObject(const QString jstr){
    QJsonDocument jdoc = QJsonDocument::fromJson(jstr.toUtf8());
    return jdoc.object();
}

QString toJsonString(const QJsonObject jobj){
    QJsonDocument jsonDoc(jobj);
    return jsonDoc.toJson(QJsonDocument::Compact);
}

