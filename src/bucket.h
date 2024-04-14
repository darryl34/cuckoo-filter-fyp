#pragma once

#include <cstdlib>
#include <iostream>

#define XXH_INLINE_ALL
#include "xxhash.h"

template <int bucket_size, typename fp_type>
class Bucket {
private:
    fp_type items[bucket_size];  // Array of items
    uint32_t curr_size;           // Current number of items

public:
    Bucket() : curr_size(0) { memset(items, 0, bucket_size * sizeof(fp_type)); }

    // Insert an item into the bucket if there is space
    bool insert(fp_type item) {
        if (curr_size < bucket_size) {
            for (int i = 0; i < bucket_size; i++) {
                if (items[i] == 0) {
                    items[i] = item;
                    curr_size++;
                    return true;
                }
            }
        }
        return false;
    }

    // Remove an item from the bucket
    bool remove(fp_type item) {
        for (uint32_t i = 0; i < curr_size; i++) {
            if (items[i] == item) {
                items[i] = 0;
                curr_size--;
                return true;
            }
        }
        return false;
    }

    // Check if the bucket contains item
    bool contains(fp_type item) {
        for (uint32_t i = 0; i < curr_size; i++) {
            if (items[i] == item) { return true; }
        }
        return false;
    }

    // Randomly select an item to evict and swap it with the new item
    fp_type swap(fp_type item) {
        //uint32_t idx = XXH32(&counter, sizeof(counter), 0) % bucket_size;
        //std::cout << "Swap random idx: " << idx << std::endl;
        uint32_t idx = rand() & (bucket_size - 1);
        fp_type temp = items[idx];
        items[idx] = item;
        return temp;
    }

    bool is_full() { return curr_size == bucket_size; }

    uint32_t size() { return curr_size; }

    void clear() {
        curr_size = 0;
        memset(items, 0, bucket_size * sizeof(fp_type));
    }
};