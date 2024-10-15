#ifndef QPY_H
#define QPY_H

#include <QJsonObject>
#include "qlog.h"

bool runPython(const QString interp, const QString wdir, const QString script, const int ts_ms, const QJsonObject input, QJsonObject& output, QLog &logger);

#endif // QPY_H
