
#include <list>
#include <set>
#include <sstream>

#include "../utilslib/utilslib.h"

void advanceIterator(const list<uint32_t> *scores, list<uint32_t>::iterator &it, uint32_t *idx)
{
    auto value = *it;
    auto new_idx = (value + 1 + *idx) % scores->size();

    advance(it, new_idx - *idx);
    *idx = new_idx;
}

bool checkForN(const list<uint32_t>::iterator it, const vector<int> &n_digits)
{
    const auto val = *it;
    for (int i = 0; i < n_digits.size(); i++)
    {
        if (n_digits[i] == val)
        {
            auto temp_it = it;
            advance(temp_it, i * -1);
            auto j = 0;
            bool is_valid = true;
            while (j <= i)
            {
                if (*temp_it != n_digits[j])
                {
                    is_valid = false;
                    break;
                }
                j++;
                temp_it = next(temp_it);
            }
            if (is_valid)
            {
                temp_it = it;
                j = i;
                while (j < n_digits.size())
                {
                    if (*temp_it != n_digits[j])
                    {
                        is_valid = false;
                        break;
                    }
                    j++;
                    temp_it = next(temp_it);
                }
            }
            if (is_valid)
            {
                return true;
            }
        }
    }

    return false;
}

int main()
{
    redirect_inp_to_stdin();
    string temp;
    getline(cin, temp);

    int n = atoi(temp.data());

    list<uint32_t> scores;
    scores.emplace_back(3);
    scores.emplace_back(7);


    uint32_t idx1 = 0;
    auto it1 = scores.begin();
    uint32_t idx2 = 1;
    auto it2 = scores.begin();
    advance(it2, 1);

    while (scores.size() < n + 10)
    {
        auto new_score = std::to_string(*it1 + *it2);
        for (char digit : new_score)
        {
            scores.push_back(digit - '0');
        }

        advanceIterator(&scores, it1, &idx1);
        advanceIterator(&scores, it2, &idx2);
    }

    stringstream ss;
    auto it = scores.end();
    advance(it, -10);
    for (int i = 0; i < 10; i++)
    {
        ss << *it;
        it++;
    }

    string ans1 = ss.str();
    cout << ans1 << endl;
    scores.clear();

    scores.emplace_back(3);
    scores.emplace_back(7);


    idx1 = 0;
    it1 = scores.begin();
    idx2 = 1;
    it2 = scores.begin();
    advance(it2, 1);

    vector<int> n_digits;
    for (auto ch : to_string(n))
    {
        n_digits.emplace_back(ch - '0');
    }

    while (true)
    {
        auto new_score = std::to_string(*it1 + *it2);
        for (char digit : new_score)
        {
            scores.push_back(digit - '0');
        }

        if (checkForN(it1, n_digits))
            break;
        if (checkForN(it2, n_digits))
            break;

        advanceIterator(&scores, it1, &idx1);
        advanceIterator(&scores, it2, &idx2);
    }

    // find index of first n_digits occurrence
    it = scores.begin();
    for (int i = 0; i < scores.size(); i++)
    {
        if (*it == n_digits[0])
        {
            auto temp_it = it;
            auto j = 0;
            bool is_valid = true;
            while (j < n_digits.size())
            {
                if (*temp_it != n_digits[j])
                {
                    is_valid = false;
                    break;
                }
                j++;
                temp_it = next(temp_it);
            }
            if (is_valid)
            {
                cout << "IDX: " << i << endl;
                break;
            }
        }
        it = next(it);
    }
}
