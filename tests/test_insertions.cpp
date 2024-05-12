#include <iostream>
#include <cstdint>
#include <time.h>
#include <cstdlib>
#include <fstream>
#include <string>
#include <chrono>

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
    // return the number of elements inserted before the filter is full
    static void calc_insert_throughput(CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, uint16_t>& cf, const std::string& filename) {
        uint32_t i = 0;
        uint32_t batch_size = num_bkts * bkt_size * 0.02;
        bool is_full = false;
        while (i < num_bkts * bkt_size && !is_full) {
            uint32_t it = 0;
            auto start = std::chrono::high_resolution_clock::now();
            for (it = 0; it < batch_size; it++) {
                if (!cf.insert(i)) {
                    is_full = true;
                    break;
                }
                i++;
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            float res = it / (float)duration.count();
            write_to_file(filename, std::to_string(cf.load_factor()) + "," + std::to_string(res));
        }
        write_to_file(filename, "Done");
    }

    // get average of runs
    static void calc_avg_insert_throughput(const std::string& filename) {
        int runs = 1000;
        // std::vector<float> avg_throughput;
        static CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, uint16_t> cf;
        for (int i = 0; i < runs; i++) {
            // calculate throughput
            calc_insert_throughput(cf, filename);
            cf.reset();
            // reset seeds every 100 runs
            if ((i+1) % 100 == 0) { cf.reset_seeds(); }
        }
    }
    
    static void instantiate(const std::string& filename) {
        std::cout << "Testing with " << num_bkts << " buckets, " << poss_bkts << " possible buckets, " << bkt_size << " bucket size" << std::endl;
        
        std::string msg = std::to_string(num_bkts) + "," +
                          std::to_string(poss_bkts) + "," + 
                          std::to_string(bkt_size);
        write_to_file(filename, msg);
        calc_avg_insert_throughput(filename);
    }
};


int main() {
    srand(0);
    const uint32_t num_buckets = 1 << 19;

    const std::string filename_2d = "../results/test_insert_speed_2d.txt";
    std::ofstream file(filename_2d);

    create_cfs<num_buckets, 2, 2>::instantiate(filename_2d);
    
    const std::string filename_4d = "../results/test_insert_speed_4d.txt";
    std::ofstream file_4d(filename_4d);
    create_cfs<num_buckets, 4, 2>::instantiate(filename_4d);

    const std::string filename_8d = "../results/test_insert_speed_8d.txt";
    std::ofstream file_8d(filename_8d);
    create_cfs<num_buckets, 8, 2>::instantiate(filename_8d);
}
