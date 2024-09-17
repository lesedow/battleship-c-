#include <iostream>
#include <stdio.h>
#include <time.h>

#include <array>
#include <vector>
#include <algorithm>
#include <limits>

const size_t MAX_SHIPS = 5;
const size_t GRID_SIZE = 10;

enum CurrentTurn
{
    PLAYER,
    CPU
};

struct GridPos
{
    int row;
    int col;
};

struct Part
{
    GridPos position;
    bool isDestroyed;
};

struct Ship
{
    int size;
    std::vector<Part> parts;
    bool isSunken;
};

typedef std::array<std::array<char, GRID_SIZE>, GRID_SIZE> Grid;
typedef std::array<Ship, MAX_SHIPS> ShipArray;

// Grid Related
void showGrids(Grid &playerGrid, Grid &attackGrid);
void populateGrid(Grid &grid);
bool isValidPosition(GridPos &position);
bool isEmptyPosition(GridPos &position, Grid &grid);

// Ship Related
void createShips(ShipArray &ships, Grid &grid);
bool placeParts(Ship &ship, Grid &grid, GridPos &position);
void placeShip(Grid &grid, Ship &ship);
Part& getPartAtPosition(ShipArray &ships, GridPos &position);
bool areAllShipsSunken(ShipArray &ships);

// Input Related
GridPos getUserInput();

// Game loop related
void doPlayerTurn(Grid &cpuGrid, ShipArray &cpuShips, Grid &attackGrid, CurrentTurn &currentTurn);
void doCpuTurn(CurrentTurn &currentTurn);

int main()
{
    CurrentTurn currentTurn = PLAYER;
    bool gameEnded = false;

    srand(time(NULL));

    // Player Grids
    Grid playerGrid;
    Grid playerAttackGrid;

    // CPU Grids
    Grid cpuGrid;
    Grid cpuAttackGrid;

    // Ships Arrays
    ShipArray playerShips;
    ShipArray cpuShips;

    populateGrid(playerGrid);
    populateGrid(cpuGrid);
    populateGrid(playerAttackGrid);
    populateGrid(cpuAttackGrid);

    createShips(playerShips, playerGrid);
    createShips(cpuShips, cpuGrid);

    showGrids(playerGrid, playerAttackGrid);

    while (!gameEnded)
    {
        switch (currentTurn)
        {
            case PLAYER:
                doPlayerTurn(cpuGrid, cpuShips, playerAttackGrid, currentTurn);
                break;
            case CPU:
                doCpuTurn(currentTurn);
                break;
        }
        showGrids(playerGrid, playerAttackGrid);
    }
}

void showGrids(Grid &playerGrid, Grid &attackGrid)
{
    printf("######## Player Grid #######\t");
    printf("######## Attack Grid #######\t");

    for (int i = 0; i < playerGrid.size(); i++)
    {
        printf("\n");
        for (int j = 0; j < playerGrid.size(); j++)
        {
            printf("%c  ", playerGrid[i][j]);
        }

        printf("\t");

        for (int j = 0; j < playerGrid.size(); j++)
        {
            printf("%c  ", attackGrid[i][j]);
        }
    }

    printf("\n");
}

void populateGrid(Grid &grid)
{
    for (int i = 0; i < grid.size(); i++)
    {
        for (int j = 0; j < grid.size(); j++)
        {
            grid[i][j] = '*';
        }
    }
}

void createShips(ShipArray &ships, Grid &grid)
{
    std::array<int, MAX_SHIPS> shipSizes = { 2, 3, 3, 4, 5 };

    for (int i = 0; i < MAX_SHIPS; i++)
    {
        ships[i].size = shipSizes[i];
        ships[i].isSunken = false;

        placeShip(grid, ships[i]);
    }

}

bool isValidPosition(GridPos &position)
{
    return (
        (position.row < GRID_SIZE && !(position.row < 0)) &&
        (position.col < GRID_SIZE && !(position.col < 0))
    );
}

