#include <iostream>
#include <cstdint>
#include <time.h>
#include <cstdlib>
#include <cassert>
#include <string>

#include "../src/cuckoo.h"


int main() {
    srand(42);
    const uint32_t num_buckets = 1 << 18;
    const uint32_t possible_buckets = 4;
    const uint32_t bucket_size = 4;
    uint32_t total_items = num_buckets * bucket_size;
    const uint32_t fp_size = 8;

    static CuckooFilter<num_buckets, possible_buckets, bucket_size, uint32_t, uint8_t> cf;

    // Insert items
    uint32_t num_inserted = 0;
    for (uint32_t i = 0; i < total_items; i++, num_inserted++) {
        if (!cf.insert(i)) {
            throw std::runtime_error("Filter is full");
        }
        if (cf.load_factor() >= 0.95 ) { break; }
    }

    // Check non-existing items, a few false positives expected
    uint32_t total_queries = 0;
    uint32_t false_queries = 0;
    for (uint32_t i = total_items; i < 2 * total_items; i++) {
        if (cf.contains(i)) {
            false_queries++;
        }
        total_queries++;
    }

    // The output gives:
    // the measured false positive rate
    // table type, fingerprint size, table length, table size in bits, number of inserted elements
    // and the space occupancy rate
    std::cout << "\nFalse positive rate is: "
              << 100.0 * false_queries / total_queries
                << "%" << std::endl;
    std::cout << "Items inserted: " << num_inserted << std::endl;
    std::cout << "Load factor: " << cf.load_factor() << std::endl;
    return 0;
}