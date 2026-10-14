#ifndef CARTA_H
#define CARTA_H

#include <QString>

enum class Palo {
    Corazones,
    Diamantes,
    Tréboles,
    Espadas
};

struct Carta {
    Palo palo;
    int valor;

    QString nombreValor() const {
        switch (valor) {
            case 1: return "A";
            case 11: return "J";
            case 12: return "Q";
            case 13: return "K";
            default: return QString::number(valor);
        }
    }

    QString nombrePalo() const {
        switch (palo) {
            case Palo::Corazones: return "♥";
            case Palo::Diamantes: return "♦";
            case Palo::Tréboles: return "♣";
            case Palo::Espadas: return "♠";
        }
        return "";
    }

    int valorBlackjack() const {
        if (valor >= 10) return 10;
        return valor;
    }

    bool esAs() const {
        return valor == 1;
    }

    QString nombreCompleto() const {
        return nombreValor() + nombrePalo();
    }
};

#endif // CARTA_H
