/*
    Conway's Game of Life Repolica
    Rules:
        1. Any LIVE cell with less than 2 live neighbours dies, underpopulation
        2. Any LIVE cell with more than 3 live neighbours dies, overpopulation
        3. Any DEAD cell with exactly   3 living neighbours becomes LIVE, birth
        4. Any LIVE cell with           2-3 living neighbours stays LIVE, life

    common structures also defined for easy placement:
        *  Block
        *  Beehive
        *  Loaf
        *  Boat
        *  Tub

        *  Blinker
        *  Toad
        *  Beacon
        *  Pulsar
        *  Pentadecathlon

        *  Glider
        *  LWSS

    larger structures:
        *  Gosmer Glider Gun
*/

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

const int WIN_WIDTH = 1024;
const int WIN_HEIGHT = 768;
const char LIVE = '#';
const char DEAD = '-';
const int PX_SIZE = 8;
const int BUTTON_SIZE = 16;
const int GENERATORS = 14;
struct px
{
    SDL_Rect loc;
    SDL_Color col;
};
struct radio
{
    SDL_Rect button;
    SDL_Color col;
    SDL_Rect bound;
    char* text;
    void* (action)(char**, int, int);
};

//prototypes
void px_init(struct px* elem);
void radio_init(struct radio* elem, char* str, void* fun);
void countNeighbors(char c[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int n[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE]);
void stepGen(char c[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int n[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE]);
void updatePxFromChar(struct px p[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], char c[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE]);
void renderGrid(SDL_Renderer* renderer, struct px arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE]);
//Statics
void addBlock(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y);
void addBeehive(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y);
void addLoaf(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y);
void addBoat(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y);
void addTub(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y);
//Oscillators
void addBlinker(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y);
void addToad(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y);
void addBeacon(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y);
void addPulsar(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y);
void addPentadecathlon(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y);
//Spaceships
void addGlider(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y);
void addLWSS(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y);
//Guns
void addGliderGun(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y);
//other
void addPx(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y);

int main (int argc, char** argv)
{
    //init of SDL
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("C's GoL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    //misc vars
    //iterators
    int i, j;
    //paused flag
    int paused = 1;
    //step-by-step flag
    int next = 0;
    //mouse coords
    int mx, my;
    //function pointer for next shape to be generated
    void* (fun)(char**, int, int);
    //event union
    SDL_Event e;
    //arrays
    //grid of chars with LIVE and DEAD
    char grid[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE];
        for (j = 0; j < WIN_HEIGHT/PX_SIZE; ++j)
            for (i = 0; i < WIN_WIDTH/PX_SIZE; ++i)
                grid[i][j] = DEAD;
        //add features
        addGliderGun(grid, 10, 10);
        addPentadecathlon(grid, 80, 20);
    //grid of count of living neighbours
    int ln[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE];
        for (j = 0; j < WIN_HEIGHT/PX_SIZE; ++j)
            for (i = 0; i < WIN_WIDTH/PX_SIZE; ++i)
                ln[i][j] = 0;
    //pixel array
    struct px pixels[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE];
        for ( i = 0; i < WIN_WIDTH/PX_SIZE; ++i )
        {
            for ( j = 0; j < WIN_HEIGHT/PX_SIZE; ++j )
            {
                px_init(&pixels[i][j]);
                pixels[i][j].loc.x = i*PX_SIZE;
                pixels[i][j].loc.y = j*PX_SIZE;
            }
        }
    //radio buttons for functions
    struct radio buttons[GENERATORS];
        //set each button to a function
        radio_init(&buttons[0], "Px", addPx);
        radio_init(&buttons[1], "Block", addBlock);
        radio_init(&buttons[2], "Beehive", addBeehive);
        radio_init(&buttons[3], "Loaf", addLoaf);
        radio_init(&buttons[4], "Boat", addBoat);
        radio_init(&buttons[5], "Tub", addTub);
        radio_init(&buttons[6], "Blinker", addBlinker);
        radio_init(&buttons[7], "Toad", addToad);
        radio_init(&buttons[8], "Beacon", addBeacon);
        radio_init(&buttons[9], "Pulsar", addPulsar);
        radio_init(&buttons[10], "Pentadecathlon", addPentadecathlon);
        radio_init(&buttons[11], "Glider", addGlider);
        radio_init(&buttons[12], "LWSS", addLWSS);
        radio_init(&buttons[13], "GliderGun", addGliderGun);
        //set vertical offset
        for (i = 0; i < GENERATORS; ++i)
        {
            buttons[i].button.y = (WIN_HEIGHT/(GENERATORS+2)*BUTTON_SIZE)*(i+1);
            buttons[i].bound.y = buttons[i].button.y;
        }

    //initial render
    //update
    updatePxFromChar(pixels, grid);
    //setup renderer
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);
    //render grid
    renderGrid(renderer, pixels);
    //render
    SDL_RenderPresent(renderer);

    while (1)
    {
        if (SDL_PollEvent(&e))
        {
            if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                    break;
                else if (e.key.keysym.sym == SDLK_p)    //toggle paused with p
                    paused = !paused;
                else if (e.key.keysym.sym == SDLK_SPACE)//advance frame with space
                    next = 1;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                SDL_GetMouseState(&mx, &my);
                if (grid[mx/PX_SIZE][my/PX_SIZE] == LIVE)
                {
                    grid[mx/PX_SIZE][my/PX_SIZE] = DEAD;
                }
                else if (grid[mx/PX_SIZE][my/PX_SIZE] == DEAD)
                {
                    grid[mx/PX_SIZE][my/PX_SIZE] = LIVE;
                }
                else
                {
                    printf("What the heck!\n");
                }
            }
        }

        //handle pause/next
        if (paused && !next)
        {
            continue;
        }
        else
        {
            next = 0;
        }

        //count neighbours
        countNeighbors(grid, ln);
        //life happens
        stepGen(grid, ln);
        //update pixel array from grid
        updatePxFromChar(pixels, grid);

        //rendering
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderClear(renderer);
        //render pixel array
        renderGrid(renderer, pixels);
        //render
        SDL_RenderPresent(renderer);
    }

    //cleanup

    //quit
    SDL_Quit();
    return 0;
}

