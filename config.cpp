#include "config.h"
#include <QFile>
#include <QTextStream>

Config::Config(const QString &filename)
{
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            int pos = line.indexOf('=');
            if (pos == -1) continue;
            QString key = line.left(pos).trimmed();
            QString value = line.mid(pos + 1).trimmed();
            data[key] = value;
        }
    }
}

QString Config::get(const QString &key) const
{
    return data.value(key, QString());
}

void Config::set(const QString &key, const QString &value)
{
    data[key] = value;
}

void Config::save(const QString &filename) const
{
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (auto i = data.begin(); i != data.end(); ++i) {
            out << i.key() << "=" << i.value() << "\n";
        }
    }
}
