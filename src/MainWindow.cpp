#include "MainWindow.h"
#include "BlackjackWidget.h"

#include <DTitlebar>
#include <QAction>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent) {
    setWindowTitle(tr("BlackJack 21"));
    setWindowIcon(QIcon(":/icons/blackjack.svg"));

    blackjackWidget = new BlackjackWidget(this);
    setCentralWidget(blackjackWidget);

    lblFichasMenu = new QLabel(this);
    QFont f = lblFichasMenu->font();
    f.setBold(true);
    lblFichasMenu->setFont(f);
    lblFichasMenu->setContentsMargins(14, 0, 0, 0);
    titlebar()->addWidget(lblFichasMenu, Qt::AlignLeft | Qt::AlignVCenter);
    auto actualizarFichas = [this](int fichas) {
        lblFichasMenu->setText(tr("Fichas: %1").arg(fichas));
    };
    actualizarFichas(blackjackWidget->fichas());
    connect(blackjackWidget, &BlackjackWidget::fichasCambiadas, this, actualizarFichas);

    QMenu *menu = new QMenu(this);
    QAction *nueva = menu->addAction(tr("Nueva partida (1000 fichas)"));
    nueva->setShortcut(QKeySequence("Ctrl+N"));
    connect(nueva, &QAction::triggered, blackjackWidget, &BlackjackWidget::nuevaPartida);
    menu->addSeparator();
    QAction *salir = menu->addAction(tr("Salir"));
    salir->setShortcut(QKeySequence("Ctrl+Q"));
    connect(salir, &QAction::triggered, this, &QWidget::close);
    titlebar()->setMenu(menu);
    // Los atajos del menu de la barra de titulo solo funcionan con el menu abierto
    addAction(nueva);
    addAction(salir);

    // El tamano de la ventana incluye la barra de titulo de DTK
    setFixedSize(blackjackWidget->width(), blackjackWidget->height() + titlebar()->sizeHint().height());
    blackjackWidget->setFocus();
}
