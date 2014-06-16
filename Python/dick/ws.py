#!/usr/bin/python
# -*- coding: utf-8 -*- from http://www.python.org/peps/pep-0263.html (http://www.python.org/dev/peps/pep-0263/)

import sys
import getopt
import sqlite3
import re
import shlex
import MultiLine

class Word_Smart():
	DB_PATH = '/home/icentrik/work/word/db/'
	WordSmart_db_filename = DB_PATH + 'WordSmart.sqlite3'
	WordSmart_db_connection = None	# read (SELECT) only; no need to commit
	WordSmart_db_cursor = None		# SELECT only
	ROOT_db_filename = DB_PATH + 'ROOT.sqlite3'
	ROOT_db_connection = None
	ROOT_db_cursor = None

	COLOR_TUPLE_DEFAULT = ('white', 'normal', False)
	COLOR_TUPLE_WORDS_DEFAULT = ('green', 'bold', False)
	COLOR_TUPLE_WORDS_WORD = ('cyan', 'bold', False)
	COLOR_TUPLE_WORDS_IPA = ('blue', 'bold', False)
	COLOR_TUPLE_WORDS_POS = ('yellow', 'bold', False)
	COLOR_TUPLE_WORDS_GW = ('magenta', 'bold', False)
	COLOR_TUPLE_WORDS_LK = ('blue', 'normal', False)
	COLOR_TUPLE_WORDS_PRON = COLOR_TUPLE_WORDS_DEFAULT
	COLOR_TUPLE_WORDS_SENSE = COLOR_TUPLE_WORDS_DEFAULT
	COLOR_TUPLE_WORDS_DEF = COLOR_TUPLE_WORDS_SENSE
	COLOR_TUPLE_WORDS_BOOK = COLOR_TUPLE_DEFAULT
	COLOR_TUPLE_WORDS_WORD_MATCHED = ('green', 'bold', True)
	COLOR_TUPLE_WORDS_WORD_MATCHED_PARTLY = COLOR_TUPLE_WORDS_WORD_MATCHED
	COLOR_TUPLE_WORDS_WORD_MATCHED_EXACTLY = ('yellow', 'bold', True)
	COLOR_TUPLE_WORDS_SENSE_MATCHED = ('green', 'bold', True)
	COLOR_TUPLE_WORDS_SENSE_MATCHED_PARTLY = COLOR_TUPLE_WORDS_SENSE_MATCHED
	COLOR_TUPLE_WORDS_SENSE_MATCHED_EXACTLY = ('magenta', 'bold', True)
	COLOR_TUPLE_WORDS_EXAMPLE = ('cyan', 'normal', False)
	COLOR_TUPLE_WORDS_EXAMPLE_MATCHED = ('cyan', 'normal', True)
	COLOR_TUPLE_WORDS_EXAMPLE_MATCHED_PARTLY = COLOR_TUPLE_WORDS_EXAMPLE_MATCHED
	COLOR_TUPLE_WORDS_EXAMPLE_MATCHED_EXACTLY = ('green', 'normal', True)
	COLOR_TUPLE_SAT_WORD = ('magenta', 'bold', False)
	COLOR_TUPLE_SAT_SENSE = ('magenta', 'bold', False)
	COLOR_TUPLE_SAT_BOOK = COLOR_TUPLE_DEFAULT
	COLOR_TUPLE_SAT_WORD_MATCHED = ('magenta', 'bold', True)
	COLOR_TUPLE_SAT_WORD_MATCHED_PARTLY = COLOR_TUPLE_SAT_WORD_MATCHED
	COLOR_TUPLE_SAT_WORD_MATCHED_EXACTLY = ('green', 'bold', True)
	COLOR_TUPLE_SAT_SENSE_MATCHED = ('magenta', 'bold', True)
	COLOR_TUPLE_SAT_SENSE_MATCHED_PARTLY = COLOR_TUPLE_SAT_SENSE_MATCHED
	COLOR_TUPLE_SAT_SENSE_MATCHED_EXACTLY = ('green', 'bold', True)
	COLOR_TUPLE_GRE_WORD = ('yellow', 'bold', False)
	COLOR_TUPLE_GRE_SENSE = ('yellow', 'bold', False)
	COLOR_TUPLE_GRE_BOOK = COLOR_TUPLE_DEFAULT
	COLOR_TUPLE_GRE_WORD_MATCHED = ('yellow', 'bold', True)
	COLOR_TUPLE_GRE_WORD_MATCHED_PARTLY = COLOR_TUPLE_GRE_WORD_MATCHED
	COLOR_TUPLE_GRE_WORD_MATCHED_EXACTLY = ('green', 'bold', True)
	COLOR_TUPLE_GRE_SENSE_MATCHED = ('yellow', 'bold', True)
	COLOR_TUPLE_GRE_SENSE_MATCHED_PARTLY = COLOR_TUPLE_GRE_SENSE_MATCHED
	COLOR_TUPLE_GRE_SENSE_MATCHED_EXACTLY = ('green', 'bold', True)
