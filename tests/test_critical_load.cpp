#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>
#include <random>
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
    // insert until filter is full
    static int test_critical_load(CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, uint32_t>& cf) {
        uint32_t it = 0;
        for (it = 0; it < (num_bkts * bkt_size); it++) {
            int res = cf.insert(it);
            if (!res) {
                std::cout << cf.load_factor() << std::endl;
                break;
            }
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, it);
        uint32_t to_insert = 1000;
        int total_inserted = 0;
        int total_duration = 0;
        // TODO: change to while loop
        for (int i = 0; i < to_insert; i++) {
            // remove random items in the filter
            uint32_t item = dis(gen);
            if (cf.contains(item)) {
                if (!cf.remove(item)) {
                    std::cout << "Error removing item" << item << std::endl;
                    break;
                }
                // insert random items 
                auto start = std::chrono::high_resolution_clock::now();
                bool res = cf.insert(++it);
                if (!res) {
                    std::cout << "Error inserting item" << i << std::endl;
                    i--;
                }
                else {
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                    total_duration += duration.count();
                    std::cout<< "Duration: " << duration.count() << std::endl;
                    total_inserted++;
                }
            }
            else { 
                i--; 
                // std::cout << "Item not found" << item << std::endl;
            }
        }
        std::cout << "Total inserted: " << total_inserted << std::endl;
        std::cout << "Load factor: " << cf.load_factor() << std::endl;
        return total_duration/total_inserted;
    }

    static float avg_critical_load() {
        int runs = 1;
        float avg_duration = 0;
        for (int i = 0; i < runs; i++) {
            static CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, uint32_t> cf;
            cf.reset();
            avg_duration += test_critical_load(cf);
        }
        return avg_duration/runs;
    }

    static void instantiate(const std::string& filename) {
        std::cout << "Testing with " << num_bkts << " buckets, " << bkt_size << " bucket size, " << poss_bkts << " possible buckets" << std::endl;
        float avg_duration = avg_critical_load();
        std::cout << "Average critical load duration: " << avg_duration << std::endl;
        std::string msg = std::to_string(num_bkts) + "," +
                          std::to_string(bkt_size) + "," +
                          std::to_string(poss_bkts) + "," + 
                          std::to_string(avg_duration);
        write_to_file(filename, msg);
        create_cfs<(num_bkts << 1), poss_bkts, bkt_size>::instantiate(filename);
    }
};

template<int bkt_size, int poss_bkts>
// base case    
// num_buckets from any range to 1 << 21
    struct create_cfs <1 << 15, bkt_size, poss_bkts> {
    static void instantiate(const std::string& filename) {
        std::cout << "Done" << std::endl;
    }
};

int main() {
    srand(time(NULL));
    const uint32_t min_num_buckets = 1 << 14;

    const std::string filename = "../results/test_critical_load.txt";
    std::ofstream file(filename);
    file << "num_buckets,bucket_size,poss_buckets,avg_duration" << std::endl;

    // min_num_buckets, possible_buckets, bucket_size
    create_cfs<min_num_buckets, 2, 2>::instantiate(filename);

}