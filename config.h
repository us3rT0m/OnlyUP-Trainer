#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QMap>

class Config
{
public:
    Config(const QString &filename);

    QString get(const QString &key) const;
    void set(const QString &key, const QString &value);
    void save(const QString &filename) const;

private:
    QMap<QString, QString> data;
};

#endif // CONFIG_H