########################################
	def __init__(self):
		self.WordSmart_db_connection = sqlite3.connect(self.WordSmart_db_filename)
#		self.WordSmart_db_connection.text_factory = str	# UTF-8; must have
		self.WordSmart_db_cursor = self.WordSmart_db_connection.cursor()
		self.ROOT_db_connection = sqlite3.connect(self.ROOT_db_filename)
		self.ROOT_db_cursor = self.ROOT_db_connection.cursor()
	def __del__(self):
		self.WordSmart_db_cursor.close()
		self.ROOT_db_cursor.close()

################################################################################
	# all or nothing
	# return err
	#	err is None: no error
	def copy_opt(self, opts):
		if not opts:
			return
		if str(type(opts)) != "<type 'str'>":
			return str(opts) + ": " + str(type(opts)) + ": <type 'str'> ONLY"
		try:
			optv, o = getopt.getopt(shlex.split(opts), self.opt_str, self.long_opt_list)
			for o in optv:
				key, val = o
				if self.option.has_key(key):
					if not self.option.has_key('.' + key):
						self.option['.' + key] = self.option[key]	# copy
						self.option[key] = []
				else:
					self.option['.' + key] = None	# mark as added key
					self.option[key] = []
				self.option[key].append(val)
		except getopt.GetoptError, err:
			return str(opts) + ": " + str(err)
		return None
		pass
	def restore_opt(self, err):
		if err is not None:
			print
			print "Err in option:"
			print "-" * 80
			print err
			print "-" * 80
			return
		del_key_list = list()
		for key in self.option:
			if key[0:2] != '.-':
				continue
			# key begins with "--"
			del_key_list.append(key)	# key itself
			if self.option[key] is None:
				# added key
				del_key_list.append(key[1:])
			else:
				# modified key
				self.option[key[1:]] = self.option[key]	# restore
		for del_key in del_key_list:
			del self.option[del_key]
		pass
	########################################
################################################################################
	# '-h': with header like "(Word Smart [I,II] <#N>)"
	# '-i': with (book, unit) like "book:unit"
	# '-s': search in "sense" including "word"
	# '-a': search in "sense", "examples" including "word"
	# '-1': "The Words"
	# '-2': 'The {SAT,GRE} Hit Parade"
	# '-p': plain text rather than ansi effects
	# '-f': forced to write
	# '-N': threshold for words found
	opt_str = 'hisa12pfN:'
	long_opt_list = [
		'info-only',
		'verbose',
		'exclude=',	# belie --exclude belief --exclude believe
		'include=',	# soothe --include soothing
		'exact=',	# in case there are too many matches or after use of --info and --verbose, match exactly which invalidates --include and --exclude in color_match2 function; for example, prod --exact prods --exact prodded
	]
	def usage(self):
		print """
%s [-hisa12p] word

	-h: including "(Word Smart I/II/for the GRE/SAT/GRE <###>)" (default)
	-i: book/day "1:10" (book I, day 10)
	-s: word/sense only
	-a: word/sense/example (default)
	-1: Word Smart I/II/for the GRE only
	-2: SAT/GRE only
	-p: plain text
""" % sys.argv[0]
	option = {
		'-a': '',
		'-h': '',
		'-N': [10]
	}
	def set_opt(self, o_str = None):
		optv, wordv = getopt.getopt(sys.argv[1:], self.opt_str, self.long_opt_list)
		if o_str is not None:
			optv, wordv = getopt.getopt(o_str.split(), self.opt_str, self.long_opt_list)
		for o in optv:
			key, val = o
			if not self.option.has_key(key):
				self.option[key] = list()
			self.option[key].append(val)
		#print 'db.option: %s: ' % self.option
		return wordv
	def chk_opt(self, word, opt):
		key, val = opt
		if key == '--exclude':
			return val.upper().upper().find(word.upper()) >= 0
		return True


	def color_match2(self,
		s,
		str_matched,
		color_tuple,
		color_tuple_matched_partly,
		color_tuple_matched_exactly,
		left_margin = 8,
		right_margin = 8,
		hanging_indent = 0,
		return_only = False
	):		#	// {
		left_over = s
		carry_over = ''
		text_a = list()
		word_a = list()
		color_a = list()
		m2 = list()
		while True:
