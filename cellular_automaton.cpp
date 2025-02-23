#include <emscripten.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdint> // For uint8_t

// Define the grid dimensions
const int WIDTH = 50;
const int HEIGHT = 50;

// The current state of the grid (using uint8_t instead of bool)
std::vector<uint8_t> grid(WIDTH *HEIGHT);
// Temporary grid for the next state
std::vector<uint8_t> nextGrid(WIDTH *HEIGHT);

// Initialize the grid with random values
extern "C"
{
    EMSCRIPTEN_KEEPALIVE
    void initializeGrid()
    {
        srand(time(nullptr));
        for (int i = 0; i < WIDTH * HEIGHT; i++)
        {
            grid[i] = rand() % 2;
        }
    }

    // Count the number of live neighbors for a cell
    int countNeighbors(int x, int y)
    {
        int count = 0;

        for (int dy = -1; dy <= 1; dy++)
        {
            for (int dx = -1; dx <= 1; dx++)
            {
                if (dx == 0 && dy == 0)
                    continue; // Skip the cell itself

                // Wrap around grid edges (toroidal)
                int nx = (x + dx + WIDTH) % WIDTH;
                int ny = (y + dy + HEIGHT) % HEIGHT;

                if (grid[ny * WIDTH + nx])
                {
                    count++;
                }
            }
        }

        return count;
    }

    // Update the grid based on the rules of Conway's Game of Life
    EMSCRIPTEN_KEEPALIVE
    void updateGrid()
    {
        for (int y = 0; y < HEIGHT; y++)
        {
            for (int x = 0; x < WIDTH; x++)
            {
                int index = y * WIDTH + x;
                int neighbors = countNeighbors(x, y);
                bool alive = grid[index] != 0;

                // Apply Conway's Game of Life rules
                if (alive && (neighbors < 2 || neighbors > 3))
                {
                    nextGrid[index] = 0; // Die from underpopulation or overpopulation
                }
                else if (!alive && neighbors == 3)
                {
                    nextGrid[index] = 1; // Reproduction
                }
                else
                {
                    nextGrid[index] = grid[index]; // Stay the same
                }
            }
        }

        // Swap grids
        grid.swap(nextGrid);
    }

    // Get a pointer to the grid data for JavaScript to access
    EMSCRIPTEN_KEEPALIVE
    uint8_t *getGridData()
    {
        return grid.data();
    }

    // Get grid dimensions
    EMSCRIPTEN_KEEPALIVE
    int getWidth()
    {
        return WIDTH;
    }

    EMSCRIPTEN_KEEPALIVE
    int getHeight()
    {
        return HEIGHT;
    }
}