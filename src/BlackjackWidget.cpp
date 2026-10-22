#include "BlackjackWidget.h"
#include "CartaPainter.h"

#include <QPainter>
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSettings>

namespace {
const int ANCHO = 760;
const int ALTO = 520;
const int CARTA_ANCHO = 84;
const int CARTA_ALTO = 120;
const int Y_DEALER = 52;
const int Y_JUGADOR = 262;
}

BlackjackWidget::BlackjackWidget(QWidget *parent)
    : QWidget(parent)
    , m_mazo(6)
    , m_fichas(QSettings().value("fichas", FICHAS_INICIALES).toInt())
    , m_apuesta(QSettings().value("apuesta", 50).toInt())
    , m_ultimoResultado(0)
    , m_estado(Estado::Apostando)
{
    setFixedSize(ANCHO, ALTO);
    setFocusPolicy(Qt::StrongFocus);

    auto *barra = new QHBoxLayout();
    barra->setSpacing(8);
    auto crear = [this, barra](const QString &texto, void (BlackjackWidget::*slot)()) {
        auto *b = new QPushButton(texto, this);
        b->setFocusPolicy(Qt::NoFocus);
        b->setMinimumHeight(36);
        connect(b, &QPushButton::clicked, this, slot);
        barra->addWidget(b);
        return b;
    };
    m_btnMenos = crear(tr("− Apuesta"), &BlackjackWidget::reducirApuesta);
    m_btnMas = crear(tr("+ Apuesta"), &BlackjackWidget::aumentarApuesta);
    m_btnRepartir = crear(tr("Repartir"), &BlackjackWidget::repartir);
    barra->addSpacing(20);
    m_btnPedir = crear(tr("Pedir (H)"), &BlackjackWidget::pedirCarta);
    m_btnPlantarse = crear(tr("Plantarse (S)"), &BlackjackWidget::plantarse);
    m_btnDoblar = crear(tr("Doblar (D)"), &BlackjackWidget::doblar);
    m_btnSiguiente = crear(tr("Nueva ronda"), &BlackjackWidget::siguienteRonda);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 0, 16, 14);
    layout->addStretch();
    layout->addLayout(barra);

    if (m_fichas < APUESTA_MINIMA)
        m_fichas = FICHAS_INICIALES;
    ajustarApuesta();
    m_mensaje = tr("Elige tu apuesta y pulsa Repartir");
    actualizarBotones();
}

void BlackjackWidget::guardar()
{
    QSettings s;
    s.setValue("fichas", m_fichas);
    s.setValue("apuesta", m_apuesta);
}

// La apuesta va en pasos de 10 y nunca supera las fichas disponibles
void BlackjackWidget::ajustarApuesta()
{
    m_apuesta = qBound(APUESTA_MINIMA, (m_apuesta / APUESTA_MINIMA) * APUESTA_MINIMA,
                       qMax(APUESTA_MINIMA, (m_fichas / APUESTA_MINIMA) * APUESTA_MINIMA));
}

void BlackjackWidget::nuevaPartida()
{
    m_fichas = FICHAS_INICIALES;
    m_apuesta = 50;
    m_manoJugador.clear();
    m_manoDealer.clear();
    m_estado = Estado::Apostando;
    m_mazo.reiniciar();
    m_mensaje = tr("Nueva partida: tienes %1 fichas").arg(m_fichas);
    guardar();
    emit fichasCambiadas(m_fichas);
    actualizarBotones();
    update();
}

void BlackjackWidget::aumentarApuesta()
{
    if (m_estado != Estado::Apostando) return;
    const int paso = m_apuesta < 100 ? 10 : 50;
    if (m_apuesta + paso <= m_fichas) m_apuesta += paso;
    actualizarBotones();
    update();
}

void BlackjackWidget::reducirApuesta()
{
    if (m_estado != Estado::Apostando) return;
    const int paso = m_apuesta <= 100 ? 10 : 50;
    m_apuesta = qMax(APUESTA_MINIMA, m_apuesta - paso);
    actualizarBotones();
    update();
}

void BlackjackWidget::repartir()
{
    if (m_estado != Estado::Apostando || m_apuesta > m_fichas) return;

    if (m_mazo.necesitaBarajar())
        m_mazo.reiniciar();

    m_manoJugador = {m_mazo.repartir()};
    m_manoDealer = {m_mazo.repartir()};
    m_manoJugador.append(m_mazo.repartir());
    m_manoDealer.append(m_mazo.repartir());
    m_mensaje.clear();
    m_estado = Estado::Jugando;
    guardar();

    // Blackjack de cualquiera de los dos termina la mano en el acto
    if (esBlackjack(m_manoJugador) || esBlackjack(m_manoDealer))
        resolver();

    actualizarBotones();
    update();
}

void BlackjackWidget::pedirCarta()
{
    if (m_estado != Estado::Jugando) return;

    m_manoJugador.append(m_mazo.repartir());
    const int puntos = calcularPuntos(m_manoJugador);
    if (puntos > 21)
        resolver();
    else if (puntos == 21)
        plantarse();  // con 21 no tiene sentido seguir pidiendo
    actualizarBotones();
    update();
}

