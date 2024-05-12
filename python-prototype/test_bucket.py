import pytest
from cuckoo import Bucket

def bucket():
    return Bucket()

def test_init():
    b = bucket()
    assert b.size == 4
    assert b.items == []

def test_insert():
    b = bucket()
    assert b.insert(1)

def test_insert_full():
    b = bucket()
    for i in range(4):
        assert b.insert(i)
    assert not b.insert(4)

def test_contains():
    b = bucket()
    b.insert(1)
    assert b.contains(1)
    assert not b.contains(2)

def test_remove():
    b = bucket()
    b.insert(1)
    assert b.remove(1)
    assert not b.contains(1)

def test_remove_not_present():
    b = bucket()
    assert not b.remove(1)

def test_swap():
    b = bucket()
    b.insert(1)
    assert b.swap(2) == 1
    assert b.contains(2)
    assert not b.contains(1)

def test_is_full():
    b = bucket()
    assert not b.is_full()
    for i in range(4):
        b.insert(i)
    assert b.is_full()