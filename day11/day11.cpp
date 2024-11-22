#include <cmath>
#include <limits>
#include <vector>

#include "../utilslib/utilslib.h"

int get_power_level(const int x, const int y, const int grid_serial_number)
{
    const auto rack_id = x + 10;
    int32_t power_level = rack_id * y;

    power_level += grid_serial_number;
    power_level *= rack_id;

    if (power_level > 100)
    {
        int32_t subtract = 10;
        while (subtract < power_level)
        {
            subtract *= 10;
        }
        subtract /= 10;

        while (subtract != 1)
        {
            while (power_level > subtract && power_level > 1000)
            {
                power_level -= subtract;
            }

            if (power_level == 1000 || power_level < 100)
            {
                power_level = 0;
                break;
            }
            if (power_level < 1000)
            {
                int hundred_digit = 0;
                while (hundred_digit * 100 <= power_level)
                {
                    hundred_digit++;
                }
                power_level = hundred_digit - 1;
                break;
            }

            subtract /= 10;
        }
    }

    power_level -= 5;
    return power_level;
}

int main()
{
    constexpr int GRID_SERIAL_NO = 18;
    constexpr int GRID_DIM = 300;
    vector grid(GRID_DIM, vector(GRID_DIM, 0));

    int32_t largest_sum = numeric_limits<int32_t>::min();
    pair<int, int> largest_pos;

    for (int y = 0; y < GRID_DIM; y++)
    {
        for (int x = 0; x < GRID_DIM; x++)
        {
            grid[y][x] = get_power_level(x, y, GRID_SERIAL_NO);
        }
    }

    for (int y = 0; y < GRID_DIM - 3; y++)
    {
        for (int x = 0; x < GRID_DIM - 3; x++)
        {
            int32_t sum = 0;

            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    sum += grid[y + i][x + j];
                }
            }
            if (sum > largest_sum)
            {
                largest_sum = sum;
                largest_pos = {x, y};
            }
        }
    }
    cout << largest_sum << endl;
    cout << largest_pos.first << " " << largest_pos.second << endl;

    pair<pair<int, int>, int> largest_pos_and_size;
    largest_sum = numeric_limits<int32_t>::min();
    for (int y = 0; y < GRID_DIM; y++)
    {
        for (int x = 0; x < GRID_DIM; x++)
        {
            int32_t sum = 0;
            int size = 0;
            while (x + size < GRID_DIM && y + size < GRID_DIM)
            {
                for (int i = 0; i < size; i++)
                {
                    sum += grid[y + size][x + i];
                    sum += grid[y + i][x + size];
                }
                sum += grid[y + size][x + size];
                size++;

                if (sum > largest_sum)
                {
                    largest_sum = sum;
                    largest_pos_and_size = {{x, y}, size};
                }
            }
        }
    }
    cout << largest_sum << endl;
    cout << largest_pos_and_size.first.first << " " << largest_pos_and_size.first.second << " " << largest_pos_and_size.second;
}

