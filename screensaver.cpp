#include <SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

// Tamaño del canvas
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Cantidad de círculos a renderizar
int numCircles = 0;

// Estructura para representar un círculo
struct Circle {
    int x, y;           // Posición
    int radius;         // Radio
    int xVel, yVel;     // Velocidad
    Uint8 r, g, b;      // Color
};

// Vector de círculos
std::vector<Circle> circles;

// Inicialización de SDL y círculos
bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    srand(time(NULL));

    // Crear círculos con propiedades aleatorias
    for (int i = 0; i < numCircles; ++i) {
        Circle circle;
        circle.radius = rand() % 20 + 10;
        circle.x = rand() % (SCREEN_WIDTH - 2 * circle.radius) + circle.radius;
        circle.y = rand() % (SCREEN_HEIGHT - 2 * circle.radius) + circle.radius;
        circle.xVel = rand() % 5 + 1;
        circle.yVel = rand() % 5 + 1;
        circle.r = rand() % 256;
        circle.g = rand() % 256;
        circle.b = rand() % 256;
        circles.push_back(circle);
    }

    return true;
}

// Liberar recursos y cerrar SDL
void close() {
    SDL_Quit();
}

int main(int argc, char* args[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << args[0] << " <numCircles>" << std::endl;
        return 1;
    }

    numCircles = std::atoi(args[1]);

    if (!init()) {
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Screensaver", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        close();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        close();
        return 1;
    }

    bool quit = false;
    SDL_Event e;
    Uint32 startTime, frameTime;
    int frames = 0;

    while (!quit) {
        startTime = SDL_GetTicks();

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Limpiar la pantalla
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Actualizar posición de los círculos y manejar rebotes
        for (Circle& circle : circles) {
            circle.x += circle.xVel;
            circle.y += circle.yVel;

            if (circle.x - circle.radius < 0 || circle.x + circle.radius > SCREEN_WIDTH) {
                circle.xVel = -circle.xVel;
            }
            if (circle.y - circle.radius < 0 || circle.y + circle.radius > SCREEN_HEIGHT) {
                circle.yVel = -circle.yVel;
            }

            // Dibujar el círculo
            SDL_SetRenderDrawColor(renderer, circle.r, circle.g, circle.b, 255);
            for (int i = 0; i <= 360; ++i) {
                float angle = i * 3.14159 / 180;
                int x = circle.x + circle.radius * cos(angle);
                int y = circle.y + circle.radius * sin(angle);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }

        SDL_RenderPresent(renderer);

        frameTime = SDL_GetTicks() - startTime;
        if (frameTime < 1000 / 30) {
            SDL_Delay(1000 / 30 - frameTime);
        }

        frames++;
        if (frames >= 30) {
            std::cout << "FPS: " << frames / ((SDL_GetTicks() - startTime) / 1000.0) << std::endl;
            frames = 0;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    close();

    return 0;
}
