import unittest
from io_ops import *


class IoManagerTest(unittest.TestCase):
    def setUp(self):
        self.manager = IoManager(self.read, self.write, self.sync)

    def read(self, chain_id, proto):
        # TODO: Enrich these test implementations.
        print 'Read', chain_id, proto

    def write(self, proto):
        print 'Write', proto

    def sync(self, chain_id):
        print 'Sync', chain_id

    def test_basic_read(self):
        # TODO: Nothing interesting. Add some actual tests.
        self.manager >> any >> line
        self.manager >> lit('abcde') >> any.raw >> lit('fghi')


if __name__ == '__main__':
    unittest.main()