void BlackjackWidget::plantarse()
{
    if (m_estado != Estado::Jugando) return;
    turnoDealer();
    resolver();
    actualizarBotones();
    update();
}

// Doblar: se duplica la apuesta, se recibe exactamente una carta y se planta
void BlackjackWidget::doblar()
{
    if (m_estado != Estado::Jugando || m_manoJugador.size() != 2 || m_apuesta * 2 > m_fichas)
        return;
    m_apuesta *= 2;
    m_manoJugador.append(m_mazo.repartir());
    if (calcularPuntos(m_manoJugador) <= 21)
        turnoDealer();
    resolver();
    m_apuesta /= 2;  // la siguiente ronda vuelve a la apuesta base
    actualizarBotones();
    update();
}

// El crupier pide hasta 17 y se planta con cualquier 17 (regla S17)
void BlackjackWidget::turnoDealer()
{
    while (calcularPuntos(m_manoDealer) < 17)
        m_manoDealer.append(m_mazo.repartir());
}

void BlackjackWidget::pagar(int cambioFichas, const QString &texto)
{
    m_fichas += cambioFichas;
    m_ultimoResultado = cambioFichas;
    m_mensaje = texto;
    m_estado = Estado::Terminado;
    guardar();
    emit fichasCambiadas(m_fichas);
}

void BlackjackWidget::resolver()
{
    const int jugador = calcularPuntos(m_manoJugador);
    const int dealer = calcularPuntos(m_manoDealer);
    const bool bjJugador = esBlackjack(m_manoJugador);
    const bool bjDealer = esBlackjack(m_manoDealer);

    if (bjJugador && bjDealer)
        pagar(0, tr("Los dos tenéis Blackjack. Empate."));
    else if (bjJugador)
        pagar(m_apuesta * 3 / 2, tr("¡Blackjack! Ganas %1 fichas (paga 3:2).").arg(m_apuesta * 3 / 2));
    else if (bjDealer)
        pagar(-m_apuesta, tr("El crupier tiene Blackjack. Pierdes %1 fichas.").arg(m_apuesta));
    else if (jugador > 21)
        pagar(-m_apuesta, tr("Te pasaste de 21. Pierdes %1 fichas.").arg(m_apuesta));
    else if (dealer > 21)
        pagar(m_apuesta, tr("El crupier se pasa. Ganas %1 fichas.").arg(m_apuesta));
    else if (jugador > dealer)
        pagar(m_apuesta, tr("¡Ganas %1 fichas! (%2 contra %3)").arg(m_apuesta).arg(jugador).arg(dealer));
    else if (jugador < dealer)
        pagar(-m_apuesta, tr("Gana el crupier (%1 contra %2). Pierdes %3 fichas.").arg(dealer).arg(jugador).arg(m_apuesta));
    else
        pagar(0, tr("Empate a %1. Recuperas tu apuesta.").arg(jugador));

    if (m_fichas < APUESTA_MINIMA)
        m_mensaje += QLatin1Char('\n') + tr("Te quedaste sin fichas. Pulsa Nueva partida.");
}

void BlackjackWidget::siguienteRonda()
{
    if (m_estado != Estado::Terminado) return;
    if (m_fichas < APUESTA_MINIMA) {
        nuevaPartida();
        return;
    }
    m_manoJugador.clear();
    m_manoDealer.clear();
    m_estado = Estado::Apostando;
    ajustarApuesta();
    m_mensaje = tr("Elige tu apuesta y pulsa Repartir");
    actualizarBotones();
    update();
}

int BlackjackWidget::calcularPuntos(const QVector<Carta> &mano, bool *blanda)
{
    int total = 0;
    bool hayAs = false;
    for (const Carta &carta : mano) {
        total += carta.valorBlackjack();
        hayAs = hayAs || carta.esAs();
    }
    // Un As vale 11 si no hace pasarse (mano "blanda")
    const bool usarOnce = hayAs && total + 10 <= 21;
    if (blanda) *blanda = usarOnce;
    return usarOnce ? total + 10 : total;
}

bool BlackjackWidget::esBlackjack(const QVector<Carta> &mano)
{
    return mano.size() == 2 && calcularPuntos(mano) == 21;
}

void BlackjackWidget::actualizarBotones()
{
    const bool apostando = m_estado == Estado::Apostando;
    const bool jugando = m_estado == Estado::Jugando;
    m_btnMenos->setVisible(apostando);
    m_btnMas->setVisible(apostando);
    m_btnRepartir->setVisible(apostando);
    m_btnMenos->setEnabled(m_apuesta > APUESTA_MINIMA);
    m_btnMas->setEnabled(m_apuesta + (m_apuesta < 100 ? 10 : 50) <= m_fichas);
    m_btnRepartir->setEnabled(m_apuesta <= m_fichas);

    m_btnPedir->setVisible(jugando);
    m_btnPlantarse->setVisible(jugando);
    m_btnDoblar->setVisible(jugando);
    m_btnDoblar->setEnabled(jugando && m_manoJugador.size() == 2 && m_apuesta * 2 <= m_fichas);

    m_btnSiguiente->setVisible(m_estado == Estado::Terminado);
    m_btnSiguiente->setText(m_fichas < APUESTA_MINIMA ? tr("Nueva partida") : tr("Nueva ronda (Enter)"));
}

void BlackjackWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_H: pedirCarta(); break;
    case Qt::Key_S: plantarse(); break;
    case Qt::Key_D: doblar(); break;
    case Qt::Key_Plus: case Qt::Key_Up: aumentarApuesta(); break;
    case Qt::Key_Minus: case Qt::Key_Down: reducirApuesta(); break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        if (m_estado == Estado::Apostando) repartir();
        else if (m_estado == Estado::Terminado) siguienteRonda();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void BlackjackWidget::dibujarMano(QPainter &p, const QVector<Carta> &mano, int y, bool ocultarSegunda)
{
    if (mano.isEmpty()) return;
    // Las cartas se solapan si la mano es larga para que siempre quepan
    const int n = mano.size();
    const int disponible = ANCHO - 2 * 40;
    const int paso = qMin(CARTA_ANCHO + 14, n > 1 ? (disponible - CARTA_ANCHO) / (n - 1) : CARTA_ANCHO);
    const int total = CARTA_ANCHO + (n - 1) * paso;
    int x = (ANCHO - total) / 2;
    for (int i = 0; i < n; ++i, x += paso)
        CartaPainter::dibujar(p, mano[i], QRectF(x, y, CARTA_ANCHO, CARTA_ALTO), !(ocultarSegunda && i == 1));
}

void BlackjackWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Tapete
    QRadialGradient fondo(rect().center(), ANCHO * 0.7);
    fondo.setColorAt(0, QColor(30, 120, 60));
    fondo.setColorAt(1, QColor(10, 70, 35));
    p.fillRect(rect(), fondo);
    p.setPen(QPen(QColor(255, 255, 255, 40), 2));
    p.setBrush(Qt::NoBrush);
    p.drawArc(QRectF(-ANCHO * 0.1, ALTO * 0.35, ANCHO * 1.2, ALTO * 0.9), 20 * 16, 140 * 16);

    QFont f = p.font();
    f.setPixelSize(15);
    f.setBold(true);
    p.setFont(f);
    p.setPen(QColor(235, 235, 220));

    const bool oculta = m_estado == Estado::Jugando;
    if (!m_manoDealer.isEmpty()) {
        // Mientras la segunda carta esta oculta solo se muestra el valor de la visible
        const QString pd = oculta ? QString::number(calcularPuntos({m_manoDealer.first()})) + QStringLiteral(" + ?")
                                  : QString::number(calcularPuntos(m_manoDealer));
        p.drawText(QRect(0, Y_DEALER - 30, ANCHO, 24), Qt::AlignCenter, tr("Crupier: %1").arg(pd));
    } else {
        p.drawText(QRect(0, Y_DEALER - 30, ANCHO, 24), Qt::AlignCenter, tr("Crupier"));
    }
    dibujarMano(p, m_manoDealer, Y_DEALER, oculta);

    if (!m_manoJugador.isEmpty()) {
        bool blanda = false;
        const int pj = calcularPuntos(m_manoJugador, &blanda);
        const QString txt = (blanda && pj < 21) ? QStringLiteral("%1/%2").arg(pj - 10).arg(pj) : QString::number(pj);
        p.drawText(QRect(0, Y_JUGADOR + CARTA_ALTO + 6, ANCHO, 24), Qt::AlignCenter, tr("Tú: %1").arg(txt));
    }
    dibujarMano(p, m_manoJugador, Y_JUGADOR, false);

    // Franja de mensaje y apuesta
    const QRect franja(0, Y_DEALER + CARTA_ALTO + 14, ANCHO, Y_JUGADOR - Y_DEALER - CARTA_ALTO - 28);
    if (!m_mensaje.isEmpty()) {
        QColor c(255, 225, 90);
        if (m_estado == Estado::Terminado)
            c = m_ultimoResultado > 0 ? QColor(140, 255, 140) : m_ultimoResultado < 0 ? QColor(255, 150, 140) : QColor(230, 230, 230);
        p.setPen(c);
        f.setPixelSize(17);
        p.setFont(f);
        p.drawText(franja, Qt::AlignCenter, m_mensaje);
    }

    // Ficha con la apuesta actual
    const QPointF centroFicha(70, Y_JUGADOR + CARTA_ALTO / 2);
    p.setPen(QPen(Qt::white, 3, Qt::DashLine));
    p.setBrush(QColor(190, 30, 40));
    p.drawEllipse(centroFicha, 34, 34);
    p.setPen(Qt::white);
    f.setPixelSize(16);
    p.setFont(f);
    p.drawText(QRectF(centroFicha.x() - 34, centroFicha.y() - 34, 68, 68), Qt::AlignCenter,
               QString::number(m_apuesta));
    f.setPixelSize(11);
    f.setBold(false);
    p.setFont(f);
    p.drawText(QRectF(centroFicha.x() - 50, centroFicha.y() + 36, 100, 18), Qt::AlignCenter, tr("apuesta"));
}
