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

const Vec4 CAMERA_NORMAL = {0.0f, 0.0f, 1.0f};

SDL_Window *win = NULL;
SDL_Renderer *ren = NULL;
SDL_Surface *scr = NULL;
SDL_Surface *john = NULL;

void swap(Vec4 *v1, Vec4 *v2)
{
    Vec4 temp = *v1;
    *v1 = *v2;
    *v2 = temp;
}

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

bool cull_face(Vec4 *faceNormal, Vec4 *cameraNormal)
{
    return dot(faceNormal, cameraNormal) < __FLT_EPSILON__;
}

void transform_normal(Vec4 *normal, Matrix4x4 *modelMatrix, Vec4 *out)
{
    // Вычисляем обратную транспонированную матрицу модели
    Matrix4x4 invModelMatrix = {0};
    Matrix4x4 transposedModelMatrix = {0};
    invert_matrix4x4(modelMatrix, &invModelMatrix);
    transpose_matrix4x4(&invModelMatrix, &transposedModelMatrix);

    // Трансформируем нормаль с использованием этой матрицы
    // Vec4 transformedNormal = multiplyMatrixVec4(&transposedModelMatrix, normal);
    *out = multiplyMatrixVec4(&transposedModelMatrix, normal);
    // printf("%f", out->y);
    // vec4_normalize(&transformedNormal, out);
}

void render_polygon_wired(SDL_Renderer *renderer, Vec4 *v1, Vec4 *v2, Vec4 *v3, Matrix4x4 *modelMatrix, Matrix4x4 *projectionMatrix)
{
    Vec4 transformedVertex1 = multiplyMatrixVec4(modelMatrix, v1);
    Vec4 transformedVertex2 = multiplyMatrixVec4(modelMatrix, v2);
    Vec4 transformedVertex3 = multiplyMatrixVec4(modelMatrix, v3);

    // Apply the perspective projection
    Vec4 projectedVertex1 = applyPerspectiveProjection(projectionMatrix, &transformedVertex1);
    Vec4 projectedVertex2 = applyPerspectiveProjection(projectionMatrix, &transformedVertex2);
    Vec4 projectedVertex3 = applyPerspectiveProjection(projectionMatrix, &transformedVertex3);

    float x1 = (projectedVertex1.x + 1) * SCREEN_WIDTH_HALF;
    float y1 = (projectedVertex1.y + 1) * SCREEN_HEIGHT_HALF;

    float x2 = (projectedVertex2.x + 1) * SCREEN_WIDTH_HALF;
    float y2 = (projectedVertex2.y + 1) * SCREEN_HEIGHT_HALF;

    float x3 = (projectedVertex3.x + 1) * SCREEN_WIDTH_HALF;
    float y3 = (projectedVertex3.y + 1) * SCREEN_HEIGHT_HALF;

    Vec4 u = vec4_sub(&transformedVertex2, &transformedVertex1); 
    Vec4 v = vec4_sub(&transformedVertex3, &transformedVertex1); 

    Vec4 cross = vec4_cross(&u, &v);
    Vec4 cn = CAMERA_NORMAL;

    double dot_product = dot(&cross, &cn);

    if (dot_product < __DBL_EPSILON__)
    {
        return;
    }

    SDL_RenderDrawLineF(renderer, x1, y1, x2, y2);
    SDL_RenderDrawLineF(renderer, x2, y2, x3, y3);
    SDL_RenderDrawLineF(renderer, x3, y3, x1, y1);
}

