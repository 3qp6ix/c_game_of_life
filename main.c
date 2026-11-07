#include "SDL2/SDL.h"
#include "graphics.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

size_t W_WIDTH = 900;
size_t W_HEIGHT = 800;

const size_t UNIT = 10;
const size_t ROWS = 50;
const size_t COLS = 50;

const size_t BOARD_W = UNIT * COLS;
const size_t BOARD_H = UNIT * ROWS;

#define X0 (W_WIDTH-BOARD_W)/2
#define Y0 (W_HEIGHT-BOARD_H)/2

#define ALIVE_CELL_COLOR 0XFFFF0066
#define DEAD_CELL_COLOR 0XFF000000

#define DT 60

void draw_board(
    SDL_Surface* surface,
    uint8_t* board
){
    if(!surface || !board) return;

    uint32_t* pixels = (uint32_t*)surface->pixels;

    for(int row=0;row<ROWS;row++){
        for(int col=0;col<COLS;col++){
            uint32_t color = board[row*COLS+col]? ALIVE_CELL_COLOR : DEAD_CELL_COLOR;

            fill_rect(
                surface,
                X0+col*UNIT,Y0+row*UNIT,
                UNIT,UNIT,
                color
            );
        }
    }
}

size_t alive_neighbours(uint8_t* board,int x,int y){
    if(!board) return 0;

    size_t n_count = 0;

    for(int dy=-1;dy<=1;dy++){
        for(int dx=-1;dx<=1;dx++){
            if(dx==0 && dy==0) continue;

            int c_x = x+dx;
            if(c_x<0 || c_x>=COLS) continue;

            int c_y = y+dy;
            if(c_y<0 || c_y>=ROWS) continue;

            n_count += board[c_y*COLS+c_x];
        }
    }

    return n_count;
}

void update_board(uint8_t* board,uint8_t* backboard){
    if(!board || !backboard) return;
    for(int y=0;y<ROWS;y++){
        for(int x=0;x<COLS;x++){
            size_t n = alive_neighbours(board,x,y);
            int bit = board[y*COLS+x];

            if(bit){
                backboard[y*COLS+x] = (n==2 || n==3);
            }else{
                backboard[y*COLS+x] = (n==3);
            }
        }
    }
}

void init_board(uint8_t* board){
    if(!board) return;
    for(size_t i=0;i<ROWS*COLS;i++){
        int n = rand() % 2;
        board[i] = n;
    }
}

int main(int argc,char** argv){
    if(SDL_Init(SDL_INIT_VIDEO)!=0) return 1;
    srand(time(NULL));

    uint8_t* board = (uint8_t*)calloc(ROWS*COLS,sizeof(uint8_t));
    init_board(board);

    uint8_t* backboard = (uint8_t*)calloc(ROWS*COLS,sizeof(uint8_t));

    SDL_Window* window = SDL_CreateWindow(
        "Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        W_WIDTH,W_HEIGHT,
        0
    );

    if(!window) goto quit;

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    if(!surface) goto quit;

    bool running = true;
    SDL_Event e;

    uint32_t last = SDL_GetTicks();
    
    while(running){
        while(SDL_PollEvent(&e)){
            switch(e.type){
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    if(e.key.keysym.sym == SDLK_r){
                        init_board(board);
                    }
                    break;
            }
        }

        uint32_t now = SDL_GetTicks();

        if(now-last>=DT){
            draw_board(surface,board);
            update_board(board,backboard);
            
            uint8_t* temp = board;
            board = backboard;
            backboard = temp;

            SDL_UpdateWindowSurface(window);
            last = now;
        }
    }

quit:
    free(board);
    free(backboard);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}