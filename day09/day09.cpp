#include "../utilslib/utilslib.h"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

struct Window
{
    Window(int capacity) : capacity(capacity), n_elements(0)
    {
        for (int i = 0; i < capacity; i++)
        {
            window[i] = UNINTIALZED_FIELD_VALUE;
        }
    }

    int operator[](int index) const
    {
        auto it = window.find(index);
        if (it != window.end())
        {
            return it->second;
        }
        return -1;

        // throw logic_error("No element found");
    }

  private:
    const int UNINTIALZED_FIELD_VALUE = -1;
    map<int, int> window;
    int capacity;
    int n_elements;

  public:
    int GetCapacity()
    {
        return capacity;
    }
    int GetSize()
    {
        return n_elements;
    }
    void RemoveHead()
    {
        for (int i = 0; i < capacity; i++)
        {
            if (window[i] != UNINTIALZED_FIELD_VALUE)
            {
                window[i] = UNINTIALZED_FIELD_VALUE;
                n_elements--;
                break;
            }
        }
    }
    void InsertAtTail(int value)
    {
        auto tail_idx = capacity - 1;
        InsertShiftLeft(tail_idx, value);
    }

    void InsertShiftLeft(int insert_pos, int value)
    {
        // Shift all the elements before insert pos to the left
        for (int i = 0; i <= insert_pos - 1; i++)
        {
            if (window[i + 1] == UNINTIALZED_FIELD_VALUE)
            {
                continue;
            }
            window[i] = window[i + 1];
        }

        window[insert_pos] = value;
        n_elements = min(n_elements + 1, capacity);
    }

    void InsertShiftRight(int insert_pos, int value)
    {
        if (insert_pos < n_elements)
        {
            // Shift all the elements after insert pos to the right
            for (int i = capacity; i > insert_pos; i--)
            {
                if (window[i - 1] == UNINTIALZED_FIELD_VALUE)
                {
                    continue;
                }
                window[i] = window[i - 1];
            }
        }

        window[insert_pos] = value;

        n_elements = min(n_elements + 1, capacity);
    }
    void Print()
    {
        cout << endl;
        for (int i = 0; i < capacity; i++)
        {
            cout << window[i] << " ";
        }
        cout << endl;
    }
};

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

    int current_marble_idx = 0;
    int current_marble = -1;
    int player_turn = 0;

    int it_between = 0;
    int last_max_score_marble = 0;
    uint32_t last_max_score = 0;

    n_players = 10;
    last_marble_worth = 1618;

    int c = -1;
    int window_start_idx = -1;
    int window_end_idx = -1;

    const int FRONT_SIZE = 1024;
    Window last_7_marbles_wnd = Window(8);
    Window front_7_marbles_wnd = Window(FRONT_SIZE);

    int n_marbles = 0;

    map<int, int> magic_map;

    int magic_idx = 0;
    int magic_size = 0;
    int n_turnarounds = 0;

    int prev_marble_idx = -1;

    while (current_marble != last_marble_worth)
    {
        int new_marble_idx = 0;
        int new_marble = current_marble + 1;

        if (new_marble > 0 && new_marble % 23 == 0)
        {
            magic_size++;

            scores[player_turn] += new_marble;
            scores[player_turn] += last_7_marbles_wnd[0];
            last_7_marbles_wnd.RemoveHead();
        }
        else
        {
            magic_size++;

            magic_idx = magic_idx++;

            prev_marble_idx = current_marble_idx;
            current_marble_idx = new_marble_idx;
        }

        current_marble = new_marble;
        player_turn = (player_turn + 1) % n_players;

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

// if (new_marble_idx < FRONT_SIZE)
// {
//     front_7_marbles_wnd.InsertShiftRight(new_marble_idx, new_marble);
// }

// if (new_marble_idx <= FRONT_SIZE)
// {
//     if (new_marble_idx <= current_marble_idx)
//     {
//         for (int i = 0; i < new_marble_idx; i++)
//         {
//             last_7_marbles_wnd.RemoveHead();
//             last_7_marbles_wnd.InsertAtTail(front_7_marbles_wnd[i]);
//         }
//     }
//     else
//     {
//         for (int i = current_marble_idx + 1; i < new_marble_idx; i++)
//         {
//             last_7_marbles_wnd.RemoveHead();
//             last_7_marbles_wnd.InsertAtTail(front_7_marbles_wnd[i]);
//         }
//     }
// }
// last_7_marbles_wnd.InsertAtTail(new_marble);

// front_7_marbles_wnd.Print();
// last_7_marbles_wnd.Print();
