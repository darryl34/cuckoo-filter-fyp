#include <iostream>
#include <cstdint>
#include <time.h>
#include <cstdlib>
#include <fstream>
#include <string>
#include <chrono>
#include <tuple>

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


template <int num_bkts, int poss_bkts, int bkt_size>
struct create_cfs {
    // insert as many elements as possible into the filter
    static float measure_insert_speed(CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, uint32_t>& cf) {
        uint32_t it = 0;
        auto start = std::chrono::high_resolution_clock::now();
        for (it = 0; it < num_bkts * bkt_size; it++) {
            int res = cf.insert(it);
            if (!res) {
                break;
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return it/(float)duration.count();
        
    }

    // get average of runs
    static float calc_min_insert_speed() {
        int runs = 10;
        float avg_throughput = 0;
        int duration_ms;
        for (int i = 0; i < runs; i++) {
            static CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, uint32_t> cf;
            cf.reset();
            // std::cout << "Run " << i << std::endl;
            avg_throughput += measure_insert_speed(cf);
        }
        return avg_throughput/runs;
    }
    
    static void instantiate(const std::string& filename) {
        std::cout << "Testing with " << num_bkts << " buckets, " << bkt_size << " bucket size, " << poss_bkts << " possible buckets" << std::endl;

        float avg_throughput = calc_min_insert_speed();
        std::cout << "Average Throughput: " << avg_throughput << std::endl;
        std::string msg = std::to_string(num_bkts) + "," +
                          std::to_string(bkt_size) + "," +
                          std::to_string(poss_bkts) + "," + 
                          std::to_string(avg_throughput);
        write_to_file(filename, msg);
        create_cfs<(num_bkts << 1), poss_bkts, bkt_size>::instantiate(filename);
    }
};

template<int poss_bkts, int bkt_size>
// base case    
// num_buckets from any range to 1 << 21
    struct create_cfs <1 << 15, poss_bkts, bkt_size> {
    static void instantiate(const std::string& filename) {
        std::cout << "Done" << std::endl;
    }
};


int main() {
    srand(0);
    const uint32_t min_num_buckets = 1 << 14;

    const std::string filename = "../results/test_insert_speed.txt";
    std::ofstream file(filename);
    file << "num_buckets,bucket_size,poss_buckets,avg_throughput" << std::endl;

    // bucket_size, min_num_buckets, possible_buckets
    create_cfs<min_num_buckets, 2, 2>::instantiate(filename);
    // create_cfs<2, min_num_buckets, 4>::instantiate(filename);
    // create_cfs<2, min_num_buckets, 8>::instantiate(filename);

    // create_cfs<4, min_num_buckets, 2>::instantiate(filename);
    // create_cfs<4, min_num_buckets, 4>::instantiate(filename);
    // create_cfs<4, min_num_buckets, 8>::instantiate(filename);

    // create_cfs<8, min_num_buckets, 2>::instantiate(filename);
    // create_cfs<8, min_num_buckets, 4>::instantiate(filename);
    // create_cfs<8, min_num_buckets, 8>::instantiate(filename);
}
