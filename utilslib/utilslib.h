#pragma once
#include <iostream>
#include <stdlib.h>

using namespace std;

#define PRINT(value) (std::cout << (value) << std::endl)

struct point2d
{
    int x;
    int y;

  public:
    void print()
    {
        cout << x << " " << y << endl;
    }
};

void rewind_stdin_on_eof();

bool redirect_file_to_stdin(const char *filename);
bool redirect_inp_to_stdin();
