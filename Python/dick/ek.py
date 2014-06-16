#!/usr/bin/python
# -*- coding: utf-8 -*- from http://www.python.org/peps/pep-0263.html (http://www.python.org/dev/peps/pep-0263/)

import sys
import sqlite3
import AnsiText
import MultiLine



class English_Korean():	#	// {
	DB_PATH = '/home/icentrik/work/word/db/'
	English_Korean_dbfilename = DB_PATH + 'EK.sqlite3'
	English_Korean_dbconnection = None
	English_Korean_dbcursor = None
	def __init__(self):
		#print 'English_Korean::__init__'
		self.English_Korean_dbconnection = sqlite3.connect(self.English_Korean_dbfilename)
		#self.English_Korean_dbconnection.text_factory = str
		self.English_Korean_dbcursor = self.English_Korean_dbconnection.cursor()
	def __del__(self):
		self.English_Korean_dbconnection.commit()
		self.English_Korean_dbconnection = None
		self.English_Korean_dbcursor.close()
		self.English_Korean_dbcursor = None
	def create(self, dbfilename, datafilename):
		connection = sqlite3.connect(dbfilename)
		"""
sqlite3.ProgrammingError: You must not use 8-bit bytestrings unless you use a text_factory that can interpret 8-bit bytestrings (like text_factory = str). It is highly recommended that you instead just switch your application to Unicode strings.
"""
		connection.text_factory = str
		cursor = connection.cursor()
		# make table
		cursor.execute('CREATE TABLE English_Korean (word TEXT, sense TEXT)')
		connection.commit()
		# insert bulk data
		for data in open(datafilename):
			(word, sense) = data.strip().split('\t')
			cursor.execute('INSERT INTO English_Korean (word, sense) VALUES (?, ?)', (word, sense))
		connection.commit()
		cursor.close()
	def lookup(self, wordstr, return_only = False):
		self.English_Korean_dbcursor.execute('SELECT * FROM English_Korean WHERE word LIKE ? ORDER BY word', (wordstr,))
		a = []
		for r in self.English_Korean_dbcursor:
			a.append(r)

		maxlen = 0
		for e in a:
			(word, sense) = e
			n = len(word)
			if n > maxlen:
				maxlen = n
		c0 = ('green', 'normal', False)
		c1 = ('green', 'normal', True)
		color = [c0, c1]
		i = 0
		aa = []
		column0, lines0 = MultiLine.getTerminalSize()
		for e in a:
			(word, sense) = e
			c = color[i % 2]
			i = 0 if i else 1

			rw = AnsiText.color_text(word, c)
			#print '|%s|' % rw
			text_a = [(c, sense)]
			lm = maxlen + 2
			rm = 8
			hi = -len(word)
			rs = MultiLine.multi_line(text_a, (column0, lm, rm, hi), euckr = True)
			#print '|%s|' % rs
			aa.append('%s%s' % (rw, rs))
		if aa:
			if return_only:
				return '\n'.join(aa)
			print '\n'.join(aa)
# end of class English_Korean		// }



if __name__ == "__main__":
	# instantiate the parser and feed it some HTML
	ek = English_Korean()
	if len(sys.argv) == 1:
		print """
%s word
""" % (sys.argv[0])
		sys.exit(1)
	a = sys.argv[1:]
	while True:
		if not len(a):
			break
		wordstr = a.pop(0)
		ek.lookup(wordstr)
