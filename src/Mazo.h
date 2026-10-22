#ifndef MAZO_H
#define MAZO_H

#include "Carta.h"
#include <QVector>
#include <QRandomGenerator>
#include <algorithm>

// Zapato de varias barajas francesas. Se rebaraja cuando quedan pocas cartas,
// como en un casino, en lugar de usar una baraja nueva en cada mano.
class Mazo {
public:
    explicit Mazo(int barajas = 1) : m_barajas(barajas) {
        reiniciar();
    }

    void reiniciar() {
        cartas.clear();
        for (int b = 0; b < m_barajas; ++b)
            for (int p = 0; p < 4; ++p)
                for (int v = 1; v <= 13; ++v)
                    cartas.append(Carta{static_cast<Palo>(p), v});
        mezclar();
    }

    void mezclar() {
        std::shuffle(cartas.begin(), cartas.end(), *QRandomGenerator::global());
    }

    Carta repartir() {
        if (cartas.isEmpty())
            reiniciar();
        return cartas.takeLast();
    }

    int cartasRestantes() const {
        return cartas.size();
    }

    // Menos de un cuarto del zapato: toca barajar antes de la siguiente mano
    bool necesitaBarajar() const {
        return cartas.size() < m_barajas * 52 / 4;
    }

private:
    int m_barajas;
    QVector<Carta> cartas;
};

#endif // MAZO_H
