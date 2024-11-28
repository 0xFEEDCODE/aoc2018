#include "../utilslib/utilslib.h"

#include <map>
#include <set>
#include <sstream>

#define N_REGISTERS 4

using ll = long long;

struct CPU
{
    map<ll, ll> registersBefore;
    map<ll, ll> registers;
#define R registers

    string registersAsStr()
    {
        stringstream ss;
        ss << "[";
        bool isStart = true;
        for (auto [r, v] : R)
        {
            if (!isStart)
            {
                ss << ", ";
            }
            ss << v;
            isStart = false;
        }
        ss << "]";
        return ss.str();
    }

    void printRegisters()
    {
        cout << registersAsStr() << endl;
    }

    void storeRegistersBeforeOperation()
    {
        for (auto [r, v] : R)
        {
            registersBefore[r] = v;
        }
    }

    void addr(const ll ra, const ll rb, const ll rc)
    {
        storeRegistersBeforeOperation();
        R[rc] = R[ra] + R[rb];
    }
    void addi(const ll ra, const ll vb, const ll rc)
    {
        storeRegistersBeforeOperation();
        R[rc] = R[ra] + vb;
    }
    void mulr(const ll ra, const ll rb, const ll rc)
    {
        storeRegistersBeforeOperation();
        R[rc] = R[ra] * R[rb];
    }
    void muli(const ll ra, const ll vb, const ll rc)
    {
        storeRegistersBeforeOperation();
        R[rc] = R[ra] * vb;
    }
    void banr(const ll ra, const ll rb, const ll rc)
    {
        storeRegistersBeforeOperation();
        R[rc] = R[ra] & R[rb];
    }
    void bani(const ll ra, const ll vb, const ll rc)
    {
        storeRegistersBeforeOperation();
        R[rc] = R[ra] & vb;
    }
    void borr(const ll ra, const ll rb, const ll rc)
    {
        storeRegistersBeforeOperation();
        R[rc] = R[ra] | R[rb];
    }
    void bori(const ll ra, const ll vb, const ll rc)
    {
        storeRegistersBeforeOperation();
        R[rc] = R[ra] | vb;
    }
    void setr(const ll ra, const ll rb, const ll rc)
    {
        storeRegistersBeforeOperation();
        R[rc] = R[ra];
    }
    void seti(const ll va, const ll vb, const ll rc)
    {
        storeRegistersBeforeOperation();
        R[rc] = va;
    }
    void gtir(const ll va, const ll rb, const ll rc)
    {
        storeRegistersBeforeOperation();
        R[rc] = va > R[rb] ? 1 : 0;
    }
    void gtri(const ll ra, const ll vb, const ll rc)
    {
        storeRegistersBeforeOperation();
        R[rc] = R[ra] > vb ? 1 : 0;
    }
    void gtrr(const ll ra, const ll rb, const ll rc)
    {
        storeRegistersBeforeOperation();
        R[rc] = R[ra] > R[rb] ? 1 : 0;
    }
    void eqir(const ll va, const ll rb, const ll rc)
    {
        storeRegistersBeforeOperation();
        R[rc] = va == R[rb] ? 1 : 0;
    }
    void eqri(const ll ra, const ll vb, const ll rc)
    {
        storeRegistersBeforeOperation();
        R[rc] = R[ra] == vb ? 1 : 0;
    }
    void eqrr(const ll ra, const ll rb, const ll rc)
    {
        storeRegistersBeforeOperation();
        R[rc] = R[ra] == R[rb] ? 1 : 0;
    }
    void revertOperation()
    {
        for (auto reg : registersBefore)
        {
            R[reg.first] = reg.second;
        }
    }

    void reset()
    {
        registers.clear();
        registersBefore.clear();
    }
};

using namespace std::placeholders;
int main()
{

    // 6 0 7
    // 7 2
    // 11 9 14
    CPU cpu;

    map<ll, ll> opcodeMap = {{0, 8},  {1, 15}, {2, 11}, {3, 3},   {4, 13}, {5, 6},   {6, 7},  {7, 2},
                             {8, 12}, {9, 9},  {10, 4}, {11, 14}, {12, 1}, {13, 10}, {14, 0}, {15, 5}};
    const vector<function<void(ll, ll, ll)>> operations = {
        bind(&CPU::addi, &cpu, _1, _2, _3), bind(&CPU::addr, &cpu, _1, _2, _3), bind(&CPU::mulr, &cpu, _1, _2, _3),
        bind(&CPU::muli, &cpu, _1, _2, _3), bind(&CPU::banr, &cpu, _1, _2, _3), bind(&CPU::bani, &cpu, _1, _2, _3),
        bind(&CPU::borr, &cpu, _1, _2, _3), bind(&CPU::bori, &cpu, _1, _2, _3), bind(&CPU::setr, &cpu, _1, _2, _3),
        bind(&CPU::seti, &cpu, _1, _2, _3), bind(&CPU::gtir, &cpu, _1, _2, _3), bind(&CPU::gtri, &cpu, _1, _2, _3),
        bind(&CPU::gtrr, &cpu, _1, _2, _3), bind(&CPU::eqir, &cpu, _1, _2, _3), bind(&CPU::eqri, &cpu, _1, _2, _3),
        bind(&CPU::eqrr, &cpu, _1, _2, _3)};

    redirect_inp_to_stdin();

    map<ll, set<int>> history;

    int c = 0;
    string line;
    while (getline(cin, line))
    {
        if (line.size() == 0)
        {
            continue;
        }

        if (line.size() < 8)
        {
            break;
        }

        string before = line.substr(9);
        before = before.substr(0, before.size() - 1);
        getline(cin, line);
        string instruction = line;
        getline(cin, line);
        string after = line.substr(8);

        stringstream ss(before);
        for (int i = 0; i < N_REGISTERS; i++)
        {
            string value;
            getline(ss, value, ',');
            cpu.registers[i] = stoi(value);
        }

        auto args = vector(4, -1l);
        ss = stringstream(instruction);
        for (auto &it : args)
        {
            string value;
            getline(ss, value, ' ');
            it = stoi(value);
        }

        cpu.printRegisters();

        int i = 0;
        int lc = 0;
        for (auto op : operations)
        {
            auto opCode = args[0];
            op(args[1], args[2], args[3]);
            if (cpu.registersAsStr() == after)
            {
                history[opCode].emplace(i);
                lc++;
            }
            cpu.revertOperation();
            i++;
        }
        if (lc >= 3)
        {
            c++;
        }
    }

    for (int i = 0; i < operations.size(); i++)
    {
        vector<ll> possibleOp;
        for (auto [k, v] : history)
        {
            if (v.contains(i))
            {
                possibleOp.emplace_back(k);
            }
        }
        cout << "Operation n " << i << ", possible opcodes: ";
        for (auto it : possibleOp)
        {
            cout << it << " ";
        }
        cout << endl;
    }

    cpu.reset();
    while (true)
    {
        cout << line << endl;
        auto args = vector(4, -1l);
        auto ss = stringstream(line);
        for (auto &it : args)
        {
            string value;
            getline(ss, value, ' ');
            it = stoi(value);
        }

        operations[opcodeMap[args[0]]](args[1], args[2], args[3]);
        if (!getline(cin, line))
        {
            break;
        }
    }
    cout << cpu.registers[0] << endl;
}
