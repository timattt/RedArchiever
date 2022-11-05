#pragma once

#include <tuple>
#include <cstdint>
#include <vector>
#include <iosfwd>
#include <map>

namespace archiver
{

using data_t = uint8_t;

struct code_t
{
    int len; // In bits
    uint64_t bits;
};

struct node_t
{
    bool leaf;
    int left, right;
    data_t value;
};

inline uint64_t num_bits2num_bytes(uint64_t num_bits)
{
    return num_bits / 8 + !!(num_bits % 8);
}

class ArchiverCPU
{
    template <typename Iter>
    std::vector<int>
    calc_freq_table_impl(Iter begin,
                         Iter end,
                         data_t min,
                         data_t max);

    static std::vector <node_t>
    calc_haff_tree (const std::vector <int>& freq_table);
    
    static std::vector <code_t>
    calc_codes_table (const std::vector <node_t>& freq_tree,
                    int alphabet_size);

    template <typename Iter>
    std::tuple<std::vector<uint8_t>, unsigned>
    archive_impl(Iter begin,
                 Iter end,
                 const std::vector<code_t> &codes_table,
                 data_t min);

public:
    template <typename Iter>
    std::tuple<std::vector<uint8_t>, unsigned, std::vector<node_t>>
    archive(Iter begin,
            Iter end,
            data_t min,
            data_t max);

    std::vector<data_t>
    dearchive(const std::vector<uint8_t> &data,
                unsigned num_bits,
                const std::vector<node_t> &haff_tree,
                data_t min);
};

template <typename Iter>
std::vector<int>
ArchiverCPU::calc_freq_table_impl(Iter begin,
                                  Iter end,
                                  data_t min,
                                  data_t max)
{
    std::map<data_t, int> freq_map;
    for (auto it = begin; it != end; ++it)
        ++freq_map[*it];

    const auto alphabet_size = max - min + 1;
    std::vector<int> freq_table(alphabet_size);
    for (const auto &[value, freq] : freq_map)
        freq_table[value - min] = freq;

    return freq_table;
}

template <typename Iter>
std::tuple <std::vector <uint8_t>, unsigned>
ArchiverCPU::archive_impl (Iter begin,
                           Iter end,
                           const std::vector <code_t>& codes_table,
                           data_t min) {
    constexpr auto size_coef = sizeof(uint64_t) / sizeof(data_t);
    std::size_t max_size_coded = std::distance(begin, end) * size_coef;
    if (max_size_coded % size_coef != 0)
        max_size_coded += sizeof(uint64_t);
    
    std::vector <uint8_t> coded (max_size_coded);

    unsigned pos = 0;
    for (auto it = begin; it != end; ++it) {
        code_t code = codes_table[*it - min];
        uint64_t code_val = code.bits;
        uint64_t* cur = (uint64_t*) (coded.data () + pos / 8);
        *cur |= code_val << (pos % 8);
        pos += code.len;
    }

    coded.resize (pos / 8 + (pos % 8 != 0));
    coded.shrink_to_fit ();

    return {coded, pos};
}

template <typename Iter>
std::tuple<std::vector<uint8_t>, unsigned, std::vector<node_t>>
ArchiverCPU::archive(Iter begin,
                     Iter end,
                     data_t min,
                     data_t max)
{
    const auto alphabet_size = max - min + 1;
    std::vector<int> freq_table = calc_freq_table_impl(begin, end, min, max);
    std::vector<node_t> haff_tree = calc_haff_tree(freq_table);
    std::vector<code_t> codes_table = calc_codes_table(haff_tree, alphabet_size);

    const auto [archived_data, num_bits] = archive_impl(begin, end, codes_table, min);
    return {archived_data, num_bits, haff_tree};
}

} // namespace archiver

std::ostream &
operator<<(std::ostream &os, const archiver::code_t &code);
