# -*- coding: utf-8 -*-
import pykwic
x=pykwic.Kwic()
x.add_line(u"うなぎうなうなうなぎなう")
x.add_line(u"うらにわににわうなにわとりがいる")
x.build()
res = x.search(u'うな')
for x in res:
    print x[0],x[1]
