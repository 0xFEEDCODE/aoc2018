#include "../utilslib/utilslib.h"
#include <map>
#include <set>
#include <stdio.h>
#include <vector>

int get_manhattan_dist(const point2d *a, const point2d *b)
{
    return abs(a->x - b->x) + abs(a->y - b->y);
}

struct point2dWithId
{
    int id;
    point2d point;
    int x = point.x;
    int y = point.y;

    point2dWithId(int id, int x, int y) : id(id), point{x, y}
    {
    }
};

int main(int argc, char *argv[])
{
    redirect_inp_to_stdin();

    char buffer[256];
    vector<point2dWithId> points;

    int id = 0;
    while (fgets(buffer, sizeof(buffer), stdin) != nullptr)
    {
        auto token = strtok(buffer, ",");
        auto token2 = strtok(nullptr, ",");
        points.emplace_back(point2dWithId{id++, atoi(token), atoi(token2)});
    }

    int x_lo = numeric_limits<int>::max();
    int x_hi = numeric_limits<int>::min();
    int y_lo = numeric_limits<int>::max();
    int y_hi = numeric_limits<int>::min();

    for (auto &point : points)
    {
        x_lo = min(point.x, x_lo);
        x_hi = max(point.x, x_hi);

        y_lo = min(point.y, y_lo);
        y_hi = max(point.y, y_hi);
    }

    map<int, pair<int, int>> proximities;
    map<int, int> proximity_score;
    vector<int> finites;

    for (auto &point : points)
    {
        proximities[point.id] = {(point.x - x_lo), (point.y - y_lo)};
    }

    vector grid(y_hi - y_lo + 1, vector(x_hi - x_lo + 1, -1));

    for (int y = y_lo; y <= y_hi; y++)
    {
        for (int x = x_lo; x <= x_hi; x++)
        {
            map<int, int> proximity_score_local;

            int lowest_score = numeric_limits<int>::max();
            for (const auto [id, proximity] : proximities)
            {
                auto [prox_x, prox_y] = proximity;

                int score = abs(x - (prox_x + x_lo)) + abs(y - (prox_y + y_lo));
                proximity_score_local[id] = score;

                lowest_score = min(lowest_score, score);
            }

            int occurences = 0;
            int lowest_score_id = 0;
            for (const auto [id, score] : proximity_score_local)
            {
                if (abs(score) == lowest_score)
                {
                    lowest_score_id = id;
                    occurences++;
                }
            }
            if (occurences == 1)
            {
                proximity_score[lowest_score_id]++;
                grid[y - y_lo][x - x_lo] = lowest_score_id;
            }
        }
    }

    map<int, int> scores;
    set<int> infs;

    for (int x = x_lo; x <= x_hi; x++)
    {
        auto id1 = grid[0][x - x_lo];
        auto id2 = grid[y_hi - y_lo][x - x_lo];
        infs.insert(id1);
        infs.insert(id2);
    }

    for (int y = y_lo; y <= y_hi; y++)
    {
        auto id1 = grid[y - y_lo][0];
        auto id2 = grid[y - y_lo][x_hi - x_lo];
        infs.insert(id1);
        infs.insert(id2);
    }

    int region = 0;
    for (int y = y_lo; y <= y_hi; y++)
    {
        for (int x = x_lo; x <= x_hi; x++)
        {
            auto id = grid[y - y_lo][x - x_lo];
            if (id != -1)
            {
                scores[id]++;
            }

            int dist_to_all_points = 0;
            for (const auto &p : points)
            {
                dist_to_all_points += get_manhattan_dist(new point2d{x, y}, &p.point);
                if (dist_to_all_points >= 10000)
                {
                    break;
                }
            }
            if (dist_to_all_points < 10000)
            {
                region++;
            }
        }
    }

    auto highest_score = numeric_limits<int>::min();
    for (auto [id, score] : scores)
    {
        if (infs.find(id) != infs.end())
        {
            continue;
        }
        highest_score = max(highest_score, score);
    }

    cout << "ans1 " << highest_score << endl;
    cout << "ans2 " << region;
}

/*     pair<int, int> lowest_x = {0, numeric_limits<int>::max()};
    pair<int, int> lowest_y = {0, numeric_limits<int>::max()};
    pair<int, int> highest_x = {0, numeric_limits<int>::min()};
    pair<int, int> highest_y = {0, numeric_limits<int>::min()};
    for (auto &point : points)
    {
        if (point.x < lowest_x.second)
        {
            lowest_x = {point.id, point.x};
        }
        if (point.y < lowest_y.second)
        {
            lowest_y = {point.id, point.y};
        }
        if (point.x > highest_x.second)
        {
            highest_x = {point.id, point.x};
        }
        if (point.y > highest_y.second)
        {
            highest_y = {point.id, point.y};
        }
    }
    vector<pair<int, int>> sorted_proximity_score(proximity_score.begin(), proximity_score.end());
    sort(sorted_proximity_score.begin(), sorted_proximity_score.end(),
         [](const pair<int, int> &a, const pair<int, int> &b) { return a.second < b.second; });

    map<int, point2d> id_to_point_map;
    for (auto &point : points)
    {
        id_to_point_map[point.id] = point.point;
    }

    for (const auto [id, score] : sorted_proximity_score)
    {
        auto point = id_to_point_map[id];

        if (point.x == lowest_x.second || point.x == highest_x.second || point.y == lowest_y.second ||
            point.y == highest_y.second)
        {
            cout << id << endl;
            continue;
        }
        cout << id << " " << score << endl;
    }
 */