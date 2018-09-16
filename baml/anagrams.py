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


class TestAnagrams(unittest.TestCase):

    def test_anagrams(self):
        anagrams = Anagrams()
        self.assertEquals(anagrams.get_anagrams('plates'), ['palest', 'pastel', 'petals', 'plates', 'staple'])
        self.assertEquals(anagrams.get_anagrams('eat'), ['ate', 'eat', 'tea'])


if __name__ == '__main__':
    unittest.main()
