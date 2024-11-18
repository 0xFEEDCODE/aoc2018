#include "utilslib.h"
#include <cstdio>
#include <iostream>

using namespace std;

void rewind_stdin_on_eof()
{
    if (feof(stdin))
    {
        clearerr(stdin);
        fseek(stdin, 0, SEEK_SET);
    }
}

bool redirect_file_to_stdin(const char *filename)
{
    FILE *file = freopen(filename, "r", stdin);
    if (file == nullptr)
    {
        perror("Error opening file");
        return false;
    }
    return true;
}

bool redirect_inp_to_stdin()
{
    FILE *file = freopen("inp.txt", "r", stdin);
    if (file == nullptr)
    {
        perror("Error opening file");
        return false;
    }
    return true;
}
