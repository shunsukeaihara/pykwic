# -*- coding: utf-8 -*-
import pykwic
import nose
import hashlib
import os
import random
from  datetime import datetime
class TestKwicBase():

    def setup(self):
        self._kwic=pykwic.Kwic()
        self._kwic.add_line(u"うなぎうなうなうなぎなう")
        self._kwic.add_line(u"うらにわににわうなにわとりがいる")
        for i in xrange(10000):
            self._kwic.add_line(u"".join([unichr(random.randint(1,10000)) for i in range(random.randint(10,30))]))
        self._kwic.build()

    def teardown(self):
        self._kwic = None


class TestKwic(TestKwicBase):

    def search(self):
        res = self._kwic.search(u'うな')
        for x in res:
            print x[0],x[1]

    @nose.with_setup(TestKwicBase.setup, TestKwicBase.teardown)
    def test_search(self):
        self.search()

    @nose.with_setup(TestKwicBase.setup, TestKwicBase.teardown)
    def test_save_and_load(self):
        fname = "/tmp/"+ hashlib.sha1(str(datetime.now())).hexdigest()
        assert self._kwic.save(fname) == 0, "save failed"
        assert self._kwic.load(fname) == 0, "load failed"
        self.search()
        os.remove(fname)
