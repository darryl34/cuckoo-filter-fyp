#include "gtest/gtest.h"
#include "../src/bucket.h"

TEST(BucketTest, TestInsert) {
    // Setup
    Bucket<2, uint32_t> bucket;

    // Exercise
    bool insertResult = bucket.insert(10);

    // Verify
    ASSERT_TRUE(insertResult);
}

TEST(BucketTest, TestContainsAfterInsert) {
    // Setup
    Bucket<2, uint32_t> bucket;

    // Exercise
    bucket.insert(10);

    // Verify
    ASSERT_TRUE(bucket.contains(10));
}

TEST(BucketTest, TestContainsAfterInsertMultiple) {
    // Setup
    Bucket<2, uint32_t> bucket;

    // Exercise
    bucket.insert(10);
    bucket.insert(20);

    // Verify
    ASSERT_TRUE(bucket.contains(10));
    ASSERT_TRUE(bucket.contains(20));
}

TEST(BucketTest, TestContainsAfterInsertMultipleFalse) {
    // Setup
    Bucket<2, uint32_t> bucket;

    // Exercise
    bucket.insert(10);
    bucket.insert(20);

    // Verify
    ASSERT_FALSE(bucket.contains(30));
}

TEST(BucketTest, TestInsertFull) {
    // Setup
    Bucket<2, uint32_t> bucket;

    // Exercise
    bucket.insert(10);
    bucket.insert(20);
    bool insertResult = bucket.insert(30);

    // Verify
    ASSERT_FALSE(insertResult);
}

TEST(BucketTest, TestRemove) {
    // Setup
    Bucket<2, uint32_t> bucket;

    // Exercise
    bucket.insert(10);
    bool removeResult = bucket.remove(10);

    // Verify
    ASSERT_TRUE(removeResult);
}

TEST(BucketTest, TestRemoveAfterInsertMultiple) {
    // Setup
    Bucket<2, uint32_t> bucket;

    // Exercise
    bucket.insert(10);
    bucket.insert(20);
    bool removeResult = bucket.remove(10);

    // Verify
    ASSERT_TRUE(removeResult);
}

TEST(BucketTest, TestRemoveAfterInsertMultipleFalse) {
    // Setup
    Bucket<2, uint32_t> bucket;

    // Exercise
    bucket.insert(10);
    bucket.insert(20);
    bool removeResult = bucket.remove(30);

    // Verify
    ASSERT_FALSE(removeResult);
}

TEST(BucketTest, TestContainsAfterRemove) {
    // Setup
    Bucket<2, uint32_t> bucket;

    // Exercise
    bucket.insert(10);
    bucket.remove(10);

    // Verify
    ASSERT_FALSE(bucket.contains(10));
}

TEST(BucketTest, TestContainsAfterRemoveMultiple) {
    // Setup
    Bucket<2, uint32_t> bucket;

    // Exercise
    bucket.insert(10);
    bucket.insert(20);
    bucket.remove(10);

    // Verify
    ASSERT_FALSE(bucket.contains(10));
    ASSERT_TRUE(bucket.contains(20));
}

TEST(BucketTest, TestSwap) {
    // Setup
    Bucket<2, uint32_t> bucket;

    // Exercise
    bucket.insert(10);
    bucket.insert(20);
    uint32_t swapResult = bucket.swap(30);

    // Verify
    ASSERT_TRUE(swapResult == 10 || swapResult == 20);
    ASSERT_TRUE(bucket.contains(30));
}

TEST(BucketTest, TestIsFull) {
    // Setup
    Bucket<2, uint32_t> bucket;

    // Exercise
    bucket.insert(10);
    bucket.insert(20);

    // Verify
    ASSERT_TRUE(bucket.is_full());
}