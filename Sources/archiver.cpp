#include <map>
#include <vector>
#include <stack>
#include <algorithm>
#include <set>

#include "archiver.hpp"
#include "PrintLib.hpp"

#define PRINT(obj) // std::cout << #obj ": " << obj << std::endl
#define DUMP(obj) std::cout << #obj ": " << obj << std::endl

namespace archiver
{

// ============================\\
// Static function for archiver -----------------------------------------------
// ============================//

struct freq_t
{
    int pos = -1;
    int freq = -1;

    freq_t(int pos, int freq) : pos(pos),
                                freq(freq)
    {
    }
};

static uint64_t
fill_bits(const std::vector<bool> &stack)
{
    uint64_t res = 0;
    if (stack.size() > 8 * sizeof(res))
    {
        throw std::invalid_argument("stack.size () > 8 * sizeof (res)");
    }

    uint32_t i = 0;
    do
    {
        res <<= 1;
        res |= stack[stack.size() - i - 1];
    } while (++i != stack.size());

    return res;
}

static void
fill_codes(const std::vector<node_t> &tree,
            std::size_t pos, bool is_right,
            std::vector<code_t> &codes,
            std::vector<int> &vals,
            std::vector<bool> &stack)
{
    stack.push_back(is_right);

    const node_t &node = tree[pos];
    if (node.leaf)
    {

        code_t code = {
            .len = static_cast<int>(stack.size()),
            .bits = fill_bits(stack)};
        codes.push_back(code);
        vals.push_back(node.value);
    }
    else
    {
        fill_codes(tree, node.left, false, codes, vals, stack);
        fill_codes(tree, node.right, true, codes, vals, stack);
    }

    stack.pop_back();
}

static void
fill_codes(const std::vector<node_t> &tree,
            std::vector<code_t> &codes,
            std::vector<int> &vals)
{
    node_t root = tree[tree.size() - 1];
    std::vector<bool> stack;

    fill_codes(tree, root.left, false, codes, vals, stack);
    fill_codes(tree, root.right, true, codes, vals, stack);
}

static void
sort(std::vector<freq_t> &vec)
{
    std::sort(vec.begin(), vec.end(),
                [](const auto &lhs, const auto &rhs)
                {
                    return lhs.freq > rhs.freq;
                });
}

// TODO: optimize
std::vector<node_t>
ArchiverCPU::calc_haff_tree(const std::vector<int> &freq_table)
{
    PRINT(freq_table);

    std::vector<freq_t> freq_vec;
    freq_vec.reserve(freq_table.size());

    // Fill leafs in tree
    std::vector<node_t> tree;
    tree.reserve(freq_table.size());
    for (int i = 0; i < freq_table.size(); ++i)
    {
        if (freq_table[i] == 0)
        {
            continue;
        }

        node_t node = {
            .leaf = true,
            .left = -1,
            .right = -1,
            .value = (data_t)i};
        tree.push_back(node);

        // Push position and frequency in freq_vec
        freq_t freq_item{(int)tree.size() - 1, freq_table[i]};
        freq_vec.push_back(freq_item);
    }

    // Build Haffman-tree
    while (freq_vec.size() > 1)
    {
        sort(freq_vec);
        PRINT(freq_vec);

        const auto end = freq_vec.end();
        auto min1 = *(end - 1);
        auto min2 = *(end - 2);
        freq_vec.pop_back();
        freq_vec.pop_back();

        node_t node = {
            .leaf = false,
            .left = min1.pos,
            .right = min2.pos,
            .value = 0};

        tree.push_back(node);

        freq_t freq(tree.size() - 1, min1.freq + min2.freq);
        freq_vec.push_back(freq);
    }
    PRINT(tree);

    return tree;
}

static void
fill_codes(const std::vector<node_t> &tree,
            std::size_t pos, bool is_right,
            std::vector<std::pair<int, code_t>> &codes,
            std::vector<bool> &stack)
{
    stack.push_back(is_right);

    const node_t &node = tree[pos];
    if (node.leaf)
    {
        code_t code = {
            .len = static_cast<int>(stack.size()),
            .bits = fill_bits(stack)};
        codes.emplace_back(node.value, code);
    }
    else
    {
        fill_codes(tree, node.left, false, codes, stack);
        fill_codes(tree, node.right, true, codes, stack);
    }

    stack.pop_back();
}

static void
fill_codes(const std::vector<node_t> &tree,
            std::vector<std::pair<int, code_t>> &codes)
{
    const node_t &root = tree[tree.size() - 1];
    std::vector<bool> stack;

    if (root.leaf)
    {
        code_t code = {
            .len = 1,
            .bits = 0};
        codes.emplace_back(root.value, code);
    }
    else
    {
        fill_codes(tree, root.left, false, codes, stack);
        fill_codes(tree, root.right, true, codes, stack);
    }
}

std::vector<code_t>
ArchiverCPU::calc_codes_table(const std::vector<node_t> &freq_tree,
                                int alphabet_size)
{
    std::vector<std::pair<int, code_t>> codes;
    fill_codes(freq_tree, codes);

    PRINT(codes);
    // Sort by value
    std::sort(codes.begin(), codes.end(),
                [](const auto &lhs, const auto &rhs)
                {
                    return lhs.first < rhs.first;
                });

    PRINT(codes);

    // Create full table
    std::vector<code_t> codes_table(alphabet_size, code_t{0});
    for (const auto &[pos, code] : codes)
    {
        codes_table[pos] = code;
    }

    PRINT(codes_table);
    return codes_table;
}

// ==========\\
// AchiverCPU -----------------------------------------------------------------
// ==========//

static uint8_t
get_bit(const std::vector<uint8_t> &vec,
        unsigned pos)
{
    return (vec[pos / 8] & (1ull << (pos % 8))) != 0;
}

std::vector<data_t>
ArchiverCPU::dearchive(const std::vector<uint8_t> &archived_data,
                        unsigned num_bits,
                        const std::vector<node_t> &haff_tree,
                        data_t min)
{
    if (num_bits == 1)
        return {(data_t)(min + haff_tree[0].value)};

    std::vector<data_t> data;

    unsigned pos = 0;
    auto next_bit = [&archived_data, &pos]()
    {
        return archiver::get_bit(archived_data, pos++);
    };

    const auto root = haff_tree.size() - 1;
    PRINT(num_bits);
    while (pos < num_bits)
    {
        auto node_pos = root;
        while (!haff_tree[node_pos].leaf)
        {
            auto &node = haff_tree[node_pos];

            node_pos = next_bit() ? node.right : node.left;
            if (node_pos == -1)
            {
                throw std::runtime_error("Failed to encode data");
            }
        }
        data.push_back(haff_tree[node_pos].value + min);
    }

    return data;
}

}

static int
get_bit(uint32_t value,
        unsigned pos)
{
    return (value & (1ull << pos)) != 0;
}

std::ostream &
operator<<(std::ostream &os, const archiver::code_t &code)
{
    for (std::size_t j = 0; j < code.len; ++j)
    {
        os << get_bit(code.bits, code.len - 1 - j);
    }

    return os;
}

std::ostream &
operator<<(std::ostream &os, const archiver::freq_t &freq)
{
    return os << "{" << freq.pos << "->" << freq.freq << "}";
}

std::ostream &
operator<<(std::ostream &os, const archiver::node_t &n)
{
    return os << "{" << std::boolalpha << n.leaf << ", left: " << n.left
              << ", right: " << n.right << ", value: " << n.value << "}" << std::endl;
}
