#!/usr/bin/python
# -*- coding: utf-8 -*- from http://www.python.org/peps/pep-0263.html (http://www.python.org/dev/peps/pep-0263/)

from webdic0 import webdic
import urllib
import subprocess
import re
pron_re = re.compile('.*?/(..)_pron.*\.([^\.]*)$')
del re
import sys
import sqlite3
from analhtml import analhtml
import AnsiText
import MultiLine
from ws import Word_Smart


class Cambridge(webdic):	#	// {
	"""
r1
	CREATE TABLE href (href TEXT PRIMARY KEY, blob TEXT);
		-- rowid is, in fact, href_id
	CREATE TABLE word (word KEY, href_id INT, UNIQUE(word, href_id));
		-- href_id is a foreign key: href.rowid
		-- UNIQUE is a synonym for PRIMARY KEY in a table constraint

/*
	1. The names of tables may cause one to be confused.
	2. In a "href" table, href is equivalent to rowid(i.e., href_id)
	3. To save the space/time, instead of href, href_id is used as a key in a "word" table;
	One drawback is this:
		wheneven one record is inserted into a "href" table,
		an additional query, last_insert_rowid(), should be executed to get href_id(i.e., rowid).
	4. Without a foreign key in a table statement, all UPDATE, DELETE statements in a "href" table
	should be accompanied with the same ones in a "word" table.
 */

r2
	CREATE TABLE cambridge (href TEXT PRIMARY KEY, blob TEXT);
	CREATE TABLE dictionary (word KEY, href TEXT, UNIQUE(word, href), FOREIGN KEY(href) REFERENCES cambridge(href) ON DELETE CASCADE ON UPDATE CASCADE);
	CREATE TRIGGER delete_word BEFORE INSERT ON cambridge
	BEGIN
		DELETE FROM dictionary WHERE href = NEW.href;
	END;
INSERT OR REPLACE INTO cambridge (href, blob) VALUES ('a', 'alpha');
SELECT * FROM cambridge;
INSERT OR IGNORE INTO dictionary (word, href) VALUES ('w', 'a');
SELECT * FROM dictionary;
INSERT OR REPLACE INTO cambridge (href, blob) VALUES ('a', 'alpha2');
SELECT * FROM dictionary;
"""
	DB_PATH = '/home/icentrik/work/word/db/'
	Cambridge_dbfilename = DB_PATH + 'Cambridge.sqlite3'
	Cambridge_dbconnection = None
	Cambridge_dbcursor = None
	def __init__(self):
		self.Cambridge_dbconnection = sqlite3.connect(self.Cambridge_dbfilename)
		self.Cambridge_dbconnection.text_factory = str
		self.Cambridge_dbcursor = self.Cambridge_dbconnection.cursor()
	def __del__(self):
		self.Cambridge_dbcursor.close()

	beep = ''	# '\x07'
	follow_link = False
	british = False
	forced = False
	href0 = None	# set in lookup0
	all_href = None	# set in garner

	def cambridge_dic_test(self):
		self.walk("di")
		if not self.classref.has_key("result-list"):
			sys.stderr.write('WARN: cambridge_dic_test: no class for "result-list"\n')
			sys.stderr.write('WARN: cambridge_dic_test: instead, try "results-container-all" class\n')
			self.walk("results-container-all")
		else:
			self.walk("result-list")

	# start of all_results		// {
	# start of get_arl_text	// {
	ARL_TEXT = 0	# word or phrase
	ARL_POS = 1
	ARL_GW = 2
	def get_arl_text(self, node_table, node, arl_a):
		def get_text(text_node, mark = ''):
			assert(text_node['type'] != self.NODE_TAG)
			text = mark + text_node['val'] + mark
			pi =  text_node['parent']
			assert(0 <= pi and pi < len(node_table))
			pnode = node_table[pi]
			if pnode['dic'].has_key('class'):
				c = pnode['dic']['class']
				if c == 'pos':
					return (self.ARL_POS, text)
					return '(' + AnsiText.color_text(text, Word_Smart.COLOR_TUPLE_WORDS_POS) + ')'
				elif c == 'gw':
					return (self.ARL_GW, text)
					return '[' + AnsiText.color_text(text, Word_Smart.COLOR_TUPLE_WORDS_GW) + ']'
			return (self.ARL_TEXT, text)
			return AnsiText.color_text(text, Word_Smart.COLOR_TUPLE_WORDS_WORD)

		if node['type'] != self.NODE_TAG:
			arl_a.append(get_text(node))
			return
		# node
		for ni in node['nodes']:
			assert(0 <= ni and ni < len(node_table))
			cnode = node_table[ni]
			if cnode['type'] == self.NODE_TAG:
				for i in cnode['nodes']:
					nd = node_table[i]
					if nd['type'] == self.NODE_TAG:
						self.get_arl_text(node_table, node_table[i], arl_a)
					else:
						arl_a.append(get_text(nd))
			else:
				arl_a.append(get_text(cnode))
	# end of get_arl_text	// }
	# start of get_all_href		// {
	def get_all_href(self, verbose = False):
		#p[class=moreResultLink] a[class=cdo-link]
		ra = self.get_indices(['moreResultLink', 'cdo-link'])
		if not ra:
			return
		#[(1317, 1327), (1320, 1324)]
		#[(1361, 1371), (1364, 1368)]
		all_href = None
		for x in ra:
			#print x
			(si, ei) = x[0]
			(depth, tagitself, _, d, tag) = self.tagset[si]
			assert(tag == 'p')
			(si, ei) = x[1]
			(depth, tagitself, _, d, tag) = self.tagset[si]
			#print self.tagset[si]
			assert(tag == 'a')
			#print tagitself
			assert(d.has_key('title') and len(d['title']) == 1)
			assert(d.has_key('href') and len(d['href']) == 1)
			title = d['title'][0]
			#print "|%s|" % title
			#print "|%s|" % href
			if 0 != title.find('All results'):
				continue
			all_href = d['href'][0]
			break
		assert(all_href is not None)
		return all_href
	# end if get_all_href		// }

	def all_results(self, verbose = False):
		if self.all_href is None:
			return []
		ah = analhtml()
		cmd = 'curl --header "Accept-Encoding: gzip" -L -o- "%s" | gunzip -c' % self.all_href
		p = subprocess.Popen(cmd, shell = True, bufsize = 4096, stdin = subprocess.PIPE, stdout = subprocess.PIPE, stderr = subprocess.PIPE, close_fds = True)
		#s = ''.join(p.stdout.readlines())
		for _ in p.stdout:
			ah.feed(_)
		#'ul[class=result-list] li a'
		ah.close()

		ra = ah.get_indices(['result-list', '<li>', '<a>'])
		#print ra
		assert(ra)
		more_a = []
		for e in ra:
			(xr, xl, xa) = e
			node_table = []
			#r = ah.set_node_table(xr, node_table)
			#r = ah.walk_node_table(node_table, -1)
			#print '[\n  ' + '\n  '.join(r) + '\n]'
			#r = ah.get_text(xa)
			node_table = []
			r = ah.set_node_table(xa, node_table)
			"""
			print "# of nodes = %d" % len(node_table)
			for i in range(len(node_table)):
				print "node[%d]:" % i
				node = node_table[i]
				print "\tparent: %d" % node['parent']
				if node['type'] == self.NODE_TAG:
					print "\tchildren: %s" % node['children']
					print "\tnodes: %s" % node['nodes']
			"""
			#r = ah.walk_node_table(node_table, -1)
			#print '[\n  ' + '\n  '.join(r) + '\n]'

			A_node = node_table[0]
			assert(A_node['type'] == self.NODE_TAG)
			assert(A_node['tag'] == 'a')
			assert(A_node['dic'].has_key('href'))
			href = A_node['dic']['href']
			assert(self.href0)
			if href == self.href0:
				continue

			if verbose:
				print AnsiText.color_text(href, 'red')
			assert(len(A_node['children']) == 1)
			i = A_node['children'][0]
			assert(0 < i and i < len(node_table))
			ARL_node = node_table[i]
			assert(ARL_node['type'] == self.NODE_TAG)
			assert(ARL_node['dic'].has_key('class'))
			c = ARL_node['dic']['class']
			assert(c.find('arl') == 0)
			arln = int(c[3:])
			if verbose:
				print 'arl# = %s' % AnsiText.color_text('%d' % arln, 'blue')
			arl_a = []
			r = self.get_arl_text(node_table, ARL_node, arl_a)
			text_a = []
			pos = None
			gw = None
			for (arl_type, arl_text) in arl_a:
				if arl_type == self.ARL_TEXT:
					text_a.append(arl_text)
					type_str = 'text'
					color = Word_Smart.COLOR_TUPLE_WORDS_WORD
				elif arl_type == self.ARL_POS:
					if pos:
						pos = pos + ',' + arl_text
					else:
						pos = arl_text
					type_str = 'pos'
					color = Word_Smart.COLOR_TUPLE_WORDS_POS
				elif arl_type == self.ARL_GW:
					assert(gw is None)
					gw = arl_text
					type_str = 'gw'
					color = Word_Smart.COLOR_TUPLE_WORDS_GW
				if verbose:
					print '%s: |%s|' % (type_str, AnsiText.color_text(arl_text, color))
			assert(len(text_a))
			if verbose:
				a = []
				a.append('%s' % AnsiText.color_text(''.join(text_a).strip(), Word_Smart.COLOR_TUPLE_WORDS_WORD))
				if pos is not None:
					a.append('%s' % AnsiText.color_text('(' + pos + ')', Word_Smart.COLOR_TUPLE_WORDS_POS))
				if gw is not None:
					a.append('%s' % AnsiText.color_text('[' + gw + ']', Word_Smart.COLOR_TUPLE_WORDS_GW))
				print ' '.join(a)
			more_a.append((href, ''.join(text_a).strip(), pos, gw, arln))
		return more_a
	# end of all_results		// }

	### GHK:
	###	collect all and sort by index
	T_REP = 0
	T_PV = 1	# phrasal verb
	T_POS = 2
	T_GW = 3
	T_DEF = 4
	T_EXP = 5
	T_IPA = 6
	T_WORD = 20
	T_UK_MP3 = 10
	T_UK_OGG = 11
	T_US_MP3 = 12
	T_US_OGG = 13
	def garner(self, verbose = False):	#	// {
		aa = []
		ra = self.get_indices(['di-head', 'di-title', 'hw'])
		notword = False
		if not len(ra):
			notword = True
		for x in ra:
			r = self.get_text(x[-1])
			if verbose:
				print 'representative:\t%s\t|%s|' % (x[-1], r)
			ii = r.find('“')
			fi = r.find('”')
			if ii != -1 and fi != -1:
				# English definition of “squire”
				r = r[ii + len('“'):fi]
			aa.append((x[-1][0], self.T_REP, r))
		ra = self.get_indices(['di-head', 'di-title', 'phrase'])
		if notword and not len(ra):
			return
		for x in ra:
			r = self.get_text(x[-1])
			if verbose:
				print 'phrase:\t%s\t|%s|' % (x[-1], r)
			aa.append((x[-1][0], self.T_PV, r))
		ra = self.get_indices(['di-info', 'pos'])
		for x in ra:
			r = self.get_text(x[-1])
			if verbose:
				print 'pos:\t%s\t|%s|' % (x[-1], r)
			aa.append((x[-1][0], self.T_POS, r))
		ra = self.get_indices(['di-head', 'gw'])
		for x in ra:
			r = self.get_text(x[-1])
			if verbose:
				print 'gw:\t%s\t|%s|' % (x[-1], r)
			aa.append((x[-1][0], self.T_GW, r))
		ra = self.get_indices(['di-head', 'ipa'])
		for x in ra:
			r = self.get_text(x[-1])
			if verbose:
				print 'ipa: |%s|' % r
			aa.append((x[-1][0], self.T_IPA, r))
		ra = self.get_indices(['di-body', 'def-head', 'def'])
		for x in ra:
			r = self.get_text(x[-1])
			if verbose:
				print 'definition:\t%s\t|%s|' % (x[-1], r)
			aa.append((x[-1][0], self.T_DEF, r))
		ra = self.get_indices(['di-body', 'def-body', 'examp'])
		for x in ra:
			r = self.get_text(x[-1])
			if verbose:
				print 'example:\t%s\t|%s|' % (x[-1], r)
			aa.append((x[-1][0], self.T_EXP, r))
		ra = self.get_indices(['di-body', 'def-body', 'trans'])
		for x in ra:
			r = self.get_text(x[-1])
			if verbose:
				print 'trans:\t%s\t|%s|' % (x[-1], r)
			aa.append((x[-1][0], self.T_EXP, r))
		ra = self.get_indices(['di-body', 'runon', 'runon-head', 'w'])
		for x in ra:
			r = self.get_text(x[-1])
			if verbose:
				print 'runon/word:\t%s\t|%s|' % (x[-1], r)
			aa.append((x[-1][0], self.T_WORD, r))
		ra = self.get_indices(['di-body', 'runon', 'runon-head', 'pos'])
		for x in ra:
			r = self.get_text(x[-1])
			if verbose:
				print 'runon/pos:\t%s\t|%s|' % (x[-1], r)
			aa.append((x[-1][0], self.T_POS, r))
		ra = self.get_indices(['di-body', 'runon', 'runon-head', 'ipa'])
		for x in ra:
			r = self.get_text(x[-1])
			if verbose:
				print 'runon/ipa: |%s|' % r
			aa.append((x[-1][0], self.T_IPA, r))
		"""
		if self.classref.has_key('audio_file_source'):
			ra = self.classref['audio_file_source']
			print ra
			for index in ra:
				(i, link, xxx, d) = self.tagset[index][:4]
				if verbose:
					print 'audio:\t%s\t|%s|' % ((index, index), link)
				assert(d.has_key('src') and len(d['src']) == 1)
				href = d['src'][0]
#	(478, 5, 'http://dictionary.cambridge.org/media/british/uk_pron/u/ukc/ukcom/ukco
#	(479, 5, 'http://dictionary.cambridge.org/media/british/uk_pron_ogg/u/ukc/ukcom/
#	(486, 5, 'http://dictionary.cambridge.org/media/british/us_pron/e/eus/eus70/eus7
#	(487, 5, 'http://dictionary.cambridge.org/media/british/us_pron_ogg/e/eus/eus70/
				m = pron_re.match(href)
				assert(m)
				#print '|%s|, |%s|' % (m.group(1), m.group(2))
				if m.group(1) == 'uk':
					if m.group(2) == 'mp3':
						T_X = self.T_UK_MP3
					else:
						T_X = self.T_UK_OGG
				else:
					if m.group(2) == 'mp3':
						T_X = self.T_US_MP3
					else:
						T_X = self.T_US_OGG
				aa.append((index, T_X, href))
"""
		if self.classref.has_key('sound audio_play_button pron-us'):
			ra = self.classref['sound audio_play_button pron-us']
			for index in ra:
				(i, link, xxx, d) = self.tagset[index][:4]
				if verbose:
					print 'audio:\t%s\t|%s|' % ((index, index), link)
				"""
<a class="sound audio_play_button pron-us" href="#" title="wrangle: listen to American pronunciation" data-src-mp3="http://dictionary.cambridge.org/media/american-english/us_pron/w/wra/wrang/wrangle.mp3" data-src-ogg="http://dictionary.cambridge.org/media/american-english/us_pron_ogg/w/wra/wrang/wrangle.ogg">
"""
				href = d['data-src-ogg'][0]
				T_X = self.T_US_OGG
				aa.append((index, T_X, href))
		# more
		if self.all_href is None:
			self.all_href = self.get_all_href()
		return aa

		ra = self.get_indices(['result-list', 'hw'])
		for x in ra:
			r = self.get_text(x[-1])
			if verbose:
				print 'hw:\t%s\t|%s|' % (x[-1], r)
			aa.append((x[-1][0], self.T_MORE_HW, r))
		ra = self.get_indices(['result-list', 'phrase'])
		for x in ra:
			r = self.get_text(x[-1])
			if verbose:
				print 'hw:\t%s\t|%s|' % (x[-1], r)
			aa.append((x[-1][0], self.T_MORE_PV, r))
		ra = self.get_indices(['result-list', 'pos'])
		for x in ra:
			r = self.get_text(x[-1])
			if verbose:
				print 'pos:\t%s\t|%s|' % (x[-1], r)
			aa.append((x[-1][0], self.T_MORE_POS, r))
		ra = self.get_indices(['result-list', 'gw'])
		for x in ra:
			r = self.get_text(x[-1])
			if verbose:
				print 'gw:\t%s\t|%s|' % (x[-1], r)
			aa.append((x[-1][0], self.T_MORE_GW, r))
		ra = self.get_indices(['result-list', '<li>', '<a>'])
		for x in ra:
			href = self.tagset[x[-1][0]][3]['href'][0]
			if verbose:
				print 'link:\t%s\t|%s|' % (x[-1], href)
			aa.append((x[-1][0], self.T_MORE_LK, href))
		ra = self.get_indices(['moreResultLink', 'cdo-link'])
		for x in ra:
			href = self.tagset[x[-1][0]][3]['href'][0]
			if verbose:
				print 'href:\t%s\t|%s|' % (x[-1], href)
			aa.append((x[-1][0], self.T_MORE_ALL, href))
		return aa
	# end of garner					// }
	### GHK: (word, def, examples)
	###	def may be None
	###	examples may be None
	###
	### return:
	###	([(word, (uk_mp3, uk_ogg, us_mp3, us_ogg), pos, gw,[def, [examples]])], [(hw, pos, gw)])
	def collect(self, aa, verbose = False):	#	// {
		raa = sorted(aa, key = lambda x: x[0])
		assert(len(raa))
		(i, T_X, data) = raa[0]
		assert(T_X == self.T_REP or T_X == self.T_PV)
		rep = data
		daa = []	# list to be displayed
		daa.append(data)
		#print '%s: |%s|' % ('REP' if T_X == self.T_REP else 'PV', rep)
		aa = []
		w = rep	# mandatory
		uk_mp3 = None
		uk_ogg = None
		us_mp3 = None
		us_ogg = None
		pos = None	# mandatory
		gw = None	# optional
		ipa = []
		a = []		# (defn, examples)
		defn = None
		examples = []
		for e in raa[1:]:
			(i, T_X, data) = e
			if T_X == self.T_WORD:
				if defn or len(examples):
					a.append((defn, examples))
					defn = None
					examples = []
				if w:
					assert(pos)
					aa.append((w, '/, /'.join(ipa), (uk_mp3, uk_ogg, us_mp3, us_ogg), pos, gw, a))
					uk_mp3 = None
					uk_ogg = None
					us_mp3 = None
					us_ogg = None
					pos = None
					gw = None
					ipa = []
					a = []
				w = data
			elif T_X == self.T_POS:
				#assert(pos is None)
				if pos:
					pos = pos + ',' + data
				else:
					pos = data
			elif T_X == self.T_GW:
				gw = data
			elif T_X == self.T_DEF:
				if defn or len(examples):
					a.append((defn, examples))
					defn = None
					examples = []
				defn = data
			elif T_X == self.T_EXP:
				examples.append(data)
			elif T_X == self.T_IPA:
				#ipa = data
				ipa.append(data)
			elif T_X == self.T_UK_MP3:
				assert(not uk_mp3)
				uk_mp3 = data
			elif T_X == self.T_UK_OGG:
				assert(not uk_ogg)
				uk_ogg = data
			elif T_X == self.T_US_MP3:
				#assert(not us_mp3)
				if us_mp3:	# Cambridge bug
					sys.stderr.write('WARN: two us_mpg: bug\n')
					assert(us_mpg == data)
					#print us_mp3
					#print data
				us_mp3 = data
			elif T_X == self.T_US_OGG:
				#assert(not us_ogg)
				if us_ogg:	# Cambridge bug
					sys.stderr.write('WARN: two us_ogg: bug\n')
					assert(us_ogg == data)
					#print us_ogg
					#print data
				us_ogg = data
		if defn or len(examples):
			a.append((defn, examples))
			defn = None
			examples = []
		if w or len(a):
			assert(w)
			#assert(pos)
			aa.append((w, '/, /'.join(ipa), (uk_mp3, uk_ogg, us_mp3, us_ogg), pos, gw, a))
			uk_mp3 = None
			uk_ogg = None
			us_mp3 = None
			us_ogg = None
			pos = None
			gw = None
			ipa = None
			a = []
		a = []	# unique
		return (aa, a)
	# end of collect				// }
	def get_word_index(self, word, aa):
		index = 0	# default: first word, i.e., representative word
		for i in range(len(aa)):
			(w, ipa, audio, pos, gw, a) = aa[i]
			if w == word:
				index = i
				break
		return index
	def get_all(self):
		aa = self.garner()
		if not aa:
			return
		return self.collect(aa)
	###	[(word, (uk_mp3, uk_ogg, us_mp3, us_ogg), pos, [def, [examples]])]
	def set_display(self, (aa, more_a), verbose = False):	#	// {
		ra = []	# word,pos,gw
			# def
			# example
		column0, lines0 = MultiLine.getTerminalSize()
		for ee in aa:
			(w, ipa, audio, pos, gw, a) = ee
			text_a = []
			text_a.append((Word_Smart.COLOR_TUPLE_WORDS_WORD, w))
			if ipa:
				text_a.append((Word_Smart.COLOR_TUPLE_WORDS_IPA, ' ' + '/' + ipa + '/'))
			if pos:
				text_a.append((Word_Smart.COLOR_TUPLE_WORDS_POS, ' (' + pos + ')'))
			if gw:
				text_a.append((Word_Smart.COLOR_TUPLE_WORDS_GW, ' [' + gw + ']'))
			hi = 0
			lm = 0
			rm = 8
			tup = (column0, lm, rm, hi)
			r = MultiLine.multi_line(text_a, tup)
			#print r
			ra.append(r)
			for e in a:
				(defn, examples) = e
				if defn:
					text_a = [(Word_Smart.COLOR_TUPLE_WORDS_SENSE, defn)]
					hi = 0
					lm = 0
					rm = 8
					tup = (column0, lm, rm, hi)
					r = MultiLine.multi_line(text_a, tup)
					#print r
					ra.append(r)
				for x in examples:
					text_a = [(('cyan', 'bold', False), '· ')]
					text_a.append((Word_Smart.COLOR_TUPLE_WORDS_EXAMPLE, x))
					lm = 6
					rm = 4
					hi = -2
					tup = (column0, lm, rm, hi)
					r = MultiLine.multi_line(text_a, tup)
					#print r
					ra.append(r)
		assert(ra)
		a = []	# word/phrasal verb,pos,gw
			# href
		link_a = []	# href for next search
		for (more_lk, more_xxx, more_pos, more_gw, arln) in more_a:
			link_a.append(more_lk)
			if self.follow_link:
				continue
			text_a = []
			text_a.append((Word_Smart.COLOR_TUPLE_WORDS_WORD, more_xxx))
			word_a = []
			color_a = []
			if more_pos:
				text_a.append((Word_Smart.COLOR_TUPLE_WORDS_POS, ' (' + more_pos + ')'))
			if more_gw:
				text_a.append((Word_Smart.COLOR_TUPLE_WORDS_GW, ' [' + more_gw + ']'))
			lm = 2
			rm = 8
			hi = -2
			tup = (column0, lm, rm, hi)
			r = MultiLine.multi_line(text_a, tup)
			r = r + "\n    " + AnsiText.color_text(more_lk, Word_Smart.COLOR_TUPLE_WORDS_LK)
			a.append(r)
		return (ra, a, link_a)
	# end of set_display					// }
	"""
    <ul id="cdo-spellcheck" class="bullet">
                    <li><a class="cdo-link" href="http://dictionary.cambridge.or
                    <li><a class="cdo-link" href="http://dictionary.cambridge.or
                    <li><a class="cdo-link" href="http://dictionary.cambridge.or
                    <li><a class="cdo-link" href="http://dictionary.cambridge.or
                    <li><a class="cdo-link" href="http://dictionary.cambridge.or
                    <li><a class="cdo-link" href="http://dictionary.cambridge.or
                    <li><a class="cdo-link" href="http://dictionary.cambridge.or
                    <li><a class="cdo-link" href="http://dictionary.cambridge.or
                    <li><a class="cdo-link" href="http://dictionary.cambridge.or
                    <li><a class="cdo-link" href="http://dictionary.cambridge.or
            </ul>
	"""
	def trythese(self, word):
		print '|%s|: not found' % word
		print 'try these words:'
		ra = self.get_indices(['bullet', 'cdo-link'])
		for x in ra:
			r = self.get_text(x[-1])
			print '\t|%s|' % r
		return

	def select(self, href):
		self.Cambridge_dbcursor.execute('SELECT rowid,blob FROM href WHERE href LIKE ?', (href,))
		return self.Cambridge_dbcursor.fetchone()
	def insert_href(self, href, (aa, more_a)):
		### r2
		### self.Cambridge_dbcursor.execute('INSERT OR REPLACE INTO cambridge (href, blob) VALUES (?, ?)', (href, '%s' % ((aa, more_a),)))
		try:
			self.Cambridge_dbcursor.execute('INSERT INTO href (href, blob) VALUES (?, ?)', (href, '%s' % ((aa, more_a),)))
			self.Cambridge_dbconnection.commit()
			self.Cambridge_dbcursor.execute('SELECT last_insert_rowid()')
			return self.Cambridge_dbcursor.fetchone()[0]
		except sqlite3.IntegrityError, msg:
			if str(msg) != 'column href is not unique':
				sys.stderr.write('ERROR: |%s|\n' % msg)
				return
			# update "blob" only
			"""
UPDATE table_name
SET column1=value, column2=value2,...
WHERE some_column=some_value
			"""
			rowid = self.select(href)[0]
			assert(rowid)
			self.Cambridge_dbcursor.execute('UPDATE href SET blob = ? WHERE rowid = ?', ('%s' % ((aa, more_a),), rowid))
			# trigger
			self.Cambridge_dbcursor.execute('DELETE FROM word WHERE href_id = ?', (rowid,))
			self.Cambridge_dbconnection.commit()
			return rowid
	def insert_word(self, (word, href_id)):
		### r2
		### self.Cambridge_dbconnection.execute('INSERT OR IGNORE INTO dictionary (word, href) VALUES (?, ?)', (word, href))
		try:
			self.Cambridge_dbcursor.execute('INSERT INTO word (word, href_id) VALUES (?, ?)', (word, href_id))
		except sqlite3.IntegrityError, msg:
			if str(msg) != 'columns word, href_id are not unique':
				sys.stderr.write('ERROR: |%s|\n' % msg)
			pass
		self.Cambridge_dbconnection.commit()

	# core
	def lookup0(self, word, url):	#	// {
		assert((word is None and url is not None) or (word is not None and url is None))
		if url:
			href0 = href = url
		else:
			if self.british:
				href = 'http://dictionary.cambridge.org/search/british/direct/?q=%s' % urllib.quote_plus(word)
			else:
				href = 'http://dictionary.cambridge.org/search/american-english/direct/?q=%s' % urllib.quote_plus(word)
			signature = "url_effective"
			n = len(signature)
			cmd = 'curl -L -I -o /dev/null -w "%s: |%%{url_effective}|\n" "%s"' % (signature, href)
			p = subprocess.Popen(cmd, shell = True, bufsize = 4096, stdin = subprocess.PIPE, stdout = subprocess.PIPE, stderr = subprocess.PIPE, close_fds = True)
			for _ in p.stdout:
				if _[:n] == signature:
					href = _[n + 3:-2]	# 3: ": |", 2: "|\n"
					href0 = href[:href.rfind('?')]
					break
			assert(href0)
		self.href0 = href0
		# check DB with href0
		if not self.forced:
			r = self.select(href0)
			if r:
				(href0_id, blob) = r
				(aa, more_a) = eval(blob)
				return ((href0, href0_id), (aa, more_a))
				index = 0 if url else self.get_word_index(word, aa)
				return ((href0, href0_id), index, (aa, more_a))

		cmd = 'curl --header "Accept-Encoding: gzip" -L -o- "%s" | gunzip -c' % href	# not href0; to get "trythese" only if "query=..."
		#print cmd
		p = subprocess.Popen(cmd, shell = True, bufsize = 4096, stdin = subprocess.PIPE, stdout = subprocess.PIPE, stderr = subprocess.PIPE, close_fds = True)
		self.reset()
		#s = ''.join(p.stdout.readlines())
		for _ in p.stdout:
			self.feed(_)
		self.close()
		r = self.get_all()
		if not r:
			return
		#(aa, more_a) = r
		return ((href0,), r)
		index = 0 if url else self.get_word_index(word, r[0])
		href0_id = self.insert_href(href0, r)
		return ((href0, href0_id), index, r)
	# end of lookup0		// }

	def lookup(self, wordstr, return_only = False):	#	// {
		# look up Cambridge dictionary
		thisonly = False
		if wordstr[:4].lower() == 'http':
			word = None
			url = wordstr
			thisonly = True
		else:
			word = wordstr
			url = None
		ndot = 1
		sys.stderr.write('.' + self.beep)
		r = self.lookup0(word, url)
		if not r:
			sys.stderr.write('\r \r')
			self.trythese(word)
			return
		(h, (aa, more_a)) = r
		# for play below
		index = 0 if url else self.get_word_index(word, r[1][0])
		(word0, ipar, audio0) = aa[index][:3]
		# from DB
		#	return ((href0, href0_id), (aa, more_a))
		# from Web
		#	return ((href0,), r)	# r = (aa, [])	# [] means more_a not set yet
		href0 = h[0]
		from_DB = len(h) == 2
		if not from_DB:	# from Web
			more_a = self.all_results()
			href0_id = self.insert_href(href0, (aa, more_a))
		else:		# from DB
			href0_id = h[1]
		"""
# from Cambridge::collect
	return (index, aa, more_a)
		more_a = []	# (more_lk, more_hw, more_pos)
a.append((defn, examples))
aa.append((w, (uk_mp3, uk_ogg, us_mp3, us_ogg), pos, gw, a))
more_a.append((more_lk, more_xxx, more_pos, more_gw))
	more_xxx = more_hw
	more_xxx = more_pv
"""
		r = self.set_display((aa, more_a), word)
		(ra, a, link_a) = r
		"""
# from Cambridge::set_display
	return (ra, a, link_a)
ra = []	# word,pos,gw
	# def
	# example
a = []	# word/phrasal verb,pos,gw
	# href
link_a = []	# href for next search
"""

		da = []	# array to be displayed
		sa = []	# array to be saved
		da.append((ra, a))
		if not from_DB:
			sa.append(((href0, href0_id), (aa, more_a)))
		if not thisonly and self.follow_link and link_a:
			for link in link_a:
				#cdic = Cambridge()
				sys.stderr.write('.' + self.beep)
				ndot = ndot + 1
				r = self.lookup0(None, link)
				if not r:
					continue
				(h, (aa, more_a)) = r
				if len(h) == 1:
					href_id = self.insert_href(h[0], (aa, more_a))
					sa.append(((href0, href_id), (aa, more_a)))
				r = self.set_display((aa, more_a), word)
				(ra, a, link_a_xxx) = r
				da.append((ra, a))

		### save sa
		#	sa.append((aa, more_a))
		#	a.append((defn, examples))
		#	aa.append((w, (uk_mp3, uk_ogg, us_mp3, us_ogg), pos, gw, a))
		#	more_a.append((more_lk, more_xxx, more_pos, more_gw))
		for _0_ in sa:
			((href0, href_id), (aa, more_a)) = _0_
			for _1_ in aa:
				(w, ipa, (uk_mp3, uk_ogg, us_mp3, us_ogg), pos, gw, a) = _1_
				self.insert_word((w, href_id))

		### pronunciation
		(uk_mp3, uk_ogg, us_mp3, us_ogg) = audio0
		self.play(us_ogg, word0)

		### print
		daa = []	# array of array to be displayed
		for ee in da:
			(ra, a) = ee
			s = '\n'.join(ra)
			if a:
				#print '\n'.join(a)
				s = s + '\n\n%s\n' % AnsiText.color_text('more (to get all, use "-a" option)', ('yellow', 'normal', False))
				s = s + '\n'.join(a)
			daa.append(s)
		sys.stderr.write('\r%s\r' % ' ' * ndot)
		if daa:
			if return_only:
				return (href0, '\n\n'.join(daa))
			print '\n\n'.join(daa)
		return
	# end of lookup						// }
# end of class Cambridge		// }



if __name__ == "__main__":
	# instantiate the parser and feed it some HTML
	cdic = Cambridge()

	cdic.lookup('word')

	cdic.british = True
	cdic.lookup('word')
	cdic.british = False		# set to default

	cdic.follow_link = True
	cdic.lookup('word')
	cdic.follow_link = False	# set to default
