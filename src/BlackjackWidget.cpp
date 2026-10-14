#include "BlackjackWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <QRandomGenerator>

BlackjackWidget::BlackjackWidget(QWidget *parent)
    : QWidget(parent), fichas(1000), apuesta(50), estado(Estado::Apostando) {
    setFixedSize(700, 500);

    btnApostarMas = new QPushButton("+50", this);
    btnApostarMas->setGeometry(20, 440, 80, 40);
    connect(btnApostarMas, &QPushButton::clicked, this, &BlackjackWidget::aumentarApuesta);

    btnApostarMenos = new QPushButton("-50", this);
    btnApostarMenos->setGeometry(110, 440, 80, 40);
    connect(btnApostarMenos, &QPushButton::clicked, this, &BlackjackWidget::reducirApuesta);

    btnPedir = new QPushButton("Pedir", this);
    btnPedir->setGeometry(250, 440, 100, 40);
    connect(btnPedir, &QPushButton::clicked, this, &BlackjackWidget::pedirCarta);

    btnPlantarse = new QPushButton("Plantarse", this);
    btnPlantarse->setGeometry(360, 440, 100, 40);
    connect(btnPlantarse, &QPushButton::clicked, this, &BlackjackWidget::plantarse);

    btnNuevaRonda = new QPushButton("Nueva Ronda", this);
    btnNuevaRonda->setGeometry(500, 440, 120, 40);
    connect(btnNuevaRonda, &QPushButton::clicked, this, &BlackjackWidget::nuevaRonda);

    lblFichas = new QLabel(this);
    lblFichas->setGeometry(20, 10, 300, 30);
    lblFichas->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");

    lblMensaje = new QLabel(this);
    lblMensaje->setGeometry(200, 10, 300, 30);
    lblMensaje->setStyleSheet("color: yellow; font-size: 16px; font-weight: bold;");
    lblMensaje->setAlignment(Qt::AlignCenter);

    actualizarBotones();
    lblFichas->setText(QString("Fichas: %1 | Apuesta: %2").arg(fichas).arg(apuesta));
}

void BlackjackWidget::aumentarApuesta() {
    if (estado == Estado::Apostando && fichas >= apuesta + 50) {
        apuesta += 50;
        lblFichas->setText(QString("Fichas: %1 | Apuesta: %2").arg(fichas).arg(apuesta));
    }
}

void BlackjackWidget::reducirApuesta() {
    if (estado == Estado::Apostando && apuesta > 50) {
        apuesta -= 50;
        lblFichas->setText(QString("Fichas: %1 | Apuesta: %2").arg(fichas).arg(apuesta));
    }
}

void BlackjackWidget::pedirCarta() {
    if (estado != Estado::Jugando) return;

    manoJugador.append(mazo.repartir());
    int puntos = calcularPuntos(manoJugador);
    if (puntos > 21) {
        estado = Estado::Terminado;
        fichas -= apuesta;
        mensaje = "¡Te pasaste de 21! Pierdes " + QString::number(apuesta) + " fichas.";
        emit fichasCambiadas(fichas);
    }
    actualizarBotones();
    update();
}

void BlackjackWidget::plantarse() {
    if (estado != Estado::Jugando) return;

    turnoDealer();
    actualizarBotones();
    update();
}

void BlackjackWidget::nuevaRonda() {
    manoJugador.clear();
    manoDealer.clear();
    mensaje.clear();
    repartirCartasIniciales();
    lblFichas->setText(QString("Fichas: %1 | Apuesta: %2").arg(fichas).arg(apuesta));
    update();
}

void BlackjackWidget::repartirCartasIniciales() {
    mazo.reiniciar();
    manoJugador.clear();
    manoDealer.clear();

    manoJugador.append(mazo.repartir());
    manoDealer.append(mazo.repartir());
    manoJugador.append(mazo.repartir());
    manoDealer.append(mazo.repartir());

    estado = Estado::Jugando;
    actualizarBotones();

    if (esBlackjack(manoJugador)) {
        turnoDealer();
        actualizarBotones();
    }
}

