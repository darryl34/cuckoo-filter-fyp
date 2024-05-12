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


template <int num_bkts, int poss_bkts, int bkt_size, typename fp_type, int fp_int>
struct create_cfs {
    static float calc_lf(CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, fp_type>& cf) {
        for (uint32_t it = 0; it < num_bkts * bkt_size; it++) {
            int res = cf.insert(it);
            if (!res) {
                break;
            }
        }
        return cf.load_factor();
    }
    
    // get avg of runs
    static float calc_avg_lf() {
        int runs = 10;
        float avg_lf = 0;
        static CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, fp_type> cf;
        for (int i = 0; i < runs; i++) {
            avg_lf += calc_lf(cf);
            cf.reset();
            cf.reset_seeds();
        }
        return avg_lf/runs;
    }

    static void instantiate(const std::string& filename) {
        std::cout << "Testing with " << num_bkts << " buckets, " 
                << poss_bkts << " possible buckets, " << bkt_size 
                << " bucket size, " << fp_int << "-bit fp" << std::endl;

        float avg_lf = calc_avg_lf();
        std::cout << "Average Load Factor: " << avg_lf << std::endl;
        std::string msg = std::to_string(num_bkts) + "," +
                          std::to_string(poss_bkts) + "," + 
                          std::to_string(bkt_size) + "," +
                          std::to_string(fp_int) + "," +
                          std::to_string(avg_lf);
        write_to_file(filename, msg);
    }
};


int main() {
    srand(time(NULL));
    const uint32_t min_num_bkts = 1 << 20;

    const std::string filename = "../results/test_fp.txt";
    std::ofstream file(filename);
    file << "num_buckets,possible_buckets,bucket_size,fp_size,load_factor" << std::endl;

    // min_num_buckets, possible_buckets, bucket_size, fp_size, fp_int
    create_cfs<min_num_bkts, 2, 1, uint8_t, 8>::instantiate(filename);
    create_cfs<min_num_bkts, 2, 1, uint16_t, 16>::instantiate(filename);
    create_cfs<min_num_bkts, 2, 1, uint32_t, 32>::instantiate(filename);
    create_cfs<min_num_bkts, 2, 1, uint64_t, 64>::instantiate(filename);

    create_cfs<min_num_bkts, 4, 1, uint8_t, 8>::instantiate(filename);
    create_cfs<min_num_bkts, 4, 1, uint16_t, 16>::instantiate(filename);
    create_cfs<min_num_bkts, 4, 1, uint32_t, 32>::instantiate(filename);
    create_cfs<min_num_bkts, 4, 1, uint64_t, 64>::instantiate(filename);

    create_cfs<min_num_bkts, 8, 1, uint8_t, 8>::instantiate(filename);
    create_cfs<min_num_bkts, 8, 1, uint16_t, 16>::instantiate(filename);
    create_cfs<min_num_bkts, 8, 1, uint32_t, 32>::instantiate(filename);
    create_cfs<min_num_bkts, 8, 1, uint64_t, 64>::instantiate(filename);

}