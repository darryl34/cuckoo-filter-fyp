#include <iostream>
#include <cstdint>
#include <time.h>
#include <cstdlib>
#include <fstream>
#include <string>

#include "cuckoo.h"

int main() {
    srand(time(NULL));

    const uint32_t bkt_size = 4;
    const uint32_t max_num_bkts = 1 << 12;
    const uint32_t poss_bkts = 4;
    static CuckooFilter<bkt_size, max_num_bkts, poss_bkts> cf;

    int runs = 10;
    uint32_t avg_evictions[max_num_bkts * bkt_size];
    memset(avg_evictions, 0, sizeof(avg_evictions));

    for (int i = 0; i < runs; i++) {
        cf.reset();
        uint32_t evictions[max_num_bkts * bkt_size];
        // A single run
        // insert as many elements as possible into the filter
        for (uint32_t it = 0; it < max_num_bkts * bkt_size; it++) {
            int res = cf.insert(it);
            evictions[it] = cf.get_num_evictions();
            // std::cout << "Index " << it << " Evictions: " << evictions[it] << std::endl;
            if (!res) {
                // fill the rest of the array with max evictions
                for (uint32_t j = it; j < max_num_bkts * bkt_size; j++) {
                    evictions[j] = 500; // TODO: change to max + 1?
                }
                // std::cout << cf.load_factor() << std::endl;
                break;
            }
        }
        for (uint32_t j = 0; j < max_num_bkts * bkt_size; j++) {
            avg_evictions[j] += evictions[j];
        }
    }
    // get average of runs
    for (uint32_t i = 0; i < max_num_bkts * bkt_size; i++) {
        avg_evictions[i] /= runs;
    }

    std::ofstream file;
    file.open("evictions.txt");
    std::cout << sizeof(avg_evictions) / sizeof(uint32_t) << std::endl;
    for (uint32_t i = 0; i < sizeof(avg_evictions) / sizeof(uint32_t); i++) {
        file << avg_evictions[i] << "\n";
    }
}