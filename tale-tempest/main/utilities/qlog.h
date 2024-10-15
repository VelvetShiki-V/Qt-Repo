#ifndef QLOG_H
#define QLOG_H

#include <QMetaObject>
#include <QWidget>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>


enum Level{
    DEBUG,
    INFO,
    WARN,
    ERROR,
    CRITICAL
};

enum IO{
    QDBG,
    LDOC
};

class QLog: public QObject
{
Q_OBJECT

public:
QLog(const QString name=nullptr);

    ~QLog();

    void setupLog(QWidget *parent, Level level=DEBUG, IO io=QDBG, int size=64, int rot_len=7);

    // collect log of the Q_INVOKABLE member method
    template<typename RetT, typename... ArgTs>
    inline
    bool invoke(const char* funcName, RetT& ret, ArgTs&... args){
        log(QString(funcName).append(" BEFO"), args...);
        bool r = QMetaObject::invokeMethod(
            m_parent,
            funcName,
            Qt::AutoConnection,
            getRetArg<RetT>(ret),
            getArg<ArgTs>(args)...
        );
        log(QString(funcName).append(" POST"), ret);
        return r;
    }

    template<typename... RTs>
    void line(Level level, RTs... args){
        if (level >= m_level){
            log(args...);
        }
    }

    template<typename... RTs>
    void dLine(RTs... args){
        line(DEBUG, args...);
    }

    template<typename... RTs>
    void iLine(RTs... args){
        line(INFO, args...);
    }

    template<typename... RTs>
    void wLine(RTs... args){
        line(WARN, args...);
    }

    template<typename... RTs>
    void eLine(RTs... args){
        line(ERROR, args...);
    }

    template<typename... RTs>
    void cLine(RTs... args){
        line(CRITICAL, args...);
    }

private:
    template<typename... RTs>
    bool log(QString arg){
        if (m_io==QDBG){
            QDateTime curtime = QDateTime::currentDateTime();
            qDebug() << curtime.toString("yyyy-MM-dd hh:mm:ss") << "|" << arg;
            return true;
        }
        if (m_io==LDOC&&m_logfile.open(QFile::Append | QFile::Text | QFile::Unbuffered)){
            QTextStream out(&m_logfile);
            QDateTime curtime = QDateTime::currentDateTime();
            out << curtime.toString("yyyy-MM-dd hh:mm:ss") << "|" << arg << "\n";
            m_logfile.close();
            if ((m_logfile.size() / 1024) > m_size){
                // remove old backup
                if(m_rot_len > 0){
                    QString originfile, backupfile;
                    backupfile = QString("%1.%2").arg(m_logfile.fileName()).arg(m_rot_len);
                    if (QFile::exists(backupfile))
                        QFile::remove(backupfile);
                    for (int i=m_rot_len-1; i>=0; --i){
                        if(i==0){
                            originfile = m_logfile.fileName();
                        }else{
                            originfile = QString("%1.%2").arg(m_logfile.fileName()).arg(i);
                        }
                        if (QFile::exists(originfile)){
                            backupfile = QString("%1.%2").arg(m_logfile.fileName()).arg(i+1);
                            QFile::rename(originfile, backupfile);
                        }
                    }
                }
            }

            return true;
        }
        return false;
    }

    template<typename... RTs>
    bool log(RTs... args){
        if ((sizeof...(args)) <= 0){
            return log("\n");
        }
        QString s;
        combineStr(s, args...);
        return log(s);
    }

    template<typename T>
    QReturnArgument<T> getRetArg(T& x)
    {
        return QReturnArgument(typeid(x).name(), x);
    }

    template<typename T>
    QArgument<T> getArg(T& x)
    {
        return QArgument(typeid(x).name(), x);
    }

    inline
    QString& combineStr(QString& tot)
    {
        return tot;
    }

    template<typename... RTs>
    inline
    QString& combineStr(QString& tot, QString arg, RTs... r_args){
        if (sizeof...(r_args) > 0){
            tot.append(arg);
            tot.append(",");
            return combineStr(tot, r_args...);
        }else{
            tot.append(arg);
            return tot;
        }
    }

    template<typename... RTs>
    inline
    QString& combineStr(QString& tot, const char* arg, RTs... r_args){
        if (sizeof...(r_args) > 0){
            tot.append(arg);
            tot.append(",");
            return combineStr(tot, r_args...);
        }else{
            tot.append(arg);
            return tot;
        }
    }

    template<typename... RTs>
    inline
    QString& combineStr(QString& tot, bool arg, RTs... r_args){
        if (sizeof...(r_args) > 0){
            tot.append(QString::number(arg));
            tot.append(",");
            return combineStr(tot, r_args...);
        }else{
            tot.append(QString::number(arg));
            return tot;
        }
    }

    template<typename... RTs>
    inline
    QString& combineStr(QString& tot, int arg, RTs... r_args){
        if (sizeof...(r_args) > 0){
            tot.append(QString::number(arg));
            tot.append(",");
            return combineStr(tot, r_args...);
        }else{
            tot.append(QString::number(arg));
            return tot;
        }
    }

    template<typename... RTs>
    inline
    QString& combineStr(QString& tot, double arg, RTs... r_args){
        if (sizeof...(r_args) > 0){
            tot.append(QString::number(arg));
            tot.append(",");
            return combineStr(tot, r_args...);
        }else{
            tot.append(QString::number(arg));
            return tot;
        }
    }

    // TOMODIFY
    // WARN: some problem exists about QVariant!
    template<typename T, typename... RTs>
    inline
    QString& combineStr(QString& tot, T arg, RTs... r_args){
        QVariant v;
        v.fromValue(arg);
        QString s = v.toString();
        if (sizeof...(r_args) > 0){
            tot.append(s);
            tot.append(",");
            return combineStr(tot, r_args...);
        }else{
            tot.append(s);
            return tot;
        }
    }

    QString getParentClassName();
    void setParent(QWidget *parent);
    QWidget* getParent();
    bool setFileName(const QString filename);

    QWidget *m_parent;
    QFile m_logfile;
    int m_size;
    Level m_level;
    IO m_io;
    int m_rot_len;
};

#endif // QLOG_H
