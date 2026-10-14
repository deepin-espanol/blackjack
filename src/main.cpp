#include "MainWindow.h"
#include <DApplication>
#include <DWidgetUtil>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[]) {
    DApplication app(argc, argv);
    app.setApplicationName("blackjack");
    app.setApplicationDisplayName("BlackJack 21");
    app.setOrganizationName("deepin");

    MainWindow w;
    w.show();

    Dtk::Widget::moveToCenter(&w);

    return app.exec();
}
