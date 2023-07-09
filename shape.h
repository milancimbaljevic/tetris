#pragma once
#include <SDL2/SDL.h>
#include <thread>

class Shape{
public:
    int matrix[5][5] = {
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0}
    };


    int N = 5;

    int x_cell = 2;
    int y_cell = -5;

    SDL_Color color;

    Shape() = delete;

    Shape(int type){
        color.a = 255;
        color.r = rand()%256;
        color.g = rand()%256;        
        color.g = rand()%256;

        if(type == 0){
            matrix[2][1] = 1;
            matrix[2][2] = 1;
            matrix[2][3] = 1;
            matrix[3][2] = 1;
        }else if(type == 1){
            matrix[0][1] = 1;
            matrix[1][1] = 1;
            matrix[2][1] = 1;
            matrix[0][2] = 1;
        }else if(type == 2){
            matrix[0][2] = 1;
            matrix[1][1] = 1;
            matrix[1][2] = 1;
            matrix[2][1] = 1;
        }
        else if(type == 3){
            matrix[0][1] = 1;
            matrix[0][2] = 1;
            matrix[1][1] = 1;
            matrix[1][2] = 1;
        }
        else if(type == 4){
            matrix[0][1] = 1;
            matrix[0][2] = 1;
            matrix[0][3] = 1;
            matrix[1][1] = 1;
        }
        else if(type == 5){
            matrix[0][2] = 1;
            matrix[0][3] = 1;
            matrix[1][1] = 1;
            matrix[1][2] = 1;
        }
        else if(type == 6){
            matrix[0][0] = 1;
            matrix[0][1] = 1;
            matrix[0][2] = 1;
            matrix[0][3] = 1;
        }
    }

    void rotateClockwise();
    void rotateAntiClockwise();
    void draw_shape(SDL_Renderer* renderer);
    void update();
    bool check_for_bottom_collision();
    bool check_for_right_collision();
    bool check_for_left_collision();
};