#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <sstream>
#include <vector>

#include "../utilslib/utilslib.h"

const vector<pair<int, int>> neighborOffsets = {{0, -1}, {-1, 0}, {1, 0}, {0, 1}};

namespace FieldType
{
    enum Type
    {
        Wall = 0,
        Open = 1,
        Elf = 2,
        Goblin = 3
    };
} // namespace FieldType

struct Field
{
public:
    Point2D Position;

    [[nodiscard]] FieldType::Type getTypeOfField() const
    {
        return Type;
    }

protected:
    FieldType::Type Type;

    explicit Field(const FieldType::Type type, const Point2D position) : Position(position), Type(type)
    {
    }
};

class Open : public Field
{
public:
    explicit Open(const Point2D position) : Field(FieldType::Open, position)
    {
    }
};

class Wall : public Field
{
public:
    explicit Wall(const Point2D position) : Field(FieldType::Wall, position)
    {
    }
};

class NPC : public Field
{
public:
    int HitPoints = 200;
    int AttackPower = 3;
    int ReadingOrder = -1;

    [[nodiscard]] bool isAlive() const
    {
        return HitPoints > 0;
    }

    bool Attack(NPC &target) const
    {
        target.HitPoints -= AttackPower;
        const bool hasKilledTarget = !target.isAlive();
        return hasKilledTarget;
    }

protected:
    explicit NPC(FieldType::Type fieldType, const Point2D position) : Field(fieldType, position)
    {
    }
};

class Goblin : public NPC
{
public:
    explicit Goblin(const Point2D position) : NPC(FieldType::Goblin, position)
    {
    }
};

class Elf : public NPC
{
public:
    explicit Elf(const Point2D position) : NPC(FieldType::Elf, position)
    {
    }
};

int compareReadingOrder(const Point2D &a, const Point2D &b)
{
    if (a.y == b.y)
    {
        return a.x < b.x ? -1 : 1;
    }
    return a.y < b.y ? -1 : 1;
}

int compareNpcByReadingOrder(const shared_ptr<NPC> &a, const shared_ptr<NPC> &b)
{
    return a->ReadingOrder < b->ReadingOrder ? -1 : 1;
}
void sortNPCsByReadingOrder(list<shared_ptr<NPC>> &npcs)
{
    npcs.sort([](const shared_ptr<NPC> &a, const shared_ptr<NPC> &b)
              { return compareReadingOrder(a->Position, b->Position) == -1; });
}

struct PriorityQueueItem
{
    Point2D Item;
    int Priority;
};


struct GetDistanceResult
{
    int Distance{};
    Point2D NextStep;
};

int getManhattanDist(const Point2D &p1, const Point2D &p2)
{
    return abs(p1.x - p2.x) + abs(p1.y - p2.y);
};


map<Point2D, optional<int>> areTargetsReachable(const vector<vector<shared_ptr<Field>>> &grid, const Point2D &origin,
                                                const vector<Point2D> &targets)
{
    map<Point2D, optional<int>> results;
    for (auto target : targets)
    {
        results[target] = nullopt;
    }

    queue<pair<pair<Point2D, Point2D>, int>> queue;
    map<string, bool> visited;

    for (auto target : targets)
    {
        queue.emplace(make_pair(origin, target), 0);
    }

    int shortestPath = INT_MAX;

    int nTargetsLeftToProcess = targets.size();
    while (!queue.empty() && nTargetsLeftToProcess > 0)
    {
        auto [pathInfo, currentItemCost] = queue.front();
        auto [current, target] = pathInfo;

        queue.pop();

        if (current == target)
        {
            results[target] = currentItemCost;
            nTargetsLeftToProcess--;
            shortestPath = min(shortestPath, currentItemCost);
            if (nTargetsLeftToProcess == 0)
            {
                break;
            }
        }

        for (auto [offsetX, offsetY] : neighborOffsets)
        {
            auto neighborPos = Point2D(current.x + offsetX, current.y + offsetY);

            stringstream ss;
            ss << neighborPos.x << "_" << neighborPos.y << "-" << target.x << "_" << target.y;
            auto key = ss.str();

            if (visited.contains(key))
            {
                continue;
            }
            if (currentItemCost > shortestPath)
            {
                continue;
            }
            visited[key] = true;

            const auto next = grid[neighborPos.y][neighborPos.x].get();
            if (next->getTypeOfField() != FieldType::Open)
            {
                continue;
            }

            queue.emplace(make_pair(next->Position, target), currentItemCost + 1);
        }
    }

    return results;
}

