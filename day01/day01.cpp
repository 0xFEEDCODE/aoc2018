#pragma warning(disable:4996)

#include <cstdio>
#include <cstdlib>
#include <map>
#include "../utilslib/utilslib.h"

using namespace std;

int main(int argc, char* argv[])
{
    puts(argv[1]);

    redirect_file_to_stdin(argv[1]);

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


    print(ans1);
    
    map<int, char> capturedFrequencies;
    int freq = 0;
    int ans2 = 0;
    
    rewind_stdin_on_eof();
    while (fgets(buffer, sizeof(buffer), stdin) != nullptr)
    {
        int value = atoi(buffer + 1);

        if (buffer[0] == '+')
        {
            freq += value;
        }
        else
        {
            freq -= value;
        }
        
        if(capturedFrequencies[freq] != 0)
        {
            ans2 = freq;
            break;
        }
        
        capturedFrequencies[freq] = 1;

        rewind_stdin_on_eof();
    }
    
    print(ans2);
    
    return 0;
}
