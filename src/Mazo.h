#ifndef MAZO_H
#define MAZO_H

#include "Carta.h"
#include <QVector>
#include <QRandomGenerator>
#include <algorithm>

class Mazo {
public:
    Mazo() {
        reiniciar();
    }

    void reiniciar() {
        cartas.clear();
        for (int p = 0; p < 4; ++p) {
            for (int v = 1; v <= 13; ++v) {
                cartas.append(Carta{static_cast<Palo>(p), v});
            }
        }
        mezclar();
    }

    void mezclar() {
        auto *rng = QRandomGenerator::global();
        std::shuffle(cartas.begin(), cartas.end(), *rng);
    }

    Carta repartir() {
        if (cartas.isEmpty()) {
            reiniciar();
        }
        return cartas.takeLast();
    }

    int cartasRestantes() const {
        return cartas.size();
    }

private:
    QVector<Carta> cartas;
};

#endif // MAZO_H
