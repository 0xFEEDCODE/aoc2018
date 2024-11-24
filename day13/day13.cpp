#include <thread>
#include <vector>
#include <map>

#include "../utilslib/utilslib.h"

enum Direction
{
    UP, DOWN, LEFT, RIGHT
};

enum IntersectionChoice
{
    IC_LEFT, IC_STRAIGHT, IC_RIGHT
};

struct Cart
{
    point2d Position;
    Direction Direction;
    IntersectionChoice NextIntersectionDecision;
    bool IsOutOfOrder = false;


    bool operator==(const Cart &other) const
    {
        return Position == other.Position && Direction == other.Direction && NextIntersectionDecision == other.
            NextIntersectionDecision && IsOutOfOrder == other.IsOutOfOrder;
    }

public:
    void RotateLeft()
    {
        switch (Direction)
        {
        case DOWN:
            Direction = RIGHT;
            break;
        case RIGHT:
            Direction = UP;
            break;
        case UP:
            Direction = LEFT;
            break;
        case LEFT:
            Direction = DOWN;
            break;
        }
    }

    void RotateRight()
    {
        switch (Direction)
        {
        case DOWN:
            Direction = LEFT;
            break;
        case LEFT:
            Direction = UP;
            break;
        case UP:
            Direction = RIGHT;
            break;
        case RIGHT:
            Direction = DOWN;
            break;
        }
    }

    void AdvanceNextIntersectionDecision()
    {
        switch (NextIntersectionDecision)
        {
        case IC_LEFT:
            NextIntersectionDecision = IC_STRAIGHT;
            break;
        case IC_STRAIGHT:
            NextIntersectionDecision = IC_RIGHT;
            break;
        case IC_RIGHT:
            NextIntersectionDecision = IC_LEFT;
            break;
        }
    }

    void AdvanceInCurrentDirection()
    {
        switch (Direction)
        {
        case UP:
            Position.y--;
            break;
        case DOWN:
            Position.y++;
            break;
        case LEFT:
            Position.x--;
            break;
        case RIGHT:
            Position.x++;
            break;
        }
    }
};

char convertToChar(const Direction direction)
{
    switch (direction)
    {
    case UP:
        return '^';
    case DOWN:
        return 'v';
    case LEFT:
        return '<';
    case RIGHT:
        return '>';
    default: ;
        throw logic_error("wtf2");
    }
}

Direction convertToDirection(const char ch)
{
    switch (ch)
    {
    case '^':
        return UP;
    case 'v':
        return DOWN;
    case '<':
        return LEFT;
    case '>':
        return RIGHT;
    default: ;
        throw logic_error("wtf1");
    }
}

void printGrid(int rows, int cols, vector<vector<char>> grid)
{
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            cout << grid[r][c];
        }
        cout << endl;
    }
}

void turn45(Cart &cart)
{
    // "/"
    if (cart.Direction == LEFT)
    {
        cart.Direction = DOWN;
    }
    else if (cart.Direction == RIGHT)
    {
        cart.Direction = UP;
    }
    else if (cart.Direction == UP)
    {
        cart.Direction = RIGHT;
    }
    else if (cart.Direction == DOWN)
    {
        cart.Direction = LEFT;
    }
}

void turn135(Cart &cart)
{
    // "\"
    if (cart.Direction == LEFT)
    {
        cart.Direction = UP;
    }
    else if (cart.Direction == RIGHT)
    {
        cart.Direction = DOWN;
    }
    else if (cart.Direction == UP)
    {
        cart.Direction = LEFT;
    }
    else if (cart.Direction == DOWN)
    {
        cart.Direction = RIGHT;
    }
}

void printSimulation(int rows, int cols, vector<Cart> carts, vector<vector<char>> grid)
{
    vector temp(rows, vector(cols, ' '));
    for (auto c : carts)
    {
        temp[c.Position.y][c.Position.x] = grid[c.Position.y][c.Position.x];
        grid[c.Position.y][c.Position.x] = convertToChar(c.Direction);
    }
    ClearConsoleOutput();
    printGrid(rows, cols, grid);
    for (auto c : carts)
    {
        grid[c.Position.y][c.Position.x] = temp[c.Position.y][c.Position.x];;
    }
    this_thread::sleep_for(chrono::milliseconds(1000));
}

void removeOutOfOrderCards(vector<Cart> *carts)
{
    auto it = std::ranges::remove_if(*carts, [](const Cart &c)
    {
        return c.IsOutOfOrder;
    });
    carts->erase(it.begin(), it.end());
}

void sortCarts(vector<Cart> *carts)
{
    ranges::sort(*carts, [](const Cart &a, const Cart &b)
    {
        if (a.Position.y == b.Position.y)
        {
            return a.Position.x < b.Position.x;
        }
        return a.Position.y < b.Position.y;
    });
}

int main()
{
    int rows = 0;
    int cols = -1;

    redirect_inp_to_stdin();

    string line;
    vector<string> lines;

    while (getline(cin, line))
    {
        rows++;
        if (cols == -1)
        {
            cols = line.size();
        }
        lines.emplace_back(line);
    }

    vector<Cart> carts;
    vector grid(rows, vector(cols, ' '));

    int r = 0;
    for (const auto &l : lines)
    {
        int c = 0;
        for (const char ch : l)
        {
            if (ch == '^' || ch == 'v')
            {
                grid[r][c] = '|';
                carts.emplace_back(Cart{point2d(c, r), convertToDirection(ch), IC_LEFT});
            }
            else if (ch == '<' || ch == '>')
            {
                grid[r][c] = '-';
                carts.emplace_back(Cart{point2d(c, r), convertToDirection(ch), IC_LEFT});
            }
            else
            {
                grid[r][c] = ch;
            }
            c++;
        }
        r++;
    }

    //printGrid(rows, cols, grid);

    constexpr char TURN45 = '/';
    constexpr char TURN135 = '\\';
    constexpr char INTERSECTION = '+';

    string first_collision;

    while (carts.size() > 1)
    {
        removeOutOfOrderCards(&carts);
        if (carts.size() == 1)
        {
            cout << to_string(carts[0].Position.x) + "," + to_string(carts[0].Position.y) << endl;
            break;
        }

        sortCarts(&carts);

        for (auto &cart : carts)
        {
            if (cart.IsOutOfOrder)
            {
                continue;
            }
            cart.AdvanceInCurrentDirection();

            auto [x,y] = cart.Position;
            for (auto &other_cart : carts)
            {
                if (&cart == &other_cart)
                {
                    continue;
                }
                if (other_cart.Position == cart.Position)
                {
                    cart.IsOutOfOrder = true;
                    other_cart.IsOutOfOrder = true;

                    if (first_collision.empty())
                    {
                        first_collision = to_string(cart.Position.x) + "," + to_string(cart.Position.y);
                        cout << first_collision << endl;
                    }
                }
            }
            if (cart.IsOutOfOrder)
            {
                continue;
            }

            const auto track_part = grid[y][x];

            if (track_part == TURN45)
            {
                turn45(cart);
            }
            else if (track_part == TURN135)
            {
                turn135(cart);
            }
            else if (track_part == INTERSECTION)
            {
                if (cart.NextIntersectionDecision == IC_LEFT)
                {
                    cart.RotateLeft();
                }
                else if (cart.NextIntersectionDecision == IC_RIGHT)
                {
                    cart.RotateRight();
                }
                cart.AdvanceNextIntersectionDecision();
            }
        }

        //printSimulation(rows, cols, carts, grid);
    }
}
