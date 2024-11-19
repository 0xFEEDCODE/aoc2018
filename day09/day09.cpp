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

struct Window
{
    Window(int capacity) : capacity(capacity), n_elements(0)
    {
    }

  private:
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
    void InsertAt(int insert_pos, int value)
    {
        for (int i = capacity; i > insert_pos; i--)
        {
            window[i] = window[i - 1];
        }
        window[insert_pos] = value;
    }
    void SlideLeft(int slide_by, int slide_limit = -1)
    {
        for (int i = 0; i < (slide_limit > -1 ? slide_limit : capacity); i++)
        {
            if (i >= (capacity - slide_by))
            {
                window[i] = -1;
            }
            else if (window[i + slide_by] != -1)
            {
                window[i] = window[i + slide_by];
            }
        }
    }
    void SetAtIndex(int index, int value)
    {
        if (!window.contains(index))
        {
            n_elements++;
        }
        window[index] = value;
    }
    void SetLast(int value)
    {
        if (!window.contains(capacity - 1))
        {
            n_elements++;
        }
        window[capacity - 1] = value;
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
    last_marble_worth = 3000;

    map<int, int> front;
    const int FRONT_SIZE = 30;
    for (int i = 0; i < FRONT_SIZE; i++)
    {
        front[i] = -1;
    }

    int c = -1;
    int window_start_idx = -1;
    int window_end_idx = -1;

    Window window = Window(8);

    int n_marbles = 0;

    while (current_marble != last_marble_worth)
    {
        int board_len = board.size();
        int position_to_insert_at = 0;
        int new_marble = current_marble + 1;

        if (new_marble > 0 && new_marble % 23 == 0)
        {
            n_marbles--;
            scores[player_turn] += new_marble;

            auto pop_marble_idx = (current_marble_idx - 7 + board_len) % board_len;
            if (pop_marble_idx < FRONT_SIZE)
            {
                scores[player_turn] += front[pop_marble_idx];
                for (int i = pop_marble_idx; i < (30 - pop_marble_idx); i++)
                {
                    if (front[i] != -1)
                    {
                        front[i] = front[i + 1];
                    }
                }
            }
            else
            {
                cout << pop_marble_idx << " " << board_len << " " << current_marble_idx << endl;
            }

            scores[player_turn] += board[pop_marble_idx];

            board.erase(board.begin() + current_marble_idx);
        }
        else
        {
            if (board_len > 0)
            {
                if (board_len == 1 || current_marble_idx + 2 == board_len)
                {
                    position_to_insert_at = board_len;
                }
                else
                {
                    position_to_insert_at = (current_marble_idx + 2) % (board_len);
                }
            }

            board.insert(board.begin() + position_to_insert_at, new_marble);
            n_marbles++;

            if (position_to_insert_at < FRONT_SIZE)
            {
                front[position_to_insert_at] = new_marble;
            }

            // THIS IS WHERE I LEFT AT
            // NEED OT IMPLEMENT HOW THE WINDOW IS MAINTAINED, IT HAS TO BE MODIFIED ON ROUDNABOUT
            if (window.GetSize() < window.GetCapacity())
            {
                window.InsertAt(position_to_insert_at, new_marble);
            }
            // THIS IS WHERE I LEFT AT

            if (n_marbles > window.GetCapacity())
            {
                cout << "";
            }

            current_marble_idx = position_to_insert_at;
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