//general
void px_init(struct px* elem)
{
    elem->loc.x = 0;
    elem->loc.y = 0;
    elem->loc.w = PX_SIZE;
    elem->loc.h = PX_SIZE;

    //default black
    elem->col.r = 0;
    elem->col.g = 0;
    elem->col.b = 0;
    elem->col.a = 0xFF;
}
void radio_init(struct radio* elem, char* str, void* fun)
{
    //button
    elem->button.x = 3*WIN_WIDTH/4;;
    elem->button.y = 0;
    elem->button.w = BUTTON_SIZE;
    elem->button.h = BUTTON_SIZE;
    //default light-gray
    elem->col.r = 0xF0;
    elem->col.g = 0xF0;
    elem->col.b = 0xF0;
    elem->col.a = 0xFF;

    //text
    elem->bound.x = 3*WIN_WIDTH/4 + BUTTON_SIZE;
    elem->bound.y = 0;
    elem->bound.w = BUTTON_SIZE;
    elem->bound.h = BUTTON_SIZE;
    //string
    elem->text = str;
    //action
    action = fun;
}
void countNeighbors(char c[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int n[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE])
{
    int i, j;

    for (j = 0; j < WIN_HEIGHT/PX_SIZE; ++j)
    {
        for (i = 0; i < WIN_WIDTH/PX_SIZE; ++i)
        {
            //clear
            n[i][j] = 0;
            //starting above, going clockwise; 2 to a block
            if (j > 0)
            {
                if (c[i][j-1] == LIVE)
                {
                    ++n[i][j];         //up
                }
                if (i < (WIN_WIDTH/PX_SIZE-1))
                {
                    if (c[i+1][j-1] == LIVE)
                    {
                        ++n[i][j];     //up-right
                    }
                }
            }
            if (i < (WIN_WIDTH/PX_SIZE-1))
            {
                if (c[i+1][j] == LIVE)
                {
                    ++n[i][j];         //right
                }
                if (i < (WIN_HEIGHT/PX_SIZE-1))
                {
                    if (c[i+1][j+1] == LIVE)
                    {
                        ++n[i][j];     //right-down
                    }
                }
            }
            if (j < (WIN_HEIGHT/PX_SIZE-1))
            {
                if (c[i][j+1] == LIVE)
                {
                    ++n[i][j];         //down
                }
                if (i > 0)
                {
                    if (c[i-1][j+1] == LIVE)
                    {
                        ++n[i][j];     //down-left
                    }
                }
            }
            if (i > 0)
            {
                if (c[i-1][j] == LIVE)
                {
                    ++n[i][j];         //left
                }
                if (j > 0)
                {
                    if (c[i-1][j-1] == LIVE)
                    {
                        ++n[i][j];     //left-up
                    }
                }
            }
        }
    }
}
void stepGen(char c[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int n[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE])
{
    int i, j;

    for (j = 0; j < WIN_HEIGHT/PX_SIZE; ++j)
    {
        for (i = 0; i < WIN_WIDTH/PX_SIZE; ++i)
        {
            //3. birth
            if (c[i][j] == DEAD)
            {
                if (n[i][j] == 3)
                {
                    c[i][j] = LIVE;
                }
            }
            else if (c[i][j] == LIVE)
            {
                //1. underpopulation
                if (n[i][j] < 2)
                {
                    c[i][j] = DEAD;
                }
                //2. overpopulation
                else if (n[i][j] > 3)
                {
                    c[i][j] = DEAD;
                }
                //4. life
                else if ((n[i][j] == 2) || (n[i][j] == 3))
                {
                    c[i][j] = LIVE;
                }
            }
            else
            {
                printf("Error: chars[%i %i] == %c\n", i, j, c[i][j]);
                break;
            }
        }
    }
}
void updatePxFromChar(struct px p[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], char c[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE])
{
    int i, j;

    //update pixel array from grid
    for (j = 0; j < WIN_HEIGHT/PX_SIZE; ++j)
    {
        for (i = 0; i < WIN_WIDTH/PX_SIZE; ++i)
        {
            if (c[i][j] == DEAD)
            {
                p[i][j].col.r = 0;
                p[i][j].col.g = 0;
                p[i][j].col.b = 0;
            }
            else if (c[i][j] == LIVE)
            {
                p[i][j].col.r = 0;
                p[i][j].col.g = 0xFF;
                p[i][j].col.b = 0;
            }
            else
            {
                printf("Error: chars[%i %i] == %c\n", i, j, c[i][j]);
                break;
            }
        }
    }
}
void renderGrid(SDL_Renderer* renderer, struct px arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE])
{
    int i, j;

    //render pixel array
    for ( i = 0; i < WIN_WIDTH/PX_SIZE; ++i )
    {
        for ( j = 0; j < WIN_HEIGHT/PX_SIZE; ++j )
        {
            SDL_SetRenderDrawColor(renderer, arr[i][j].col.r, arr[i][j].col.g, arr[i][j].col.b, arr[i][j].col.a);
            SDL_RenderFillRect(renderer, &arr[i][j].loc);
        }
    }
}
//object generators
//Statics
void addBlock(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y)
{
    arr[x+0][y+0] = LIVE;
    arr[x+0][y+1] = LIVE;
    arr[x+1][y+0] = LIVE;
    arr[x+1][y+1] = LIVE;
}
void addBeehive(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y)
{
    arr[x+0][y+1] = LIVE;
    arr[x+1][y+0] = LIVE;
    arr[x+1][y+2] = LIVE;
    arr[x+2][y+0] = LIVE;
    arr[x+2][y+2] = LIVE;
    arr[x+3][y+1] = LIVE;
}
void addLoaf(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y)
{
    arr[x+0][y+1] = LIVE;
    arr[x+1][y+0] = LIVE;
    arr[x+1][y+2] = LIVE;
    arr[x+2][y+0] = LIVE;
    arr[x+2][y+3] = LIVE;
    arr[x+3][y+1] = LIVE;
    arr[x+3][y+2] = LIVE;
}
void addBoat(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y)
{
    arr[x+0][y+0] = LIVE;
    arr[x+0][y+1] = LIVE;
    arr[x+1][y+0] = LIVE;
    arr[x+1][y+2] = LIVE;
    arr[x+2][y+1] = LIVE;
}
void addTub(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y)
{
    arr[x+0][y+1] = LIVE;
    arr[x+1][y+0] = LIVE;
    arr[x+1][y+2] = LIVE;
    arr[x+2][y+1] = LIVE;
}
//Oscillators
void addBlinker(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y)
{
    arr[x+1][y+0] = LIVE;
    arr[x+1][y+1] = LIVE;
    arr[x+1][y+2] = LIVE;
}
void addToad(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y)
{
    arr[x+0][y+1] = LIVE;
    arr[x+0][y+2] = LIVE;
    arr[x+1][y+3] = LIVE;
    arr[x+2][y+0] = LIVE;
    arr[x+3][y+1] = LIVE;
    arr[x+3][y+2] = LIVE;
}
void addBeacon(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y)
{
    addBlock(arr, x, y);
    addBlock(arr, x+2, y+2);
}
void addPulsar(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y)
{
    //left bars
    arr[x+0][y+2] = LIVE;
    arr[x+0][y+3] = LIVE;
    arr[x+0][y+4] = LIVE;
    arr[x+0][y+8] = LIVE;
    arr[x+0][y+9] = LIVE;
    arr[x+0][y+10] = LIVE;
    //top bars
    arr[x+2][y+0] = LIVE;
    arr[x+3][y+0] = LIVE;
    arr[x+4][y+0] = LIVE;
    arr[x+8][y+0] = LIVE;
    arr[x+9][y+0] = LIVE;
    arr[x+10][y+0] = LIVE;
    //right bars
    arr[x+12][y+2] = LIVE;
    arr[x+12][y+3] = LIVE;
    arr[x+12][y+4] = LIVE;
    arr[x+12][y+8] = LIVE;
    arr[x+12][y+9] = LIVE;
    arr[x+12][y+10] = LIVE;
    //bottom bars
    arr[x+2][y+12] = LIVE;
    arr[x+3][y+12] = LIVE;
    arr[x+4][y+12] = LIVE;
    arr[x+8][y+12] = LIVE;
    arr[x+9][y+12] = LIVE;
    arr[x+10][y+12] = LIVE;
    //top-right arm
    arr[x+10][y+5] = LIVE;
    arr[x+9][y+5] = LIVE;
    arr[x+8][y+5] = LIVE;
    arr[x+7][y+2] = LIVE;
    arr[x+7][y+3] = LIVE;
    arr[x+7][y+4] = LIVE;
    //bottom-right arm
    arr[x+10][y+7] = LIVE;
    arr[x+9][y+7] = LIVE;
    arr[x+8][y+7] = LIVE;
    arr[x+7][y+8] = LIVE;
    arr[x+7][y+9] = LIVE;
    arr[x+7][y+10] = LIVE;
    //bottom-left arm
    arr[x+2][y+7] = LIVE;
    arr[x+3][y+7] = LIVE;
    arr[x+4][y+7] = LIVE;
    arr[x+5][y+8] = LIVE;
    arr[x+5][y+9] = LIVE;
    arr[x+5][y+10] = LIVE;
    //top-left arm
    arr[x+2][y+5] = LIVE;
    arr[x+3][y+5] = LIVE;
    arr[x+4][y+5] = LIVE;
    arr[x+5][y+2] = LIVE;
    arr[x+5][y+3] = LIVE;
    arr[x+5][y+4] = LIVE;
    
}
void addPentadecathlon(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y)
{
    //top T part
    arr[x+0][y+2] = LIVE;
    arr[x+1][y+0] = LIVE;
    arr[x+1][y+1] = LIVE;
    arr[x+1][y+2] = LIVE;
    arr[x+2][y+2] = LIVE;
    //middle part
    //top bar
    arr[x+0][y+5] = LIVE;
    arr[x+1][y+5] = LIVE;
    arr[x+2][y+5] = LIVE;
    //middle bar
    arr[x+1][y+6] = LIVE;
    arr[x+1][y+7] = LIVE;
    arr[x+1][y+8] = LIVE;
    arr[x+1][y+9] = LIVE;
    //bottom bar
    arr[x+0][y+10] = LIVE;
    arr[x+1][y+10] = LIVE;
    arr[x+2][y+10] = LIVE;
    //bottom T part
    arr[x+0][y+13] = LIVE;
    arr[x+1][y+13] = LIVE;
    arr[x+1][y+14] = LIVE;
    arr[x+1][y+15] = LIVE;
    arr[x+2][y+13] = LIVE;
}
//Spaceships
void addGlider(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y)
{
    arr[x+1][y+0] = LIVE;
    arr[x+2][y+1] = LIVE;
    arr[x+0][y+2] = LIVE;
    arr[x+1][y+2] = LIVE;
    arr[x+2][y+2] = LIVE;
}
void addLWSS(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y)
{
    addBlock(arr, x, y+1);
    addBlock(arr, x+1, y);
    //bottom right L
    arr[x+2][y+3] = LIVE;
    arr[x+3][y+1] = LIVE;
    arr[x+3][y+2] = LIVE;
    arr[x+3][y+3] = LIVE;
    // + rightmost block
    arr[x+4][y+2] = LIVE;
}
//Guns
void addGliderGun(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y)
{
    //make glider gun; L2R
    //left block
    addBlock(arr, x, y+4);
    //left thing
    arr[x+5][y+5] = LIVE;
    arr[x+6][y+4] = LIVE;
    arr[x+6][y+5] = LIVE;
    arr[x+6][y+6] = LIVE;
    arr[x+7][y+3] = LIVE;
    arr[x+7][y+7] = LIVE;
    arr[x+8][y+2] = LIVE;
    arr[x+8][y+4] = LIVE;
    arr[x+8][y+5] = LIVE;
    arr[x+8][y+6] = LIVE;
    arr[x+8][y+8] = LIVE;
    arr[x+9][y+3] = LIVE;
    arr[x+9][y+4] = LIVE;
    arr[x+9][y+5] = LIVE;
    arr[x+9][y+6] = LIVE;
    arr[x+9][y+7] = LIVE;
    //middle thing
    //top part
    arr[x+15][y+2] = LIVE;
    arr[x+15][y+3] = LIVE;
    arr[x+16][y+4] = LIVE;
    //main part
    arr[x+15][y+6] = LIVE;
    arr[x+16][y+6] = LIVE;
    arr[x+17][y+6] = LIVE;
    arr[x+17][y+7] = LIVE;
    arr[x+18][y+7] = LIVE;
    arr[x+18][y+8] = LIVE;
    arr[x+19][y+7] = LIVE;
    arr[x+19][y+8] = LIVE;
    arr[x+19][y+5] = LIVE;
    arr[x+20][y+5] = LIVE;
    arr[x+20][y+6] = LIVE;
    arr[x+20][y+7] = LIVE;
    //right thing
    //left part
    arr[x+23][y+0] = LIVE;
    arr[x+23][y+1] = LIVE;
    arr[x+23][y+5] = LIVE;
    arr[x+23][y+6] = LIVE;
    arr[x+24][y+3] = LIVE;
    //right part
    arr[x+25][y+0] = LIVE;
    arr[x+25][y+6] = LIVE;
    arr[x+26][y+1] = LIVE;
    arr[x+26][y+2] = LIVE;
    arr[x+26][y+4] = LIVE;
    arr[x+26][y+5] = LIVE;
    arr[x+27][y+2] = LIVE;
    arr[x+27][y+4] = LIVE;
    arr[x+28][y+3] = LIVE;
    arr[x+29][y+3] = LIVE;
    //right block
    addBlock(arr, x+34, y+2);
}
//other
void addPx(char arr[WIN_WIDTH/PX_SIZE][WIN_HEIGHT/PX_SIZE], int x, int y)
{
    arr[x+0][y+0] = LIVE;
}
