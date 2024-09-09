#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include "gmath.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

const int SCREEN_WIDTH_HALF = SCREEN_WIDTH / 2;
const int SCREEN_HEIGHT_HALF = SCREEN_HEIGHT / 2;

const float ASPECT_RATIO = SCREEN_WIDTH / SCREEN_HEIGHT;

SDL_Window *win = NULL;
SDL_Renderer *ren = NULL;
SDL_Surface *scr = NULL;
SDL_Surface *john = NULL;

struct Point2D
{
    float x, y;
};

struct Point3D
{
    float x, y, z;
};

struct Edge
{
    int start, end;
};

int init()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Can't init");
        // std::cout <<  << std::endl;
        system("pause");
        return 1;
    }

    win = SDL_CreateWindow("GoldShell 0.1_a", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (win == NULL)
    {
        printf("Can't create window");
        // std::cout << "Can't create window: " << SDL_GetError() << std::endl;
        system("pause");
        return 1;
    }

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (ren == NULL)
    {
        printf("Can't create Renderer");
        return 1;
    }

    scr = SDL_GetWindowSurface(win);

    return 0;
}

int load()
{
    john = SDL_LoadBMP("sprite.bmp");

    if (john == NULL)
    {
        printf("Can't load image");
        // std::cout << "Can't load image: " << SDL_GetError() << std::endl;
        system("pause");
        return 1;
    }

    return 0;
}

int quit()
{
    SDL_FreeSurface(john);

    SDL_DestroyWindow(win);

    SDL_Quit();

    return 0;
}

void render_line(SDL_Renderer *renderer, float startX, float startY, float endX, float endY)
{
    SDL_RenderDrawLineF(renderer, startX, startY, endX, endY);
}

void render_polygon(SDL_Renderer *renderer, Vec4 *v1, Vec4 *v2, Vec4 *v3, Matrix4x4 *modelMatrix, Matrix4x4 *projectionMatrix)
{
    Vec4 transformedVertex1 = multiplyMatrixVec4(modelMatrix, v1);
    Vec4 transformedVertex2 = multiplyMatrixVec4(modelMatrix, v2);
    Vec4 transformedVertex3 = multiplyMatrixVec4(modelMatrix, v3);

    // Apply the perspective projection
    Vec4 projectedVertex1 = applyPerspectiveProjection(projectionMatrix, &transformedVertex1);
    Vec4 projectedVertex2 = applyPerspectiveProjection(projectionMatrix, &transformedVertex2);
    Vec4 projectedVertex3 = applyPerspectiveProjection(projectionMatrix, &transformedVertex3);

    SDL_RenderDrawLineF(renderer, (projectedVertex1.x + 1) * SCREEN_WIDTH_HALF, (projectedVertex1.y + 1) * SCREEN_HEIGHT_HALF, (projectedVertex2.x + 1) * SCREEN_WIDTH_HALF, (projectedVertex2.y + 1) * SCREEN_HEIGHT_HALF);
    SDL_RenderDrawLineF(renderer, (projectedVertex2.x + 1) * SCREEN_WIDTH_HALF, (projectedVertex2.y + 1) * SCREEN_HEIGHT_HALF, (projectedVertex3.x + 1) * SCREEN_WIDTH_HALF, (projectedVertex3.y + 1) * SCREEN_HEIGHT_HALF);
    SDL_RenderDrawLineF(renderer, (projectedVertex3.x + 1) * SCREEN_WIDTH_HALF, (projectedVertex3.y + 1) * SCREEN_HEIGHT_HALF, (projectedVertex1.x + 1) * SCREEN_WIDTH_HALF, (projectedVertex1.y + 1) * SCREEN_HEIGHT_HALF);
}

