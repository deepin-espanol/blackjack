#ifndef BLACKJACKWIDGET_H
#define BLACKJACKWIDGET_H

#include <QWidget>
#include <QPushButton>
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

    int fichas() const { return m_fichas; }

public slots:
    void nuevaPartida();  // vuelve a 1000 fichas

signals:
    void fichasCambiadas(int fichas);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void aumentarApuesta();
    void reducirApuesta();
    void repartir();
    void pedirCarta();
    void plantarse();
    void doblar();
    void siguienteRonda();

private:
    void turnoDealer();
    void resolver();
    void pagar(int cambioFichas, const QString &texto);
    static int calcularPuntos(const QVector<Carta> &mano, bool *blanda = nullptr);
    static bool esBlackjack(const QVector<Carta> &mano);
    void ajustarApuesta();
    void actualizarBotones();
    void guardar();
    void dibujarMano(QPainter &p, const QVector<Carta> &mano, int y, bool ocultarSegunda);

    static constexpr int FICHAS_INICIALES = 1000;
    static constexpr int APUESTA_MINIMA = 10;

    Mazo m_mazo;
    QVector<Carta> m_manoJugador;
    QVector<Carta> m_manoDealer;
    int m_fichas;
    int m_apuesta;
    int m_ultimoResultado;  // >0 gana, <0 pierde, 0 empate
    Estado m_estado;
    QString m_mensaje;

    QPushButton *m_btnMenos;
    QPushButton *m_btnMas;
    QPushButton *m_btnRepartir;
    QPushButton *m_btnPedir;
    QPushButton *m_btnPlantarse;
    QPushButton *m_btnDoblar;
    QPushButton *m_btnSiguiente;
};

#endif // BLACKJACKWIDGET_H
