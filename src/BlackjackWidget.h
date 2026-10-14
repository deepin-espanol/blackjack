#ifndef BLACKJACKWIDGET_H
#define BLACKJACKWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVector>
#include "Carta.h"
#include "Mazo.h"

class BlackjackWidget : public QWidget {
    Q_OBJECT

public:
    enum class Estado {
        Apostando,
        Jugando,
        Terminado
    };

    explicit BlackjackWidget(QWidget *parent = nullptr);

signals:
    void fichasCambiadas(int fichas);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void aumentarApuesta();
    void reducirApuesta();
    void pedirCarta();
    void plantarse();
    void nuevaRonda();

private:
    void repartirCartasIniciales();
    void turnoDealer();
    int calcularPuntos(const QVector<Carta> &mano);
    bool esBlackjack(const QVector<Carta> &mano);
    void actualizarBotones();
    void dibujarCarta(QPainter &painter, const Carta &carta, int x, int y, bool bocaArriba = true);

    Mazo mazo;
    QVector<Carta> manoJugador;
    QVector<Carta> manoDealer;
    int fichas;
    int apuesta;
    Estado estado;
    QString mensaje;

    QPushButton *btnApostarMas;
    QPushButton *btnApostarMenos;
    QPushButton *btnPedir;
    QPushButton *btnPlantarse;
    QPushButton *btnNuevaRonda;
    QLabel *lblFichas;
    QLabel *lblMensaje;
};

#endif // BLACKJACKWIDGET_H
