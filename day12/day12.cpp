#include <sstream>

#include "../utilslib/utilslib.h"

int main()
{
    redirect_inp_to_stdin();


    string first_line;
    getline(cin, first_line);
    string discard;
    istringstream iss(first_line);

    string initial_configuration;
    iss >> discard >> discard >> initial_configuration;

    cout << initial_configuration;

    constexpr int size = 7;
    for (int i = size; i < initial_configuration.size(); i += size)
    {
    }
}
