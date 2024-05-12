import hashlib
import random
import math

from xxhash import xxh32

random.seed(0)

class Bucket:
    def __init__(self, size=4):
        self.size = size
        self.items = []

    def insert(self, item):
        if len(self.items) < self.size:
            self.items.append(item)
            return True
        else:
            return False
        
    def contains(self, item):
        return item in self.items
    
    def remove(self, item):
        if item in self.items:
            self.items.remove(item)
            return True
        else:
            return False
        
    def swap(self, item, counter):
        '''
        Swap a item with a randomly chosen item in the bucket.
        
        The given item is inserted into the bucket, 
        and the swapped out item is returned
        '''
        index = xxh32(counter.to_bytes(4, byteorder='little', signed=False), seed=0).intdigest() % self.size
        # print(f"Swap random idx: {index}")
        # index = random.choice(range(len(self.items)))
        item, self.items[index] = self.items[index], item
        return item

    def is_full(self):
        return len(self.items) >= self.size

class CuckooFilter:
    def __init__(self, num_buckets, bucket_size, possible_buckets, fingerprint_size=4, max_kicks=500):
        '''
        Initialize Cuckoo filter parameters.

        num_buckets : size of the filter
            Defines how many buckets the filter contains.
        bucket_size : nr. of entries a bucket can hold
            A bucket can hold multiple entries.
            Default size is 4, which closely approaches the best size for FPP between 0.00001 and 0.002 (see Fan et al.).
            If your targeted FPP is greater than 0.002, a bucket size of 2 is more space efficient.
        fingerprint_size: size of the fingerprint in bytes
            A larger fingerprint size results in a lower FPP.
        max_kicks : nr. of times entries are kicked around before deciding the filter is full
            Defaults to 500. This is an arbitrary number also used by Fan et al. and seems reasonable enough.
        '''
        self.possible_buckets = possible_buckets

        # num_buckets must be a power of 4
        if num_buckets % possible_buckets != 0:
            raise Exception('num_buckets must be a power of possible_buckets')
        else:
            self.num_buckets = num_buckets
        self.bucket_size = bucket_size
        self.fingerprint_size = fingerprint_size
        self.max_kicks = max_kicks
        self.buckets = [Bucket(bucket_size) for _ in range(self.num_buckets)]
        self.size = 0
        self.check_hash = self.calc_hash_mask(self.num_buckets.bit_length()-1)
        self.mask = self.calc_bucket_mask(self.num_buckets.bit_length()-1)
        self.inverted = ~self.mask & (self.num_buckets - 1)
        self.counter = 0


    def insert(self, item):
        if self.contains(item):
            return True
        f = self.fingerprint(item)
        indices = self.get_indices(item, f)
        # print(f"Indices: {indices}")

        for i in indices:
            if self.buckets[i].insert(f):
                self.size += 1
                # print(f"Bucket 15 size: {len(self.buckets[15].items)}")
                return True
        
        self.counter += 1
        rand = xxh32(self.counter.to_bytes(4, byteorder='little', signed=False), seed=0).intdigest() % self.possible_buckets
        # print(f"Random bucket: {rand}")
        next_idx = indices[rand]
        for _ in range(self.max_kicks):
            self.counter += 1
            f = self.buckets[i].swap(f, self.counter)
            next_idx = self.calc_bucket_index(next_idx, f)
            for j in range(self.possible_buckets):
                if self.buckets[next_idx].insert(f):
                    self.size += 1
                    return True
                next_idx = self.calc_bucket_index(next_idx, f)
        
        raise Exception('Filter is full')
    
    def contains(self, item):
        f = self.fingerprint(item)
        indices = self.get_indices(item, f)
        return any([self.buckets[i].contains(f) for i in indices])
    
    def remove(self, item):
        f = self.fingerprint(item)
        indices = self.get_indices(item, f)
        if any([self.buckets[i].remove(f) for i in indices]):
            self.size -= 1
            return True
        return False

    def fingerprint(self, item):
        item_bytes = item.to_bytes(4, byteorder='little', signed=False)
        return xxh32(item_bytes, seed=42).intdigest()
    
    def index_hash(self, item):
        item_bytes = item.to_bytes(4, byteorder='little', signed=False)
        return xxh32(item_bytes, seed=0).intdigest() % self.num_buckets
    
    def get_indices(self, item, fingerprint):
        index = self.index_hash(item)
        indices = [index]
        for _ in range(self.possible_buckets-1):
            index = self.calc_bucket_index(index, fingerprint)
            indices.append(index)

        return indices
    
    def calc_bucket_index(self, index, fingerprint):
        H = self.index_hash(fingerprint)
        i = 0
        while (H & self.check_hash) == 0:
            i += 1
            H = self.index_hash(fingerprint + i)
        next_index = (((H & self.mask) + (index & self.mask)) & self.mask) \
                       + (((H & self.inverted) + (index & self.inverted)) & self.inverted)
        return next_index


    def calc_hash_mask(self, num_bkts_bits):
        bkts_2_mask = 0b11111111111111111111111111111111
        bkts_4_mask = 0b01010101010101010101010101010101
        bkts_8_mask = 0b01001001001001001001001001001001
        res = math.log(self.num_buckets)/math.log(self.possible_buckets)
        if res.is_integer():
            bit_mask = (1 << num_bkts_bits) - 1
        else:
            bit_mask = (1 << (num_bkts_bits-(self.possible_buckets.bit_length()-1))) - 1

        if self.possible_buckets == 2:
            return bkts_2_mask & bit_mask
        elif self.possible_buckets == 4:
            return bkts_4_mask & bit_mask
        elif self.possible_buckets == 8:
            return bkts_8_mask & bit_mask
        else:
            raise Exception('Possible buckets must be 2, 4 or 8')
    
    def calc_bucket_mask(self, num_bkts_bits):
        bkts_2_mask = 0b01010101010101010101010101010101
        bkts_4_mask = 0b00110011001100110011001100110011
        bkts_8_mask = 0b11000111000111000111000111000111
        bit_mask = (1 << num_bkts_bits) - 1

        if self.possible_buckets == 2:
            return bkts_2_mask & bit_mask
        elif self.possible_buckets == 4:
            return bkts_4_mask & bit_mask
        elif self.possible_buckets == 8:
            return bkts_8_mask & bit_mask
        else:
            raise Exception('Possible buckets must be 2, 4 or 8')
    
    def load_factor(self):
        return self.size / (self.num_buckets * self.bucket_size)
    

