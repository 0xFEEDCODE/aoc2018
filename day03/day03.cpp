#include <sstream>

#include "../utilslib/utilslib.h"
using namespace std;

struct Claim {
    int id;
    int left;
    int top;
    int width;
    int height;
};

/*
Claim parse_claim(const std::string& claim_str) {
    Claim claim;
    char discard;
    std::stringstream ss(claim_str);
    ss >> discard >> claim.id >> discard >> claim.left >> discard >> claim.top >> discard >> claim.width >> discard >> claim.height;
    return claim;
}
*/

Claim parse_claim(const std::string &claim_str) {
    Claim claim;
    char *context = nullptr;
    char str[256];
    strcpy_s(str, claim_str.c_str());

    int* fields[] = { &claim.id, &claim.left, &claim.top, &claim.width, &claim.height };

    string delimiter = "# @,:x";
    char* token = strtok_s(str, delimiter.data(), &context);

    int i = 0;
    while(token != nullptr)
    {
        *fields[i++] = std::stoi(token);
        token = strtok_s(nullptr, delimiter.data(), &context);
    }

    return claim;
}

int main(int argc, char* argv[])
{
    redirect_file_to_stdin("inp.txt");

    char buffer[256];
    while(fgets(buffer, sizeof(buffer), stdin) != nullptr)
    {
        auto claim = parse_claim(buffer);
        PRINT(claim.id);
        PRINT(claim.left);
        PRINT(buffer);
    }
    
    return 0;
}
