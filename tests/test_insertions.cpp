#include <iostream>
#include <cstdint>
#include <time.h>
#include <cstdlib>
#include <fstream>
#include <string>
#include <chrono>
#include <tuple>

#include "../src/cuckoo.h"

struct Result {
    float avg_throughput;
    float avg_lf;
};

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
    // return the number of elements inserted before the filter is full
    static float calc_insert_speed(CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, uint16_t>& cf) {
        uint32_t it = 0;
        auto start = std::chrono::high_resolution_clock::now();
        for (it = 0; it < num_bkts * bkt_size; it++) {
            int res = cf.insert(it);
            if (!res) {  break; }
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return it/(float)duration.count();
    }

    // get average of runs
    static Result calc_avg_insert_speed() {
        int runs = 50;
        float avg_throughput = 0;
        float avg_lf = 0;
        // int duration_us;
        static CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, uint16_t> cf;
        for (int i = 0; i < runs; i++) {
            // calculate throughput
            avg_throughput += calc_insert_speed(cf);
            // calc load factor
            avg_lf += cf.load_factor();
            cf.reset();
            // reset seeds every 10 runs
            if ((i+1) % 10 == 0) { cf.reset_seeds(); }
        }
        Result res = {avg_throughput/runs, avg_lf/runs};
        return res;
    }
    
    static void instantiate(const std::string& filename) {
        std::cout << "Testing with " << num_bkts << " buckets, " << poss_bkts << " possible buckets, " << bkt_size << " bucket size" << std::endl;

        Result res = calc_avg_insert_speed();
        std::cout << "Average Throughput: " << res.avg_throughput << std::endl;
        std::string msg = std::to_string(num_bkts) + "," +
                          std::to_string(poss_bkts) + "," + 
                          std::to_string(bkt_size) + "," +
                          std::to_string(res.avg_throughput) + "," +
                          std::to_string(res.avg_lf);
        write_to_file(filename, msg);
        create_cfs<(num_bkts << 1), poss_bkts, bkt_size>::instantiate(filename);
    }
};

template<int poss_bkts, int bkt_size>
// base case    
// num_buckets from any range to 1 << 21
    struct create_cfs <1 << 22, poss_bkts, bkt_size> {
    static void instantiate(const std::string& filename) {
        std::cout << "Done" << std::endl;
    }
};


int main() {
    srand(time(NULL));
    const uint32_t min_num_buckets = 1 << 16;

    const std::string filename = "../results/test_insert_speed.txt";
    std::ofstream file(filename);
    file << "num_buckets,poss_buckets,bucket_size,avg_throughput,avg_lf" << std::endl;

    // min_num_buckets, possible_buckets, bucket_size
    create_cfs<min_num_buckets, 2, 2>::instantiate(filename);
    create_cfs<min_num_buckets, 2, 4>::instantiate(filename);

    create_cfs<min_num_buckets, 4, 2>::instantiate(filename);
    create_cfs<min_num_buckets, 4, 4>::instantiate(filename);

    create_cfs<min_num_buckets, 8, 2>::instantiate(filename);
    create_cfs<min_num_buckets, 8, 4>::instantiate(filename);
}
