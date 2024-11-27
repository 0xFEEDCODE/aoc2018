#pragma once
#include <cstdint>
#include <iostream>
#include <list>

using namespace std;

#define PRINT(value) (std::cout << (value) << std::endl)

namespace Graph
{
    template <typename T>
    struct Vertex
    {
        T Value;
        optional<int> Index;
        list<Vertex *> Edges = list<Vertex *>();
    };
} // namespace Graph

struct Point2D
{
    int32_t x;
    int32_t y;

    Point2D(int32_t x, int32_t y) : x(x), y(y)
    {
    }

public:
    void Print()
    {
        cout << x << " " << y << endl;
    }

    bool operator==(const Point2D &other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator<(const Point2D &other) const
    {
        if (x == other.x)
        {
            return y < other.y;
        }
        return x < other.x;
    }

    Point2D()
    {
        x = -1;
        y = -1;
    }
};

void rewind_stdin_on_eof();

bool redirect_file_to_stdin(const char *filename);
bool redirect_inp_to_stdin();

inline void reset_stdin_to_console()
{
    freopen("CON", "r", stdin);
}

inline void ClearConsoleOutput()
{
#if defined _WIN32
    system("cls");
    // clrscr(); // including header file : conio.h
#elif defined(__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    system("clear");
    // std::cout<< u8"\033[2J\033[1;1H"; //Using ANSI Escape Sequences
#elif defined(__APPLE__)
    system("clear");
#endif
}
