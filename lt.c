/*
    Conway's Game of Life Repolica - terminal version
    Rules:
        1. Any LIVE cell with less than 2 live neighbours dies, underpopulation
        2. Any LIVE cell with more than 3 live neighbours dies, overpopulation
        3. Any DEAD cell with exactly   3 living neighbours becomes LIVE, birth
        4. Any LIVE cell with           2-3 living neighbours stays LIVE, life
*/
#include <stdio.h>
#include <unistd.h>

const int W = 128;
const int H = 48;
const char LIVE = '#';
const char DEAD = '-';

int main (int argc, char** argv)
{
    int i, j;
    //grid of chars with LIVE and DEAD
    char grid[W][H];
        for (j = 0; j < H; ++j)
            for (i = 0; i < W; ++i)
                grid[i][j] = DEAD;
    //grid of count of living neighbours
    char ln[W][H];
        for (j = 0; j < H; ++j)
            for (i = 0; i < W; ++i)
                ln[i][j] = 0;

    //make top left glider
    grid[1][0] = LIVE;
    grid[2][1] = LIVE;
    grid[0][2] = LIVE;
    grid[1][2] = LIVE;
    grid[2][2] = LIVE;

    while(1)
    {
        //count neighbours
        for (j = 0; j < H; ++j)
        {
            for (i = 0; i < W; ++i)
            {
                //clear
                ln[i][j] = 0;
                //starting above, going clockwise; 2 to a block
                if (j > 0)
                {
                    if (grid[i][j-1] == LIVE)
                    {
                        ++ln[i][j];         //up
                    }
                    if (i < (W-1))
                    {
                        if (grid[i+1][j-1] == LIVE)
                        {
                            ++ln[i][j];     //up-right
                        }
                    }
                }
                if (i < (W-1))
                {
                    if (grid[i+1][j] == LIVE)
                    {
                        ++ln[i][j];         //right
                    }
                    if (i < (H-1))
                    {
                        if (grid[i+1][j+1] == LIVE)
                        {
                            ++ln[i][j];     //right-down
                        }
                    }
                }
                if (j < (H-1))
                {
                    if (grid[i][j+1] == LIVE)
                    {
                        ++ln[i][j];         //down
                    }
                    if (i > 0)
                    {
                        if (grid[i-1][j+1] == LIVE)
                        {
                            ++ln[i][j];     //down-left
                        }
                    }
                }
                if (i > 0)
                {
                    if (grid[i-1][j] == LIVE)
                    {
                        ++ln[i][j];         //left
                    }
                    if (j > 0)
                    {
                        if (grid[i-1][j-1] == LIVE)
                        {
                            ++ln[i][j];     //left-up
                        }
                    }
                }
            }
        }

        //life happens
        for (j = 0; j < H; ++j)
        {
            for (i = 0; i < W; ++i)
            {
                //3. birth
                if (grid[i][j] == DEAD)
                {
                    if (ln[i][j] == 3)
                    {
                        grid[i][j] = LIVE;
                    }
                }
                else if (grid[i][j] == LIVE)
                {
                    //1. underpopulation
                    if (ln[i][j] < 2)
                    {
                        grid[i][j] = DEAD;
                    }
                    //2. overpopulation
                    else if (ln[i][j] > 3)
                    {
                        grid[i][j] = DEAD;
                    }
                    //4. life
                    else if ((ln[i][j] == 2) || (ln[i][j] == 3))
                    {
                        grid[i][j] = LIVE;
                    }
                }
                else
                {
                    printf("Error: grid[%i %i] == %c\n", i, j, grid[i][j]);
                    break;
                }
            }
        }

        //render?
        for (j = 0; j < H; ++j)
        {
            for (i = 0; i < W; ++i)
                printf("%c", grid[i][j]);
            printf("\n");
        }

        sleep(1);
    }

    return 0;
}