#			print 'left_over: |%s|' % left_over
#			m = re.search(r'(.*?)((?i)%s)(.*)' % str_matched, left_over)
			a = [str_matched]
			if self.option.has_key('--include'):
				a = a + self.option['--include']
			match_re = r'(.*?)(([a-zA-Z]*)((?i)%s)([a-zA-Z]*))(.*)' % '|'.join(a)
			#if self.option.has_key('--exact'):
			#	match_re = r'(.*?)((?i)%s([a-zA-Z]*))(.*)' % '|'.join(self.option['--exact'])
			m = re.search(match_re, left_over)
			if m is None:
				text = carry_over + left_over
				carry_over = ''
				#text = text.strip(' \t\r\n')
				text_a.append(text)
				break
			text = carry_over + m.group(1)
			#text = text.strip(' \t\r\n')
			word = m.group(2)
			left_over = m.group(6)
			if self.option.has_key('--exact'):
				found = False
				for e in self.option['--exact']:
					if e.upper() == word.upper():
						found = True
						break
				if not found:
					carry_over = text + ' ' + m.group(2)
					continue
			text_a.append(text)
			word_a.append(word)
			if self.option.has_key('--exact'):
				m2.append(str(word.upper()))
				c = color_tuple_matched_exactly
				if self.option.has_key('-p'):
					c = None
				color_a.append(c)
				continue
			excluded = False
			if self.option.has_key('--exclude'):
				found = False
				for x in self.option['--exclude']:
					if word.upper().find(x.upper()) != -1:
						found = True
						break
				excluded = found
			included = False
			for e in a:
				if word.upper().find(e.upper()) != -1:
					included = True
					break
			if not excluded and included:
				m2.append(str(word.upper()))
			partly = True
			for e in a:
				if e.upper() == word.upper():
					partly = False
					break
			c = None
			if partly:
				if not self.option.has_key('-p'):
					c = color_tuple_matched_partly
			else:
				if not self.option.has_key('-p'):
					c = color_tuple_matched_exactly
			color_a.append(c)
		assert(len(text_a))
		assert(len(text_a) == len(word_a) + 1)
		assert(len(word_a) == len(color_a))
		"""
		print '\ntext[0]: |%s|' % text_a[0]
		for i in range(len(word_a)):
			print 'word[%d]: |%s|' % (i, word_a[i])
			print 'color[%d]: |%s|' % (i, color_a[i])
			print 'text[%d]: |%s|' % (1 + i, text_a[1 + i])
		"""
		if return_only:
			return (text_a, word_a, color_a, m2)
		c = color_tuple
		if self.option.has_key('-p'):
			c = None
		a = []
		if len(text_a) and text_a[0].lstrip() != '':
			a.append((color_tuple, text_a[0].lstrip()))
		for i in range(len(word_a)):
			a.append((color_a[i], word_a[i]))
			a.append((color_tuple, text_a[1 + i]))
		column0, lines = MultiLine.getTerminalSize()
		tup = (column0, left_margin, right_margin, hanging_indent)
		ansi = False if self.option.has_key('-p') else True
		#print MultiLine.multi_line(a, tup, ansi)
		return MultiLine.multi_line(a, tup, ansi), m2
	# end of color_match2	// }

	def lookup(self, match_str, o = None, return_only = False):
		if self.WordSmart_db_cursor is None:
			return

		mm2_list = None
		rset1, rset2, rsetEK, rsetROOT = None, None, None, None
		rset = None
		try:
			#print self.option
			err = self.copy_opt(o)
			#print self.option

			if self.option.has_key('-1') or not self.option.has_key('-2'):
				r = self.find0(match_str)
				if r is not None:
					mm2_list, rset1 = r
			if self.option.has_key('-2') or not self.option.has_key('-1'):
				rset2 = self.find2(match_str)
			#rsetEK = self.lookup_EK(match_str)
			rsetROOT = self.find_root(match_str)

			if self.option.has_key('-k'):
				self.__add_new(match_str)
			if self.option.has_key('-t'):
				self.list3()

			rset = (mm2_list, rset1, rset2, rsetEK, rsetROOT)
			return self.writeall(rset, return_only)
		finally:
