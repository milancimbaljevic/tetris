#include <stdio.h>
#include <SDL2/SDL.h>
#include "constants.h"
#include "shape.h"
#include <vector>
#include <time.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
int game_is_running = FALSE;
int last_frame_time = 0;
Shape *current_shape = nullptr;
SDL_Color color_map[GRID_HEIGHT][GRID_WIDTH];

int global_grid[GRID_HEIGHT + 1][GRID_WIDTH];

int init_window()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "Error initializing SDL.\n");
        return FALSE;
    }

    SDL_Window *window = SDL_CreateWindow(
        NULL,                   // window title
        SDL_WINDOWPOS_CENTERED, // WINDOW_X_POS
        SDL_WINDOWPOS_CENTERED, // WINDOW_Y_POS
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0);

    if (!window)
    {
        fprintf(stderr, "Error creating SDL window.\n");
        return FALSE;
    }
    renderer = SDL_CreateRenderer(window, -1 /*default dispay driver*/, 0);

    if (!renderer)
    {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return FALSE;
    }

    return TRUE;
}

void process_input()
{
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type)
    {
    case SDL_QUIT:
        game_is_running = FALSE;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
        {
            game_is_running = FALSE;
        }
        else if (event.key.keysym.sym == SDLK_DOWN)
        {
            if (!current_shape->check_for_bottom_collision())
                current_shape->y_cell += 1; // this should be an atomic operation, implement some locking
        }
        else if (event.key.keysym.sym == SDLK_UP)
        {
            current_shape->rotateAntiClockwise();
            // current_shape->rotateClockwise();
        }
        else if (event.key.keysym.sym == SDLK_LEFT)
        {
            if (!current_shape->check_for_left_collision())
                current_shape->x_cell--;
        }
        else if (event.key.keysym.sym == SDLK_RIGHT)
        {
            if (!current_shape->check_for_right_collision())
                current_shape->x_cell++;
        }
        break;
    }
}

std::thread *move_down_thread;

void setup()
{
    srand(time(NULL));
    for (int i = 0; i < GRID_HEIGHT; i++)
    {
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            global_grid[i][j] = 0;
        }
    }

    for (int j = 0; j < GRID_WIDTH; j++)
        global_grid[GRID_HEIGHT][j] = 1;

    current_shape = new Shape(rand()%7);

    move_down_thread = new std::thread(&Shape::update, current_shape);
}

void draw_grid()
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 1; i < GRID_HEIGHT; i++)
    {
        SDL_RenderDrawLine(renderer, 0, i * SQUARE_LENGTH, WINDOW_WIDTH, i * SQUARE_LENGTH);
    }
    for (int i = 1; i < GRID_WIDTH; i++)
    {
        SDL_RenderDrawLine(renderer, i * SQUARE_LENGTH, 0, i * SQUARE_LENGTH, WINDOW_HEIGHT);
    }


    for (int i = 0; i < GRID_HEIGHT; i++)
    {
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            if (global_grid[i][j] == 1)
            {
                SDL_SetRenderDrawColor(renderer, color_map[i][j].r, color_map[i][j].g, color_map[i][j].b, 255);
                SDL_Rect rect = {
                    j * SQUARE_LENGTH,
                    i * SQUARE_LENGTH,
                    SQUARE_LENGTH,
                    SQUARE_LENGTH};
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

void update()
{
    // waste some time / sleep until we reach the frame target time
    // while(!SDL_TICKS_PASSED(SDL_GetTicks(), last_frame_time + FRAME_TARGET_TIME));
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0;
    last_frame_time = SDL_GetTicks();
}

void render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    draw_grid();
    current_shape->draw_shape(renderer);

    SDL_RenderPresent(renderer); // swaping the buffers
}

void destroy_window()
{
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    delete move_down_thread;
    delete current_shape;
}

int main()
{
    game_is_running = init_window();

    setup();

    while (game_is_running)
    {
        process_input();
        // update();
        render();
    }

    destroy_window();

    move_down_thread->join();
    return 0;
}
