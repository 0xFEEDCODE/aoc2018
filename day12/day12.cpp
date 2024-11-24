#include <limits>
#include <map>
#include <sstream>

#include "../utilslib/utilslib.h"

#define IS_PLANT(value) value == '#'

struct note
{
    bool has_plant;
    note *next_plant;
    note *next_no_plant;

    explicit note(const bool has_plant) :
        has_plant(has_plant), next_plant(nullptr), next_no_plant(nullptr)
    {
    }
};

bool willGrowPlant(const string &state, const note *note, map<string, bool> &configurations)
{
    if (configurations.find(state) != configurations.end())
    {
        return configurations[state];
    }

    auto current_note = note;
    for (const char ch : state)
    {
        if (IS_PLANT(ch))
        {
            current_note = current_note->next_plant;
        }
        else
        {
            current_note = current_note->next_no_plant;
        }
        if (current_note == nullptr)
        {
            configurations[state] = false;
            return false;
        }
    }
    if (current_note->next_plant != nullptr)
    {
        configurations[state] = true;
        return true;
    }
    if (current_note->next_no_plant != nullptr)
    {
        configurations[state] = false;
        return false;
    }

    if ((current_note->next_no_plant != nullptr && current_note->next_plant != nullptr) || (current_note->next_plant ==
        nullptr && current_note->next_no_plant == nullptr))
    {
        throw logic_error("wtf1");
    }
    throw logic_error("wtf2");
}

pair<int, int> expand(string &current_configuration)
{
    int expand_l = 0;
    int expand_r = 0;
    if (current_configuration.substr(0, 4) != "....")
    {
        current_configuration = "...." + current_configuration;
        expand_l += 4;
    }
    if (current_configuration.substr(current_configuration.size() - 4, 4) != "....")
    {
        current_configuration += "....";
        expand_r += 4;
    }
    return make_pair(expand_l, expand_r);
}

int count(const int padding_size, const string &current_configuration)
{
    int sum = 0;
    for (int i = 0; i < current_configuration.size(); i++)
    {
        if (current_configuration[i] == '#')
        {
            sum += i - padding_size;
        }
    }
    return sum;
}

uint32_t getSumOfPlants(const string &current_configuration, int left_shift)
{
    uint32_t s = 0;
    for (int i = 0; i < current_configuration.size(); i++)
    {
        if (current_configuration[i] == '#')
        {
            s += i - left_shift;
        }
    }
    return s;
}

int main()
{
    redirect_inp_to_stdin();

    /*
    ##### => .
    #.#.# => .
    */

    string line;
    getline(cin, line);
    string discard;
    istringstream iss(line);

    string initial_configuration;
    iss >> discard >> discard >> initial_configuration;

    getline(cin, discard);

    auto root = note(false);
    while (getline(cin, line))
    {
        note *current_note = &root;

        for (int i = 0; i < line.size(); i++)
        {
            if (line[i] != '#' && line[i] != '.')
            {
                continue;
            }

            if (IS_PLANT(line[i]))
            {
                if (current_note->next_plant == nullptr)
                {
                    current_note->next_plant = new note(true);
                }
                current_note = current_note->next_plant;
            }
            else
            {
                if (current_note->next_no_plant == nullptr)
                {
                    current_note->next_no_plant = new note(false);
                }
                current_note = current_note->next_no_plant;
            }
        }
    }

    string current_configuration = initial_configuration;

    map<string, bool> configurations;

    int left_shift = 0;
    int64_t prev_sum = 0;

    //cout << endl << current_configuration << endl;
    for (uint32_t i = 0; i < 10000; i++)
    {
        stringstream ss;

        auto [expand_left, _] = expand(current_configuration);

        for (auto j = 2; j < current_configuration.size() - 2; j++)
        {
            auto str = current_configuration.substr(j - 2, 5);
            ss << (willGrowPlant(str, &root, configurations) ? '#' : '.');
        }
        left_shift += expand_left - 2;
        ss >> current_configuration;

        int64_t s = getSumOfPlants(current_configuration, left_shift);
        cout << s << " " << prev_sum - s << endl;
        prev_sum = s;
    }

    cout << (50000000000 - 10000) * 96 + prev_sum << endl;
}



