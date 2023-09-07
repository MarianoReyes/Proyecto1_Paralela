# How to 'Kompilieren' C++ code



You just have to hace CMAKE 22 installed and then you can run the following commands:

```bash
./run.sh
```

And now just do
    
```bash
./build/EXEC <numPacmans> <numGhosts>
```

And you will see the result of the simulation.

## Kompilieren mit G++

```bash
cd ./src
```

```bash
g++ sceensaversequential.cpp $(pkg-config --cflags --libs sdl2) -o screensaversequential
```

```bash
g++ sceensaverparalel.cpp $(pkg-config --cflags --libs sdl2) -fopenmp -o screensaverparalel
``````