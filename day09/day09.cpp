#include "../utilslib/utilslib.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

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

    vector<int> board;

    map<int, uint32_t> scores;

    int marble_idx = 0;
    int current_marble = -1;
    int player_turn = 0;

    int it_between = 0;
    int last_max_score_marble = 0;
    uint32_t last_max_score = 0;

    n_players = 10;
    last_marble_worth = 3000;

    map<int, int> front;
    queue<int> marble_q;

    int c = 0;
    int prev_marble_idx = 0;

    while (current_marble != last_marble_worth)
    {
        int board_len = board.size();
        int position_to_insert_at = 0;
        int new_marble = current_marble + 1;

        marble_q.emplace(new_marble);
        if (marble_q.size() > 10)
        {
            marble_q.pop();
        }

        if (new_marble > 0 && new_marble % 23 == 0)
        {
            scores[player_turn] += new_marble;

            auto pop_marble_idx = (marble_idx - 7 + board_len) % board_len;
            if (pop_marble_idx < 30)
            {
                scores[player_turn] += front[pop_marble_idx];
                for (int i = pop_marble_idx; i < 30; i++)
                {
                    front[pop_marble_idx] = front[pop_marble_idx + 1];
                }
            }

            marble_idx = (marble_idx - 7 + board_len) % board_len;
            scores[player_turn] += board[marble_idx];

            // if (board_len > 0)
            // {
            //     auto x = board[marble_idx];
            //     int y = 0;
            //     for (int i = 0; i < 6; i++)
            //     {
            //         y = marble_q.front();
            //         marble_q.pop();
            //     }
            //     y /= 2;
            //     cout << marble_idx << " " << board_len << " " << abs(board_len - marble_idx) << endl;
            // }

            if (prev_marble_idx != 0 && marble_idx < prev_marble_idx)
            {
                c = 0;
            }

            board.erase(board.begin() + marble_idx);

            prev_marble_idx = marble_idx;
            c++;
        }
        else
        {
            if (board_len > 0)
            {
                if (board_len == 1 || marble_idx + 2 == board_len)
                {
                    position_to_insert_at = board_len;
                }
                else
                {
                    position_to_insert_at = (marble_idx + 2) % (board_len);
                }
            }

            board.insert(board.begin() + position_to_insert_at, new_marble);

            if (position_to_insert_at < 30)
            {
                front[position_to_insert_at] = new_marble;
            }

            marble_idx = position_to_insert_at;
        }

        current_marble = new_marble;
        player_turn = (player_turn + 1) % n_players;

        it_between++;
        auto max_score = get_max_score(scores);
        if (max_score != last_max_score)
        {
            last_max_score = max_score;
            // print_max_score(scores);
            // cout << endl << it_between << endl;
            it_between = 0;
        }

        // for (int x = 0; x < board.size(); x++)
        // {
        //     cout << board[x] << " ";
        // }
        // cout << endl;
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
