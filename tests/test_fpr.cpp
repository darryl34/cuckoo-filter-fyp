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
    static float calc_fpr(CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, fp_type>& cf) {
        uint32_t num_inserted = 0;
        uint32_t filter_size = num_bkts * bkt_size;
        for (uint32_t i = 0; i < filter_size; i++, num_inserted++) {
            if (!cf.insert(i)) { break; }
        }

        // Check non-existing items, a few false positives expected
        uint32_t total_queries = 0;
        uint32_t false_queries = 0;
        for (uint32_t i = filter_size; i < 2 * filter_size; i++) {
            if (cf.contains(i)) {
                false_queries++;
            }
            total_queries++;
        }
        return false_queries/(float)total_queries;
    }
    
    // get avg of runs
    static float calc_avg_fpr() {
        int runs = 10;
        float avg_fpr = 0;
        static CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, fp_type> cf;
        for (int i = 0; i < runs; i++) {
            avg_fpr += calc_fpr(cf);
            cf.reset();
            cf.reset_seeds();
        }
        return avg_fpr/runs;
    }

    static void instantiate(const std::string& filename) {
        std::cout << "Testing with " << num_bkts << " buckets, " 
                << poss_bkts << " possible buckets, " << bkt_size 
                << " bucket size, " << fp_int << "-bit fp" << std::endl;

        float avg_fpr = calc_avg_fpr();
        std::cout << "Average FPR: " << avg_fpr << std::endl;
        std::string msg = std::to_string(num_bkts) + "," +
                          std::to_string(poss_bkts) + "," + 
                          std::to_string(bkt_size) + "," +
                          std::to_string(fp_int) + "," +
                          std::to_string(avg_fpr);
        write_to_file(filename, msg);
        create_cfs<(num_bkts << 1), poss_bkts, bkt_size, fp_type, fp_int>::instantiate(filename);
    }
};

template<int bkt_size, int poss_bkts, typename fp_type, int fp_int>
// limit range of num_buckets
    struct create_cfs <1 << 20, poss_bkts, bkt_size, fp_type, fp_int> {
    static void instantiate(const std::string& filename) {
        std::cout << "Done" << std::endl;
    }
};

int main() {
    srand(time(NULL));
    const uint32_t min_num_bkts = 1 << 15;

    const std::string filename = "../results/test_fpr.txt";
    std::ofstream file(filename);
    file << "num_buckets,poss_buckets,bucket_size,fp_size,fpr" << std::endl;

    // min_num_buckets, possible_buckets, bucket_size, fp_size, fp_int
    create_cfs<min_num_bkts, 2, 2, uint8_t, 8>::instantiate(filename);
    create_cfs<min_num_bkts, 2, 2, uint16_t, 16>::instantiate(filename);
    create_cfs<min_num_bkts, 2, 2, uint32_t, 32>::instantiate(filename);
    // create_cfs<min_num_bkts, 2, 2, uint64_t, 64>::instantiate(filename);

    create_cfs<min_num_bkts, 4, 2, uint8_t, 8>::instantiate(filename);
    create_cfs<min_num_bkts, 4, 2, uint16_t, 16>::instantiate(filename);
    create_cfs<min_num_bkts, 4, 2, uint32_t, 32>::instantiate(filename);
    // create_cfs<min_num_bkts, 4, 2, uint64_t, 64>::instantiate(filename);

    create_cfs<min_num_bkts, 8, 2, uint8_t, 8>::instantiate(filename);
    create_cfs<min_num_bkts, 8, 2, uint16_t, 16>::instantiate(filename);
    create_cfs<min_num_bkts, 8, 2, uint32_t, 32>::instantiate(filename);
    // create_cfs<min_num_bkts, 8, 2, uint64_t, 64>::instantiate(filename);

}