void BlackjackWidget::turnoDealer() {
    while (calcularPuntos(manoDealer) < 17) {
        manoDealer.append(mazo.repartir());
    }

    int puntosJugador = calcularPuntos(manoJugador);
    int puntosDealer = calcularPuntos(manoDealer);
    bool jugadorBlackjack = esBlackjack(manoJugador);

    if (jugadorBlackjack && !esBlackjack(manoDealer)) {
        int ganancia = static_cast<int>(apuesta * 2.5);
        fichas += ganancia;
        mensaje = "¡Blackjack natural! Ganaste " + QString::number(ganancia) + " fichas.";
    } else if (puntosJugador > 21) {
        mensaje = "¡Te pasaste! Pierdes " + QString::number(apuesta) + " fichas.";
    } else if (puntosDealer > 21) {
        fichas += apuesta;
        mensaje = "¡Dealer se pasa! Ganaste " + QString::number(apuesta) + " fichas.";
    } else if (puntosJugador > puntosDealer) {
        fichas += apuesta;
        mensaje = "¡Ganaste! Ganaste " + QString::number(apuesta) + " fichas.";
    } else if (puntosJugador < puntosDealer) {
        fichas -= apuesta;
        mensaje = "¡Dealer gana! Pierdes " + QString::number(apuesta) + " fichas.";
    } else {
        mensaje = "Empate. Tus fichas se devuelven.";
    }

    emit fichasCambiadas(fichas);
    estado = Estado::Terminado;
}

int BlackjackWidget::calcularPuntos(const QVector<Carta> &mano) {
    int total = 0;
    int ases = 0;

    for (const Carta &carta : mano) {
        total += carta.valorBlackjack();
        if (carta.esAs()) ases++;
    }

    while (ases > 0 && total + 10 <= 21) {
        total += 10;
        ases--;
    }

    return total;
}

bool BlackjackWidget::esBlackjack(const QVector<Carta> &mano) {
    return mano.size() == 2 && calcularPuntos(mano) == 21;
}

void BlackjackWidget::actualizarBotones() {
    btnApostarMas->setEnabled(estado == Estado::Apostando);
    btnApostarMenos->setEnabled(estado == Estado::Apostando);
    btnPedir->setEnabled(estado == Estado::Jugando);
    btnPlantarse->setEnabled(estado == Estado::Jugando);
    btnNuevaRonda->setEnabled(estado == Estado::Terminado);

    if (fichas <= 0 && estado == Estado::Terminado) {
        mensaje = "¡Te quedaste sin fichas! Juego terminado.";
        btnNuevaRonda->setEnabled(false);
    }
}

void BlackjackWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), QColor(0, 100, 0));

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.drawText(20, 50, "Dealer:");

    painter.drawText(20, 280, "Jugador:");

    if (estado == Estado::Jugando || estado == Estado::Terminado) {
        for (int i = 0; i < manoDealer.size(); ++i) {
            bool bocaArriba = (i != 1 || estado == Estado::Terminado);
            dibujarCarta(painter, manoDealer[i], 20 + i * 80, 60, bocaArriba);
        }

        for (int i = 0; i < manoJugador.size(); ++i) {
            dibujarCarta(painter, manoJugador[i], 20 + i * 80, 290, true);
        }

        painter.setFont(QFont("Arial", 12));
        painter.drawText(20, 270, "Dealer: " + QString::number(calcularPuntos(manoDealer)));
        painter.drawText(20, 430, "Jugador: " + QString::number(calcularPuntos(manoJugador)));
    }

    if (!mensaje.isEmpty()) {
        painter.setPen(QColor(255, 255, 0));
        painter.setFont(QFont("Arial", 16, QFont::Bold));
        painter.drawText(rect(), Qt::AlignHCenter | Qt::AlignVCenter, mensaje);
    }
}

void BlackjackWidget::dibujarCarta(QPainter &painter, const Carta &carta, int x, int y, bool bocaArriba) {
    painter.setPen(Qt::black);
    painter.setBrush(bocaArriba ? Qt::white : QColor(0, 0, 150));
    painter.drawRoundedRect(x, y, 70, 100, 8, 8);

    if (bocaArriba) {
        QColor colorPalo = (carta.palo == Palo::Corazones || carta.palo == Palo::Diamantes)
                            ? QColor(200, 0, 0) : QColor(0, 0, 0);
        painter.setPen(colorPalo);
        painter.setFont(QFont("Arial", 16, QFont::Bold));
        painter.drawText(QRect(x, y + 5, 70, 30), Qt::AlignCenter, carta.nombreValor());
        painter.setFont(QFont("Arial", 20));
        painter.drawText(QRect(x, y + 30, 70, 40), Qt::AlignCenter, carta.nombrePalo());
    }
}