#			print self.option
			self.restore_opt(err)
#			print self.option

	def writeall(self, rset, return_only = False):
		words_delimiter1 = '\n\n'	# The Words
		words_delimiter2 = '\n'		# SAT, GRE
		roots_delimiter = '\n\n'
		group_delimiter = '\n\n\n'
		if rset is None:
			return

		mm2_list, rset1, rset2, rsetEK, rsetROOT = rset
		if rset1 and len(rset1) and (self.option.has_key('--info-only') or (len(rset1) > int(self.option['-N'][0]) and not self.option.has_key('-f'))):
			sys.stderr.write('%s\n' % '# of word representative matched: %d' % len(mm2_list))
			dd = dict()
			if self.option.has_key('--verbose'):
				sys.stderr.write('%s\n' % '-' * 40)
			for ee in mm2_list:
				word, m2_list = ee
				d = dict()
				for e in m2_list:
					if not d.has_key(e):
						d[e] = 0
					d[e] = d[e] + 1
					if not dd.has_key(e):
						dd[e] = 0
					dd[e] = dd[e] + 1
				a = sorted(d.keys(), key = str.lower)
				s = []
				n = 0
				for e in a:
					s.append('%s (%d)' % (e, d[e]))
					n = n + d[e]
				if self.option.has_key('--verbose'):
					sys.stderr.write('%s\n' % '\t|%s|' % word)
					sys.stderr.write('%s\n' % '\t\t%s' % '\n\t\t'.join(s))
			if self.option.has_key('--verbose'):
				sys.stderr.write('%s\n' % '-' * 40)
			a = sorted(dd.keys(), key = str.lower)
			s = []
			n = 0
			for e in a:
				s.append('%s (%d)' % (e, dd[e]))
				n = n + dd[e]
			sys.stderr.write('%s\n' % ('=' * 40))
			sys.stderr.write('%s\n' % 'list of occurrences (# = %d)' % n)
			sys.stderr.write('%s\n' % ('-' * 40))
			sys.stderr.write('%s\n' % '\t%s' % '\n\t'.join(s))
			sys.stderr.write('%s\n' % ('=' * 40))
			if not self.option.has_key('--info-only'):
				sys.stderr.write('%s\n' % 'To see all, use "-f" option or\nincrease threshold with "-N" option (current value = %d)' % int(self.option['-N'][0]))
				sys.stderr.write('%s' % 'or use --exclude option')
				if self.option.has_key('--exclude'):
					sys.stderr.write('%s\n' % ' (excluded: |%s|)' % self.option['--exclude'])
				else:
					sys.stderr.write('\n')
			#print

			if not self.option.has_key('-f'):
				return

		a = []
		rset1_str = None
		if rset1 and len(rset1):
			for e in rset1:
				wordset, examples = e
				#print '%s\n\n%s\n' % (wordset, '\n\n'.join(examples))
				a.append('%s\n\n%s' % (wordset, '\n\n'.join(examples)))
		if a:
			rset1_str = words_delimiter1.join(a)

		rset2_str = None
		if rset2 and len(rset2):
			#print '%s\n' % '\n'.join(rset2)
			rset2_str = '%s' % words_delimiter2.join(rset2)

		rsetROOT_str = None
		if rsetROOT and len(rsetROOT):
			_ = sorted(rsetROOT.keys(), key = unicode.lower)
			a = []
			for root in _:
				#print '%s (%s)' % (root, rsetROOT[root]['sense'])
				s = '%s (%s)\n' % (root, rsetROOT[root]['sense'])
				r = self.list_words(rsetROOT[root]['words'], left_margin = 8, right_margin = 8, TAB_SIZE = 8, return_only = True)
				if r[-1] == '\n':
					r = r[:-1]
				s = s + r
				a.append(s)
				#print
			rsetROOT_str = roots_delimiter.join(a)
		aa = []
		if rset1_str:
			aa.append(rset1_str)
		if rset2_str:
			aa.append(rset2_str)
		if rsetROOT_str:
			aa.append(rsetROOT_str)
		if aa:
			if return_only:
				return group_delimiter.join(aa)
			print group_delimiter.join(aa)

		pass
	# end of writeall

	# The Words
	def find0(self, match_str):
		# 1. set and execute query
		#print match_str
		SEARCH_STR0 = match_str.strip('%').upper()
		#print SEARCH_STR0
		if self.option.has_key('-s') or self.option.has_key('-a'):
			keys = ["word", "sense"]
			if self.option.has_key('-a'):
				keys.append("examples")
			a = list()
			# tested with "belie", "aver"
