# BlackJack 21

Blackjack (21) contra el crupier. Desarrollado con **Qt6** y **Deepin Tool Kit (DTK6)** para Deepin Linux.

## Características

- Apuesta, pide, plántate o dobla
- Blackjack natural paga 3:2; el crupier se planta con 17
- Zapato de 6 barajas
- Fichas guardadas entre sesiones; nueva partida al quedarte sin fichas
- Traducido al inglés

## Controles

| Tecla | Acción |
|-------|--------|
| H | Pedir carta |
| S | Plantarse |
| D | Doblar |
| + / − (↑ / ↓) | Cambiar la apuesta |
| Enter / Espacio | Repartir / nueva ronda |
| Ctrl+N | Nueva partida (1000 fichas) |
| Ctrl+Q | Salir |

## Instalación

Descarga el paquete de la [última release](https://github.com/deepin-espanol/blackjack/releases/latest):

- `.deb` (amd64, arm64, riscv64, loong64):
  ```bash
  sudo apt install ./blackjack_*.deb
  ```
- `.uab` de linyaps (amd64, arm64): ejecutable directamente.
  ```bash
  chmod +x org.deepinenespanol.*.uab && ./org.deepinenespanol.*.uab
  ```

## Compilar

```bash
sudo apt build-dep ./          # dependencias declaradas en debian/control
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/blackjack
```

O generar el paquete `.deb`:

```bash
dpkg-buildpackage -us -uc -b
```

## Publicar una versión

Añade una entrada nueva en `debian/changelog` y haz push: el workflow
*Build and publish .deb* crea la release con los `.deb` y los `.uab`.

## Licencia

GPL-3.0 o posterior. © Deepin en Español — https://deepinenespanol.org
