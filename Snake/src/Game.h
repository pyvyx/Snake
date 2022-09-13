#pragma once
#include <array>
#include <string>

#include "raylib.h"

#include "Snake.h"
#include "EndText.h"
#include "DisplayText.h"
#include "Constants.h"
#include "Score.h"
#include "Layer.h"


static constexpr std::array<Rectangle, Const::GridSize> GenerateGrid()
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


inline int GetApplePos(int cells)
{
    const int value = GetRandomValue(0, cells * Const::CellSize);
    const int modCellSize = value % Const::CellSize;
    if (modCellSize == 0)
        return value;

    // e.g. v = 74, cs = 50 => mcs = 24 ==> round down
    const int remainder = Const::CellSize - modCellSize;
    if (remainder > Const::CellSize % 2)
        return value - modCellSize;

    // e.g. v = 76, cs = 50 => mcs = 26 ==> round up
    // cs - mcs = 24 => v + 24 = 100
    return value + remainder;
}


inline Rectangle GetApple(const Snake& snake)
{
    Rectangle apple;
    apple.width = Const::CellSize;
    apple.height = Const::CellSize;

    // TODO: keep track of empty cells than randomly choose one to spawn the apple,
    // since this loop can take very long if only 1 cell is left
    // However we can leave it this way until I encounter that it takes a very long time
    do
    {
        apple.x = static_cast<float>(GetApplePos(Const::BoardWidth - 1));
        apple.y = static_cast<float>(GetApplePos(Const::BoardHeight - 1));
    } while (snake.Collision(apple));
    return apple;
}


inline bool& GameSetStarted()
{
    static bool started = false;
    return started;
}


inline bool GameHasStarted()
{
    return GameSetStarted();
}


class Game final : public Layer
{
private:
    static constexpr std::array<Rectangle, Const::GridSize> s_Grid = GenerateGrid();
    static constexpr Color s_GridColorDone{ 255, 255, 255, 75 };

    Snake snake{ Const::CellSize * (Const::BoardWidth / 2), Const::CellSize * (Const::BoardHeight / 2) };
    Rectangle apple = GetApple(snake);

    Score score;
    EndText doneTxt;
    bool finished = false;
    bool reset = false;
    Color gridColor = s_GridColorDone;
private:
    inline void Finish(bool removeApple = false)
    {
        if (removeApple)
        {
            apple.x = Const::AppleWinOffset;
            apple.y = Const::AppleWinOffset;
        }

        finished = true;
        gridColor = s_GridColorDone;
    }


    inline void Reset()
    {
        snake.Reset();
        apple = GetApple(snake);

        score = 0;
        reset = true;
    }


    inline bool Started()
    {
        static bool started = false;
        if (!started && GameHasStarted())
        {
            gridColor = WHITE;
            started = true;
        }
        return started;
    }
public:
    inline void OnKeyPress(int keyPressed, float dt) override
    {
        if (!finished && snake.HandleInput(keyPressed, dt))
            Finish();

        if (finished && keyPressed == KEY_SPACE)
            Reset();
    }


    inline void OnUpdate(float dt) override
    {
        if (!Started())
            return;

        if (!finished && CheckCollisionRecs(apple, snake.GetHead()))
        {
            if (!snake.Append())
            {
                doneTxt.SetWin();
                Finish(true);
            }
            else
                apple = GetApple(snake);
            ++score;
        }


        if (reset && doneTxt.Reset(dt))
        {
            reset = false;
            finished = false;
            gridColor = WHITE;
        }
    }


    inline void OnRender() const override
    {
        for (const Rectangle& cell : s_Grid)
            DrawRectangleLinesEx(cell, Const::GridOutlineThickness, gridColor);
        DrawRectangleRec(apple, RED);
        snake.Draw();
        score.Draw();
        doneTxt.Draw(finished);
        DrawFPS(0, 0);
    }
};