shared_ptr<NPC> choseAttackTarget(const vector<shared_ptr<NPC>> &attackTargets)
{
    shared_ptr<NPC> targetToAttack = nullptr;
    for (const auto &target : attackTargets)
    {
        if (targetToAttack == nullptr)
        {
            targetToAttack = target;
            continue;
        }

        if (target->HitPoints < targetToAttack->HitPoints)
        {
            targetToAttack = target;
        }
        else if (target->HitPoints == targetToAttack->HitPoints &&
                 compareNpcByReadingOrder(target, targetToAttack) == -1)
        {
            targetToAttack = target;
        }
    }
    if (targetToAttack == nullptr)
    {
        throw logic_error("wtf");
    }
    return targetToAttack;
}

vector<shared_ptr<NPC>> getAttackTargets(const vector<vector<shared_ptr<Field>>> &grid, const Point2D &origin,
                                         const FieldType::Type enemyType, const list<shared_ptr<NPC>> &enemies)
{
    vector<shared_ptr<NPC>> targets;
    for (auto [offsetX, offsetY] : neighborOffsets)
    {
        auto field = grid[origin.y + offsetY][origin.x + offsetX];
        if (field->getTypeOfField() == enemyType)
        {
            for (const auto &enemy : enemies)
            {
                if (enemy->Position == field->Position)
                {
                    targets.emplace_back(enemy);
                    break;
                }
            }
        }
    }
    return targets;
}

optional<int> getDistanceToTarget(const vector<vector<shared_ptr<Field>>> &grid, const Point2D &origin,
                                  const Point2D &target)
{
    if (target == origin)
    {
        return 0;
    }

    auto cmp = [](const PriorityQueueItem &a, const PriorityQueueItem &b) -> bool { return a.Priority > b.Priority; };
    priority_queue<PriorityQueueItem, vector<PriorityQueueItem>, decltype(cmp)> pQueue(cmp);

    pQueue.push(PriorityQueueItem{origin, 0});

    map<Point2D, int> distance;
    distance[origin] = 0;

    while (!pQueue.empty())
    {
        auto [currentItem, currentItemPriority] = pQueue.top();
        pQueue.pop();

        if (currentItem == target)
        {
            return distance[currentItem];
        }

        for (auto [offsetX, offsetY] : neighborOffsets)
        {
            auto neighborPos = Point2D(currentItem.x + offsetX, currentItem.y + offsetY);
            if (distance.contains(neighborPos))
            {
                continue;
            }

            const auto next = grid[neighborPos.y][neighborPos.x].get();
            if (neighborPos != target && next->getTypeOfField() != FieldType::Open)
            {
                continue;
            }

            const auto cost = distance[currentItem] + 1;
            distance[next->Position] = cost;
            pQueue.push(PriorityQueueItem{next->Position, cost});
        }
    }

    return nullopt;
}

