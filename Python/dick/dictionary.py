#!/usr/bin/python
# -*- coding: utf-8 -*- from http://www.python.org/peps/pep-0263.html (http://www.python.org/dev/peps/pep-0263/)

from cambridge import Cambridge
from merriam_webster import Merriam_Webster
import sys
import getopt
import shlex
from ek import English_Korean
from ws import Word_Smart
import AnsiText
import MultiLine

class dictionary(Cambridge, Merriam_Webster, English_Korean, Word_Smart):	#	// {
	def __init__(self):
		Cambridge.__init__(self)
		Word_Smart.__init__(self)
		English_Korean.__init__(self)

	opt = {}
	opt_str = "ahbuwcWKmsf0"	### (b: british, u: american)
					### (w: merriam-webster, c: cambridge)
					### exclusive
					### W: Word Smart, K: English-Korean
					### m: mute, s: sound only
					### f: forced
					### 0: href0

	def usage(self):
		print """
Usage:
	%s [options with '-'] word or http://...
		""" % (sys.argv[0])
		self.option_usage()
		print
	def option_usage(self, err = None):
		a = []
		if err:
			a.append(err)
		a.append('valid options: "%s"' % self.opt_str)
		a.append('\t"h": help')
		a.append('\t"a": all')
		a.append('\t"u": american english (default)')
		a.append('\t"b": british')
		a.append('\t"w": merriam-webster')
		a.append('\t"c": cambridge (default)')
		a.append('\t"W": Word Smart')
		a.append('\t"K": English-Korean')
		a.append('\t"m": mute')
		a.append('\t"s": sound only')
		a.append('\t"f": forced; ignore cache if any')
		a.append('\t"0": show href0')
		o = sys.stdout
		if err:
			o = sys.stderr
		o.write('%s\n' % '\n'.join(a))
	def check_opt(self, optv):
		opt = {}
		for o in optv:
			val = o[1]
			if o[0] == '-w' and self.opt.has_key('-c'):
				del self.opt['-c']
			if o[0] == '-c' and self.opt.has_key('-w'):
				del self.opt['-w']
			if o[0] == '-b' and self.opt.has_key('-u'):
				del self.opt['-u']
			if o[0] == '-u' and self.opt.has_key('-b'):
				del self.opt['-b']
			opt[o[0]] = val
		return opt
	def get_options(self, opt_a):
		if not len(opt_a):
			return (True, [])
		#print opt_a
		opts = ' '.join(['"%s"' % x for x in opt_a])
		#print opts
		o = []
		r = False
		try:
			optv, o = getopt.getopt(shlex.split(opts), self.opt_str)
			#print optv
			#print o
			opt = self.check_opt(optv)
			for key in opt:
				self.opt[key] = opt[key]
		except getopt.GetoptError, err:
			self.option_usage(str(opts) + ": " + str(err))
			r = True
		if self.opt.has_key('-a'):
			Cambridge.follow_link = True
		else:
			Cambridge.follow_link = False
		if self.opt.has_key('-b'):
			Cambridge.british = True
		if self.opt.has_key('-u'):
			Cambridge.british = False
		if self.opt.has_key('-h'):
			r = True
			self.usage()
		if self.opt.has_key('-m'):
			webdic.mute = True
		if self.opt.has_key('-f'):
			Cambridge.forced = True
		return (r, o)
	def lookup(self, wordstr):	#	// {
		a = []
		ro = True
		if dic.opt.has_key('-w'):
			# look up Merriam-Webster dictionary
			word = wordstr
			rMW = Merriam_Webster.lookup(self, word, return_only = ro)
			if rMW:
				a.append(rMW)
		else:
			rC = Cambridge.lookup(self, wordstr, return_only = ro)
			if rC:
				(href0, r) = rC
				if self.opt.has_key('-0'):
					print href0
				a.append(r)
		if not self.opt.has_key('-s') and self.opt.has_key('-W'):
			rW = Word_Smart.lookup(self, wordstr, return_only = ro)
			if rW:
				a.append(rW.encode('utf-8'))
		if not self.opt.has_key('-s') and self.opt.has_key('-K'):
			rK = English_Korean.lookup(self, wordstr, return_only = ro)
			if rK:
				a.append(rK.encode('utf-8'))
		if not self.opt.has_key('-s') and a:
			#print '/*\n' + '\n */\n/* \n'.join(a) + '\n */'
			column0, lines0 = MultiLine.getTerminalSize()
			delimiter = AnsiText.color_text(' ' * column0, ('white', 'normal', True))
			delimiter = ''
			pre_delimiter = delimiter + '\n' if delimiter else ''
			post_delimiter = '\n' + delimiter if delimiter else ''
			print pre_delimiter + ('\n%s\n' % delimiter).join(a) + post_delimiter
	# end of lookup				// }
# end of class dictionary		// }



if __name__ == "__main__":
	# instantiate the parser and feed it some HTML
	dic = dictionary()

	if len(sys.argv) == 1:
		dic.usage()
		sys.exit(1)
	a = sys.argv[1:]
	while True:
		(r, a) = dic.get_options(a)
		if r:
			# "-h" option or ...
			break
		if not len(a):
			break
		word = a.pop(0)
		dic.lookup(word)
