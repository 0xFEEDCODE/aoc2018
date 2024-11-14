#include "../utilslib/utilslib.h"

int main(int argc, char* argv[])
{
    redirect_file_to_stdin(argv[1]);

    char buffer[256];
    while(fgets(buffer, sizeof(buffer), stdin) != nullptr)
    {
        print(buffer);
    }
    
    return 0;
}
