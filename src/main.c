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
        system("pause");
        return 1;
    }

    win = SDL_CreateWindow("GoldShell", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (win == NULL)
    {
        printf("Can't create window");
        system("pause");
        return 1;
    }

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (ren == NULL)
    {
        printf("Can't create Renderer");
        return 1;
    }

    return 0;
}

int quit()
{
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}

bool cull_face(Vec4 *v1, Vec4 *v2, Vec4 *v3)
{
    Vec4 u = vec4_sub(v2, v1);
    Vec4 v = vec4_sub(v3, v1);

    Vec4 cross = vec4_cross(&u, &v);
    Vec4 cn = CAMERA_NORMAL;

    return dot(&cross, &cn) < 0.0f;
}

float get_cull_face_dot(Vec4 *v1, Vec4 *v2, Vec4 *v3)
{
    Vec4 u = vec4_sub(v2, v1);
    Vec4 v = vec4_sub(v3, v1);

    Vec4 cross = vec4_cross(&u, &v);
    Vec4 cn = CAMERA_NORMAL;

    return dot(&cross, &cn);
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

    if (cull_face(&projectedVertex1, &projectedVertex2, &projectedVertex3))
        return;

    float x1 = (projectedVertex1.x + 1) * SCREEN_WIDTH_HALF;
    float y1 = (projectedVertex1.y + 1) * SCREEN_HEIGHT_HALF;

    float x2 = (projectedVertex2.x + 1) * SCREEN_WIDTH_HALF;
    float y2 = (projectedVertex2.y + 1) * SCREEN_HEIGHT_HALF;

    float x3 = (projectedVertex3.x + 1) * SCREEN_WIDTH_HALF;
    float y3 = (projectedVertex3.y + 1) * SCREEN_HEIGHT_HALF;

    SDL_RenderDrawLineF(renderer, x1, y1, x2, y2);
    SDL_RenderDrawLineF(renderer, x2, y2, x3, y3);
    SDL_RenderDrawLineF(renderer, x3, y3, x1, y1);
}

// Function to calculate if the point P is inside triangle v1-v2-v3
Vec4 barycentric_coords(Vec4 *v1, Vec4 *v2, Vec4 *v3, Vec4 *point)
{
    // Compute denominator of the Barycentric coordinates
    float denominator = (v2->y - v3->y) * (v1->x - v3->x) + (v3->x - v2->x) * (v1->y - v3->y);

    // Compute Barycentric coordinates
    float u = ((v2->y - v3->y) * (point->x - v3->x) + (v3->x - v2->x) * (point->y - v3->y)) / denominator;
    float v = ((v3->y - v1->y) * (point->x - v3->x) + (v1->x - v3->x) * (point->y - v3->y)) / denominator;
    float w = 1.0f - u - v;

    Vec4 pos = {u, v, w, 0.0f};

    return pos;
}

void render_rasterized(SDL_Renderer *renderer, Vec4 *v1, Vec4 *v2, Vec4 *v3, Vec4 *v1_col, Vec4 *v2_col, Vec4 *v3_col)
{
    // Our current point;
    Vec4 point = {0.0f};

    // Get the bounding box of the triangle
    int minX = min(min(v1->x, v2->x), v3->x);
    int minY = min(min(v1->y, v2->y), v3->y);

    int maxX = max(max(v1->x, v2->x), v3->x);
    int maxY = max(max(v1->y, v2->y), v3->y);

    bool start = true;

    int startX = 0;
    int startY = 0;

    int endX = 0;
    int endY = 0;

    // Loop through all the pixels of the bounding box
    for (int y = minY; y <= maxY; y++)
    {
        for (int x = minX; x <= maxX; x++)
        {
            point.x = x;
            point.y = y;

            Vec4 barycentric = barycentric_coords(v1, v2, v3, &point);

            if (barycentric.x >= 0.0f && barycentric.y >= 0.0f && barycentric.z >= 0.0f)
            {
                // We don`t fill each pixel one by one for performance
                if (start)
                {
                    startX = x;
                    startY = y;

                    start = false;
                }

                endX = x;
                endY = y;

                float r = v1_col->x + v2_col->x + v3_col->x;
                float g = v1_col->y + v2_col->y + v3_col->y;
                float b = v1_col->z + v2_col->z + v3_col->z;

                SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
            }
        }

        SDL_RenderDrawLineF(renderer, startX, startY, endX, endY);
        start = true;
    }
}

