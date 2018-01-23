
import unittest
import insureco


class TestInsureCo(unittest.TestCase):

    def test_chunks(self):
        l1 = [1, 2, 3, 4, 5]
        c1 = list(insureco.chunks(l1, 2))
        self.assertEqual(len(c1), 3)
        c2 = list(insureco.chunks(l1, 5))
        self.assertEqual(len(c2), 1)
        c3 = list(insureco.chunks(l1, 15))
        self.assertEqual(len(c3), 1)

    def test_contract(self):
        expected = [{'Attribute': 'Location', 'Include': ['USA', 'Canada']},
                    {'Attribute': 'Peril', 'Exclude': ['Tornado']}]
        max_amount, coverage = insureco.read_contract()
        self.assertEqual(max_amount, 3000)
        self.assertListEqual(coverage, expected)

    def test_deals(self):
        expected = [
            {'Company': 'WestCoast',
             'DealId': '1',
             'Location': 'USA',
             'Peril': 'Earthquake'},
            {'Company': 'WestCoast',
             'DealId': '2',
             'Location': 'Canada',
             'Peril': 'Hailstone'},
            {'Company': 'GeorgiaInsurance',
             'DealId': '5',
             'Location': 'USA',
             'Peril': 'Hurricane'}
        ]
        max_amount, coverage = insureco.read_contract()
        actual = insureco.read_deals(coverage)
        self.assertEqual(len(expected), len(actual))

    def test_results(self):
        expected = {'Earthquake': 3500}
        # there is not way to get Hurricane: 3000 from losses.cvs file, a bug? 4000 > MaxAmount

        actual = insureco.process(chunk_size=2)
        self.assertDictEqual(actual, expected)

        actual = insureco.process(chunk_size=12)
        self.assertDictEqual(actual, expected)


if __name__ == '__main__':
    unittest.main()
