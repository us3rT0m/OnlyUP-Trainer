#include "mainwindow.h"
#include "config.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QFile>
#include <QMessageBox>
#include <QDebug>

#include "keyboardmapper.h"

QTranslator translator;
Config config("config.ini");

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KeyboardMapper kbm;

    #if defined(QT_DEBUG)
        // Redirection de la sortie de débogage vers la console
        qSetMessagePattern("%{time yyyy-MM-dd hh:mm:ss.zzz} %{type} %{threadid} %{function}(): %{message}");
    #endif

    QString lang = config.get("lang");

    QLocale locale;
    if (lang == "en") {
        locale = QLocale(QLocale::English, QLocale::UnitedKingdom);
    } else if (lang == "fr") {
        locale = QLocale(QLocale::French, QLocale::France);
    } else if (lang == "zh") {
        locale = QLocale(QLocale::Chinese, QLocale::China);
    }

    const QString baseName = "OnlyUP_Trainer_" + locale.name();
    if (translator.load(":/i18n/" + baseName)) {
        a.installTranslator(&translator);
    } else {
        qDebug() << "Failed to load translation:" << baseName;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
