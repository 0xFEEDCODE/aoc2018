#include "../utilslib/utilslib.h"
#include <algorithm>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <xutility>

struct step
{
    char id;
    char depends_on_id;
};

void parse_steps(map<char, list<char>> &pending_steps, vector<char> &all_steps)
{
    redirect_inp_to_stdin();
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), stdin))
    {
        istringstream iss(buffer);

        string temp;
        char step1, step2;
        // Step C must be finished before step F can begin.
        iss >> temp >> step1 >> temp >> temp >> temp >> temp >> temp >> step2 >> temp;

        pending_steps[step2].emplace_back(step1);
        if (pending_steps.find(step1) == pending_steps.end())
        {
            pending_steps[step1] = list<char>();
        }

        if (find(all_steps.begin(), all_steps.end(), step1) == all_steps.end())
        {
            all_steps.emplace_back(step1);
        }
        if (find(all_steps.begin(), all_steps.end(), step2) == all_steps.end())
        {
            all_steps.emplace_back(step2);
        }
    }
    sort(all_steps.begin(), all_steps.end());
}

void pt1()
{
    vector<char> all_steps;
    map<char, list<char>> pending_steps;
    parse_steps(pending_steps, all_steps);

    vector<char> finished_steps;
    vector<char> available_steps;

    while (finished_steps.size() != all_steps.size())
    {
        for (auto &[step, depending_steps] : pending_steps)
        {
            if (depending_steps.empty())
            {
                available_steps.emplace_back(step);

                auto it = pending_steps.find(step);
                pending_steps.erase(it);

                for (auto &[other_step, other_depending_steps] : pending_steps)
                {
                    auto it = find(other_depending_steps.begin(), other_depending_steps.end(), step);
                    if (it != other_depending_steps.end())
                    {
                        other_depending_steps.erase(it);
                        /*                         cout << "Erased " << step << " from" << other_step << endl; */
                    }
                }

                break;
            }
        }

        sort(available_steps.begin(), available_steps.end(), greater<char>());
        if (!available_steps.empty())
        {
            std::cout << available_steps.back();
            finished_steps.emplace_back(available_steps.back());
            available_steps.pop_back();
        }
    }
}

void pt2()
{
    vector<char> all_steps;
    map<char, list<char>> pending_steps;
    parse_steps(pending_steps, all_steps);

    vector<char> finished_steps;
    vector<char> to_insert;
    vector<char> inserted;
    deque<pair<char, int>> available_steps;

    const int N_WORKERS = 5;
    const int SECONDS_PER_STEP = 60;

    int seconds = 0;

    while (finished_steps.size() != all_steps.size())
    {
        to_insert.clear();
        for (auto &[step, depending_steps] : pending_steps)
        {
            if (depending_steps.empty())
            {
                if (find(inserted.begin(), inserted.end(), step) == inserted.end())
                {
                    to_insert.emplace_back(step);
                }
            }
        }

        if (to_insert.empty() && available_steps.empty())
        {
            continue;
        }

        vector<char> steps_to_insert_ordered;
        for (const auto step : to_insert)
        {
            steps_to_insert_ordered.push_back(step);
        }
        sort(steps_to_insert_ordered.begin(), steps_to_insert_ordered.end());

        for (const auto step : steps_to_insert_ordered)
        {
            available_steps.push_back({step, SECONDS_PER_STEP + step - 'A' + 1});
            inserted.emplace_back(step);
        }

        seconds++;
        vector<pair<char, int>> re_insert;

        const auto n_available_steps = available_steps.size();

        for (int i = 0; i < N_WORKERS && i < n_available_steps; ++i)
        {
            auto [step, seconds_remaining] = available_steps.front();
            available_steps.pop_front();

            if (seconds_remaining - 1 == 0)
            {
                finished_steps.emplace_back(step);

                auto it = pending_steps.find(step);
                pending_steps.erase(it);

                for (auto &[other_step, other_depending_steps] : pending_steps)
                {
                    auto it = find(other_depending_steps.begin(), other_depending_steps.end(), step);
                    if (it != other_depending_steps.end())
                    {
                        other_depending_steps.erase(it);
                    }
                }
            }
            else
            {
                re_insert.emplace_back(step, seconds_remaining - 1);
            }
        }

        for (const auto insert : re_insert)
        {
            available_steps.push_front(insert);
        }
    }

    std::cout << endl << seconds << endl;
}

int main(int argc, char *argv[])
{
    pt1();
    std::cout << endl;
    pt2();

    return 0;
}
