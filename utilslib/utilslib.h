#pragma once
#include <cstdint>
#include <iosfwd>
#include <iostream>

using namespace std;

#define PRINT(value) (std::cout << (value) << std::endl)

struct point2d
{
    int32_t x;
    int32_t y;

    point2d(int32_t x, int32_t y) : x(x), y(y)
    {
    }

public:
    void Print()
    {
        cout << x << " " << y << endl;
    }

    bool operator==(const point2d& other) const
    {
        return x == other.x && y == other.y;
    }
};

void rewind_stdin_on_eof();

bool redirect_file_to_stdin(const char* filename);
bool redirect_inp_to_stdin();

inline void reset_stdin_to_console()
{
    freopen("CON", "r", stdin);
}

inline void ClearConsoleOutput()
{
#if defined _WIN32
    system("cls");
    //clrscr(); // including header file : conio.h
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    system("clear");
    //std::cout<< u8"\033[2J\033[1;1H"; //Using ANSI Escape Sequences
#elif defined (__APPLE__)
    system("clear");
#endif
}
