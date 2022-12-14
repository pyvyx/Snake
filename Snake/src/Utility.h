#pragma once
#include <array>
#include <random>
#include <iterator>
#include <vector>

#include "raylib.h"

#include "Constants.h"

namespace Utility
{
    inline std::vector<Rectangle>::iterator GetRandomIterator(std::vector<Rectangle>::iterator start, std::vector<Rectangle>::iterator end)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        const int distance = static_cast<int>(std::distance(start, end) - 1);
        if (distance < 0)
            return start;
        std::uniform_int_distribution<> dis(0, distance);
        std::advance(start, dis(gen));
        return start;
    }


    inline Rectangle GetRandomCell(std::vector<Rectangle>& emptyCells)
    {
        const std::vector<Rectangle>::iterator it = Utility::GetRandomIterator(emptyCells.begin(), emptyCells.end());
        const Rectangle rect = *it;
        emptyCells.erase(it);
        return rect;
    }


    inline bool RectanglesAreSame(const Rectangle& rect1, const Rectangle& rect2)
    {
        return rect1.x == rect2.x && rect1.y == rect2.y && rect1.width == rect2.width && rect1.height == rect2.height;
    }


    constexpr std::array<Rectangle, Const::GridSize> GenerateGrid()
    {
        std::array<Rectangle, Const::GridSize> grid{};
        uint16_t currentX = 0;
        uint16_t currentY = 0;

        for (size_t i = 0; i < grid.size(); ++i)
        {
            grid[i].x = currentX;
            grid[i].y = currentY;
            grid[i].width = Const::CellSize;
            grid[i].height = Const::CellSize;

            const bool endOfLine = (i + 1) % Const::BoardWidth == 0;
            currentX = endOfLine ? 0 : static_cast<uint16_t>(currentX + Const::CellSize);
            currentY = endOfLine ? static_cast<uint16_t>(currentY + Const::CellSize) : currentY;
        }
        return grid;
    }
}

namespace Const
{
    inline constexpr std::array<Rectangle, Const::GridSize> Grid = Utility::GenerateGrid();
}