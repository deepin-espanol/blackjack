#include "MainWindow.h"
#include "BlackjackWidget.h"
#include <DTitlebar>

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent) {
    setWindowTitle("BlackJack 21");
    setMinimumSize(720, 560);

    blackjackWidget = new BlackjackWidget(this);
    setCentralWidget(blackjackWidget);

    lblFichasMenu = new QLabel("Fichas: 1000", this);
    lblFichasMenu->setStyleSheet("color: white; font-size: 14px; font-weight: bold; padding: 4px;");
    lblFichasMenu->setFixedWidth(200);
    titlebar()->addWidget(lblFichasMenu, Qt::AlignLeft | Qt::AlignVCenter);

    connect(blackjackWidget, &BlackjackWidget::fichasCambiadas, this, [this](int fichas) {
        lblFichasMenu->setText(QString("Fichas: %1").arg(fichas));
    });
}