class CuckooFilterTwoBuckets(CuckooFilter):
    def __init__(self, num_buckets, bucket_size, fingerprint_size=4, max_kicks=500):
        super().__init__(num_buckets, bucket_size, fingerprint_size, max_kicks)

        self.possible_buckets = 2

    def get_indices(self, item, fingerprint):
        i1 = self.index_hash(item)
        i2 = (i1 ^ self.index_hash(fingerprint)) % self.num_buckets
        return [i1, i2]

    def insert(self, item):
        if self.contains(item):
            return True
        f = self.fingerprint(item)
        indices = self.get_indices(item, f)

        for i in indices:
            if self.buckets[i].insert(f):
                self.size += 1
                return True
        
        i = random.choice(indices)
        for _ in range(self.max_kicks):
            f = self.buckets[i].swap(f)
            i = (i ^ self.index_hash(f)) % self.num_buckets
            if self.buckets[i].insert(f):
                self.size += 1
                return True
            
        raise Exception('Filter is full')
    

    def __init__(self, num_buckets, bucket_size, fingerprint_size=4, max_kicks=500):
        super().__init__(num_buckets, bucket_size, fingerprint_size, max_kicks)

        self.possible_buckets = 8

        # num_buckets must be a power of 8
        if num_buckets % self.possible_buckets != 0:
            raise Exception('num_buckets must be a power of 4')
        else:
            self.num_buckets = num_buckets

    def calc_masks(self):
        '''
        Calculate the masks used by calc_bucket_index.
        '''
        num_bits = int(math.log2(self.num_buckets))
        check_hash = 0
        mask = 0

        for i in range(num_bits//3):
            check_hash = (check_hash << 3) | 0b1

        for i in range(num_bits//3):
            if i % 2 == 1:
                mask = (mask << 3) | 0b111
            else:
                mask = mask << 3

        inverted = ~mask & (2**num_bits - 1)

        return check_hash, mask, inverted