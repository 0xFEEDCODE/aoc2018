#pragma warning(disable:4996)

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <bits/fs_fwd.h>
#include <bits/fs_path.h>

#include "../utilslib/utilslib.h"

using namespace std;

int main(int argc, char* argv[])
{
    redirect_inp_to_stdin();

    int ans1 = 0;
    
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), stdin) != nullptr)
    {
        int value = atoi(buffer + 1);

        if (buffer[0] == '+')
        {
            ans1 += value;
        }
        else
        {
            ans1 -= value;
        }
    }

    PRINT(ans1);
    
    map<int, char> capturedFrequencies;
    int freq = 0;
    int ans2 = 0;
    
    rewind_stdin_on_eof();
    while (fgets(buffer, sizeof(buffer), stdin) != nullptr)
    {
        freq += stoi(buffer);
        
        if(capturedFrequencies[freq] != 0)
        {
            ans2 = freq;
            break;
        }
        
        capturedFrequencies[freq] = 1;

        rewind_stdin_on_eof();
    }
    
    PRINT(ans2);
    
    return 0;
}
