#include <list>
#include <optional>
#include <sstream>
#include <stack>
#include <vector>

#include "../utilslib/utilslib.h"
using namespace std;

struct claim {
    int id;
    int left;
    int top;
    int width;
    int height;

private:
    static int calculate_overlap(int low_start, const int low_end, const int high_start, int high_end)
    {
        if (high_start > low_end)
        {
            return 0;
        }
        return low_end - high_start;
    }
    
    int get_overlap_hor(const claim &other_claim) const
    {
        const claim &low = (left < other_claim.left) ? *this : other_claim;
        const claim &high = (&low == this) ? other_claim : *this;
        
        const auto low_start = low.left;
        const auto low_end = low.left + low.width;

        const auto high_start = high.left;
        const auto high_end = high.left + high.width;

        return calculate_overlap(low_start, low_end, high_start, high_end);
    }

    int get_overlap_vert(const claim &other_claim) const
    {
        const claim &low = (top < other_claim.top) ? *this : other_claim;
        const claim &high = (&low == this) ? other_claim : *this;
        
        const auto low_start = low.top;
        const auto low_end = low.top + low.height;

        const auto high_start = high.top;
        const auto high_end = high.height + high.height;

        return calculate_overlap(low_start, low_end, high_start, high_end);
    }

public:
    int get_overlap_volume(const claim &other_claim) const
    {
        return get_overlap_hor(other_claim) * get_overlap_vert(other_claim);
    }

    optional<claim> get_overlap_as_claim(const claim &other_claim) const
    {
        const int overlap_hor = get_overlap_hor(other_claim);
        if(overlap_hor < 0)
        {
            return nullopt;
        }

        const int overlap_vert = get_overlap_vert(other_claim);
        if(overlap_vert < 0)
        {
            return nullopt;
        }
        
        const claim &low_vert = (top < other_claim.top) ? *this : other_claim;
        const claim &high_vert = (&low_vert == this) ? other_claim : *this;
        
        const claim &low_hor = (left < other_claim.left) ? *this : other_claim;
        const claim &high_hor = (&low_hor == this) ? other_claim : *this;


        return claim { id, high_hor.left, high_vert.top, overlap_hor, overlap_vert };
    }

    /*
    claim merge(const claim &other_claim) const
    {
        if(get_overlap_volume(other_claim) <= 0)
        {
            throw std::logic_error("no overlap");
        }
        
        const claim &low_vert = (top < other_claim.top) ? *this : other_claim;
        const claim &high_vert = (&low_vert == this) ? other_claim : *this;
        
        const claim &low_hor = (top < other_claim.top) ? *this : other_claim;
        const claim &high_hor = (&low_hor == this) ? other_claim : *this;

        int overlap_vert = get_overlap_hor(other_claim);
        int overlap_hor = get_overlap_vert(other_claim);

        return
        {
            this->id,
            low_hor.left,
            low_vert.top, low_hor.width + high_hor.width - overlap_hor,
            low_vert.width + high_vert.width - overlap_vert
        };
    }
    */

    void print_details() const
    {
        PRINT(id);
        PRINT("L" + std::to_string(left));
        PRINT("T" + std::to_string(top));
        PRINT("W" + std::to_string(width));
        PRINT("H" + std::to_string(height));    }
    };

claim parse_claim(const std::string &claim_str)
{
    claim claim;
    char *context = nullptr;
    char str[256];
    strcpy_s(str, claim_str.c_str());

    int* fields[] = { &claim.id, &claim.left, &claim.top, &claim.width, &claim.height };

    const string delimiter = "# @,:x";
    const char* token = strtok_s(str, delimiter.data(), &context);

    int i = 0;
    while(token != nullptr)
    {
        *fields[i++] = std::stoi(token);
        token = strtok_s(nullptr, delimiter.data(), &context);
    }

    return claim;
}

#define FIELD_EMPTY byte{0}
#define FIELD_VISITED byte{1}
#define FIELD_OVERLAP byte{2}

int main(int argc, char* argv[])
{
    redirect_file_to_stdin("inp.txt");

    vector<claim> claims;

    char buffer[256];
    while(fgets(buffer, sizeof(buffer), stdin) != nullptr)
    {
        auto claim = parse_claim(buffer);
        claims.emplace_back(claim);
    }

    constexpr int rows = 1024;
    constexpr int cols = 1024;
    vector grid(rows, vector(cols, byte{0}));

    for (auto [id, left, top, width, height] : claims) 
    {
        for (int c = left; c < left+width; ++c)
        {
            for (int r = top; r < top+height; ++r)
            {
                if (grid[r][c] == FIELD_EMPTY)
                {
                    grid[r][c] = FIELD_VISITED;
                }
                else if(grid[r][c] == FIELD_VISITED)
                {
                    grid[r][c] = FIELD_OVERLAP;
                }
            }
        }
    }

    int ans1 = 0;
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            ans1 += grid[i][j] == byte{2} ? 1 : 0;
        }
    }

    PRINT(ans1);
    
    for (auto [id, left, top, width, height] : claims) 
    {
        bool has_overlap = false;
        
        for (int c = left; c < left+width; ++c)
        {
            for (int r = top; r < top+height; ++r)
            {
                if(grid[r][c] == FIELD_OVERLAP)
                {
                    has_overlap = true;
                }
            }
        }
        
        if(!has_overlap)
        {
            PRINT(id);
        }
    }
    
    return 0;
}
