#include "../utilslib/utilslib.h"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>

uint32_t get_max_score(std::map<int, uint32_t> &scores)
{
    uint32_t max_score = 0;
    for (const auto &score : scores)
    {
        if (score.second > max_score)
        {
            max_score = score.second;
        }
    }
    return max_score;
}

void print_max_score(std::map<int, uint32_t> &scores)
{
    cout << "The highest score is: " << get_max_score(scores) << endl;
}

int main(int argc, char *argv[])
{
    redirect_inp_to_stdin();

    string input;
    getline(cin, input);

    istringstream iss(input);

    int n_players = -1;
    int last_marble_worth = -1;
    string discard;

    // 419 players; last marble is worth 71052 points
    iss >> n_players;
    for (int i = 0; i < 5; ++i)
    {
        iss >> discard;
    }
    iss >> last_marble_worth;

    list<int> board;

    map<int, uint32_t> scores;

    int current_marble = -1;
    int player_turn = 0;

    last_marble_worth *= 100;

    list<int>::iterator current_marble_it = board.begin();

    while (current_marble <= last_marble_worth)
    {
        int new_marble = current_marble + 1;

        if (new_marble > 0 && new_marble % 23 == 0)
        {
            scores[player_turn] += new_marble;
            for (int i = 0; i < 7; ++i)
            {
                if (current_marble_it == board.begin())
                {
                    current_marble_it = board.end();
                }
                --current_marble_it;
            }
            scores[player_turn] += *current_marble_it;
            current_marble_it = board.erase(current_marble_it);
            if (current_marble_it == board.end())
            {
                current_marble_it = board.begin();
            }
        }
        else
        {
            if (board.empty())
            {
                board.push_back(new_marble);
                current_marble_it = board.begin();
            }
            else
            {
                ++current_marble_it;
                if (current_marble_it == board.end())
                {
                    current_marble_it = board.begin();
                }
                ++current_marble_it;
                if (current_marble_it == board.end())
                {
                    board.push_back(new_marble);
                    current_marble_it = --board.end();
                }
                else
                {
                    current_marble_it = board.insert(current_marble_it, new_marble);
                }
            }
        }
        current_marble = new_marble;
        player_turn = (player_turn + 1) % n_players;
    }
    print_max_score(scores);

    cout << endl;

    // auto it = common_indexes.begin();
    // while (it != common_indexes.end())
    // {
    //     auto val = *it;
    //     cout << val.first << " " << val.second << endl;
    //     it++;
    // }

    return 0;
}