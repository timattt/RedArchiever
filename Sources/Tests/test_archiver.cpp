#include <gtest/gtest.h>
#include <random>
#include <type_traits>

#include "../archiver.hpp"

template <typename RandEngine, typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
std::vector <T>
genRandomVector (RandEngine rand_gen,
                 std::size_t size,
                 T min,
                 T max) {
    std::uniform_int_distribution<T> dist(min, max);
    std::vector <T> vec(size);
    std::for_each(std::begin(vec), std::end(vec), [&] (auto& value) {
        value = dist(rand_gen);
    });

    return vec;
}

template <typename RandEngine, typename T>
T get_rand (RandEngine rand_gen,
            T min,
            T max) {
    return rand_gen () % (max + 1 - min) + min;
}

TEST (ACHIVER_CPU_TEST, RANDOM_GENERATED) {
    using archiver::data_t;

    const unsigned min_alphabet_size = 2;
    const unsigned max_alphabet_size = 256;

    const unsigned min_data_size = 1;
    const unsigned max_data_size = 2'000'000;

    std::random_device rd;
    std::mt19937 mersenne {rd ()};

    const data_t min = 0;
    int num_repeats = 10;

    auto gen_rand = [&mersenne] (auto min, auto max) {
        return get_rand (mersenne, min, max);
    };

    auto run_test = [&mersenne](unsigned size, data_t min, data_t max) {
        const std::vector <data_t> data = genRandomVector (mersenne, size, min, max);
        
        archiver::ArchiverCPU arch;
        auto [archived_data, num_bits, haff_tree] = arch.archive (data.cbegin(), data.cend(), min, max);
        const std::vector <data_t> data_decoded = arch.dearchive (archived_data, num_bits, haff_tree, min);

        ASSERT_EQ (data.size(), data_decoded.size());
        ASSERT_EQ (data, data_decoded);
    };

    run_test(1, min, min + max_alphabet_size - 1);
    run_test(2, min, min +  max_alphabet_size - 1);
    run_test(10, min, min + max_alphabet_size - 1);

    for (int i = 1; i < 10; ++i)
        run_test((min + max_alphabet_size) * i, min, min + max_alphabet_size - 1);

    while (num_repeats--) {
        const auto size = gen_rand (min_data_size, max_data_size);
        const auto alphabet_size = gen_rand (min_alphabet_size, max_alphabet_size);
        const data_t max = min + alphabet_size;

        run_test(size, min, max);
    }
}
