
#include <cstdint>
#include <iterator>

#include "../utilslib/utilslib.h"
#include <sstream>
#include <stack>
#include <vector>

struct node
{
    int n_nodes;
    int n_metadata;

    bool initialized;

    vector<node> children;
    vector<int> metadata;

    node()
    {
        n_nodes = -1;
        n_metadata = -1;
        initialized = false;
    }

    void initialize(int n_nodes, int n_metadata)
    {
        this->n_nodes = n_nodes;
        this->n_metadata = n_metadata;
        this->initialized = true;
    }
};

vector<std::string> split(string const &input)
{
    istringstream buffer(input);
    vector<string> ret;

    std::copy(istream_iterator<string>(buffer), istream_iterator<string>(), back_inserter(ret));
    return ret;
}

uint64_t get_metadata_size(const node &node)
{
    uint64_t sum = 0;
    for (const auto metadata_value : node.metadata)
    {
        sum += metadata_value;
    }
    for (const auto &ch : node.children)
    {
        sum += get_metadata_size(ch);
    }
    return sum;
}

uint64_t get_node_value(const node &node)
{
    uint64_t sum = 0;
    if (node.n_nodes == 0)
    {
        for (const auto metadata_value : node.metadata)
        {
            sum += metadata_value;
        }
    }
    else
    {
        for (const auto metadata_value : node.metadata)
        {
            if (metadata_value - 1 < node.children.size())
            {
                sum += get_node_value(node.children.at(metadata_value - 1));
            }
        }
    }
    return sum;
}

int main(int argc, char *argv[])
{
    redirect_inp_to_stdin();

    string node_data_str;
    getline(cin, node_data_str);
    auto node_data = split(node_data_str);

    node root_node = node();
    node *prev_node = nullptr;
    node *curr_node = &root_node;

    int msum = 0;

    stack<pair<node *, int>> node_stack;

    int node_data_pointer = 0;
    int n_nodes = atoi(node_data[node_data_pointer++].data());
    int n_metada = atoi(node_data[node_data_pointer++].data());
    root_node.initialize(n_nodes, n_metada);

    node_stack.push({&root_node, root_node.n_nodes});

    while (!node_stack.empty())
    {
        auto [curr_node, n_children_remaining_to_be_processed] = node_stack.top();
        node_stack.pop();

        if (n_children_remaining_to_be_processed > 0)
        {
            node_stack.push({curr_node, n_children_remaining_to_be_processed - 1});
        }
        else
        {
            for (int i = 0; i < curr_node->n_metadata; i++)
            {
                curr_node->metadata.emplace_back(atoi(node_data[node_data_pointer++].data()));
            }
            continue;
        }

        n_nodes = atoi(node_data[node_data_pointer++].data());
        n_metada = atoi(node_data[node_data_pointer++].data());

        curr_node->children.emplace_back();
        curr_node->children.back().initialize(n_nodes, n_metada);
        node_stack.push({&curr_node->children.back(), n_nodes});
    }

    cout << endl << get_metadata_size(root_node);
    cout << endl << get_node_value(root_node);

    return 0;
}