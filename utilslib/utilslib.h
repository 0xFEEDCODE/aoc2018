#pragma once
#include <iostream>

#define PRINT(value) (std::cout << (value) << std::endl)

void rewind_stdin_on_eof();

bool redirect_file_to_stdin(const char* filename);
bool redirect_inp_to_stdin();
