#include <iostream>
#include <cstdint>
#include <time.h>
#include <cstdlib>
#include <fstream>
#include <string>
#include <chrono>

#include "../src/cuckoo.h"

struct Result {
    float avg_query_speed_half;
    float avg_query_speed_critical_pos;
    float avg_query_speed_critical_neg;
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
    // Fill filter to 50% capacity
    static int fill_to_half(CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, uint16_t>& cf) {
        for (uint32_t it = 0; it < num_bkts * bkt_size / 2; it++) {
            int res = cf.insert(it);
            if (!res) { return false; }
        }
        return true;
    }

    // Fill filter to critical capacity without breaking
    static int fill_to_critical(CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, uint16_t>& cf) {
        for (uint32_t it = num_bkts * bkt_size / 2; it < num_bkts * bkt_size; it++) {
            int res = cf.insert(it);
            if (!res) { return false; }
            if (bkt_size == 2 && cf.load_factor() >= 0.85) { break; }
        }
        return true;
    }

    static float calc_query_speed_half(CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, uint16_t>& cf) {
        uint32_t it = 0;
        int res;
        auto start = std::chrono::high_resolution_clock::now();
        for (it = 0; it < num_bkts * bkt_size / 2; it++) {
            res = cf.contains(it);
            if (!res) { throw std::runtime_error("False negative"); }
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return (float)duration.count();
    }

    static float calc_avg_query_speed_half(CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, uint16_t>& cf) {
        int runs = 100;
        float avg_query_speed = 0;
        
        if (!fill_to_half(cf)) { throw std::runtime_error("Filter is full"); }
        for (int i = 0; i < runs; i++) {
            avg_query_speed += calc_query_speed_half(cf);
        }
        // std::cout << avg_query_speed/runs << std::endl;
        return avg_query_speed/runs;
    }
    
    static float calc_query_speed_critical(CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, uint16_t>& cf) {
        uint32_t it = 0;
        uint32_t total_inserted = cf.load_factor() * num_bkts * bkt_size;
        int res;

        auto start = std::chrono::high_resolution_clock::now();
        for (it = 0; it < total_inserted; it++) {
            res = cf.contains(it);
            if (!res) { throw std::runtime_error("False negative"); }
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return (float)duration.count();
    }

    static float calc_neg_query(CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, uint16_t>& cf) {
        uint32_t total_inserted = cf.load_factor() * num_bkts * bkt_size;
        int res;

        auto start = std::chrono::high_resolution_clock::now();
        for (uint32_t it = total_inserted; it < total_inserted * 2; it++) {
            cf.contains(it);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return (float)duration.count();
    }

    static Result calc_avg_query_speed_critical(CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, uint16_t>& cf) {
        int runs = 100;
        float avg_query_speed_pos = 0;
        float avg_query_speed_neg = 0;
        
        if (!fill_to_critical(cf)) { throw std::runtime_error("Filter is full"); }
        for (int i = 0; i < runs; i++) {
            avg_query_speed_pos += calc_query_speed_critical(cf);
            avg_query_speed_neg += calc_neg_query(cf);
        }
        Result res = {0, avg_query_speed_pos/runs, avg_query_speed_neg/runs};
        return res;
    }


    static Result init_filter() {
        static CuckooFilter<num_bkts, poss_bkts, bkt_size, uint32_t, uint16_t> cf;
        int runs = 10;
        float avg_query_speed_half = 0;
        float avg_query_speed_critical_pos = 0;
        float avg_query_speed_critical_neg = 0;
        for (int i = 0; i < runs; i++) {
            avg_query_speed_half += calc_avg_query_speed_half(cf);
            Result res = calc_avg_query_speed_critical(cf);
            avg_query_speed_critical_pos += res.avg_query_speed_critical_pos;
            avg_query_speed_critical_neg += res.avg_query_speed_critical_neg;
            cf.reset();
            cf.reset_seeds();
        }
        avg_query_speed_half /= runs;
        avg_query_speed_critical_pos /= runs;
        avg_query_speed_critical_neg /= runs;
        Result res = {avg_query_speed_half, avg_query_speed_critical_pos, avg_query_speed_critical_neg};
        return res;
    }

    static void instantiate(const std::string& filename) {
        std::cout << "Testing with " << num_bkts << " buckets, " << poss_bkts << " possible buckets, " << bkt_size << " bucket size" << std::endl;
        Result res = init_filter();
        std::cout << "Avg query speed at 50% capacity: " << res.avg_query_speed_half << "ms" << std::endl;
        std::cout << "Avg query speed at critical capacity: " << res.avg_query_speed_critical_pos << "ms" << std::endl;
        std::cout << "Avg query speed for negative queries: " << res.avg_query_speed_critical_neg << "ms" << std::endl;
        std::string msg = std::to_string(num_bkts) + "," +
                          std::to_string(poss_bkts) + "," + 
                          std::to_string(bkt_size) + "," +
                          std::to_string(res.avg_query_speed_half) + "," +
                          std::to_string(res.avg_query_speed_critical_pos) + "," +
                          std::to_string(res.avg_query_speed_critical_neg);
        write_to_file(filename, msg);
    }
};


int main() {
    srand(time(NULL));
    const uint32_t num_buckets = 1 << 19;

    const std::string filename = "../results/test_queries_in.txt";
    std::ofstream file(filename);
    file << "num_buckets,poss_buckets,bucket_size,avg_query_half,avg_query_critical_pos,avg_query_critical_neg" << std::endl;

    // min_num_buckets, possible_buckets, bucket_size
    create_cfs<num_buckets, 2, 2>::instantiate(filename);
    create_cfs<num_buckets, 4, 2>::instantiate(filename);
    // create_cfs<num_buckets, 8, 2>::instantiate(filename);
}