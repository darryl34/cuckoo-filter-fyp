import pytest
from cuckoo import CuckooFilterTwoBuckets

possible_buckets = 2    # possible buckets for each item
@pytest.fixture
def cf():
    return CuckooFilterTwoBuckets(2**15, 4, 4)

def test_insert(cf):
    assert cf.insert("hello")
    assert cf.size == 1

def test_insert_full(cf):
    for i in range(cf.bucket_size * cf.possible_buckets):
        cf.insert(str(i))

    assert cf.size == cf.bucket_size * cf.possible_buckets
    
def test_insert_over_capacity(cf):
    with pytest.raises(Exception) as e:
        for i in range((cf.num_buckets * cf.bucket_size) + 1):
            cf.insert(str(i))
    assert str(e.value) == "Filter is full"
    # assert cf.load_factor() > 0.9

def test_contains(cf):
    cf.insert("hello")
    assert cf.contains("hello")
    assert not cf.contains("goodbye")

def test_delete(cf):
    cf.insert("hello")
    assert cf.contains("hello")
    assert cf.remove("hello")
    assert not cf.contains("hello")
    assert not cf.remove("hello")

def test_delete_not_present(cf):
    assert not cf.remove("hello")

def test_load_factor_empty(cf):
    assert cf.load_factor() == 0
    
def test_load_factor_not_empty(cf):
    cf.insert("hello")
    assert cf.load_factor() == (1 / (cf.num_buckets * cf.bucket_size))