# $ voc aver --info --verbose --ex waver --ex slaver --ex quaver --ex maver --ex cleaver --ex braver --ex avert --ex avers --ex average -f
# $ voc belie --info --verbose --ex belief --ex believ -f
# $ voc soothe --info --inc soothing
			for e in keys:
				"""
				s = '%s LIKE "%%%s%%"' % (e, SEARCH_STR0)
				if self.option.has_key('--exclude'):
					s = '(' + s + ''.join(map(lambda x: ' AND %s NOT LIKE "%%%s%%"' % (e, x), self.option['--exclude'])) + ')'

				a.append(s)
				"""
				aa = [SEARCH_STR0]
				if self.option.has_key('--include'):
					aa = aa + self.option['--include']
				aaa = []
				for ee in aa:
					s = '%s LIKE "%%%s%%"' % (e, ee)
					if self.option.has_key('--exclude'):
						s = '(' + s + ''.join(map(lambda x: ' AND %s NOT LIKE "%%%s%%"' % (e, x), self.option['--exclude'])) + ')'
					aaa.append(s)
				a.append('(' + ' OR '.join(aaa) + ')')
			q = 'SELECT * FROM TheWords WHERE ' + ' OR '.join(a) + ' ORDER BY book_no, word'
			#print q
		if self.option.has_key('-i'):
			# "-i": (book, unit) as "book:unit"
			index = match_str.find(':')
			if index == -1:
				sys.stderr.write('Err: "%s": invalid format([12]:#)\n' % match_str)
				return None
			book, unit = None, None
			try:
				book = int(match_str[:index])
				unit = int(match_str[index + 1:])
			except Exception as err:
				sys.stderr.write('Err: "%s": "%s": invalid format("[12]:#")\n' % (err, match_str))
				return None
			if (book != 1 and book != 2):
				sys.stderr.write('Err: "%s": invalid format([12]:#)\n' % match_str)
				return
			q = 'SELECT * FROM TheWords WHERE book_no = %d AND unit = %d ORDER BY book_no, word' % (book, unit)
		elif self.option.has_key('-a'):
			pass
		elif self.option.has_key('-s'):
			pass
		else:
			q = 'SELECT * FROM TheWords WHERE word LIKE "%s" ORDER BY book_no, word' % SEARCH_STR0
		self.WordSmart_db_cursor.execute(q)

		# 2. gather result
		mm2_list = list()
		rset = list()
		SEARCH_STR0 = ''.join(SEARCH_STR0.split('___'))
		for r in self.WordSmart_db_cursor:
			#print r
			book_no, unit, word, pos, pron, sense, examples_str = r

			# sense
			text_a = list()
			word_a = list()
			color_a = list()
			m2_list = list()
			WORD_text_a, WORD_word_a, WORD_color_a, m2 = self.color_match2(
				word,
				SEARCH_STR0,
				self.COLOR_TUPLE_WORDS_WORD,
				self.COLOR_TUPLE_WORDS_WORD_MATCHED_PARTLY,
				self.COLOR_TUPLE_WORDS_WORD_MATCHED_EXACTLY,
				return_only = True
			)
			m2_list = m2_list + m2
			SENSE_text_a, SENSE_word_a, SENSE_color_a, m2 = self.color_match2(
				sense,
				SEARCH_STR0,
				self.COLOR_TUPLE_WORDS_SENSE,
				self.COLOR_TUPLE_WORDS_SENSE_MATCHED_PARTLY,
				self.COLOR_TUPLE_WORDS_SENSE_MATCHED_EXACTLY,
				return_only = True
			)
			m2_list = m2_list + m2
			POS_text_a = [" ", " "]
			POS_word_a = [pos]
			POS_color_a = [self.COLOR_TUPLE_WORDS_POS]
			if self.option.has_key('-p'):
				POS_color_a = [None]
			PRON_text_a = [" ", " "]
			PRON_word_a = ["(" + pron + ")"]
			PRON_color_a = [self.COLOR_TUPLE_WORDS_PRON]
			if self.option.has_key('-p'):
				PRON_color_a = [None]

			text_a = WORD_text_a
			if book_no == 2:
				text_a = text_a[:-1] + [text_a[-1] + POS_text_a[0]] + POS_text_a[1:]
				text_a = text_a[:-1] + [text_a[-1] + PRON_text_a[0]] + PRON_text_a[1:]
				text_a = text_a[:-1] + [text_a[-1] + SENSE_text_a[0]] + SENSE_text_a[1:]
				word_a = WORD_word_a + POS_word_a + PRON_word_a + SENSE_word_a
				color_a = WORD_color_a + POS_color_a + PRON_color_a + SENSE_color_a
			else:
				text_a = text_a[:-1] + [text_a[-1] + PRON_text_a[0]] + PRON_text_a[1:]
				text_a = text_a[:-1] + [text_a[-1] + POS_text_a[0]] + POS_text_a[1:]
				text_a = text_a[:-1] + [text_a[-1] + SENSE_text_a[0]] + SENSE_text_a[1:]
				word_a = WORD_word_a + PRON_word_a + POS_word_a + SENSE_word_a
				color_a = WORD_color_a + PRON_color_a + POS_color_a + SENSE_color_a
			if self.option.has_key('-h'):
				# header
				book = 'I'
				if book_no == 2:
					book = 'II'
