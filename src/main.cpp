#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <SDL2/SDL.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

struct Entity
{
    int x, y;
    int radius;
    int xVel, yVel;
    Uint8 r, g, b;
    bool isPacman;
    float mouthOpen;
    bool mouthClosing;
    float eyeOffset;
    bool eyeMovingRight;
    bool isVisible;
    Uint32 invisibleTime;
};

std::vector<Entity> entities;

bool checkCollision(Entity &a, Entity &b)
{
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    int distance = sqrt(dx * dx + dy * dy);

    return distance < a.radius + b.radius;
}

void resolveCollision(Entity &a, Entity &b)
{
    int dx = b.x - a.x;
    int dy = b.y - a.y;
    float distance = sqrt(dx * dx + dy * dy);
    float overlap = a.radius + b.radius - distance;

    // Normalizar el vector de desplazamiento
    float nx = dx / distance;
    float ny = dy / distance;

    // Mover las entidades para resolver la superposición
    a.x -= nx * overlap / 2.0;
    a.y -= ny * overlap / 2.0;
    b.x += nx * overlap / 2.0;
    b.y += ny * overlap / 2.0;

    // Invertir las velocidades para que se muevan en direcciones opuestas
    std::swap(a.xVel, b.xVel);
    std::swap(a.yVel, b.yVel);
}

bool init(int numEntities, int numGhosts)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    srand(time(NULL));

    for (int i = 0; i < numEntities; ++i)
    {
        Entity e;
        e.radius = rand() % 20 + 10;
        e.x = rand() % (SCREEN_WIDTH - 2 * e.radius) + e.radius;
        e.y = rand() % (SCREEN_HEIGHT - 2 * e.radius) + e.radius;
        e.xVel = rand() % 10 + 1;
        e.yVel = rand() % 10 + 1;
        e.r = 255;
        e.g = 255;
        e.b = 0;
        e.isPacman = true;
        e.mouthOpen = 0.1;
        e.mouthClosing = true;
        entities.push_back(e);
    }

    for (int i = 0; i < numGhosts; ++i)
    {
        Entity e;
        e.radius = rand() % 20 + 10;
        e.x = rand() % (SCREEN_WIDTH - 2 * e.radius) + e.radius;
        e.y = rand() % (SCREEN_HEIGHT - 2 * e.radius) + e.radius;
        e.xVel = rand() % 2;
        e.yVel = rand() % 2;
        e.r = rand() % 256;
        e.g = rand() % 256;
        e.b = rand() % 256;
        e.isPacman = false;

        e.eyeOffset = 0;
        e.eyeMovingRight = true;
        e.isVisible = true;
        e.invisibleTime = 0;
        entities.push_back(e);
    }

    return true;
}

void close()
{
    SDL_Quit();
}

int main(int argc, char *args[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << args[0] << " <numPacmans> <numGhosts>" << std::endl;
        return 1;
    }

    int numPacmans = std::atoi(args[1]);
    int numGhosts = std::atoi(args[2]);

    if (!init(numPacmans, numGhosts))
    {
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Pacman and Ghosts Screensaver", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool quit = false;
    SDL_Event e;
    int lowLimit = 0;
    int realLimit = 0;
    while (!quit)
    {
        lowLimit++;
        if (lowLimit % 100 == 0)
        {
            realLimit++;
        }
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }
        int limit = std::min(realLimit, static_cast<int>(entities.size())); // Obtén el menor entre 10 y el tamaño del vector

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        Uint32 currentTime = SDL_GetTicks();

        // Handle collisions and drawing
        for (int i = 0; i < limit; ++i)
        {
            Entity &entity = entities[i];
            for (int j = 0; j < limit; ++j)
            {
                Entity &other = entities[j];
                if (&entity != &other && checkCollision(entity, other))
                {
                    if (entity.isPacman && !other.isPacman && other.isVisible)
                    {
                        other.isVisible = false;
                        other.invisibleTime = currentTime;
                    }
                    if (!entity.isPacman && other.isPacman && entity.isVisible)
                    {
                        entity.isVisible = false;
                        entity.invisibleTime = currentTime;
                    }
                    resolveCollision(entity, other);
                }
                if (&entity != &entity && checkCollision(entity, entity))
                {
                    resolveCollision(entity, other);
                }
            }

            entity.x += entity.xVel;
            entity.y += entity.yVel;

            if (entity.x - entity.radius < 0)
            {
                entity.x = entity.radius;
                entity.xVel = -entity.xVel;
            }
            else if (entity.x + entity.radius > SCREEN_WIDTH)
            {
                entity.x = SCREEN_WIDTH - entity.radius;
                entity.xVel = -entity.xVel;
            }

            if (entity.y - entity.radius < 0)
            {
                entity.y = entity.radius;
                entity.yVel = -entity.yVel;
            }
            else if (entity.y + entity.radius > SCREEN_HEIGHT)
            {
                entity.y = SCREEN_HEIGHT - entity.radius;
                entity.yVel = -entity.yVel;
            }
            SDL_SetRenderDrawColor(renderer, entity.r, entity.g, entity.b, 255);

            if (entity.isPacman)
            {
                // Draw Pacman as a filled circle with a mouth
                for (float angle = entity.mouthOpen * M_PI; angle <= 2 * M_PI - entity.mouthOpen * M_PI; angle += 0.01)
                {
                    for (int r = 0; r < entity.radius; ++r)
                    {
                        int x = entity.x + r * cos(angle);
                        int y = entity.y + r * sin(angle);
                        SDL_RenderDrawPoint(renderer, x, y);
                    }
                }

                if (entity.mouthClosing)
                {
                    entity.mouthOpen += 0.01;
                    if (entity.mouthOpen >= 0.3)
                        entity.mouthClosing = false;
                }
                else
                {
                    entity.mouthOpen -= 0.01;
                    if (entity.mouthOpen <= 0.05)
                        entity.mouthClosing = true;
                }
            }
            else
            {
                // Draw Ghost as an outlined circle
                if (entity.isVisible)
                {
                    for (float angle = 0; angle <= 2 * M_PI; angle += 0.01)
                    {
                        int x = entity.x + entity.radius * cos(angle);
                        int y = entity.y + entity.radius * sin(angle);
                        SDL_RenderDrawPoint(renderer, x, y);
                    }
                }

                // Dibujar ojos del fantasma
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                int eyeRadius = 3; // Radio del ojo

                for (int eye = 0; eye < 2; ++eye)
                {
                    int eyeX = entity.x + (eye == 0 ? -5 : 5) + entity.eyeOffset;
                    int eyeY = entity.y - 5;
                    for (float angle = 0; angle <= 2 * M_PI; angle += 0.01)
                    {
                        int x = eyeX + eyeRadius * cos(angle);
                        int y = eyeY + eyeRadius * sin(angle);
                        SDL_RenderDrawPoint(renderer, x, y);
                    }
                }

                // Mover los ojos
                if (entity.eyeMovingRight)
                {
                    entity.eyeOffset += 0.1;
                    if (entity.eyeOffset >= 5)
                        entity.eyeMovingRight = false;
                }
                else
                {
                    entity.eyeOffset -= 0.1;
                    if (entity.eyeOffset <= -5)
                        entity.eyeMovingRight = true;
                }
            }
            // Actualizar el estado de visibilidad
            if (!entity.isPacman && !entity.isVisible)
            {
                if (currentTime - entity.invisibleTime >= 2000) // 2000 milisegundos = 2 segundos
                {
                    entity.isVisible = true;
                }
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    close();

    return 0;
}