optional<Point2D> getNextPositionTowardsNextEnemy(const vector<vector<shared_ptr<Field>>> &grid,
                                                  const shared_ptr<NPC> &npc, list<shared_ptr<NPC>> &enemyNpcs)
{
    optional<Point2D> nextStepTowardsClosestEnemy;
    optional<int> distanceToTargetSquare = nullopt;
    Point2D targetSquare;
    const Point2D originPos = npc->Position;
    const vector<pair<int, int>> neighborOffsets = {{0, -1}, {-1, 0}, {1, 0}, {0, 1}};

    vector<Point2D> squaresNextToTarget;
    vector<Point2D> squaresNextToOrigin;

    enemyNpcs.sort(
        [originPos](const shared_ptr<NPC> &npc1, const shared_ptr<NPC> &npc2)
        { return getManhattanDist(npc1->Position, originPos) < getManhattanDist(npc2->Position, originPos); });

    for (auto [offsetX, offsetY] : neighborOffsets)
    {
        if (auto nextToOrigin = Point2D{originPos.x + offsetX, originPos.y + offsetY};
            grid[nextToOrigin.y][nextToOrigin.x]->getTypeOfField() == FieldType::Open)
        {
            squaresNextToOrigin.emplace_back(nextToOrigin);
        }
    }

    int i = 0;

    for (const auto &enemyNpc : enemyNpcs)
    {
        // Pruning
        if (distanceToTargetSquare.has_value() &&
            getManhattanDist(originPos, enemyNpc->Position) > (distanceToTargetSquare.value() + 2))
        {
            continue;
        }
        if (!enemyNpc->isAlive())
        {
            continue;
        }

        // Probe if the target is reachable and if it is, check if shorter path was not found already
        auto distanceToTarget = getDistanceToTarget(grid, originPos, enemyNpc->Position);
        if (distanceToTarget == nullopt)
        {
            continue;
        }
        // Pruning
        if (distanceToTargetSquare.has_value() && distanceToTarget.value() - 2 > distanceToTargetSquare.value())
        {
            continue;
        }

        squaresNextToTarget.clear();
        for (auto [offsetX, offsetY] : neighborOffsets)
        {
            const auto enemyPos = enemyNpc->Position;
            if (auto nextToEnemy = Point2D{enemyPos.x + offsetX, enemyPos.y + offsetY};
                grid[nextToEnemy.y][nextToEnemy.x]->getTypeOfField() == FieldType::Open)
            {
                squaresNextToTarget.emplace_back(nextToEnemy);
            }
        }

        optional<Point2D> target;
        optional<int> distToTarget;
        for (auto [endPoint, dist] : areTargetsReachable(grid, originPos, squaresNextToTarget))
        {
            if (!dist.has_value())
            {
                continue;
            }

            if (!target.has_value() || dist.value() < distToTarget.value() ||
                (dist.value() == distToTarget.value() && compareReadingOrder(endPoint, target.value()) == -1))
            {
                target = endPoint;
                distToTarget = dist.value();
            }
        }
        if (!target.has_value())
        {
            continue;
        }

        optional<pair<Point2D, int>> firstStepAndDist;
        for (auto startPoint : squaresNextToOrigin)
        {
            auto dist = getDistanceToTarget(grid, startPoint, target.value());
            if (!dist.has_value())
            {
                continue;
            }

            if (!firstStepAndDist.has_value() || dist < firstStepAndDist.value().second ||
                dist == firstStepAndDist.value().second &&
                    compareReadingOrder(startPoint, firstStepAndDist.value().first) == -1)
            {
                firstStepAndDist = {startPoint, dist.value()};
            }
        }

        if (!firstStepAndDist.has_value())
        {
            continue;
        }
        auto [firstStep, dist] = firstStepAndDist.value();

        if (!nextStepTowardsClosestEnemy.has_value() || dist < distanceToTargetSquare ||
            (dist == distanceToTargetSquare && compareReadingOrder(target.value(), targetSquare) == -1))
        {
            targetSquare = target.value();
            nextStepTowardsClosestEnemy = firstStep;
            distanceToTargetSquare = dist;
        }

        // Pruning
        if (i++ == 0 && distanceToTargetSquare.has_value() && enemyNpcs.size() > 1)
        {
            if (getManhattanDist(enemyNpcs.front()->Position, originPos) + 2 <
                getManhattanDist((next(enemyNpcs.begin()))->get()->Position, originPos))
            {
                break;
            }
        }
    }
    return nextStepTowardsClosestEnemy;
}

void advanceNpc(vector<vector<shared_ptr<Field>>> &grid, const shared_ptr<NPC> &npc, const Point2D nextStepToTarget)
{
    auto [prevX, prevY] = npc->Position;
    auto [newX, newY] = nextStepToTarget;
    grid[newY][newX] = npc;
    grid[prevY][prevX] = make_shared<Open>(Point2D{prevX, prevY});
    npc->Position = nextStepToTarget;
}