int WinMain(int argc, char **args)
{
    if (init() == 1)
    {
        return 1;
    }

    /*if (load() == 1)
    {
        return 1;
    }*/

    bool run = true;
    SDL_Event e;
    SDL_Rect r;

    int x = 0;
    int y = 0;

    int speed = 10;

    r.x = x;
    r.y = y;

    float fov = 90.0f * (M_PI / 180.0f); // Field of view in radians
    float zNear = 0.1f;                  // Near clipping plane
    float zFar = 100.0f;                 // Far clipping plane

    Matrix4x4 projectionMatrix = getPerspectiveProjection(fov, ASPECT_RATIO, zNear, zFar);

    // Define a vertices in 3D space (with w = 1 for homogeneous coordinates)
    Vec4 verts[36] = {  
        // Front face (two triangles)
        {-0.5f, -0.5f, 0.5f, 1.0f},
        {-0.5f, 0.5f, 0.5f, 1.0f},
        {0.5f, -0.5f, 0.5f, 1.0f},
        {0.5f, -0.5f, 0.5f, 1.0f},
        {0.5f, 0.5f, 0.5f, 1.0f},
        {-0.5f, 0.5f, 0.5f, 1.0f},

        // Back face (two triangles)
        {0.5f, -0.5f, -0.5f, 1.0f},
        {0.5f, 0.5f, -0.5f, 1.0f},
        {-0.5f, -0.5f, -0.5f, 1.0f},
        {-0.5f, -0.5f, -0.5f, 1.0f},
        {-0.5f, 0.5f, -0.5f, 1.0f},
        {0.5f, 0.5f, -0.5f, 1.0f},

        // Left face (two triangles)
        {-0.5f, -0.5f, -0.5f, 1.0f},
        {-0.5f, -0.5f, 0.5f, 1.0f},
        {-0.5f, 0.5f, -0.5f, 1.0f},
        {-0.5f, 0.5f, -0.5f, 1.0f},
        {-0.5f, 0.5f, 0.5f, 1.0f},
        {-0.5f, -0.5f, 0.5f, 1.0f},

        // Right face (two triangles)
        {0.5f, -0.5f, 0.5f, 1.0f},
        {0.5f, -0.5f, -0.5f, 1.0f},
        {0.5f, 0.5f, 0.5f, 1.0f},
        {0.5f, 0.5f, 0.5f, 1.0f},
        {0.5f, 0.5f, -0.5f, 1.0f},
        {0.5f, -0.5f, -0.5f, 1.0f},

        // Top face (two triangles)
        {0.5f, 0.5f, 0.5f, 1.0f},
        {0.5f, 0.5f, -0.5f, 1.0f},
        {-0.5f, 0.5f, 0.5f, 1.0f},
        {-0.5f, 0.5f, 0.5f, 1.0f},
        {0.5f, 0.5f, -0.5f, 1.0f},
        {-0.5f, 0.5f, -0.5f, 1.0f},

        // Bottom face (two triangles)
        {-0.5f, -0.5f, 0.5f, 1.0f},
        {-0.5f, -0.5f, -0.5f, 1.0f},
        {0.5f, -0.5f, 0.5f, 1.0f},
        {0.5f, -0.5f, 0.5f, 1.0f},
        {0.5f, -0.5f, -0.5f, 1.0f},
        {-0.5f, -0.5f, -0.5f, 1.0f}};

    float oldTime = SDL_GetTicks();

    while (run)
    {
        float deltaTime = SDL_GetTicks() - oldTime;

        SDL_SetRenderDrawColor(ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(ren);

        SDL_SetRenderDrawColor(ren, 255, 255, 255, SDL_ALPHA_OPAQUE);

        Vec4 movement = {0.0f, sinf(SDL_GetTicks() * 0.001f), 10.0f};

        Matrix4x4 rotationMatrix = getRotationMatrixY(SDL_GetTicks() * 0.001f);
        Matrix4x4 translationMatrix = getTranslationMatrix(movement.x, movement.y, movement.z);

        Matrix4x4 modelMatrix = multiplyMatrix4x4(translationMatrix, rotationMatrix);

        // Make clear: object_space -> world_space (model) -> camera_space (view) -> screen_space (projection)
        for (unsigned int i = 0; i < 36; i += 3)
        {
            render_polygon(ren, &verts[i], &verts[i + 1], &verts[i + 2], &modelMatrix, &projectionMatrix);
        }

        SDL_RenderPresent(ren);

        oldTime = SDL_GetTicks();

        while (SDL_PollEvent(&e) != NULL)
        {
            /*if (e.type == SDL_QUIT)
            {
                run = false;
            }

            if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_UP)
                {
                    y -= speed;
                }
                if (e.key.keysym.sym == SDLK_DOWN)
                {
                    y += speed;
                }
                if (e.key.keysym.sym == SDLK_RIGHT)
                {
                    x += speed;
                }
                if (e.key.keysym.sym == SDLK_LEFT)
                {
                    x -= speed;
                }
            }*/
        }

        r.x = x;
        r.y = y;

        /*SDL_FillRect(scr, NULL, SDL_MapRGB(scr->format, 0, 0, 0));

        SDL_BlitSurface(john, NULL, scr, &r);

        SDL_UpdateWindowSurface(win);*/
    }

    return quit();
}