bool isEmptyPosition(GridPos &position, Grid &grid)
{
    return grid[position.row][position.col] == '*';
}

bool areAllShipsSunken(ShipArray &ships)
{
    int numOfSunkenShips = 0;

    for (auto& ship : ships)
    {
        if (ship.isSunken) numOfSunkenShips++;
    }

    return numOfSunkenShips == ships.size();
}

bool placeParts(Ship &ship, Grid &grid, GridPos &position)
{
    std::array<GridPos, 4> directions;
    directions[0] = { 0, 1 };
    directions[1] = { 0, -1 };
    directions[2] = { 1, 0 };
    directions[3] = { -1, 0 };

    for (auto dir : directions)
    {
        std::vector<GridPos> validCells;

        for (int cell = 0; cell < ship.size; cell++)
        {
            GridPos nextCell = {
                {position.row + (dir.row * cell)},
                {position.col + (dir.col * cell)}
            };

            if (!isValidPosition(nextCell) || !isEmptyPosition(nextCell, grid)) continue;

            validCells.push_back(nextCell);
        }

        if (validCells.size() == ship.size)
        {
            for (auto cell : validCells)
            {
                Part newPart = { cell.row, cell.col, false };
                ship.parts.push_back(newPart);

                grid[cell.row][cell.col] = '#';
            }

            return true;
        }

        return false;
    }
}

void placeShip(Grid &grid, Ship &ship)
{
    bool partsPlaced = false;

    while (!partsPlaced)
    {
        GridPos newPosition;
        newPosition.row = rand() % GRID_SIZE;
        newPosition.col = rand() % GRID_SIZE;

        if (!isValidPosition(newPosition) || !isEmptyPosition(newPosition, grid)) continue;

        partsPlaced = placeParts(ship, grid, newPosition);
    }
}

Part& getPartAtPosition(ShipArray &ships, GridPos &position)
{
    for (auto ship : ships)
    {
        for (auto part : ship.parts)
        {
            if (
                part.position.row == position.row &&
                part.position.col == position.col
                ) return part;
        }
    }
}

GridPos getUserInput()
{
    GridPos userChoice;
    bool done = false;

    // Check for invalid input
    do
    {
        printf("Enter row-col(ex: 0 and 2):\n");

        std::cin >> userChoice.row;
        std::cin >> userChoice.col;

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore();
            continue;
        }

        done = true;


    } while (!done);

    return userChoice;
}

void doPlayerTurn(Grid &cpGrid, ShipArray &cpuShips, Grid &playerAttackGrid, CurrentTurn &currentTurn)
{
    GridPos userChoice = getUserInput();

    if (!isValidPosition(userChoice))
    {
        printf("Invalid position!\n");
        return;
    }

    if (!isEmptyPosition(userChoice, cpGrid))
    {

        Part &partAtPos = getPartAtPosition(cpuShips, userChoice);
        if (partAtPos.isDestroyed)
        {
            printf("Part at this position is already destroyed!\n");
            return;
        }

         //Reveal part on attack grid
        playerAttackGrid[partAtPos.position.row][partAtPos.position.col] = '#';

        // Set part to destroyed
        partAtPos.isDestroyed = true;

        // Check if all of the ship parts are destroyed
        // If they are, sunk the ship
        // After that check if 

    }
    else {
        if (playerAttackGrid[userChoice.row][userChoice.col] == '-')
        {
            printf("This position was already attacked and it's a missed spot!\n");
            doPlayerTurn(cpGrid, cpuShips, playerAttackGrid, currentTurn);
        }
        // Set position to missed on attack grid
        playerAttackGrid[userChoice.row][userChoice.col] = '-';
    }

    // Change Turn
    currentTurn = CPU;
}

void doCpuTurn(CurrentTurn& currentTurn)
{
    printf("Cpu did a turn!\n");
    currentTurn = PLAYER;
}