void render_polygon(SDL_Renderer *renderer, Vec4 *v1, Vec4 *v2, Vec4 *v3, Matrix4x4 *modelMatrix, Matrix4x4 *projectionMatrix)
{
    // Apply transformation
    Vec4 transformedVertex1 = multiplyMatrixVec4(modelMatrix, v1);
    Vec4 transformedVertex2 = multiplyMatrixVec4(modelMatrix, v2);
    Vec4 transformedVertex3 = multiplyMatrixVec4(modelMatrix, v3);

    // Apply the perspective projection
    Vec4 projectedVertex1 = applyPerspectiveProjection(projectionMatrix, &transformedVertex1);
    Vec4 projectedVertex2 = applyPerspectiveProjection(projectionMatrix, &transformedVertex2);
    Vec4 projectedVertex3 = applyPerspectiveProjection(projectionMatrix, &transformedVertex3);

    float dot_product = get_cull_face_dot(&transformedVertex1, &transformedVertex2, &transformedVertex3);

    if (dot_product < 0.0f)
        return;

    // Viewport transformations
    projectedVertex1.x = (projectedVertex1.x + 1) * SCREEN_WIDTH_HALF;
    projectedVertex1.y = (projectedVertex1.y + 1) * SCREEN_HEIGHT_HALF;
    projectedVertex2.x = (projectedVertex2.x + 1) * SCREEN_WIDTH_HALF;
    projectedVertex2.y = (projectedVertex2.y + 1) * SCREEN_HEIGHT_HALF;
    projectedVertex3.x = (projectedVertex3.x + 1) * SCREEN_WIDTH_HALF;
    projectedVertex3.y = (projectedVertex3.y + 1) * SCREEN_HEIGHT_HALF;

    Vec4 col1 = {255 * dot_product, 255 * dot_product, 255 * dot_product, 255};
    Vec4 col2 = {0.0f};
    Vec4 col3 = {0.0f};

    render_rasterized(ren, &projectedVertex1, &projectedVertex2, &projectedVertex3, &col1, &col2, &col3);
}

void render_plane(Vec4 plane[6], Matrix4x4 *model, Matrix4x4 *proj)
{
    for (unsigned int i = 0; i < 6; i += 3)
    {
        render_polygon(ren, &plane[i], &plane[i + 1], &plane[i + 2], model, proj);
    }
}

int WinMain(int argc, char **args)
{
    if (init() == 1)
    {
        return 1;
    }

    bool run = true;
    SDL_Event e;

    float fov = 60.0f * (M_PI / 180.0f); // Field of view in radians
    float zNear = 0.1f;                  // Near clipping plane
    float zFar = 100.0f;                 // Far clipping plane

    Matrix4x4 projectionMatrix = getPerspectiveProjection(fov, ASPECT_RATIO, zNear, zFar);

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

        for (unsigned int i = 0; i < 1; i++)
        {
            Vec4 movement = {0.0f, 0.0f, 25.0f, 1.0f};

            Matrix4x4 rotationMatrix = getRotationMatrixY(SDL_GetTicks() * 0.001f);
            Matrix4x4 extra_rotationMatrix = getRotationMatrixX(SDL_GetTicks() * 0.001f);
            Matrix4x4 mul_rot = multiplyMatrix4x4(extra_rotationMatrix, rotationMatrix);
            Matrix4x4 translationMatrix = getTranslationMatrix(movement.x + i, sinf(movement.y + i), movement.z);

            Matrix4x4 modelMatrix = multiplyMatrix4x4(translationMatrix, mul_rot);

            render_plane(verts_plane_f, &modelMatrix, &projectionMatrix);
            render_plane(verts_plane_b, &modelMatrix, &projectionMatrix);
            render_plane(verts_plane_l, &modelMatrix, &projectionMatrix);
            render_plane(verts_plane_r, &modelMatrix, &projectionMatrix);
            render_plane(verts_plane_t, &modelMatrix, &projectionMatrix);
            render_plane(verts_plane_bt, &modelMatrix, &projectionMatrix);
        }

        SDL_RenderPresent(ren);

        oldTime = SDL_GetTicks();

        while (SDL_PollEvent(&e) != NULL)
        {
            if (e.type == SDL_QUIT)
            {
                run = false;
            }
        }
    }

    return quit();
}