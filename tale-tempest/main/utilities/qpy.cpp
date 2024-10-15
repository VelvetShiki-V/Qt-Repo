#include <QProcess>
#include <QString>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QIODevice>
#include <QFile>
#include <QJsonParseError>
#include "qlog.h"
#include "qpy.h"

void write_file(QString filename, QString str)
{
    QFile file(filename);
    file.open(QFile::WriteOnly | QFile::Text);
    QTextStream out(&file);
    out << str;
    file.close();
}

bool runPython(const QString interp, const QString wdir, const QString script, const int ts_ms, const QJsonObject input, QJsonObject& output, QLog &logger)
{
    logger.iLine("runPython BEG");
    QProcess process;
    QString program = interp;
    QStringList arguments;
    arguments << script;
    process.setProgram(program);
    process.setArguments({script});
    process.setWorkingDirectory(wdir);
    process.start();
    if (!process.waitForStarted()) {
        QString error = "Failed to start process";
        logger.eLine(error);
        return false;
    }

    QJsonDocument jsonDoc(input);
    QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Compact);
    process.write(jsonData + '\n');
    process.closeWriteChannel();
    if (process.waitForReadyRead(ts_ms)) {
        QByteArray output_buf = process.readAllStandardOutput();
        QJsonParseError parseError;
        QJsonDocument jsonResponseDoc = QJsonDocument::fromJson(output_buf, &parseError);
        if (parseError.error == QJsonParseError::NoError) {
            output = jsonResponseDoc.object();
        } else {
            QString error = parseError.errorString();
            logger.eLine(error);
            return false;
        }
    } else {
        QString error = "No output from python within timeout period";
        logger.wLine(error);
        return false;
    }
    process.waitForFinished();
    logger.iLine("runPython END");
    return true;
}
