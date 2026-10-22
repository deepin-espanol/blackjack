#include "MainWindow.h"
#include <DAboutDialog>
#include <DApplication>
#include <DWidgetUtil>
#include <QLocale>
#include <QTranslator>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[]) {
    DApplication app(argc, argv);
    app.setOrganizationName("deepinenespanol");
    app.setApplicationName("blackjack");
    app.setApplicationVersion("0.2.0");

    QTranslator translator;
    if (translator.load(QLocale(), QStringLiteral("blackjack"), QStringLiteral("_"),
                        QStringLiteral(":/translations")))
        app.installTranslator(&translator);

    app.setApplicationDisplayName(QObject::tr("BlackJack 21"));
    app.setProductIcon(QIcon(":/icons/blackjack.svg"));
    app.setWindowIcon(QIcon(":/icons/blackjack.svg"));
    app.setProductName(QObject::tr("BlackJack 21"));

    DAboutDialog dialog;
    app.setAboutDialog(&dialog);
    dialog.setProductName(QObject::tr("BlackJack 21"));
    dialog.setProductIcon(QIcon(":/icons/blackjack.svg"));
    dialog.setVersion(DApplication::buildVersion(QStringLiteral("0.2.0")));
    dialog.setDescription(
        "<div style='text-align:center;'><img src=':/icons/Logo-Racoon.png' width='128'/></div>"
        "<span style=' font-size:8pt; font-weight:600;'>Deepin en Español </span>"
        "<a href='https://deepinenespanol.org'>https://deepinenespanol.org</a><br/>"
        "<span style=' font-size:8pt; font-weight:600;'>Deepin Latin Code - developers</span>");
    dialog.setWebsiteName("deepinenespanol.org");
    dialog.setWebsiteLink("https://deepinenespanol.org");

    MainWindow w;
    w.show();
    Dtk::Widget::moveToCenter(&w);

    return app.exec();
}
