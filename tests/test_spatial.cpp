#include <iostream>
#include <cstdint>
#include <fstream>
#include <chrono>

#include "../src/cuckoo.h"

int main() {
    srand(time(NULL));
    const std::string filename = "../results/test_spatial.txt";
    std::ofstream file(filename);
    file << "index1,index2,index3,index4\n";

    const int num_bkts = 1<<10;
    static CuckooFilter<num_bkts, 4, 1, uint32_t, uint16_t> cf;
    for (uint32_t i = 0; i < num_bkts; i++) {
        if (!cf.insert(i)) {
            std::cout << "Filter is full" << std::endl;
            break;
        }
        if (cf.load_factor() > 0.95) {break;}
    }
    int total_inserted = cf.load_factor() * (num_bkts);

    for (uint32_t i = 0; i < total_inserted; i++) {
        if (!cf.contains(i)) {
            std::cout << "Item " << i << " not found" << std::endl;
        }
        XXH64_hash_t hv = cf.generate_hash(i);
        uint16_t fp = cf.fingerprint(hv);
        uint32_t idx1 = cf.index_hash(hv);
        uint32_t idx2 = cf.calc_bucket_index(idx1, fp);
        uint32_t idx3 = cf.calc_bucket_index(idx2, fp);
        uint32_t idx4 = cf.calc_bucket_index(idx3, fp);
        file << idx1 << "," << idx2 << "," << idx3 << "," << idx4 << "\n";
    }

    return 0;
}