#include "shape.h"
#include "constants.h"
#include <unistd.h>
#include <vector>
#include <random>

extern int game_is_running;
extern Shape *current_shape;
extern int global_grid[GRID_HEIGHT + 1][GRID_WIDTH];
extern SDL_Renderer* renderer;
extern SDL_Color color_map[GRID_HEIGHT][GRID_WIDTH];


bool Shape::check_for_right_collision()
{
    for (int j = 0; j < N; j++)
    {
        for (int i = 0; i < N; i++)
        {
            int x_cell = current_shape->x_cell + j;
            int y_cell = current_shape->y_cell + i;

            if (x_cell >= 0 && x_cell < GRID_WIDTH)
            {
                if (current_shape->matrix[i][j] == 1)
                {
                    if (x_cell == GRID_WIDTH - 1)
                    {
                        printf("eee\n");
                        return true;
                    }
                    if (global_grid[y_cell][x_cell + 1] == 1)
                        return true;
                    // return false;
                }
            }
        }
    }
    return false;
}

bool Shape::check_for_bottom_collision()
{
    for (int i = N - 1; i >= 0; i--)
    {
        for (int j = 0; j < N; j++)
        {
            // check if there is a block under the current block in shape using global grid
            int grid_cell_x = x_cell + j;
            int grid_cell_y = y_cell + i;

            if (current_shape->matrix[i][j] == 1 && global_grid[grid_cell_y + 1][grid_cell_x] == 1)
                return true;
        }
    }
    return false;
}

void Shape::update()
{
    while (game_is_running)
    {
        current_shape->y_cell += 1; // this should be an atomic operation, implement some locking
        sleep(1);
        // check if the shape is layed
        if (current_shape->check_for_bottom_collision())
        {
            if (current_shape->y_cell <= 0)
            {
                printf("Kraj igre\n");
                for (int i = 0; i < GRID_HEIGHT; i++)
                {
                    for (int j = 0; j < GRID_WIDTH; j++)
                    {
                        global_grid[i][j] = 0;
                    }
                }

                delete current_shape;
                current_shape = new Shape(rand()%7);
                continue;
            }
            printf("Kolizija\n");
            for (int i = N - 1; i >= 0; i--)
            {
                for (int j = 0; j < N; j++)
                {
                    // check if there is a block under the current block in shape using global grid
                    int grid_cell_x = x_cell + j;
                    int grid_cell_y = y_cell + i;

                    if (current_shape->matrix[i][j] == 1)
                    {
                        printf("caoo\n");
                        global_grid[grid_cell_y][grid_cell_x] = 1;
                        color_map[grid_cell_y][grid_cell_x] = current_shape->color;
                    }
                }
            }

            // check if thera are full rows is full

            for (int i = GRID_HEIGHT - 1; i >= 0; i--)
            {
                bool row_full = true;
                for (int j = 0; j < GRID_WIDTH; j++)
                {
                    if (global_grid[i][j] == 0)
                    {
                        row_full = false;
                        break;
                    }
                }

                if (row_full)
                {
                    for (int j = 0; j < GRID_WIDTH; j++)
                    {
                        global_grid[i][j] = 0;
                    }

                    for (int x = i; x > 0; x--)
                    {
                        for (int j = 0; j < GRID_WIDTH; j++)
                        {
                            global_grid[x][j] = global_grid[x - 1][j];
                        }
                    }

                    i = i + 1;
                }
            }

            delete current_shape;
            current_shape = new Shape(rand()%7);
        }
    }
}

bool Shape::check_for_left_collision()
{
    for (int j = 0; j < N; j++)
    {
        for (int i = 0; i < N; i++)
        {
            int x_cell = current_shape->x_cell + j;
            int y_cell = current_shape->y_cell + i;

            if (x_cell >= 0 && x_cell < GRID_WIDTH)
            {
                if (current_shape->matrix[i][j] == 1)
                {
                    if (x_cell == 0)
                    {
                        printf("eee\n");
                        return true;
                    }
                    if (global_grid[y_cell][x_cell - 1] == 1)
                        return true;
                    // return false;
                }
            }
        }
    }
    return false;
}

void Shape::rotateClockwise()
{
    int temp[N][N];

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            temp[i][j] = matrix[i][j];
        }
    }

    for (int i = 0; i < N * N; i++)
    {
        int old_i = i / N;
        int old_j = i % N;
        int new_j = N - 1 - i / N;
        int new_i = i % N;

        matrix[new_i][new_j] = temp[old_i][old_j];
    }
}

void Shape::rotateAntiClockwise()
{
    int temp[N][N];

    int old_x = current_shape->x_cell;
    int old_y = current_shape->y_cell;

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            temp[i][j] = matrix[i][j];
        }
    }

    for (int i = 0; i < N * N; i++)
    {
        int old_i = i / N;
        int old_j = i % N;
        int new_j = i / N;
        int new_i = N - 1 - i % N;

        matrix[new_i][new_j] = temp[old_i][old_j];
    }

    // check if it is falling out left or right
    int x = false;
    for (int j = 0; j < N; j++)
    {
        for (int i = 0; i < N; i++)
        {
            int x_cell = current_shape->x_cell + j;
            int y_cell = current_shape->y_cell + i;

            if (current_shape->matrix[i][j] == 1)
            {
                if (x_cell < 0)
                {
                    x = true;
                    current_shape->x_cell -= x_cell;
                    break;
                }
            }
        }
        if (x)
            break;
    }

    x = false;

    for (int j = N - 1; j >= 0; j--)
    {
        for (int i = 0; i < N; i++)
        {
            int x_cell = current_shape->x_cell + j;
            int y_cell = current_shape->y_cell + i;

            if (current_shape->matrix[i][j] == 1)
            {
                if (x_cell >= GRID_WIDTH)
                {
                    x = true;
                    current_shape->x_cell += (GRID_WIDTH - x_cell - 1);
                    printf("caoo %d\n", (GRID_WIDTH - x_cell));
                    break;
                }
            }
        }
        if (x)
            break;
    }

    // check if it is colliding with layed shapes
    x = false;

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int x_cell = current_shape->x_cell + j;
            int y_cell = current_shape->y_cell + i;

            if (current_shape->matrix[i][j] == 1 && global_grid[y_cell][x_cell] == 1)
            {
                x = true;
                break;
            }
        }
        if (x)
            break;
    }

    if (x)
    {
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                current_shape->matrix[i][j] = temp[i][j];
            }
        }
        current_shape->x_cell = old_x;
        current_shape->y_cell = old_y;
    }
}

void Shape::draw_shape(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, current_shape->color.r, current_shape->color.g, current_shape->color.b, 255);
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            SDL_Rect rect = {
                (x_cell + j) * SQUARE_LENGTH,
                (y_cell + i) * SQUARE_LENGTH,
                SQUARE_LENGTH,
                SQUARE_LENGTH};
            if (matrix[i][j] == 1)
            {
                SDL_RenderFillRect(renderer, &rect);
            }
            else
            {
                // SDL_RenderDrawRect(renderer, &rect);
            }
        }
    }
}