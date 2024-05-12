#include "gtest/gtest.h"
#include "../src/cuckoo.h"

class FilterTest : public ::testing::Test {
protected:
    const uint32_t num_buckets = 1 << 10;
    const uint32_t poss_buckets = 8;
    const uint32_t bucket_size = 2;
    
    // TODO: investigate why we cant define with vars
    CuckooFilter<1 << 10, 8, 2, uint32_t, uint16_t> cf;

    void SetUp() override {
        cf.reset();
    }
};

TEST_F(FilterTest, TestContains) {
    uint32_t item = 123;
    cf.insert(item);
    EXPECT_TRUE(cf.contains(item));
}

TEST_F(FilterTest, TestContainsNonexistent) {
    uint32_t item = 123;
    EXPECT_FALSE(cf.contains(item));
}

TEST_F(FilterTest, TestInsert) {
    uint32_t item = 123;
    EXPECT_TRUE(cf.insert(item));
    double expected_lf = 1.0 / (FilterTest::num_buckets * FilterTest::bucket_size);
    EXPECT_TRUE(cf.load_factor() == expected_lf);
}

TEST_F(FilterTest, TestInsertDuplicate) {
    uint32_t item = 123;
    cf.insert(item);
    EXPECT_TRUE(cf.insert(item));
    double expected_lf = 2.0 / (FilterTest::num_buckets * FilterTest::bucket_size);
    EXPECT_TRUE(cf.load_factor() == expected_lf);
}

TEST_F(FilterTest, TestRemove) {
    uint32_t item = 123;
    cf.insert(item);
    EXPECT_TRUE(cf.remove(item));
    EXPECT_FALSE(cf.contains(item));
    EXPECT_TRUE(cf.load_factor() == 0);
}

// technically we are not allowed to remove non-existent items
TEST_F(FilterTest, TestRemoveNonexistent) {
    uint32_t item = 123;
    EXPECT_FALSE(cf.remove(item));

}

TEST_F(FilterTest, TestRemoveDuplicate) {
    uint32_t item = 123;
    cf.insert(item);
    cf.insert(item);
    EXPECT_TRUE(cf.remove(item));
    EXPECT_TRUE(cf.contains(item));
    EXPECT_TRUE(cf.load_factor() == 1.0 / (FilterTest::num_buckets * FilterTest::bucket_size));
}


TEST_F(FilterTest, TestLoadFactor) {
    uint32_t to_insert = FilterTest::num_buckets * FilterTest::bucket_size / 2;
    for (uint32_t i = 0; i < to_insert; i++) {
        cf.insert(i);
    }
    EXPECT_TRUE(cf.load_factor() == 0.5);
    uint32_t rand_num = rand() % to_insert;
    // perform operations at 50% load factor
    ASSERT_TRUE(cf.contains(rand_num));
    ASSERT_TRUE(cf.remove(rand_num));
    ASSERT_FALSE(cf.contains(rand_num));
    EXPECT_TRUE(cf.load_factor() == 0.5 - 1.0 / (FilterTest::num_buckets * FilterTest::bucket_size));
}


TEST_F(FilterTest, TestLoadFactorEmpty) {
    EXPECT_TRUE(cf.load_factor() == 0);
}

TEST_F(FilterTest, TestCriticalLoad) {
    uint32_t i;
    int res;
    for (i = 0; i < FilterTest::num_buckets * FilterTest::bucket_size; i++) {
        res = cf.insert(i);
        if (!res) { break; }
    }
    if (res) { 
        // all items inserted
        EXPECT_TRUE(cf.load_factor() == 1); 
        ASSERT_FALSE(cf.insert(i));
    }
    // insert failed at some point
    else { EXPECT_TRUE(cf.load_factor() < 1); }
}

TEST_F(FilterTest, TestCalcIndex) {
    uint32_t item = rand();
    uint16_t f = cf.fingerprint(item);
    uint32_t idx = cf.index_hash(item);
    ASSERT_TRUE(idx <= FilterTest::num_buckets);
    for (uint32_t i = 0; i < FilterTest::poss_buckets; i++) {
        idx = cf.calc_bucket_index(idx, f);
        ASSERT_TRUE(idx <= FilterTest::num_buckets);
    }
    // check that we cycle back to the original index
    ASSERT_TRUE(idx == cf.index_hash(item));
}