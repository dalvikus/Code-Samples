#!/usr/bin/python
# -*- coding: utf-8 -*- from http://www.python.org/peps/pep-0263.html (http://www.python.org/dev/peps/pep-0263/)

import sys
from analhtml import analhtml
import re
import subprocess

class webdic(analhtml):	#	// {
	AUDIO_PATH = '/home/icentrik/work/word/vocabulary_mp3/'
	mute = False
	verbose = False

	def play(self, audio_url, word = None):
		if self.mute:
			return
		if not audio_url:
			if self.verbose:
				sys.stderr.write('WARN: no audio info\n')
			return
		m = re.match('.*/((.*)\.(.*))', audio_url)
		assert(m)
		filename = '%s/%s' % (self.AUDIO_PATH, m.group(1))
		try:
			f = open(filename)
			f.close()
		except IOError:
			cmd = 'wget -O "%s" "%s"' % (filename, audio_url)
			p = subprocess.Popen(cmd, shell = True, bufsize = 4096, stdin = subprocess.PIPE, stdout = subprocess.PIPE, stderr = subprocess.PIPE, close_fds = True)
			p.wait()
			if word != m.group(2):
				cmd = 'ln -s "%s" "%s/%s.%s"' % (filename, self.AUDIO_PATH, word, m.group(3))
				p = subprocess.Popen(cmd, shell = True, bufsize = 4096, stdin = subprocess.PIPE, stdout = subprocess.PIPE, stderr = subprocess.PIPE, close_fds = True)
				p.wait()
		cmd = 'play -q "%s"' % filename
		# WINDOWS
		#cmd = "cmdmp3 \"H:\\word\\vocabulary_mp3\\%s\"" % (m.group(1))
		p = subprocess.Popen(cmd, shell = True, bufsize = 4096, stdin = subprocess.PIPE, stdout = subprocess.PIPE, stderr = subprocess.PIPE, close_fds = True)
		return
	pass
# end of webdic					// }


if __name__ == "__main__":
	# instantiate the parser and feed it some HTML
	wd = webdic()
	wd.play(None)
