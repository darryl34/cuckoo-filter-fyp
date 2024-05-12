#pragma once

#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <cmath>
#include <random>

#include "bucket.h"

#define XXH_INLINE_ALL
#include "xxhash.h"

template <int num_buckets, int poss_buckets, int bucket_size, typename item_type, typename fp_type>
class CuckooFilter {
private:
    // class variables
    Bucket<bucket_size, fp_type> buckets[num_buckets];  // Array of buckets
    uint32_t curr_size = 0;                         // Current number of items
    static const uint32_t max_kicks = 500;         // Maximum number of kicks

    // bit masks for hash and bucket index calculations
    const uint32_t HASH_MASK = calc_hash_mask(calc_num_bits(num_buckets));
    const uint32_t BKT_MASK = calc_bucket_mask(calc_num_bits(num_buckets));
    const uint32_t INVERTED_BKT_MASK = ~BKT_MASK & (num_buckets - 1);       // invert all bits

    // seed for hash functions
    XXH32_hash_t _seed;


 public:
    // Constructor
    CuckooFilter() {
        // Ensure possible_buckets is a power of 2
        if ((poss_buckets & (poss_buckets - 1)) != 0) {
            throw std::invalid_argument("possible_buckets must be a power of 2");
        }
        _seed = rand();
    }

    // Check if the filter contains an item
    bool contains(item_type item) {
        XXH64_hash_t hv = generate_hash(item);
        fp_type fp = fingerprint(hv);
        uint32_t idx = index_hash(hv);
        for (uint32_t i = 0; i < poss_buckets; i++) {
            if (buckets[idx].contains(fp)) { return true; }
            idx = calc_bucket_index(idx, fp);
        }
        return false;
    }

    // Insert an item into the filter
    // Returns true if the item was inserted, false if the filter is full
    // (max_kicks exceeded)
    bool insert(item_type item) {
        if (curr_size >= num_buckets * bucket_size) {
            return false;
        }
        XXH64_hash_t hv = generate_hash(item);
        fp_type fp = fingerprint(hv);
        uint32_t idx = index_hash(hv);
        uint32_t indices[poss_buckets];
        for (uint32_t i = 0; i < poss_buckets; i++) {
            indices[i] = idx;
            if (buckets[idx].insert(fp)) {
                curr_size++;
                return true;
            }
            idx = calc_bucket_index(idx, fp);  // Calculate the next index
        }

        // Pick a random bucket to evict an item from
        uint32_t rand_idx = indices[rand() & (poss_buckets - 1)];

        for (uint32_t j = 0; j < max_kicks; j++) {
            fp = buckets[rand_idx].swap(fp);
            rand_idx = calc_bucket_index(rand_idx, fp);
            if (buckets[rand_idx].insert(fp)) {
                curr_size++;
                return true;
            }
        }
        return false;
    }

  // Remove an item from the filter
    bool remove(item_type item) {
        XXH64_hash_t hv = generate_hash(item);
        fp_type fp = fingerprint(hv);
        uint32_t idx = index_hash(hv);
        for (uint32_t i = 0; i < poss_buckets; i++) {
            if (buckets[idx].remove(fp)) {
                curr_size--;
                return true;
            }
            idx = calc_bucket_index(idx, fp);
        }
        return false;
    }

    // Generate a fingerprint from a hash value
    fp_type fingerprint(XXH64_hash_t hv) {
        // cast 64-bit hash to needed precision
        return static_cast<fp_type>(hv);
    }

    // Generate index from hash value
    uint32_t index_hash(XXH64_hash_t hv) {
        // use higher bits of hash to determine bucket index
        return (hv >> 32) & (num_buckets - 1);
    }

    // Generate a hash value from an item with xxhash
    XXH64_hash_t generate_hash(item_type item) {
        return XXH64(&item, sizeof(item), _seed);
    }

    // Calculate the next bucket index
    uint32_t calc_bucket_index(uint32_t idx, fp_type fp) {
        uint32_t H = index_hash(generate_hash(fp));
        uint32_t i = 0;

        while ((H & HASH_MASK) == 0) { H = index_hash(generate_hash(fp + ++i)); }

        uint32_t next_idx = (((H & BKT_MASK) + (idx & BKT_MASK)) & BKT_MASK) +
                            (((H & INVERTED_BKT_MASK) + (idx & INVERTED_BKT_MASK)) &
                                INVERTED_BKT_MASK);
        return next_idx;
    }

    // Calculate the mask for the hash value
    // we define 3 masks for 2, 4, and 8 possible buckets
    // and slice to the number of bits needed
    uint32_t calc_hash_mask(int num_bkts_bits) {
        const uint32_t bkts_2_mask = 0b11111111111111111111111111111111;
        const uint32_t bkts_4_mask = 0b01010101010101010101010101010101;
        const uint32_t bkts_8_mask = 0b01001001001001001001001001001001;
        uint32_t bit_mask = (1 << num_bkts_bits) - 1;

        // mask to the number of bits needed
        switch (poss_buckets) {
            case 2:
                return bkts_2_mask & bit_mask;
            case 4:
                return bkts_4_mask & bit_mask;
            case 8:
                return bkts_8_mask & bit_mask;
            default:
                throw std::invalid_argument("Invalid number of possible buckets");
        }
    }

    uint32_t calc_bucket_mask(int num_bkts_bits) {
        // Generate 32 bit masks
        const uint32_t bkts_2_mask = 0b01010101010101010101010101010101;
        const uint32_t bkts_4_mask = 0b00110011001100110011001100110011;
        const uint32_t bkts_8_mask = 0b11000111000111000111000111000111;

        uint32_t bit_mask = (1 << num_bkts_bits) - 1;

        // slice the 32 bit mask to the number of bits needed
        switch (poss_buckets) {
            case 2:
                return bkts_2_mask & bit_mask;
            case 4:
                return bkts_4_mask & bit_mask;
            case 8:
                return bkts_8_mask & bit_mask;
            default:
                throw std::invalid_argument("Invalid number of possible buckets");
        }
    }

    // Calculate the number of bits needed to represent a number
    uint32_t calc_num_bits(uint32_t v) {
        uint32_t r = 0;
        while (v >>= 1) {
            r++;
        }
        return r;
    }

    // Get the current load factor
    double load_factor() {
        return (double)curr_size / (num_buckets * bucket_size);
    }


    // Print the number of items in each bucket
    void bkt_info() {
        for (int i = 0; i < num_buckets; i++) {
            std::cout << "Bucket: " << i << " Size: " << buckets[i].size() << " ";
        }
    }

    // Reset the filter
    void reset() {
        for (int i = 0; i < num_buckets; i++) {
            buckets[i].clear();
        }
        curr_size = 0;
    }

    // Reset seed for hash function
    void reset_seeds() {
        _seed = rand();
    }

};