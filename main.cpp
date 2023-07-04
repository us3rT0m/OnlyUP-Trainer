#include "mainwindow.h"
#include "config.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <Windows.h>

bool isUserAdmin()
{
    BOOL isAdmin = FALSE;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    PSID adminGroup;
    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                                 DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup))
    {
        if (!CheckTokenMembership(NULL, adminGroup, &isAdmin))
        {
            isAdmin = FALSE;
        }
        FreeSid(adminGroup);
    }
    return isAdmin == TRUE;
}

QTranslator translator;
Config config("config.ini");

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    #if defined(QT_DEBUG)
        // Redirection de la sortie de débogage vers la console
        qSetMessagePattern("%{time yyyy-MM-dd hh:mm:ss.zzz} %{type} %{threadid} %{function}(): %{message}");
    #endif

    QString lang = config.get("lang");

    QLocale locale;
    if (lang == "en") {
        locale = QLocale(QLocale::English, QLocale::UnitedKingdom);
    } else if (lang == "zh") {
        locale = QLocale(QLocale::Chinese, QLocale::China);
    } else if (lang == "ja") {
        locale = QLocale(QLocale::Japanese, QLocale::Japan);
    }

    const QString baseName = "OnlyUP_Trainer_" + locale.name();
    if (translator.load(":/i18n/" + baseName)) {
        a.installTranslator(&translator);
    } else {
        qDebug() << "Failed to load translation:" << baseName;
    }

    if (!isUserAdmin())
    {
        QMessageBox::critical(nullptr, QObject::tr("Erreur"), QObject::tr("This program requires administrator privileges."));
        return -1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
