#include <iostream>
#include <limits>
#include <list>
#include <sstream>
#include <string.h>
#include <vector>
#include <windows.h>

#include "../utilslib/utilslib.h"


struct velocity
{
    int32_t x;
    int32_t y;

    void Print()
    {
        cout << x << " " << y << endl;
    }
};

struct entry
{
    velocity velocity;
    Point2D position;

public:
    Point2D GetPosAtSecond(int second) const
    {
        const auto x = position.x + (velocity.x * second);
        const auto y = position.y + (velocity.y * second);
        return {x, y};
    }

    bool AreNeighbours(const entry& other, int second) const
    {
        auto p1 = GetPosAtSecond(second);
        auto p2 = other.GetPosAtSecond(second);
        return abs(p1.x - p2.x) <= 1 && abs(p1.y - p2.y) <= 1;
    }

    bool operator==(const entry& other) const
    {
        return position.x == other.position.x && position.y == other.position.y && velocity.x == other.velocity.x && velocity.y == other.velocity.y;
    };
};

void set_cursor(int x = 0, int y = 0)
{
    HANDLE handle;
    COORD coordinates;
    handle = GetStdHandle(STD_OUTPUT_HANDLE);
    coordinates.X = x;
    coordinates.Y = y;
    SetConsoleCursorPosition(handle, coordinates);
}

int main()
{
    redirect_inp_to_stdin();

    list<entry> entries;

    string line;

    //position=< 7,  0> velocity=<-1,  0>
    const char* delimiters = "<, >";

    while (getline(cin, line))
    {
        auto token = strtok(line.data(), delimiters);

        entries.emplace_back();

        auto& entry = entries.back();
        int* fields[] = {&entry.position.x, &entry.position.y, &entry.velocity.x, &entry.velocity.y};
        int field_idx = 0;

        while (token != nullptr)
        {
            token = strtok(nullptr, delimiters);
            if (token == nullptr)
            {
                continue;;
            }

            if (const auto first_char = token[0];
                first_char >= '0' && first_char <= '9' || first_char == '-' || first_char == '+')
            {
                *fields[field_idx++] = stoi(token);
            }
        }
    }

    reset_stdin_to_console();
    cin.clear();


    int seconds_elapsed = 0;

    while (true)
    {
        bool all_neighbours = true;
        for (const auto entry : entries)
        {
            bool has_any_neighbour = false;
            for (const auto other_entry : entries)
            {
                if (entry == other_entry)
                {
                    continue;
                }

                if (entry.AreNeighbours(other_entry, seconds_elapsed))
                {
                    has_any_neighbour = true;
                    break;
                }
            }

            if (!has_any_neighbour)
            {
                all_neighbours = false;
                break;
            }
        }

        if (all_neighbours)
        {
            int low_x = numeric_limits<int>::max();
            int high_x = numeric_limits<int>::min();
            int low_y = numeric_limits<int>::max();
            int high_y = numeric_limits<int>::min();

            for (const auto entry : entries)
            {
                auto pos = entry.GetPosAtSecond(seconds_elapsed);
                low_x = min(pos.x, low_x);
                low_y = min(pos.y, low_y);
                high_x = max(pos.x, high_x);
                high_y = max(pos.y, high_y);
            }

            for (const auto entry : entries)
            {
                auto pos = entry.GetPosAtSecond(seconds_elapsed);
                set_cursor(pos.x - low_x, pos.y - low_y);
                cout << '#';
            }

            set_cursor(0, 0);
            cout << endl << seconds_elapsed;
            break;
        }
        seconds_elapsed++;
    }
}
