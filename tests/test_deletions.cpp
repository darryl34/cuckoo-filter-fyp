#include <iostream>
#include <cstdint>
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
    // insert until critical load
    static int fill_to_critical(CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, uint16_t>& cf) {
        for (uint32_t it = 0; it < num_bkts * bkt_size; it++) {
            int res = cf.insert(it);
            if (!res) { return false; }
            if (bkt_size == 2 && cf.load_factor() >= 0.85) { break; }
        }
        return true;
    }

    static float calc_delete_speed(CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, uint16_t>& cf) {
        uint32_t it = 0;
        int res;
        uint32_t total_items = cf.load_factor() * num_bkts * bkt_size;
        auto start = std::chrono::high_resolution_clock::now();
        for (it = 0; it < total_items; it++) {
            if (!cf.remove(it)) {
                throw std::runtime_error("Deletion error"); }
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        if (cf.load_factor() != 0) { throw std::runtime_error("Filter not empty");}
        return (float)duration.count();
    }

    static void instantiate(const std::string& filename) {
        std::cout << "Testing with " << num_bkts << " buckets, " << poss_bkts << " possible buckets, " << bkt_size << " bucket size" << std::endl;
        static CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, uint16_t> cf;
        int runs = 1000;
        float avg_delete = 0;
        for (int i = 0; i < runs; i++) {
            if (!fill_to_critical(cf)) { throw std::runtime_error("Failed to fill filter"); }
            avg_delete += calc_delete_speed(cf);
            cf.reset();
            if (i+1 % 10 == 0) { cf.reset_seeds(); }
        }
        avg_delete /= runs;
        std::cout << "Average deletion time: " << avg_delete << "ms" << std::endl;
        std::string msg = std::to_string(num_bkts) + "," +
                        std::to_string(poss_bkts) + "," + 
                        std::to_string(bkt_size) + "," +
                        std::to_string(avg_delete);
        write_to_file(filename, msg);
    }
};


int main() {
    srand(time(NULL));
    const uint32_t num_buckets = 1 << 19;

    const std::string filename = "../results/test_critical_load.txt";
    std::ofstream file(filename);
    file << "num_buckets,poss_buckets,bucket_size,avg_delete_time" << std::endl;

    // min_num_buckets, possible_buckets, bucket_size
    create_cfs<num_buckets, 2, 2>::instantiate(filename);
    create_cfs<num_buckets, 4, 2>::instantiate(filename);

    // create_cfs<min_num_buckets, 2, 4>::instantiate(filename);
    // create_cfs<min_num_buckets, 4, 4>::instantiate(filename);

}