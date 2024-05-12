#include <iostream>
#include <cstdint>
#include <assert.h>

#include "../src/cuckoo.h"

int main() {
    // Define filter parameters
    const int num_buckets = 1<<15;
    const int poss_buckets = 4;
    const int bucket_size = 1;

    // Initialize filter where each item is of type uint32_t
    // and fingerprint is of type uint16_t
    CuckooFilter<num_buckets, poss_buckets, bucket_size, uint32_t, uint16_t> cf;
    for (uint32_t i = 0; i < num_buckets; i++) {
        if (!cf.insert(i)) {
            std::cout << "Filter is full" << std::endl;
            break;
        }
        if (cf.load_factor() > 0.95) {break;}
    }

    int total_inserted = cf.load_factor() * (num_buckets);

    // Check all inserted items are in the filter
    for (uint32_t i = 0; i < total_inserted; i++) {
        assert(cf.contains(i));
    }

    // Check non-existing items, expect some false positives
    uint32_t total_queries = 0;
    uint32_t false_queries = 0;
    for (uint32_t i = total_inserted; i < 2 * total_inserted; i++) {
        if (cf.contains(i)) {
            false_queries++;
        }
        total_queries++;
    }

    // Calculate false positive rate
    std::cout << "\nFalse positive rate is: "
              << 100.0 * false_queries / total_queries
                << "%" << std::endl;
    std::cout << "Items inserted: " << total_inserted << std::endl;
    std::cout << "Load factor: " << cf.load_factor() << std::endl;

}