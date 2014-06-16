#!/usr/bin/python
# -*- coding: utf-8 -*- from http://www.python.org/peps/pep-0263.html (http://www.python.org/dev/peps/pep-0263/)

import sys
from webdic0 import webdic
import urllib
import subprocess
import re
import MultiLine
from ws import Word_Smart

class Merriam_Webster(webdic):	#	// {
	def mw_dic_test(self):
		self.walk('sblk')
		self.walk('example-sentences')
	def lookup(self, word, return_only = False):	#	// {
		MW_T_DEF = 0
		MW_T_EXP = 1
		cmd = 'wget -O- "http://www.merriam-webster.com/dictionary/%s"' % urllib.quote_plus(word)
		cmd = 'curl --header "Accept-Encoding: gzip" -L -o- "http://www.merriam-webster.com/dictionary/%s" | gunzip -c' % urllib.quote_plus(word)
		p = subprocess.Popen(cmd, shell = True, bufsize = 4096, stdin = subprocess.PIPE, stdout = subprocess.PIPE, stderr = subprocess.PIPE, close_fds = True)
		s = ''.join(p.stdout.readlines())
		#print s,

		self.reset()
		self.feed(s)
		##self.mw_dic_test()
		r = self.get_indices(['spelling-help'])
		if r:
			print 'not found: |%s|' % word
			return
		ra = self.get_indices(['scnt', 'ssens'])
		aa = []
		i = None
		a = []
		for x in ra:
			r = self.get_text(x[-1])
			#print r
			#print '%s' % ((r, ''),)
			m = re.match('[a-z]\s*:\s*.*', r)
			if m:
				if not i:
					i = x[-1][0]
				a.append(r)
			else:
				if a:
					assert(i)
					aa.append((i, MW_T_DEF, a))
					i = None
					a = []
				aa.append((x[-1][0], MW_T_DEF, [r]))
			#aa.append((x[-1][0], MW_T_DEF, r))
		if a:
			assert(i)
			aa.append((i, self.T_DEF, a))
			i = None
			a = []
		ra = self.get_indices(['example-sentences', 'KonaBody', '<li>'])
		#print ra
		for x in ra:
			r = self.get_text(x[-1])
			#print "\t|%s|" % r
			if r != '[+]more[-]hide':
				aa.append((x[-1][0], MW_T_EXP, r))
		aa = sorted(aa, key = lambda x: x[0])
		a = []
		word_a = []
		color_a = []
		n = 0
		column0, lines = MultiLine.getTerminalSize()
		for ee in aa:
			lm = 0
			hi = 0
			rm = 10
			if ee[1] == MW_T_DEF:
				n = n + 1
				s = '%d. ' % n
				lm = len(s) + 2
				hi = -lm
				text_a = [(Word_Smart.COLOR_TUPLE_WORDS_SENSE, s + ee[2][0])]
				tup = (column0, lm, rm, hi)
				r = MultiLine.multi_line(text_a, tup)
				a.append(r)
				hi = -len(s)
				for e in ee[2][1:]:
					text_a = [(Word_Smart.COLOR_TUPLE_WORDS_SENSE, e)]
					tup = (column0, lm, rm, hi)
					r = MultiLine.multi_line(text_a, tup)
					a.append(r)
			else:
				n = 0
				text_a = [(('cyan', 'bold', False), '· ')]
				text_a.append((Word_Smart.COLOR_TUPLE_WORDS_EXAMPLE, ee[2]))
				hi = -2
				lm = 6
				tup = (column0, lm, rm, hi)
				#print '%s' % ((ee[2],''),)
				r = MultiLine.multi_line(text_a, tup)
				#print r
				a.append(r)
			#a.append(r)
		"""
function au (file, word, pr_text)
        {       popWin('/audio.php?file=' + escape(file) + '&word=' + escape(word) + '&text=' + (typeof(pr_text) != 'undefined' ? pr_text : ''));
        };

(18, '<input type="button" onclick="return au(\'awry0001\', \'awry\');" class="au" title="Listen to the pronunciation of awry"/>', 2)

http://www.merriam-webster.com/audio.php?file=awry0001&word=awry&text=

http://media.merriam-webster.com/soundc11/a/awry0001.wav
"""
		if not self.classref.has_key('au'):
			if self.verbose:
				sys.stderr.write('no audio\n')
		else:
			audio = self.tagset[self.classref['au'][0]]
			#print audio
			m = re.match('.*?onclick="return au\(\\\'((.).*?)\\\'.*', audio[1])
			assert(m)
			#print m.group(1), m.group(2)
			wav_url = "http://media.merriam-webster.com/soundc11/%s/%s.wav" % (m.group(2), m.group(1))
			#print wav_url
			self.play(wav_url, word)

		if len(a):
			if return_only:
				return '\n'.join(a)
			print '\n'.join(a)
	# end of lookup						// }
# end of class Merriam_Webster		// }


if __name__ == "__main__":
	# instantiate the parser and feed it some HTML
	mdic = Merriam_Webster()
	mdic.lookup('artful')
