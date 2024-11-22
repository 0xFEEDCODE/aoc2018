#include <limits>
#include <map>
#include <sstream>

#include "../utilslib/utilslib.h"

#define IS_PLANT(value) value == '#'

struct note
{
    bool has_plant;
    note* next_plant;
    note* next_no_plant;

    explicit note(const bool has_plant) : has_plant(has_plant), next_plant(nullptr), next_no_plant(nullptr)
    {
    }
};

bool willGrowPlant(const string& state, const note* note)
{
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
            return false;
        }
    }
    if (current_note->next_plant != nullptr)
    {
        return true;
    }
    if (current_note->next_no_plant != nullptr)
    {
        return false;
    }

    if ((current_note->next_no_plant != nullptr && current_note->next_plant != nullptr) || (current_note->next_plant ==
        nullptr && current_note->next_no_plant == nullptr))
    {
        throw logic_error("wtf1");
    }
    throw logic_error("wtf2");
}

void expand(string& current_configuration)
{
    if (current_configuration[0] == '#')
    {
        current_configuration = ".." + current_configuration;
    }
    if (current_configuration[current_configuration.size() - 1] == '#')
    {
        current_configuration += "..";
    }
}

int count(const int padding_size, const string& current_configuration)
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
        note* current_note = &root;

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

    map<string, int> configurations;

    //cout << endl << current_configuration << endl;
    for (int i = 0; i < 20; i++)
    {
        stringstream ss;

        int start = 0;
        if (IS_PLANT(current_configuration[0]))
        {
            current_configuration = ".." + current_configuration;
            start = 2;
            ss << "..";
        }
        else if (IS_PLANT(current_configuration[1]))
        {
            current_configuration = "." + current_configuration;
            start = 1;
            ss << ".";
        }

        if (IS_PLANT(current_configuration[current_configuration.size()-1]))
        {
            current_configuration += "..";
        }
        else if (IS_PLANT(current_configuration[current_configuration.size()-2]))
        {
            current_configuration += ".";
        }
        int len = current_configuration.size();
        for (int j = 0; j < len; j++)
        {
            if (IS_PLANT(current_configuration[i]))
            {
                start = j;
            }
        }

        for (int j = start; j < len; j++)
        {
            auto str = current_configuration.substr(j - start, 5);
            if (str.size() == 3)
            {
                if (str == "..")
                {
                    ss << "..";
                }
                else if (str[0] == '.')
                {
                    ss << '.';
                }
            }
            else
            {
                ss << (willGrowPlant(str, &root) ? '#' : '.');
            }
            //cout << str << endl;
        }

        ss >> current_configuration;
        /*
        if (configurations.find(current_configuration) != configurations.end())
        {
            cout << "yes!";
        }
        */
        cout << endl << current_configuration << endl;
        configurations[current_configuration]++;
        /*
        int sum = count(padding.size(), current_configuration);
        cout << sum << endl;
        */
        //cout << current_configuration << endl;
    }
}
