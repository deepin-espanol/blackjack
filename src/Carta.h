#ifndef CARTA_H
#define CARTA_H

#include <QString>

enum class Palo {
    Corazones,
    Diamantes,
    Treboles,
    Espadas
};

struct Carta {
    Palo palo;
    int valor;  // 1 (As) .. 13 (K)

    QString nombreValor() const {
        switch (valor) {
            case 1: return QStringLiteral("A");
            case 11: return QStringLiteral("J");
            case 12: return QStringLiteral("Q");
            case 13: return QStringLiteral("K");
            default: return QString::number(valor);
        }
    }

    QString nombrePalo() const {
        switch (palo) {
            case Palo::Corazones: return QStringLiteral("♥");
            case Palo::Diamantes: return QStringLiteral("♦");
            case Palo::Treboles: return QStringLiteral("♣");
            case Palo::Espadas: return QStringLiteral("♠");
        }
        return QString();
    }

    bool esRoja() const {
        return palo == Palo::Corazones || palo == Palo::Diamantes;
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
