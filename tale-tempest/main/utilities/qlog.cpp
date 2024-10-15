#include "qlog.h"
#include "qapplication.h"
#include <QObject>
#include <QMetaMethod>

QLog::QLog(const QString name)
{
    if (! name.isNull()){
        setObjectName(name);
    }
    m_level = DEBUG;
    m_io = QDBG;
}

QLog::~QLog()
{
    m_logfile.close();
    m_parent = nullptr;
}


void QLog::setParent(QWidget* parent){
    m_parent = parent;
}

QWidget* QLog::getParent()
{
    return m_parent;
}

QString QLog::getParentClassName()
{
    if (getParent()){
        return getParent()->metaObject()->className();
    }
    return "";
}


void QLog::setupLog(QWidget *parent, Level level, IO io, int size, int rot_len)
{
    setParent(parent);
    m_level = level;
    m_io = io;
    m_size = size;
    m_rot_len = rot_len;

    if(io==IO::LDOC){
        QString filename;
        if(parent){
            if(objectName().isNull()){
                filename = getParentClassName() + ".log";
            }else{
                filename = objectName() + ".log";
            }
        }
        else{
            filename = QCoreApplication::applicationName() + ".log";
        }
        setFileName(filename);
    }
}


void create_logfile(QString filename)
{
    QFile file(filename);
    file.open(QFile::WriteOnly | QFile::Text);
    file.close();
}


bool QLog::setFileName(QString filename){
    m_logfile.setFileName(filename);
    if(!m_logfile.exists()){
        // warning： non-existed file
        try{
            create_logfile(filename);
        }catch(...){
            return false;
        }
    }
    return true;
}