#				print '(Word Smart %s <#%s>)' % (book, unit)
#				s = s + ' (Word Smart %s <#%s>)' % (book, unit)
				BOOK_text_a = [" ", " "]
				BOOK_color_a = [self.COLOR_TUPLE_WORDS_BOOK]
				if self.option.has_key('-p'):
					BOOK_color_a = [None]
				if book_no == 3:
					BOOK_word_a = ["(Word Smart for the GRE)"]
				else:
					BOOK_word_a = ["(Word Smart %s <#%s>)" % (book, unit)]
				text_a = text_a[:-1] + [text_a[-1] + BOOK_text_a[0]] + BOOK_text_a[1:]
				word_a = word_a + BOOK_word_a
				color_a = color_a + BOOK_color_a

#			print text_a
#			print word_a
#			print color_a
			assert(len(text_a))
			assert(len(text_a) == len(word_a) + 1)
			assert(len(word_a) == len(color_a))
			"""
			print '\ntext[0]: |%s|' % text_a[0]
			for i in range(len(word_a)):
				print 'word[%d]: |%s|' % (i, word_a[i])
				print 'color[%d]: |%s|' % (i, color_a[i])
				print 'text[%d]: |%s|' % (1 + i, text_a[1 + i])
			"""
			a = []
			if len(text_a) and text_a[0].lstrip() != '':
				a.append((self.COLOR_TUPLE_WORDS_DEFAULT, text_a[0].lstrip()))
			for i in range(len(word_a)):
				a.append((color_a[i], word_a[i]))
				a.append((self.COLOR_TUPLE_WORDS_DEFAULT, text_a[1 + i]))
			column0, lines = MultiLine.getTerminalSize()
			tup = (column0, 0, 4, 0)
			ansi = False if self.option.has_key('-p') else True
			#print MultiLine.multi_line(a, tup, ansi)
			wordset = MultiLine.multi_line(a, tup, ansi)

			# examples
			examples = list()
			left_margin = None
			hanging_indent = None
			if book_no == 2:
				hanging_indent = 0#-2
				left_margin = 8
			right_margin = 8
			for s in examples_str.split('\n'):
				s = re.sub(r'___(.*?)___', r'\1', s)
				if book_no == 1 or book_no == 3:
					if s[0] == '\t':
						left_margin = 4
						hanging_indent = 0#-2
					else:
						left_margin = 0
						hanging_indent = 0
				special = False
				if s[0:8] == ' ' * 8:
					special = True
					hanging_indent = -8
					left_margin = 0
					right_margin = right_margin + 8

				r, m2 = self.color_match2(
					s,
					SEARCH_STR0,
					self.COLOR_TUPLE_WORDS_EXAMPLE,
					self.COLOR_TUPLE_WORDS_EXAMPLE_MATCHED_PARTLY,
					self.COLOR_TUPLE_WORDS_EXAMPLE_MATCHED_EXACTLY,
					left_margin,
					right_margin,
					hanging_indent,
					return_only = False
				)
				if special:
					r = '.' * 8 + r
				examples.append('%s' % r)
				m2_list = m2_list + m2

			# end of paragraph
			if len(examples):
				rset.append((wordset, examples))
			if len(m2_list):
				mm2_list.append((word, m2_list))
