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

std::string generate_message(int max_num_bkts, int bkt_size, int poss_bkts, float avg_lf) {
    return std::to_string(max_num_bkts) + "," +
           std::to_string(bkt_size) + "," +
           std::to_string(poss_bkts) + "," + std::to_string(avg_lf);
}

template <int bkt_size, int max_num_bkts, int poss_bkts>
struct create_cfs {
    // insert as many elements as possible into the filter
    static float calculate_load_factor(CuckooFilter<bkt_size, max_num_bkts, poss_bkts>& cf) {
        for (uint32_t it = 0; it < max_num_bkts * bkt_size; it++) {
            int res = cf.insert(it);
            if (!res) {
                break;
            }
        }
        return cf.load_factor();
    }

    // get average of runs
    static float calculate_average_load_factor() {
        int runs = 10;
        float avg_lf = 0;
        for (int i = 0; i < runs; i++) {
            static CuckooFilter<bkt_size, max_num_bkts, poss_bkts> cf;
            cf.reset();
            avg_lf += calculate_load_factor(cf);
        }
        return avg_lf / runs;
    }
    
    static void instantiate(const std::string& filename) {
        std::cout << "Testing with " << max_num_bkts << " buckets, " << bkt_size << " bucket size, " << poss_bkts << " possible buckets" << std::endl;

        float avg_lf = calculate_average_load_factor();
        std::cout << "Average load factor: " << avg_lf << std::endl;
        std::string msg = generate_message(max_num_bkts, bkt_size, poss_bkts, avg_lf);
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