
import json
import csv
import itertools
import multiprocessing as mp
from collections import defaultdict
from contextlib import contextmanager
from pprint import pprint


def chunks(iterable, chunk_size):
    it = iter(iterable)
    while True:
        chunk = tuple(itertools.islice(it, chunk_size))
        if not chunk:
            return
        yield chunk


def read_contract():
    with open('task/contract.json', 'r') as _file:
        contract = json.load(_file)
        print('\nContract')
        pprint(contract)
        max_amount = contract['MaxAmount']
        coverage = contract['Coverage']
    return max_amount, coverage


@contextmanager
def read_csv(filename):
    class Iterator:
        def __init__(self, csv_reader):
            self.csv_reader = csv_reader
            self.line_no = 0
            self.header = None

        def __iter__(self):
            try:
                for line in self.csv_reader:
                    if self.line_no:
                        self.line_no += 1
                        yield dict(zip(self.header, line))
                    else:
                        self.line_no += 1
                        self.header = line
            except Exception as exc:
                raise RuntimeError('File reading problem! line: %s, %s' % (self.line_no, exc))

    with open(filename, 'r') as _file:
        yield Iterator(csv.reader(_file))


def covered_deals(deals, coverage):
    def is_covered_deal(deal):
        for c in coverage:
            attribute_name = c['Attribute']
            attribute = deal[attribute_name]
            include = c.get('Include')
            if include and attribute not in include:
                return False
            exclude = c.get('Exclude')
            if exclude and attribute in exclude:
                return False
        return True

    # assumed that deals.csv has limited size
    return [d for d in deals if is_covered_deal(d)]


def read_deals(coverage):
    with read_csv('task/deals.csv') as deals:
        _covered_deals = covered_deals(deals, coverage)
        print('\nCovered deals')
        pprint(_covered_deals)
        deals_dict = {d['DealId']: d for d in _covered_deals}
        return deals_dict


class DataMap:
    def __init__(self, max_amount, deals_dict):
        self.max_amount = max_amount
        self.deals_dict = deals_dict

    def __call__(self, events):
        perils_dict = defaultdict(lambda: 0)
        for event in events:
            deal = self.deals_dict.get(event['DealId'])
            if deal:
                peril = deal['Peril']
                print(event, deal)
                loss = int(event['Loss'])
                if loss <= self.max_amount:
                    perils_dict[peril] += loss
        return dict(perils_dict)


class DataReduce:
    def __init__(self):
        self.perils_dict = defaultdict(lambda: 0)

    def __call__(self, perils_dict):
        for peril, loss in perils_dict.items():
            self.perils_dict[peril] += loss

    def get_perils(self):
        return dict(self.perils_dict)


def process(chunk_size):
    max_amount, coverage = read_contract()
    deals_dict = read_deals(coverage)

    data_map = DataMap(max_amount, deals_dict)
    data_reduce = DataReduce()

    with read_csv('task/losses.csv') as events:
        pool = mp.Pool(processes=8)

        print('\nEvents for covered deals')
        for chunk in chunks(events, chunk_size):
            pool.apply_async(data_map, args=(chunk,), callback=data_reduce)

        pool.close()
        pool.join()

    print('\nResult')
    perils = data_reduce.get_perils()
    pprint(perils)
    return perils


if __name__ == '__main__':
    process(2)
