# Given a words.txt file containing a newline-delimited list of dictionary
# words, please implement the Anagrams class so that the get_anagrams() method
# returns all anagrams from words.txt for a given word.
#
# Bonus requirements:
#   - Optimise the code for fast retrieval
#   - Write more tests
#   - Thread safe implementation

import unittest
from collections import defaultdict
from multiprocessing.pool import ThreadPool
from contextlib import closing


def encode(word):
    return ''.join(sorted(word))


class Anagrams:

    def __init__(self):
        self.data = defaultdict(list)
        with open('words.txt') as _file:
            for word in _file.read().splitlines():
                key = encode(word)
                self.data[key].append(word)

    def get_anagrams(self, word):
        """
        No threading.Lock() is needed since we have only readers, writer is in __init__ before threads are created.
        Moreover if we implement insert(word) there is still GIL (global interpreter lock) and build-in classes are thread-safe.
        """
        key = encode(word)
        return self.data[key]


# Tested with Python 2.7.11

class TestAnagrams(unittest.TestCase):

    def test_anagrams(self):
        anagrams = Anagrams()
        self.assertEquals(anagrams.get_anagrams('plates'), ['palest', 'pastel', 'petals', 'plates', 'staple'])
        self.assertEquals(anagrams.get_anagrams('eat'), ['ate', 'eat', 'tea'])

    def test_data_size(self):
        """
        Test if internal data dictionary has the right size, no duplicates.
        """
        with open('words.txt') as _file:
            size = len(_file.readlines())

        anagrams = Anagrams()
        for key, value in anagrams.data.items():
            size -= len(value)

        self.assertFalse(size)

    def test_threading(self):
        """
        Test if all parallel readers return expected data. Over-engineered!
        """
        anagrams = Anagrams()
        pool = ThreadPool(processes=18)
        with closing(pool):
            scale = 123  # arbitrary int
            words = ['palest', 'pastel', 'petals', 'plates', 'staple']
            results = pool.map(anagrams.get_anagrams, scale * words)
            size = sum([len(r) for r in results])
            self.assertEquals(size, scale * (len(words) ** 2))


if __name__ == '__main__':
    unittest.main()
