#include <list>
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

    void changeType(FieldType::Type type)
    {
        Type = type;
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

int getIndex(Point2D point)
{
    return (point.y * 32) + point.x;
}

int getIndex(const int nCols, const int x, const int y)
{
    return (y * nCols) + x;
}

template <typename T>
pair<shared_ptr<vector<vector<int>>>, shared_ptr<vector<vector<Graph::Vertex<shared_ptr<T>>>>>> floydWarshall(
    vector<Graph::Vertex<shared_ptr<T>>> &graph)
{
    const int nVertices = graph.size();

    shared_ptr<vector<vector<int>>> distancePtr =
        make_shared<vector<vector<int>>>(nVertices, vector(nVertices, INT_MAX));
    auto prevPtr = make_shared<vector<vector<Graph::Vertex<shared_ptr<T>>>>>(
        nVertices, vector<Graph::Vertex<shared_ptr<T>>>(nVertices));

    auto &distance = *distancePtr;
    auto &prev = *prevPtr;

    for (int i = 0; i < nVertices; ++i)
    {
        if (graph[i].Value->getTypeOfField() != FieldType::Open)
        {
            continue;
        }
        for (auto edge : graph[i].Edges)
        {
            distance[i][edge->Index.value()] = 1;
            prev[i][edge->Index.value()] = graph[i];
        }
    }

    for (int k = 0; k < nVertices; ++k)
    {
        for (int i = 0; i < nVertices; ++i)
        {
            for (int j = 0; j < nVertices; ++j)
            {
                // Update the distance using vertex k as an intermediate point
                if (distance[i][k] != INT_MAX && distance[k][j] != INT_MAX)
                {
                    distance[i][j] = min(distance[i][j], distance[i][k] + distance[k][j]);
                    prev[i][j] = prev[k][j];
                }
            }
        }
    }

    return make_pair(distancePtr, prevPtr);
}

int compareReadingOrder(const Point2D &a, const Point2D &b)
{
    if (a.y == b.y)
    {
        return a.x < b.x ? -1 : 1;
    }
    return a.y < b.y ? -1 : 1;
}
void sortNPCsByReadingOrder(list<shared_ptr<NPC>> &npcs)
{
    npcs.sort([](const shared_ptr<NPC> &a, const shared_ptr<NPC> &b)
              { return compareReadingOrder(a->Position, b->Position) == -1; });
}

optional<Point2D> getNextPositionTowardsNextEnemy(vector<Graph::Vertex<shared_ptr<Field>>> &graph,
                                                  const shared_ptr<NPC> &npc, const list<shared_ptr<NPC>> &enemyNpcs)
{
    auto [dist, prev] = floydWarshall<Field>(graph);

    const auto originIdx = getIndex(npc->Position);
    const auto origin = graph[originIdx];

    Graph::Vertex<shared_ptr<Field>> *closest = nullptr;
    const vector<vector<int>> &distances = *dist.get();

    int shortestDist = INT_MAX;
    for (const auto &enemyNpc : enemyNpcs)
    {
        for (const auto &target : graph[getIndex(enemyNpc->Position)].Edges)
        {
            auto distanceToTarget = distances[originIdx][target->Index.value()];
            if (closest == nullptr || distanceToTarget < shortestDist ||
                (distanceToTarget == shortestDist &&
                 compareReadingOrder(target->Value->Position, closest->Value->Position)))
            {
                shortestDist = distances[originIdx][target->Index.value()];
                closest = target;
            }
        }
    }
    if (closest == nullptr)
    {
        return nullopt;
    }

    optional<Point2D> nextStep;

    for (const auto &edge : origin.Edges)
    {
        if (!nextStep.has_value() ||
            (distances[getIndex(edge->Value->Position)][closest->Index.value()] <=
                 distances[getIndex(nextStep.value())][closest->Index.value()] &&
             compareReadingOrder(edge->Value->Position, nextStep.value())))
        {
            nextStep = edge->Value->Position;
        }
    }
    return nextStep;
}

vector<shared_ptr<NPC>> getAttackTargets(const vector<Graph::Vertex<shared_ptr<Field>>> &graph,
                                         const shared_ptr<Field> &origin, const FieldType::Type enemyType,
                                         list<shared_ptr<NPC>> &enemies)
{
    vector<shared_ptr<NPC>> targets;
    for (auto edge : graph[getIndex(origin->Position)].Edges)
    {
        if (edge->Value.get()->getTypeOfField() == enemyType)
        {
            for (auto enemy : enemies)
            {
                if (edge->Value->Position == enemy->Position)
                {
                    targets.emplace_back(enemy);
                    break;
                }
            }
        }
    }
    return targets;
}

int compareNpcByReadingOrder(const shared_ptr<NPC> &a, const shared_ptr<NPC> &b)
{
    return a->ReadingOrder < b->ReadingOrder ? -1 : 1;
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

void advanceNpc(vector<Graph::Vertex<shared_ptr<Field>>> graph, const shared_ptr<NPC> &npc,
                const Point2D nextStepToTarget)
{
    auto prevStep = getIndex(npc->Position);
    auto nextStep = getIndex(nextStepToTarget);

    auto originVertex = graph[prevStep];
    auto targetVertex = graph[nextStep];

    graph[getIndex(nextStepToTarget)] = originVertex;
    graph[getIndex(npc->Position)] = targetVertex;

    originVertex = graph[prevStep];
    targetVertex = graph[nextStep];

    originVertex.Value->Position = targetVertex.Value->Position;
    targetVertex.Value->Position = nextStepToTarget;

    targetVertex.Edges.clear();
    originVertex.Edges.clear();

    int nCols = 32;
    for (auto vertex : {&targetVertex, &originVertex})
    {
        auto [x, y] = vertex->Value->Position;
        for (auto [offX, offY] : neighborOffsets)
        {
            const auto neighY = (y + offY);
            const auto neighX = (x + offX);
            if (neighX < 0 || neighX >= nCols || neighY < 0 || neighY > nCols)
            {
                continue;
            }

            const auto neighIdx = getIndex(nCols, neighX, neighY);
            auto edge = &graph[neighIdx];
            if (&graph[neighIdx] == nullptr)
            {
                throw logic_error("wtf");
            }

            if (edge->Value->getTypeOfField() == FieldType::Open)
            {
                vertex->Edges.emplace_back(edge);
            }
        }
    }
}

void printGraph(vector<Graph::Vertex<shared_ptr<Field>>> graph)
{
    for (int i = 0; i < graph.size(); i++)
    {
        auto v = graph[i].Value;
        if (v->getTypeOfField() == FieldType::Wall)
        {
            cout << "#";
        }
        else if (v->getTypeOfField() == FieldType::Open)
        {
            cout << ".";
        }
        else if (v->getTypeOfField() == FieldType::Elf)
        {
            cout << "E";
        }
        else if (v->getTypeOfField() == FieldType::Goblin)
        {
            cout << "G";
        }
        if (i % 32 == 0)
        {
            cout << endl;
        }
    }
}
int main()
{
    vector<vector<shared_ptr<Field>>> grid;
    redirect_inp_to_stdin();

    list<shared_ptr<NPC>> allNpcs;
    list<shared_ptr<NPC>> goblins;
    list<shared_ptr<NPC>> elves;

    vector<Graph::Vertex<shared_ptr<Field>>> graph;

    string line;
    int32_t row = 0;
    while (getline(cin, line))
    {
        int32_t col = 0;
        for (auto const ch : line)
        {
            auto pos = Point2D(col, row);
            if (ch == '.')
            {
                auto value = make_shared<Open>(pos);
                graph.emplace_back(Graph::Vertex<shared_ptr<Field>>{value});
            }
            else if (ch == '#')
            {
                auto value = make_shared<Wall>(pos);
                graph.emplace_back(Graph::Vertex<shared_ptr<Field>>{value});
            }
            else if (ch == 'E')
            {
                auto value = make_shared<Elf>(pos);
                graph.emplace_back(Graph::Vertex<shared_ptr<Field>>{value});
                elves.emplace_back(value);
                allNpcs.emplace_back(value);
            }
            else if (ch == 'G')
            {
                auto value = make_shared<Goblin>(pos);
                graph.emplace_back(Graph::Vertex<shared_ptr<Field>>{value});
                goblins.emplace_back(value);
                allNpcs.emplace_back(value);
            }
            col++;
        }
        row++;
    }

    int nRows = row;
    int nCols = graph.size() / nRows;

    for (int y = 0; y < nRows; y++)
    {
        for (int x = 0; x < nCols; x++)
        {
            const auto idx = getIndex(nCols, x, y);
            const auto vertex = &graph[idx];
            if (vertex == nullptr)
            {
                throw logic_error("wtf");
            }

            vertex->Index = idx;
            if (vertex->Value->getTypeOfField() == FieldType::Wall)
            {
                continue;
            }

            for (auto [offX, offY] : neighborOffsets)
            {
                const auto neighY = (y + offY);
                const auto neighX = (x + offX);
                if (neighX < 0 || neighX >= nRows || neighY < 0 || neighY > nCols)
                {
                    continue;
                }

                const auto neighIdx = getIndex(nCols, neighX, neighY);
                auto edge = &graph[neighIdx];
                if (&graph[neighIdx] == nullptr)
                {
                    throw logic_error("wtf");
                }

                if (edge->Value->getTypeOfField() == FieldType::Open)
                {
                    vertex->Edges.emplace_back(edge);
                }
            }
        }
    }

    int rounds = 0;

    while (!elves.empty() && !goblins.empty())
    {
        rounds++;
        sortNPCsByReadingOrder(allNpcs);
        int ro = 0;
        for (auto npc : allNpcs)
        {
            npc->ReadingOrder = ro++;
        }

        // printGraph(graph);

        for (const auto &npc : allNpcs)
        {
            printGraph(graph);
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

            auto attackTargets = getAttackTargets(graph, npc, enemyType, enemies);
            if (attackTargets.empty())
            {
                auto nextStepTowardsTarget = getNextPositionTowardsNextEnemy(graph, npc, enemies);
                if (!nextStepTowardsTarget.has_value())
                {
                    continue;
                }

                advanceNpc(graph, npc, nextStepTowardsTarget.value());
                attackTargets = getAttackTargets(graph, npc, enemyType, enemies);
            }

            if (!attackTargets.empty())
            {
                if (auto target = choseAttackTarget(attackTargets); npc->Attack(*target))
                {
                    const auto targetPos = target->Position;

                    auto node = graph[getIndex(targetPos)];
                    node.Value->changeType(FieldType::Open);

                    allNpcs.remove(target);
                    enemies.remove(target);
                }
            }
        }
    }
}
