#include <cstdio>
#include <list>
#include <map>
#include <queue>

#include "../utilslib/utilslib.h"
using namespace std;

#include <chrono>

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    redirect_inp_to_stdin();

    char buffer[256];

    int idsWithTwoLetterOccurence = 0;
    int idsWithThreeLetterOccurence = 0;
    
    list<string> candidates;

    while(fgets(buffer, sizeof(buffer), stdin) != nullptr)
    {
        candidates.emplace_back(buffer);
        
        map<char, int> occurences;

        for(int i = 0; buffer[i] != '\0'; i++)
        {
            occurences[buffer[i]]++;
        }

        bool has_two = false;
        bool has_three = false;
        for (const auto& kv : occurences)
        {
            if (kv.second == 2 && !has_two)
            {
                idsWithTwoLetterOccurence++;
                has_two = true;
            }
            if (kv.second == 3 && !has_three)
            {
                idsWithThreeLetterOccurence++;
                has_three  = true;
            }
        }
    }

    const int ans1 = idsWithTwoLetterOccurence * idsWithThreeLetterOccurence;
    string ans2;

    const int id_len = static_cast<int>(candidates.front().length());

    while(!candidates.empty())
    {
        const auto candidate = candidates.front();
        candidates.pop_front();

        for(auto& other_candidate: candidates)
        {
            int diff_letter_idx = -1;
            int tolerance = 1;
            for (int i = 0; i < id_len; ++i)
            {
                if(candidate[i] != other_candidate[i])
                {
                    diff_letter_idx = i;
                    tolerance--;
                }
                if(tolerance < 0)
                {
                    break;
                }
            }
            if(tolerance == 0)
            {
                PRINT(candidate);
                PRINT(other_candidate);
                ans2 = candidate;
                ans2.erase(diff_letter_idx, 1);
            }
        }
    }

    PRINT(ans1);
    PRINT(ans2);
    
    return 0;
}
