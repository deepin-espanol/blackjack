#ifndef CARTAPAINTER_H
#define CARTAPAINTER_H

#include "Carta.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>

// Dibuja una carta de baraja francesa en el rectangulo dado.
namespace CartaPainter {

inline void dibujarReverso(QPainter &p, const QRectF &r)
{
    const qreal radio = r.width() * 0.09;
    p.setPen(QPen(QColor(20, 20, 60), 1.2));
    QLinearGradient g(r.topLeft(), r.bottomRight());
    g.setColorAt(0, QColor(40, 70, 170));
    g.setColorAt(1, QColor(20, 40, 110));
    p.setBrush(g);
    p.drawRoundedRect(r, radio, radio);

    // Trama de rombos
    QRectF in = r.adjusted(r.width() * 0.1, r.width() * 0.1, -r.width() * 0.1, -r.width() * 0.1);
    p.save();
    p.setClipRect(in);
    p.setPen(QPen(QColor(255, 255, 255, 60), 1));
    const qreal paso = r.width() / 6.0;
    for (qreal x = in.left() - in.height(); x < in.right(); x += paso) {
        p.drawLine(QPointF(x, in.top()), QPointF(x + in.height(), in.bottom()));
        p.drawLine(QPointF(x + in.height(), in.top()), QPointF(x, in.bottom()));
    }
    p.restore();
    p.setPen(QPen(QColor(255, 255, 255, 120), 1.5));
    p.setBrush(Qt::NoBrush);
    p.drawRoundedRect(in, radio * 0.6, radio * 0.6);
}

inline void dibujar(QPainter &p, const Carta &c, const QRectF &r, bool bocaArriba = true,
                    bool resaltada = false)
{
    p.save();
    p.setRenderHint(QPainter::Antialiasing);
    const qreal radio = r.width() * 0.09;

    // Sombra
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0, 0, 0, 70));
    p.drawRoundedRect(r.translated(2, 3), radio, radio);

    if (!bocaArriba) {
        dibujarReverso(p, r);
        p.restore();
        return;
    }

    p.setPen(resaltada ? QPen(QColor(255, 200, 40), 3) : QPen(QColor(150, 150, 150), 1));
    p.setBrush(QColor(252, 252, 250));
    p.drawRoundedRect(r, radio, radio);

    const QColor color = c.esRoja() ? QColor(200, 30, 40) : QColor(25, 25, 25);
    p.setPen(color);

    // Indices en esquinas
    QFont f = p.font();
    f.setBold(true);
    f.setPixelSize(qRound(r.height() * 0.16));
    p.setFont(f);
    const QRectF esquina(r.left() + r.width() * 0.06, r.top() + r.height() * 0.03,
                         r.width() * 0.3, r.height() * 0.2);
    p.drawText(esquina, Qt::AlignHCenter | Qt::AlignTop, c.nombreValor());
    f.setPixelSize(qRound(r.height() * 0.13));
    p.setFont(f);
    p.drawText(esquina.translated(0, r.height() * 0.15), Qt::AlignHCenter | Qt::AlignTop, c.nombrePalo());

    p.save();
    p.translate(r.center());
    p.rotate(180);
    p.translate(-r.center());
    f.setPixelSize(qRound(r.height() * 0.16));
    p.setFont(f);
    p.drawText(esquina, Qt::AlignHCenter | Qt::AlignTop, c.nombreValor());
    f.setPixelSize(qRound(r.height() * 0.13));
    p.setFont(f);
    p.drawText(esquina.translated(0, r.height() * 0.15), Qt::AlignHCenter | Qt::AlignTop, c.nombrePalo());
    p.restore();

    // Palo grande en el centro
    f.setPixelSize(qRound(r.height() * 0.42));
    f.setBold(false);
    p.setFont(f);
    p.drawText(r, Qt::AlignCenter, c.nombrePalo());

    p.restore();
}

} // namespace CartaPainter

#endif // CARTAPAINTER_H
