#ifndef JFILE_H
#define JFILE_H

#include "jobject.h"
#include <QFile>

void write_jsonfile(QString filename, QString json_str);

class JFile : public JObject
{
public:
    JFile(QString filename, QString text="{}"):m_qfile(filename){
        if(m_qfile.exists()){
            load();
        }else{
            // warning： non-existed file
            try{
                write_jsonfile(filename, text);
                load();
            }catch(...){
                throw QString("WARN: Failed to create the file %1!").arg(filename);
            }
        }
    };
    bool upsert(QString key, QJsonValue val);
    bool remove(QString key);

protected:
    void load();
    void sync();

private:
    QFile m_qfile;
};


#endif // JFILE_H