void render_polygon(SDL_Renderer *renderer, Vec4 *v1, Vec4 *v2, Vec4 *v3, Matrix4x4 *modelMatrix, Matrix4x4 *projectionMatrix, Vec4 *normal)
{
    if (cull_face(normal, &CAMERA_NORMAL))
        return;

    // Apply transformation
    Vec4 transformedVertex1 = multiplyMatrixVec4(modelMatrix, v1);
    Vec4 transformedVertex2 = multiplyMatrixVec4(modelMatrix, v2);
    Vec4 transformedVertex3 = multiplyMatrixVec4(modelMatrix, v3);

    // Apply the perspective projection
    Vec4 projectedVertex1 = applyPerspectiveProjection(projectionMatrix, &transformedVertex1);
    Vec4 projectedVertex2 = applyPerspectiveProjection(projectionMatrix, &transformedVertex2);
    Vec4 projectedVertex3 = applyPerspectiveProjection(projectionMatrix, &transformedVertex3);

    // Convert into screen space
    projectedVertex1.x = (projectedVertex1.x + 1) * SCREEN_WIDTH_HALF;
    projectedVertex1.y = (projectedVertex1.y + 1) * SCREEN_HEIGHT_HALF;
    projectedVertex2.x = (projectedVertex2.x + 1) * SCREEN_WIDTH_HALF;
    projectedVertex2.y = (projectedVertex2.y + 1) * SCREEN_HEIGHT_HALF;
    projectedVertex3.x = (projectedVertex3.x + 1) * SCREEN_WIDTH_HALF;
    projectedVertex3.y = (projectedVertex3.y + 1) * SCREEN_HEIGHT_HALF;

    // Sort vertices by Y coordinate
    if (projectedVertex1.y > projectedVertex2.y)
        swap(&projectedVertex1, &projectedVertex2);
    if (projectedVertex2.y > projectedVertex3.y)
        swap(&projectedVertex2, &projectedVertex3);
    if (projectedVertex1.y > projectedVertex2.y)
        swap(&projectedVertex1, &projectedVertex2);

    // Scanline
    float totalHeight = projectedVertex3.y - projectedVertex1.y;

    for (int y = projectedVertex1.y; y <= projectedVertex3.y; y++)
    {
        bool secondHalf = y > projectedVertex2.y || projectedVertex2.y == projectedVertex1.y;
        float segmentHeight = secondHalf ? projectedVertex3.y - projectedVertex2.y : projectedVertex2.y - projectedVertex1.y;
        float alpha = (float)(y - projectedVertex1.y) / totalHeight;
        float beta = (float)(y - (secondHalf ? projectedVertex2.y : projectedVertex1.y)) / segmentHeight;

        Vec4 A = vec4_lerp(&projectedVertex1, &projectedVertex3, alpha);
        Vec4 B = secondHalf ? vec4_lerp(&projectedVertex2, &projectedVertex3, beta) : vec4_lerp(&projectedVertex1, &projectedVertex2, beta);

        if (A.x > B.x)
            swap(&A, &B);

        for (int x = A.x; x <= B.x; x++)
        {
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
}

void render_plane(Vec4 plane[6], Matrix4x4 *model, Matrix4x4 *proj)
{
    for (unsigned int i = 0; i < 6; i += 3)
    {
        render_polygon_wired(ren, &plane[i], &plane[i + 1], &plane[i + 2], model, proj);
    }
}

void render_cube(float *cube[36], Matrix4x4 *model, Matrix4x4 *proj)
{
    for (unsigned int i = 0; i < 36; i += 3)
    {
        render_polygon_wired(ren, &cube[i], &cube[i + 1], &cube[i + 2], model, proj);
    }
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
    Vec4 verts_cube[36] = {
        // Front face (two triangles, CCW)
        {0.5f, 0.5f, 0.5f, 1.0f},
        {-0.5f, -0.5f, 0.5f, 1.0f},
        {0.5f, -0.5f, 0.5f, 1.0f},
        {0.5f, 0.5f, 1.5f, 1.0f},
        {-0.5f, -0.5f, 1.5f, 1.0f},
        {-0.5f, 0.5f, 1.5f, 1.0f},

        // Back face (two triangles, CCW)
        {0.5f, -0.5f, -0.5f, 1.0f},
        {0.5f, 0.5f, -0.5f, 1.0f},
        {-0.5f, 0.5f, -0.5f, 1.0f},
        {-0.5f, 0.5f, -0.5f, 1.0f},
        {-0.5f, -0.5f, -0.5f, 1.0f},
        {0.5f, -0.5f, -0.5f, 1.0f},

        // Left face (two triangles, CCW)
        {-0.5f, -0.5f, 0.5f, 1.0f},
        {-0.5f, 0.5f, 0.5f, 1.0f},
        {-0.5f, 0.5f, -0.5f, 1.0f},
        {-0.5f, 0.5f, -0.5f, 1.0f},
        {-0.5f, -0.5f, -0.5f, 1.0f},
        {-0.5f, -0.5f, 0.5f, 1.0f},

        // Right face (two triangles, CCW)
        {0.5f, 0.5f, -0.5f, 1.0f},
        {0.5f, -0.5f, -0.5f, 1.0f},
        {0.5f, -0.5f, 0.5f, 1.0f},
        {0.5f, -0.5f, 0.5f, 1.0f},
        {0.5f, 0.5f, 0.5f, 1.0f},
        {0.5f, 0.5f, -0.5f, 1.0f},

        // Top face (two triangles, CCW)
        {-0.5f, 0.5f, 0.5f, 1.0f},
        {-0.5f, 0.5f, -0.5f, 1.0f},
        {0.5f, 0.5f, -0.5f, 1.0f},
        {0.5f, 0.5f, -0.5f, 1.0f},
        {0.5f, 0.5f, 0.5f, 1.0f},
        {-0.5f, 0.5f, 0.5f, 1.0f},

        // Bottom face (two triangles, CCW)
        {0.5f, -0.5f, -0.5f, 1.0f},
        {0.5f, -0.5f, 0.5f, 1.0f},
        {-0.5f, -0.5f, 0.5f, 1.0f},
        {-0.5f, -0.5f, 0.5f, 1.0f},
        {-0.5f, -0.5f, -0.5f, 1.0f},
        {0.5f, -0.5f, -0.5f, 1.0f}};

    Vec4 verts_plane_f[6] = {
        {0.5f, 0.5f, -0.5f, 1.0f},
        {-0.5f, 0.5f, -0.5f, 1.0f},
        {-0.5f, -0.5f, -0.5f, 1.0f},
        {-0.5f, -0.5f, -0.5f, 1.0f},
        {0.5f, -0.5f, -0.5f, 1.0f},
        {0.5f, 0.5f, -0.5f, 1.0f}};

    Vec4 verts_plane_b[6] = {
        {0.5f, 0.5f, 0.5f, 1.0f},
        {0.5f, -0.5f, 0.5f, 1.0f},
        {-0.5f, -0.5f, 0.5f, 1.0f},
        {-0.5f, -0.5f, 0.5f, 1.0f},
        {-0.5f, 0.5f, 0.5f, 1.0f},
        {0.5f, 0.5f, 0.5f, 1.0f}};

    Vec4 verts_plane_l[6] = {
        {-0.5f, 0.5f, 0.5f, 1.0f},
        {-0.5f, -0.5f, 0.5f, 1.0f},
        {-0.5f, 0.5f, -0.5f, 1.0f},
        {-0.5f, 0.5f, -0.5f, 1.0f},
        {-0.5f, -0.5f, 0.5f, 1.0f},
        {-0.5f, -0.5f, -0.5f, 1.0f}};

    Vec4 verts_plane_r[6] = {
        {0.5f, 0.5f, 0.5f, 1.0f},
        {0.5f, 0.5f, -0.5f, 1.0f},
        {0.5f, -0.5f, 0.5f, 1.0f},
        {0.5f, -0.5f, 0.5f, 1.0f},
        {0.5f, 0.5f, -0.5f, 1.0f},
        {0.5f, -0.5f, -0.5f, 1.0f}};

    Vec4 verts_plane_t[6] = {
        {0.5f, 0.5f, 0.5f, 1.0f},
        {-0.5f, 0.5f, 0.5f, 1.0f},
        {0.5f, 0.5f, -0.5f, 1.0f},
        {0.5f, 0.5f, -0.5f, 1.0f},
        {-0.5f, 0.5f, 0.5f, 1.0f},
        {-0.5f, 0.5f, -0.5f, 1.0f}};

    Vec4 verts_plane_bt[6] = {
        {0.5f, -0.5f, 0.5f, 1.0f},
        {0.5f, -0.5f, -0.5f, 1.0f},
        {-0.5f, -0.5f, -0.5f, 1.0f},
        {-0.5f, -0.5f, -0.5f, 1.0f},
        {-0.5f, -0.5f, 0.5f, 1.0f},
        {0.5f, -0.5f, 0.5f, 1.0f}};

    float oldTime = SDL_GetTicks();

    while (run)
    {
        float deltaTime = SDL_GetTicks() - oldTime;

        SDL_SetRenderDrawColor(ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(ren);

        SDL_SetRenderDrawColor(ren, 255, 255, 255, SDL_ALPHA_OPAQUE);

        Vec4 movement = {0.0f, sinf(SDL_GetTicks() * 0.00f), 10.0f};

        Matrix4x4 rotationMatrix = getRotationMatrixY(SDL_GetTicks() * 0.001f);
        Matrix4x4 extra_rotationMatrix = getRotationMatrixX(SDL_GetTicks() * 0.001f);
        Matrix4x4 mul_rot = multiplyMatrix4x4(extra_rotationMatrix, rotationMatrix);
        Matrix4x4 translationMatrix = getTranslationMatrix(movement.x, movement.y, movement.z);

        Matrix4x4 modelMatrix = multiplyMatrix4x4(translationMatrix, mul_rot);

        // Make clear: object_space -> world_space (model) -> camera_space (view) -> screen_space (projection)
        render_plane(verts_plane_f, &modelMatrix, &projectionMatrix);
        render_plane(verts_plane_b, &modelMatrix, &projectionMatrix);
        render_plane(verts_plane_l, &modelMatrix, &projectionMatrix);
        render_plane(verts_plane_r, &modelMatrix, &projectionMatrix);
        render_plane(verts_plane_t, &modelMatrix, &projectionMatrix);
        render_plane(verts_plane_bt, &modelMatrix, &projectionMatrix);

        SDL_RenderPresent(ren);

        oldTime = SDL_GetTicks();

        while (SDL_PollEvent(&e) != NULL)
        {
            if (e.type == SDL_QUIT)
            {
                run = false;
            }

            /*if (e.type == SDL_KEYDOWN)
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