#include <sstream>
#include <string>

#include "../utilslib/utilslib.h"

using namespace std;

string process_polymer(const string& original_polymer)
{
    bool reaction = true;

    stringstream polymer_new;
    polymer_new << original_polymer;
    
    while(reaction)
    {
        string polymer_current = polymer_new.str();
        polymer_new.str("");
        
        reaction = false;

        int i = 0;
        while(i < polymer_current.size())
        {
            if(abs(polymer_current[i]-polymer_current[i+1]) == 32)
            {
                i+=2;
                reaction = true;
            }
            else
            {
                polymer_new << polymer_current[i];
                i++;
            }
        }
    }
    
    return polymer_new.str();
}

int main(int argc, char* argv[])
{
    redirect_inp_to_stdin();

    string polymer;
    getline(cin, polymer);

    polymer = process_polymer(polymer);
    
    PRINT(polymer);
    PRINT(polymer.size());

    getline(cin, polymer);

    uint32_t shortest_polymer_recorded = polymer.size();
    for (int letter = 'A'; letter <= 'Z'; ++letter)
    {
        stringstream new_polymer;

        int i = 0;
        while(i < polymer.size())
        {
            if(letter == polymer[i] || (letter+32) == polymer[i])
            {
                i+=1;
            }
            else
            {
                new_polymer << polymer[i];
                i++;
            }
        }
        
        if(letter == 'C')
        {
            PRINT(new_polymer.str());
        }

        const string processed_polymer = process_polymer(new_polymer.str());
        shortest_polymer_recorded = min(shortest_polymer_recorded, static_cast<uint32_t>(processed_polymer.size()));
    }

    PRINT(shortest_polymer_recorded);
    
    return 0;
}
