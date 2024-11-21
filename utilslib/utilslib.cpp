#include "utilslib.h"
#include <cstdio>
#include <iostream>
#include <map>

using namespace std;

void rewind_stdin_on_eof()
{
    if (feof(stdin))
    {
        clearerr(stdin);
        fseek(stdin, 0, SEEK_SET);
    }
}

bool redirect_file_to_stdin(const char *filename)
{
    FILE *file = freopen(filename, "r", stdin);
    if (file == nullptr)
    {
        perror("Error opening file");
        return false;
    }
    return true;
}

bool redirect_inp_to_stdin()
{
    FILE *file = freopen("inp.txt", "r", stdin);
    if (file == nullptr)
    {
        perror("Error opening file");
        return false;
    }
    return true;
}


struct Window
{
    Window(int capacity) : capacity(capacity), n_elements(0)
    {
        Reset();
    }

    int operator[](int index) const
    {
        auto it = window.find(index);
        if (it != window.end())
        {
            return it->second;
        }
        return -1;

        // throw logic_error("No element found");
    }

  private:
    const int UNINTIALZED_FIELD_VALUE = -1;
    map<int, int> window;
    int capacity;
    int n_elements;

  public:
    void Reset()
    {
        for (int i = 0; i < capacity; i++)
        {
            window[i] = UNINTIALZED_FIELD_VALUE;
        }
        n_elements = 0;
    }

    bool Contains(int value)
    {
        for (const auto kv : window)
        {
            if ((kv.second) == value){
            return true;}

        }
        return false;
    }

    int GetCapacity()
    {
        return capacity;
    }
    int GetSize()
    {
        return n_elements;
    }
    void RemoveHead()
    {
        for (int i = 0; i < capacity; i++)
        {
            if (window[i] != UNINTIALZED_FIELD_VALUE)
            {
                window[i] = UNINTIALZED_FIELD_VALUE;
                n_elements--;
                break;
            }
        }
    }
    void InsertAtTail(int value)
    {
        auto tail_idx = capacity - 1;
        InsertShiftLeft(tail_idx, value);
    }

    void InsertShiftLeft(int insert_pos, int value)
    {
        // Shift all the elements before insert pos to the left
        for (int i = 0; i <= insert_pos - 1; i++)
        {
            if (window[i + 1] == UNINTIALZED_FIELD_VALUE)
            {
                continue;
            }
            window[i] = window[i + 1];
        }

        window[insert_pos] = value;
        n_elements = min(n_elements + 1, capacity);
    }

    void InsertShiftRight(int insert_pos, int value)
    {
        if (insert_pos < n_elements)
        {
            // Shift all the elements after insert pos to the right
            for (int i = capacity; i > insert_pos; i--)
            {
                if (window[i - 1] == UNINTIALZED_FIELD_VALUE)
                {
                    continue;
                }
                window[i] = window[i - 1];
            }
        }

        window[insert_pos] = value;

        n_elements = min(n_elements + 1, capacity);
    }
    void Print()
    {
        cout << endl;
        for (int i = 0; i < capacity; i++)
        {
            cout << window[i] << " ";
        }
        cout << endl;
    }
};
