# -*- coding: utf-8 -*-
import pykwic
import nose
import hashlib
import os
import random
from  datetime import datetime

class TestKwic():

    def setup(self):
        self._kwic=pykwic.Kwic()
        self._kwic.add_line(u"うなぎうなうなうなぎなう")
        self._kwic.add_line(u"うらにわににわうなにわとりがいる")
        for i in xrange(10000):
            self._kwic.add_line(u"".join([unichr(random.randint(1,10000)) for i in range(random.randint(10,30))]))
        self._kwic.build()

    def teardown(self):
        self._kwic = None


    def search(self):
        query = u'うな'
        res = self._kwic.search(query)
        for x in res:
            print x[0],query,x[1][len(query):]

    def test_search(self):
        self.search()

    def test_save_and_load(self):
        fname = "/tmp/"+ hashlib.sha1(str(datetime.now())).hexdigest()
        assert self._kwic.save(fname) == 0, "save failed"
        assert self._kwic.load(fname) == 0, "load failed"
        self.search()
        os.remove(fname)


class TestEKwic():

    def setup(self):
        self._kwic=pykwic.EKwic()
        self._kwic.add_line(u"うなぎうなうなうなぎなう")
        self._kwic.add_line(u"うらにわににわうなにわとりがいる")
        self._kwic.build()

    def teardown(self):
        self._kwic = None

    def test_maxsubst(self):
        for maxsubst in self._kwic.maxmal_substring():
            print maxsubst[0],maxsubst[1]