#			if len(examples) and not ReturnOnly:
#				print '\n\n'.join(examples)
#				print
		# end of for-loop

		return mm2_list, rset
		pass
	#end of find0

	# The {SAT,GRE} Hit Parade; '-a' and '-n' options not used
	def find2(self, match_str):
		SEARCH_STR0 = match_str.strip('%').upper()
		if self.option.has_key('-s') or self.option.has_key('-a'):
			keys = ["word"]
			if self.option.has_key('-s'):
				keys.append("sense")
			a = list()
			for e in keys:
				aa = [SEARCH_STR0]
				if self.option.has_key('--include'):
					aa = aa + self.option['--include']
				aaa = []
				for ee in aa:
					s = '%s LIKE "%%%s%%"' % (e, ee)
					if self.option.has_key('--exclude'):
						s = '(' + s + ''.join(map(lambda x: ' AND %s NOT LIKE "%%%s%%"' % (e, x), self.option['--exclude'])) + ')'
					aaa.append(s)
				a.append('(' + ' OR '.join(aaa) + ')')
			q = 'SELECT * FROM TheHitParade WHERE ' + ' OR '.join(a) + ' ORDER BY type, book, page, line'
#			print q
		if self.option.has_key('-i'):
			# "-i": (type, book) as "type:book"
			index = match_str.find(':')
			if index == -1:
				sys.stderr.write('Err: "%s": invalid format([12]:[12])\n' % match_str)
				return None
			type_id, book = None, None
			try:
				type_id = int(match_str[:index])
				book = int(match_str[index + 1:])
			except Exception as err:
				sys.stderr.write('Err: "%s": "%s": invalid format("[12]:[12]")\n' % (err, match_str))
				return None
			if (type_id != 1 and type_id != 2) and (book != 1 and book != 2):
				sys.stderr.write('Err: "%s": invalid format([12]:[12])\n' % match_str)
				return
			q = 'SELECT * FROM TheHitParade WHERE type = %d AND book = %d ORDER BY page, line' % (type_id, book)
		elif self.option.has_key('-s'):
			pass
		else:
			pass
#		print q
		self.WordSmart_db_cursor.execute(q)

		rset = list()
		for r in self.WordSmart_db_cursor:
			type_id, book, page, line, word, sense = r

			# sense
			text_a = list()
			word_a = list()
			color_a = list()
			m2_list = list()
			WORD_text_a, WORD_word_a, WORD_color_a, m2 = self.color_match2(
				word,
				SEARCH_STR0,
				self.COLOR_TUPLE_GRE_WORD if type_id == 2 else self.COLOR_TUPLE_SAT_WORD,
				self.COLOR_TUPLE_GRE_WORD_MATCHED_PARTLY if type_id == 2 else self.COLOR_TUPLE_SAT_WORD_MATCHED_PARTLY,
				self.COLOR_TUPLE_GRE_WORD_MATCHED_EXACTLY if type_id == 2 else self.COLOR_TUPLE_SAT_WORD_MATCHED_EXACTLY,
				return_only = True
			)
			m2_list = m2_list + m2
			SENSE_text_a, SENSE_word_a, SENSE_color_a, m2 = self.color_match2(
				sense, SEARCH_STR0,
				self.COLOR_TUPLE_GRE_SENSE if type_id == 2 else self.COLOR_TUPLE_SAT_SENSE,
				self.COLOR_TUPLE_GRE_SENSE_MATCHED_PARTLY if type_id == 2 else self.COLOR_TUPLE_SAT_SENSE_MATCHED_PARTLY,
				self.COLOR_TUPLE_GRE_SENSE_MATCHED_EXACTLY if type_id == 2 else self.COLOR_TUPLE_SAT_SENSE_MATCHED_EXACTLY,
				return_only = True
			)
			m2_list = m2_list + m2
			#print WORD_text_a
			#print WORD_word_a
			#print WORD_color_a
			#print SENSE_text_a
			#print SENSE_word_a
			#print SENSE_color_a
			text_a = WORD_text_a
			if text_a[-1]:
				text_a[-1] = text_a[-1] + ' '
			text_a = WORD_text_a[:-1] + [WORD_text_a[-1] + ' ' + SENSE_text_a[0]] + SENSE_text_a[1:]
			word_a = WORD_word_a + SENSE_word_a
			color_a = WORD_color_a + SENSE_color_a
			if self.option.has_key('-h'):
				# header
				type_str = 'SAT'
				if type_id == 2:
					type_str = 'GRE'
				book_str = 'I'
				if book == 2:
					book_str = 'II'
				BOOK_text_a = ["", ""]
				BOOK_color_a = [self.COLOR_TUPLE_GRE_BOOK if type_id == 2 else self.COLOR_TUPLE_SAT_BOOK]
				#s = '%s %s (The %s Hit Parade %s::%d:%d)' % (word, sense, type_str, book_str, page, line)
				BOOK_word_a = ['(The %s Hit Parade %s::%d:%d)' % (type_str, book_str, page, line)]
				text_a = text_a[:-1] + [text_a[-1] + BOOK_text_a[0]] + BOOK_text_a[1:]
				word_a = word_a + BOOK_word_a
				color_a = color_a + BOOK_color_a

