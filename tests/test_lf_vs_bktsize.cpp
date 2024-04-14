#include <iostream>
#include <cstdint>
#include <time.h>
#include <cstdlib>
#include <fstream>
#include <string>

#include "../src/cuckoo.h"

void write_to_file(const std::string& filename, const std::string& data) {
    std::ofstream file(filename, std::ios::app);
    if (file.is_open()) {
        file << data << "\n";
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

template <int num_bkts, int poss_bkts, int bkt_size>
struct create_cfs {
    // insert as many elements as possible into the filter
    static float calculate_load_factor(CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, uint32_t>& cf) {
        for (uint32_t it = 0; it < num_bkts * bkt_size; it++) {
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
            static CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, uint32_t> cf;
            cf.reset();
            avg_lf += calculate_load_factor(cf);
        }
        return avg_lf/runs;
    }
    
    static void instantiate(const std::string& filename) {
        std::cout << "Testing with " << num_bkts << " buckets, " << bkt_size << " bucket size, " << poss_bkts << " possible buckets" << std::endl;

        float avg_lf = calculate_average_load_factor();
        std::cout << "Average load factor: " << avg_lf << std::endl;
        std::string msg = generate_message(num_bkts, bkt_size, poss_bkts, avg_lf);
        write_to_file(filename, msg);
        create_cfs<(num_bkts << 1), poss_bkts, bkt_size>::instantiate(filename);
    }
};

template<int bkt_size, int poss_bkts>
// limit range of num_buckets
    struct create_cfs <1 << 21, poss_bkts, bkt_size> {
    static void instantiate(const std::string& filename) {
        std::cout << "Done" << std::endl;
    }
};

int main() {
    //static const std::vector<uint32_t> bkts = {1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576}; // 2^10 to 2^20
    srand(time(NULL));
    
    const uint32_t min_num_buckets = 1 << 12;
    
    const std::string filename = "../results/test_lf.txt";
    std::ofstream file(filename);
    file << "num_buckets,bucket_size,possible_buckets,load_factor" << std::endl;

    // min_num_buckets, possible_buckets, bucket_size, fp_size
    create_cfs<min_num_buckets, 2, 2>::instantiate(filename);
    // create_cfs<min_num_buckets, 4, 2, 32>::instantiate(filename);
    // create_cfs<min_num_buckets, 8, 2, 32>::instantiate(filename);

    // create_cfs<min_num_buckets, 2, 4, 32>::instantiate(filename);
    // create_cfs<min_num_buckets, 4, 4, 32>::instantiate(filename);
    // create_cfs<min_num_buckets, 8, 4, 32>::instantiate(filename);

    // create_cfs<min_num_buckets, 2, 8, 32>::instantiate(filename);
    // create_cfs<min_num_buckets, 4, 8, 32>::instantiate(filename);
    // create_cfs<min_num_buckets, 8, 8, 32>::instantiate(filename);

    // create_cfs< min_num_buckets, 2, 16, 32>::instantiate(filename);
    // create_cfs< min_num_buckets, 4, 16, 32>::instantiate(filename);
    // create_cfs< min_num_buckets, 8, 16, 32>::instantiate(filename);

}