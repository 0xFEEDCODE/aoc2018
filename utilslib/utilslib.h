#pragma once
#include <iostream>

void rewind_stdin_on_eof();

bool redirect_file_to_stdin(const char* filename);

template<class T>
void print(T value)
{
    std::cout << value;
}