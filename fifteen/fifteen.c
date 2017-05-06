/**
 * fifteen.c
 *
 * Implements Game of Fifteen (generalized to d x d).
 *
 * Usage: fifteen d
 *
 * whereby the board's dimensions are to be d x d,
 * where d must be in [DIM_MIN,DIM_MAX]
 *
 * Note that usleep is obsolete, but it offers more granularity than
 * sleep and is simpler to use than nanosleep; `man usleep` for more.
 */
 
#define _XOPEN_SOURCE 500

#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// constants
#define DIM_MIN 3
#define DIM_MAX 9

// board
int board[DIM_MAX][DIM_MAX];

// dimensions
int d;

int tile_i;
int tile_j;
int blank_i;
int blank_j;

// prototypes
void clear(void);
void greet(void);
void init(void);
void draw(void);
bool move(int tile);
bool won(void);

void search(int tile);
bool legalmove(void);
void swaptile(int tile);

int main(int argc, string argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        printf("Usage: fifteen d\n");
        return 1;
    }

    // ensure valid dimensions
    d = atoi(argv[1]);
    if (d < DIM_MIN || d > DIM_MAX)
    {
        printf("Board must be between %i x %i and %i x %i, inclusive.\n",
            DIM_MIN, DIM_MIN, DIM_MAX, DIM_MAX);
        return 2;
    }

    // open log
    FILE* file = fopen("log.txt", "w");
    if (file == NULL)
    {
        return 3;
    }

    // greet user with instructions
    greet();

    // initialize the board
    init();

    // accept moves until game is won
    while (true)
    {
        // clear the screen
        clear();

        // draw the current state of the board
        draw();

        // log the current state of the board (for testing)
        for (int i = 0; i < d; i++)
        {
            for (int j = 0; j < d; j++)
            {
                fprintf(file, "%i", board[i][j]);
                if (j < d - 1)
                {
                    fprintf(file, "|");
                }
            }
            fprintf(file, "\n");
        }
        fflush(file);

        // check for win
        if (won())
        {
            printf("ftw!\n");
            break;
        }

        // prompt for move
        printf("Tile to move: ");
        int tile = GetInt();
        
        // quit if user inputs 0 (for testing)
        if (tile == 0)
        {
            break;
        }

        // log move (for testing)
        fprintf(file, "%i\n", tile);
        fflush(file);

        // move if possible, else report illegality
        if (!move(tile))
        {
            printf("\nIllegal move.\n");
            usleep(500000);
        }

        // sleep thread for animation's sake
        usleep(500000);
    }
    
    // close log
    fclose(file);

    // success
    return 0;
}

/**
 * Clears screen using ANSI escape sequences.
 */
void clear(void)
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

/**
 * Greets player.
 */
void greet(void)
{
    clear();
    printf("WELCOME TO GAME OF FIFTEEN\n");
    usleep(2000000);
}

/**
 * Initializes the game's board with tiles numbered 1 through d*d - 1
 * (i.e., fills 2D array with values but does not actually print them).  
 */
void init(void)
{
    // Need to know the number of slots needed
    int n = (d*d) - 1;
    
    // Loop through 2D array with columns then rows
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            board[i][j] = n;
            n--;
        }
        printf("\n");
    }
    
    // Check if the number of tiles is odd. If so swap the 1 and 2 tiles.
    if (((d*d) - 1) % 2 != 0)
    {
        int temp = board[d-1][d-2];
        board[d-1][d-2] = board[d-1][d-3];
        board[d-1][d-3] = temp;
    }
    
    // Mark position of blank tile (bottom right)
    blank_i = d-1;
    blank_j = d-1;
}

/**
 * Prints the board in its current state.
 */
void draw(void)
{
    // Prints the board, but if the value is 0 print an underscore instead.
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            if (board[i][j] < 10) //formatting for single digits is different.
            {
                if (board[i][j] == 0) //checking if value is 0 to print a blank tile.
                {
                    printf(" [_] ");
                }
                else
                {
                printf(" [%d] ", board[i][j]);
                }
            }
            else
            {
                printf("[%d] ", board[i][j]);
            }
        }
        printf("\n"); //space between each line
    }
}

/**
 * If tile borders empty space, moves tile and returns true, else
 * returns false. 
 */
bool move(int tile)
{
    // check if tile actually exists
    if (tile > (d*d)-1 || tile < 1)
        return false;
    
    // Linear search for the tile user gives us
    search(tile);
    
    // Once tile has been found see if blank tile is adjacent to it
    // Check if adjacent tile is a legit tile, then check if is blank
    if (legalmove())
    {
        swaptile(tile); // if legal move swap blank tile with tile
        return true;
    }
    else
        return false;
    
    return false;
}

/**
 * Returns true if game is won (i.e., board is in winning configuration), 
 * else false.
 */
bool won(void)
{
    // linear search through array and check if increments in order
    int counter = 1;
    
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            if (board[i][j] == counter)
                counter++;
        } 
    }
    
    if (counter == d*d && board[d-1][d-1] == 0)
        return true;
    else
        return false;
}

// linear searches for the selected tile's location. no need for faster search.
void search(int tile)
{
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            if (board[i][j] == tile)
            {
                tile_i = i;
                tile_j = j;
            } 
        }
    }
}

// check if the blank tile is next to the tile
bool legalmove(void)
{
    // check if on top row. If so check up for 0
    if (tile_i > 0 && board[tile_i - 1][tile_j] == 0)
        return true;
    // bottom
    if (tile_i < d-1 && board[tile_i + 1][tile_j] == 0)
        return true;
    // left
    if (tile_j > 0 && board[tile_i][tile_j - 1] == 0)
        return true;
    // right
    if (tile_j < d-1 && board[tile_i][tile_j + 1] == 0)
        return true;
    else
        return false;
}

// swaps tile with blank tile
void swaptile(tile)
{
    int temp = board[tile_i][tile_j];
    board[tile_i][tile_j] = board[blank_i][blank_j];
    board[blank_i][blank_j] = temp;
    
    // update location of blank tile
    blank_i = tile_i;
    blank_j = tile_j;
}