void printGrid(vector<vector<shared_ptr<Field>>> grid)
{
    cout << endl;
    for (auto r : grid)
    {
        for (auto c : r)
        {
            if (c->getTypeOfField() == FieldType::Open)
            {
                cout << ".";
            }
            else if (c->getTypeOfField() == FieldType::Elf)
            {
                cout << "E";
            }
            else if (c->getTypeOfField() == FieldType::Goblin)
            {
                cout << "G";
            }
            else
            {
                cout << "#";
            }
        }
        cout << endl;
    }
}

uint32_t getRemainingHp(list<shared_ptr<NPC>> npcs)
{
    return accumulate(npcs.begin(), npcs.end(), 0, [](const uint32_t sum, const shared_ptr<NPC> &npc)
                      { return npc->isAlive() ? sum + npc->HitPoints : sum; });
}

int main()
{
    vector<vector<shared_ptr<Field>>> grid;
    redirect_inp_to_stdin();

    list<shared_ptr<NPC>> allNpcs;
    list<shared_ptr<NPC>> goblins;
    list<shared_ptr<NPC>> elves;

    string line;
    int32_t row = 0;
    while (getline(cin, line))
    {
        int32_t col = 0;
        grid.emplace_back();
        auto &last_row = grid.back();

        for (auto const ch : line)
        {
            auto pos = Point2D(col, row);
            if (ch == '.')
            {
                last_row.emplace_back(make_shared<Open>(pos));
            }
            else if (ch == '#')
            {
                last_row.emplace_back(make_shared<Wall>(pos));
            }
            else if (ch == 'E')
            {
                auto elf = make_shared<Elf>(pos);
                last_row.emplace_back(elf);
                elves.emplace_back(elf);
                allNpcs.emplace_back(elf);
            }
            else if (ch == 'G')
            {
                auto goblin = make_shared<Goblin>(pos);
                last_row.emplace_back(goblin);
                goblins.emplace_back(goblin);
                allNpcs.emplace_back(goblin);
            }
            col++;
        }
        row++;
    }

    /*
    int initialElfCount = elves.size();
    for (const auto elf : elves)
    {
        elf->AttackPower = 19;
    }
    */

    int rounds = 0;
    while (!elves.empty() && !goblins.empty())
    {
        /*
        if (elves.size() != initialElfCount)
        {
            throw logic_error("wtf");
        }
        */
        rounds++;
        sortNPCsByReadingOrder(allNpcs);
        int ro = 0;
        for (auto npc : allNpcs)
        {
            npc->ReadingOrder = ro++;
        }
        // printGrid(grid);
        for (const auto &npc : allNpcs)
        {
            if (elves.empty() || goblins.empty())
            {
                rounds--;
                break;
            }

            if (!npc->isAlive())
            {
                continue;
            }

            const FieldType::Type enemyType =
                npc->getTypeOfField() == FieldType::Elf ? FieldType::Goblin : FieldType::Elf;
            list<shared_ptr<NPC>> &enemies = enemyType == FieldType::Elf ? elves : goblins;

            auto attackTargets = getAttackTargets(grid, npc->Position, enemyType, enemies);
            if (attackTargets.empty())
            {
                auto nextStepTowardsTarget = getNextPositionTowardsNextEnemy(grid, npc, enemies);
                if (!nextStepTowardsTarget.has_value())
                {
                    continue;
                }

                advanceNpc(grid, npc, nextStepTowardsTarget.value());
                attackTargets = getAttackTargets(grid, npc->Position, enemyType, enemies);
            }

            if (!attackTargets.empty())
            {
                if (auto target = choseAttackTarget(attackTargets); npc->Attack(*target))
                {
                    const auto targetPos = target->Position;
                    grid[targetPos.y][targetPos.x] = make_shared<Open>(Point2D{targetPos.x, targetPos.y});

                    allNpcs.remove(target);
                    enemies.remove(target);
                }
            }
        }
    }

    printGrid(grid);
    auto remainingHPTotal = getRemainingHp(allNpcs);
    cout << (rounds - 1) * remainingHPTotal << endl;
    cout << (rounds)*remainingHPTotal << endl;
    cout << endl;
    cout << rounds << " " << remainingHPTotal << endl;
}
