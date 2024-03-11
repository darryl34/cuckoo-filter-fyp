#include <iostream>
#include <cstdint>
#include <time.h>
#include <cstdlib>
#include <fstream>
#include <string>

#include "cuckoo.h"

void write_to_file(const std::string& filename, const std::string& data) {
    std::ofstream file(filename, std::ios::app);
    if (file.is_open()) {
        file << data << std::endl;
        file.close();
    } else {
        std::cout << "Unable to open file" << std::endl;
    }
}

template <int bkt_size, int max_num_bkts, int poss_bkts>
struct create_cfs {
    
    static void instantiate(const std::string& filename) {
        std::cout << "Testing with " << max_num_bkts << " buckets, " << bkt_size << " bucket size, " << poss_bkts << " possible buckets" << std::endl;

        float avg_lf = 0;
        for (int i = 0; i < 10; i++) {
            static CuckooFilter<bkt_size, max_num_bkts, poss_bkts> cf;
            cf.reset();
            for (uint32_t it = 0; it < max_num_bkts * bkt_size; it++) {
                int res = cf.insert(it);
                if (!res) {
                    //std::cout << "Failed to insert " << i << std::endl;
                    break;
                }
            }
            avg_lf += cf.load_factor();
        }
        avg_lf /= 10;
        std::cout << "Average load factor: " << avg_lf << std::endl;
        std::string msg = std::to_string(max_num_bkts) + "," +
                        std::to_string(bkt_size) + "," +
                          std::to_string(poss_bkts) + "," + std::to_string(avg_lf);
        write_to_file(filename, msg);
        create_cfs<bkt_size, (max_num_bkts << 1), poss_bkts>::instantiate(filename);
    }
};

template<int bkt_size, int poss_bkts>
// base case    
// num_buckets from any range to 1 << 21
    struct create_cfs <bkt_size, 1 << 21, poss_bkts> {
    static void instantiate(const std::string& filename) {
        std::cout << "Done" << std::endl;
    }
};

int main() {
    //static const std::vector<uint32_t> bkts = {1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576}; // 2^10 to 2^20
    srand(time(NULL));
    
    const uint32_t min_num_buckets = 1 << 10;
    const uint32_t bucket_size = 2;
    const uint32_t possible_buckets = 4;
    
    const std::string filename = "results.txt";
    std::ofstream file(filename);
    file << "num_buckets,bucket_size,possible_buckets,load_factor" << std::endl;

    // bucket_size, min_num_buckets, possible_buckets
    create_cfs<2, min_num_buckets, 2>::instantiate(filename);
    create_cfs<2, min_num_buckets, 4>::instantiate(filename);
    create_cfs<2, min_num_buckets, 8>::instantiate(filename);

    create_cfs<4, min_num_buckets, 2>::instantiate(filename);
    create_cfs<4, min_num_buckets, 4>::instantiate(filename);
    create_cfs<4, min_num_buckets, 8>::instantiate(filename);

    create_cfs<8, min_num_buckets, 2>::instantiate(filename);
    create_cfs<8, min_num_buckets, 4>::instantiate(filename);
    create_cfs<8, min_num_buckets, 8>::instantiate(filename);

    create_cfs<16, min_num_buckets, 2>::instantiate(filename);
    create_cfs<16, min_num_buckets, 4>::instantiate(filename);
    create_cfs<16, min_num_buckets, 8>::instantiate(filename);

}