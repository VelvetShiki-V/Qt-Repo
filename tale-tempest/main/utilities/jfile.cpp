#include "jfile.h"

void JFile::sync()
{
    m_qfile.open(QFile::WriteOnly | QFile::Text | QFile::Unbuffered);
    QTextStream out(&m_qfile);
    out << text();
    m_qfile.close();
}

void JFile::load()
{
    m_qfile.open(QFile::ReadOnly | QFile::Text | QFile::Unbuffered);
    QTextStream ts(&m_qfile);
    QString text = ts.readAll();
    m_jobject = load_json(text);
    m_qfile.close();
}

bool JFile::upsert(QString key, QJsonValue val)
{
    if (JObject::upsert(key, val)){
        sync();
        return true;
    }
    return false;
}

bool JFile::remove(QString key)
{
    return upsert(key, QJsonValue::Undefined);
}

bool file_exists(QString filename)
{
    QFile file(filename);
    return file.exists();
}

QString read_jsonfile(QString filename)
{
    QFile file(filename);
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&file);
    QString text = ts.readAll();
    file.close();
    return text;
}

void write_jsonfile(QString filename, QString json_str)
{
    QFile file(filename);
    file.open(QFile::WriteOnly | QFile::Text);
    QTextStream out(&file);
    out << json_str;
    file.close();
}

void create_jsonfile(QString filename)
{
    write_jsonfile(filename, "{}");
}
