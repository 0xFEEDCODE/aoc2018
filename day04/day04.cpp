#include <algorithm>
#include <map>
#include <optional>
#include <variant>
#include <vector>

#include "../utilslib/utilslib.h"

using namespace std;

struct timestamp
{
    int y;
    int m;
    int d;
    int hh;
    int mm;
    
private:
    static int64_t get_diff(const int& val1, const int& val2, int minutes_conversion_mul)
    {
        //return val1 == val2 ? 0 : max(val1, val2) - min(val1, val2) * minutes_conversion_mul;
        return val1 == val2 ? 0 : (val1 - val2) * minutes_conversion_mul;
    }


public:
    void print() const
    {
        printf("%04d:%02d:%02d %02d:%02d\n", y, m, d, hh, mm);
    }

    int64_t hh_mm_to_min() const
    {
        return hh*3600 + mm;
    }

    int64_t get_diff_in_minutes(const timestamp &other) const
    {
        constexpr int y_to_min = 525600;
        constexpr int m_to_min = 43800;
        constexpr int d_to_min = 1440;
        constexpr int hh_to_min = 60;

        return get_diff(y, other.y, y_to_min) +
            get_diff(m, other.m, y_to_min) +
            get_diff(d, other.d, d_to_min) +
            get_diff(m, other.m, m_to_min) +
            get_diff(hh, other.hh, hh_to_min) +
            get_diff(mm, other.mm, 1);
    }

    bool is_on_same_day(const timestamp &other) const
    {
        return (y == other.y && m == other.m && d == other.d);
    }
    
    int compare(const timestamp &other) const
    {
        auto diff_in_min = get_diff_in_minutes(other);
        if(diff_in_min < 0)
        {
            return -1;
        }
        if(diff_in_min == 0)
        {
            return 0;
        }
        if(diff_in_min > 0)
        {
            return 1;
        }
    }
};

enum action { wake_up, fall_asleep };

struct shift_record_entry
{
    timestamp timestamp;
    action action;
};

struct shift_record
{
    int id;
    timestamp timestamp;
    vector<shift_record_entry> journal;

public:
    uint64_t get_sleep_time() const
    {
        uint64_t sleep_time_total = 0;

        for (int i = 0; i < journal.size(); ++i)
        {
            auto [timestamp1, action1] = journal[i];
            if(action1 != fall_asleep)
            {
                continue;
            }
                        
            for (int j = i+1; j < journal.size(); ++j)
            {
                auto [timestamp2, action2] = journal[j];
                if(action2 != wake_up)
                {
                    continue;
                }
                
                if(timestamp1.is_on_same_day(timestamp2))
                {
                    sleep_time_total += abs(timestamp1.get_diff_in_minutes(timestamp2));
                    break;
                }
            }
        }

        return sleep_time_total;
    }
};

struct history
{
    vector<shift_record> records;

public:
    void add_record(shift_record &record)
    {
        records.emplace_back(record);
    }

    shift_record &get_last_record()
    {
        return records.back();
    }

    pair<int, int> most_slept_minute() const
    {
        vector<int> minutes(60);
        
        for (const auto& [_, __, journal] : records)
        {
            for (int j = 0; j < journal.size(); j+=2)
            {
                auto sleep_start = journal[j].timestamp;
                auto sleep_end = journal[j+1].timestamp;

                const auto end_minutes = sleep_end.hh_mm_to_min();

                auto min = sleep_start.hh_mm_to_min();
                while(min < end_minutes)
                {
                    minutes[min++%60]++;
                }
            }
        }

        int max_minute = 0;
        for (int i = 0; i < 60; ++i)
        {
            max_minute = minutes[i] > minutes[max_minute] ? i : max_minute;
        }
        return pair(max_minute, minutes[max_minute]);
    }
};

//[1518-11-05 00:55] wakes up
variant<shift_record, shift_record_entry> parse(const string &str)
{
    char to_parse[1024];
    strcpy_s(to_parse, str.c_str());

    char* context = nullptr;

    const string delimiters = "[- :]";

    const char* token = strtok_s(to_parse, delimiters.data(), &context);

    timestamp timestamp;
    int* fields[] = { &timestamp.y, &timestamp.m, &timestamp.d, &timestamp.hh, &timestamp.mm };

    for (auto& field : fields)
    {
        *field = atoi(token);
        token = strtok_s(nullptr, delimiters.data(), &context);
    }

    if(strcmp(token, "Guard") == 0)
    {
        token = strtok_s(nullptr, delimiters.data(), &context);
        const int id = atoi(&token[1]);
        return shift_record{id, timestamp, vector<shift_record_entry>()};
    }
    
    if(strcmp(token, "falls") == 0)
    {
        return shift_record_entry{timestamp, fall_asleep};
    }
    if(strcmp(token, "wakes") == 0)
    {
        return shift_record_entry{timestamp, wake_up};
    }
    
    throw std::logic_error("wtf");
}

bool compare_alphabetically (std::string a, std::string b) {return a<b;} 

int main(int argc, char* argv[])
{
    redirect_inp_to_stdin();

    char buffer[1024];

    shift_record *temp_record = nullptr;
    map<int, history> guards_records;

    vector<string> records_raw;

    while(fgets(buffer, sizeof(buffer), stdin) != nullptr)
    {
        records_raw.emplace_back(string(buffer));
    }

    ranges::sort(records_raw, compare_alphabetically);

    for (auto &record : records_raw)
    {
        const auto parsed = parse(record);
        
        if(holds_alternative<shift_record>(parsed))
        {
            auto record = get<shift_record>(parsed);
            
            if(!guards_records.contains(record.id))
            {
                guards_records.insert(make_pair(record.id, history()));
            }
            guards_records[record.id].add_record(record);
            temp_record = &guards_records[record.id].get_last_record();
        }
        else if(holds_alternative<shift_record_entry>(parsed))
        {
            temp_record->journal.emplace_back(get<shift_record_entry>(parsed));
        }
    }

    pair guard_with_most_sleep = {0, 0};
    for (auto [guard_id, history] : guards_records)
    {
        uint64_t total_sleep_time = 0;
        for (const auto record : history.records)
        {
            total_sleep_time += record.get_sleep_time();
        }
        
        if(total_sleep_time > guard_with_most_sleep.second)
        {
            guard_with_most_sleep = {guard_id, total_sleep_time};
        }
    }

    auto ans1_guard_id = guard_with_most_sleep.first;
    auto ans1_guard_most_slept_minute = guards_records[guard_with_most_sleep.first].most_slept_minute().first;
    PRINT(ans1_guard_id*ans1_guard_most_slept_minute);

    pair<int, pair<int, int>> guard_with_most_sleep_at_certain_minute = {0, {0, 0}};
    for (auto [guard_id, history] : guards_records)
    {
        auto [most_slept_minute, most_slept_minute_value] = history.most_slept_minute();
        if(most_slept_minute_value > guard_with_most_sleep_at_certain_minute.second.second)
        {
            guard_with_most_sleep_at_certain_minute = {guard_id, {most_slept_minute, most_slept_minute_value}};
        }
    }
    
    auto ans2_guard_id = guard_with_most_sleep_at_certain_minute.first;
    auto ans2_guard_most_slept_minute_at_certain_minute = guard_with_most_sleep_at_certain_minute.second.first;
    PRINT(ans2_guard_id*ans2_guard_most_slept_minute_at_certain_minute);
    
    return 0;
}
