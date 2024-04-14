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
    Bucket<bucket_size, item_type, fp_type> buckets[num_buckets];  // Array of buckets
    uint32_t curr_size = 0;
    static const uint32_t max_kicks = 500;

    // bit masks
    const uint32_t HASH_MASK = calc_hash_mask(calc_num_bits(num_buckets));
    const uint32_t BKT_MASK = calc_bucket_mask(calc_num_bits(num_buckets));
    const uint32_t INVERTED_BKT_MASK = ~BKT_MASK & (num_buckets - 1);

    // seeds for hash functions
    // const for debugging
    XXH32_hash_t _seed_fp;
    XXH32_hash_t _seed_idx;

    // testing variables
    uint32_t total_evictions;


 public:
    CuckooFilter() {
        // Ensure possible_buckets is a power of 2
        if ((poss_buckets & (poss_buckets - 1)) != 0) {
            throw std::invalid_argument("possible_buckets must be a power of 2");
        }
        _seed_fp = rand();
        _seed_idx = rand();
        total_evictions = 0;
    }

    // Check if the filter contains an item
    bool contains(item_type item) {
        fp_type fp = fingerprint(item);
        uint32_t idx = calc_bucket_index(index_hash(item), fp);
        for (uint32_t i = 0; i < poss_buckets; i++) {
            if (buckets[idx].contains(fp)) { return true; }
        }
        return false;
    }

    // Insert an item into the filter
    // Returns true if the item was inserted, false if the filter is full
    // (max_kicks exceeded)
    bool insert(item_type item) {
        // start our evictions counter
        total_evictions = 0;

        fp_type fp = fingerprint(item);
        uint32_t idx = index_hash(item);
        uint32_t indices[poss_buckets];
        for (uint32_t i = 0; i < poss_buckets; i++) {
            indices[i] = idx;
            // if (buckets[idx].contains(fp)) { return true; } // TODO: remove to support deletions
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
                total_evictions++;
                return true;
            }
            total_evictions++;
            // std::cout << total_evictions << " ";
        }
        return false;
    }

  // Remove an item from the filter
    bool remove(item_type item) {
        fp_type fp = fingerprint(item);
        for (uint32_t i = 0; i < poss_buckets; i++) {
            uint32_t idx = calc_bucket_index(index_hash(item), fp);
            if (buckets[idx].remove(fp)) {
                curr_size--;
                return true;
            }
        }
        return false;
    }

    fp_type fingerprint(item_type item) {
        XXH64_hash_t fp = XXH64(&item, sizeof(item), _seed_fp);
        return fp & ((1 << sizeof(fp_type)) - 1);
    }

    uint32_t index_hash(item_type item) {
        return XXH32(&item, sizeof(item), _seed_idx) & (num_buckets - 1);
    }

    uint32_t calc_bucket_index(uint32_t idx, fp_type fp) {
        uint32_t H = index_hash(fp);
        uint32_t i = 0;

        while ((H & HASH_MASK) == 0) { H = index_hash(fp + ++i); }

        uint32_t next_idx = (((H & BKT_MASK) + (idx & BKT_MASK)) & BKT_MASK) +
                            (((H & INVERTED_BKT_MASK) + (idx & INVERTED_BKT_MASK)) &
                                INVERTED_BKT_MASK);
        return next_idx;
    }

    uint32_t calc_hash_mask(int num_bkts_bits) {
        const uint32_t bkts_2_mask = 0b11111111111111111111111111111111;
        const uint32_t bkts_4_mask = 0b01010101010101010101010101010101;
        const uint32_t bkts_8_mask = 0b01001001001001001001001001001001;
        uint32_t bit_mask;

        // if num_buckets is not a power of poss_buckets
        // return bit mask with length that is a power of poss_buckets
        if (is_pow_of_poss_bkts(num_buckets)) {
            bit_mask = (1 << num_bkts_bits) - 1;
        }
        else {
            bit_mask = (1 << (num_bkts_bits - (calc_num_bits(poss_buckets)-1))) - 1;
        }

        // slice the 32 bit mask to the number of bits needed
        if (poss_buckets == 2) {
            return bkts_2_mask & bit_mask;
        } else if (poss_buckets == 4) {
            return bkts_4_mask & bit_mask;
        } else if (poss_buckets == 8) {
            return bkts_8_mask & bit_mask;
        } else {
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
        if (poss_buckets == 2) {
            return bkts_2_mask & bit_mask;
        } else if (poss_buckets == 4) {
            return bkts_4_mask & bit_mask;
        } else if (poss_buckets == 8) {
            return bkts_8_mask & bit_mask;
        } else {
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

    // Check if a number is a power of poss_buckets
    bool is_pow_of_poss_bkts(int num) { 
        if (num <= 0) { return false; } 
        double res = log2(num) / log2(poss_buckets);
        return res == (int)res;
    }

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
        // _seed_fp = rand();
        // _seed_idx = rand();
    }

    // Get the total number of evictions
    uint32_t get_num_evictions() {
        return total_evictions;
    }

};