# BlackJack 21

Juego de BlackJack (21) desarrollado con Qt6 y DTK6 para Deepin Linux.

## Compilación

```bash
mkdir build && cd build
cmake ..
make
```

## Ejecución

```bash
./blackjack
```

## Controles

- **+50 / -50**: Ajustar apuesta (mínimo 50 fichas)
- **Pedir**: Pedir otra carta
- **Plantarse**: Terminar turno
- **Nueva Ronda**: Empezar nueva mano

## Reglas

- Fichas iniciales: 1000
- Blackjack natural paga 3:2
- Dealer se planta en 17
- Pasarse de 21 pierde