#			print text_a
#			print word_a
#			print color_a
			assert(len(text_a))
			assert(len(text_a) == len(word_a) + 1)
			assert(len(word_a) == len(color_a))
			a = []
			if len(text_a) and text_a[0].lstrip() != '':
				a.append((self.COLOR_TUPLE_WORDS_DEFAULT, text_a[0].lstrip()))
			for i in range(len(word_a)):
				a.append((color_a[i], word_a[i]))
				a.append((self.COLOR_TUPLE_WORDS_DEFAULT, text_a[1 + i]))
			column0, lines = MultiLine.getTerminalSize()
			tup = (column0, 0, 4, 0)
			ansi = False if self.option.has_key('-p') else True
			#print MultiLine.multi_line(a, tup, ansi)
			wordset = MultiLine.multi_line(a, tup, ansi)
			if wordset:
				rset.append(wordset)
		return rset
	# end of find2

	def find_root(self, match_str):
		if self.ROOT_db_cursor is None:
			sys.stderr.write('Err: ROOT_db_cursor is None\n')
			return

		self.ROOT_db_cursor.execute('SELECT DISTINCT(root) FROM Root WHERE word LIKE ? ORDER BY root, word', (match_str,))
		ROOT_list = list()
		for r in self.ROOT_db_cursor:
			root, = r
			ROOT_list.append(root)
		if len(ROOT_list) == 0:
			return

#		print ROOT_list
		rROOT = dict()
		q = 'SELECT root, sense, word FROM Root WHERE root = "%s" ORDER BY root, word' % '" OR root = "'.join(ROOT_list)
#		print q
		self.ROOT_db_cursor.execute(q)
		for r in self.ROOT_db_cursor:
			root, sense, word = r
			if not rROOT.has_key(root):
				rROOT[root] = {'sense': sense, 'words': []}
			rROOT[root]['words'].append(word)
		return rROOT
		"""
		a = sorted(rROOT.keys(), key = unicode.lower)
		for root in a:
			print '%s (%s)' % (root, rROOT[root]['sense'])
			self.list_words(rROOT[root]['words'], left_margin = 8, right_margin = 8, TAB_SIZE = 8)
			print
		return
		"""
	# end of find_root

	def list_words(self, words, left_margin = 8, right_margin = 8, TAB_SIZE = 8, return_only = False):
		rs = ''
		max_len = -1
		for w in words:
			if len(w) > max_len:
				max_len = len(w)
		ntab = max_len / TAB_SIZE + 1
		col, _ = MultiLine.getTerminalSize()
		ncolumn = ((col - left_margin - right_margin) / TAB_SIZE) / ntab
		i = 0
		for w in words:
			if i == 0:
				if return_only:
					rs = rs + ' ' * left_margin
				else:
					sys.stdout.write(' ' * left_margin)
			fmt = '%%-%ds' % (TAB_SIZE * ntab)
			if return_only:
				rs = rs + fmt % w
			else:
				sys.stdout.write(fmt % w)
			i = i + 1
			if i == ncolumn:
				if return_only:
					rs = rs + '\n'
				else:
					sys.stdout.write('\n')
				i = 0
		if i:
			if return_only:
				rs = rs + '\n'
			else:
				sys.stdout.write('\n')
		if return_only:
			return rs
	# end of list_words


if __name__ == "__main__":
	ws = Word_Smart()

	if len(sys.argv) == 1:
		ws.usage()
		sys.exit()
	word = None
	for word in ws.set_opt():
		ws.lookup(word)
