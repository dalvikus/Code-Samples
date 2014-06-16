#!/usr/bin/python

import AnsiText

# from "http://stackoverflow.com/questions/566746/how-to-get-console-window-width-in-python"
def getTerminalSize():
    def ioctl_GWINSZ(fd):
        try:
            import fcntl, termios, struct, os
            cr = struct.unpack('hh', fcntl.ioctl(fd, termios.TIOCGWINSZ, '1234'))
        except:
            return None
        return cr
    cr = ioctl_GWINSZ(0) or ioctl_GWINSZ(1) or ioctl_GWINSZ(2)
    if not cr:
        try:
            fd = os.open(os.ctermid(), os.O_RDONLY)
            cr = ioctl_GWINSZ(fd)
            os.close(fd)
        except:
            pass
    if not cr:
        try:
            cr = (env['LINES'], env['COLUMNS'])
        except:
            cr = (25, 80)
    return int(cr[1]), int(cr[0])

# on cae use add_line several times in, for example, multi_line2
def add_line((c, t), (mline, length0, space0), (column, left_margin0, left_margin), ansi = True, euckr = False):
	assert(left_margin0 >= 0)
	assert(left_margin >= 0)
	assert(column - left_margin >= 0)
	assert(column - left_margin0 >= 0)

	__0 = ''
	if length0:
		assert(len(mline))
		__0 = mline.pop()

	a = t.split()
	__ = ''		# line comprising words in long_line
	length = 0	# len(__)
	if len(t) and (t[0] == ' ' or t[0] == '\t' or t[0] == '\n'):
		space0 = True
	for w in a:		# {
		if euckr:
			n = len(w.encode('euckr'))
		else:
			n = len(w)

		m0 = 1
		if length0 == 0 or not space0:
			m0 = 0
		n0 = 1 if length else 0

		col = column - (left_margin if len(mline) > 0 else left_margin0)
		if length0 + m0 + length + n0 + n <= col:	# enough space
			if n0:	# or "if length"
				__ = __ + ' ' + w
			else:
				__ = __ + w
			length = length + n0 + n
		else:	# not enough space,
			# so fist aftern completing line,
			if length0 == 0 and length == 0:
				mline.append(AnsiText.color_text(w, c, ansi))
			else:
				# length0 == 0 or length == 0
				if not length:	# __0 only
					mline.append(__0)
					__0 = ''
					length0 = 0
				elif not length0:	# __ only
					mline.append(AnsiText.color_text(__, c, ansi))
					__ = ''
					length = 0
				else:	# both __0 and __
					if m0:
						mline.append(__0 + ' ' + AnsiText.color_text(__, c, ansi))
					else:
						mline.append(__0 + AnsiText.color_text(__, c, ansi))
					__0 = ''
					length0 = 0
					__ = ''
					length = 0
				__ = w
				length = n
	# end of for-loop	# }
	if length:
		m0 = 1
		if length0 == 0 or not space0:
			m0 = 0
		if m0:
			__0 = __0 + ' ' + AnsiText.color_text(__, c, ansi)
		else:
			__0 = __0 + AnsiText.color_text(__, c, ansi)
		length0 = length0 + m0 + length
	if len(t):
		space0 = t[-1] == ' '
	else:
		# inherit space0 from the "parent"
		pass
	if length0:
		mline.append(__0)
	return (length0, space0)

# If one wants to add line several times,
# one can use a template like multi_line2
# note that keep (mline, length0, space0)
# and for final return, use "return ' ' * left_margin0 + ..." in multi_line2
def multi_line2(text_a, (column0, left_margin, right_margin, hanging_indent), ansi = True, euckr = False):
	assert(column0 > 0)
	assert(left_margin >= 0)
	assert(right_margin >= 0)
	column = column0 - right_margin	# column without right margin
	assert(column - left_margin >= 0)
	left_margin0 = left_margin + hanging_indent	# 1st line's left margin
	if left_margin0 < 0:
		left_margin0 = 0
	assert(column - left_margin0 >= 0)

	mline = []
	__0 = ''	# last incomplete line in mline
	length0 = 0	# length of __0
	space0 = False	# tells if there is a space between texts in text_a
			# valid only if __0 is not ''
			# "respect" given input for "space" between text_a;
			# if last character of i-th text and/or
			# fist one of (i+1)-th text, space0 is True;
			# otherwise False
			# in case (i+1)-th text is NULL, the test is done
			# with (i+2)-th text or so on.
			# So, if one wants to insert space between text_a,
			# be sure to have a space end of i-th text or
			# start of (i+1)-th text
	for ct in text_a:
		(length0, space0) = add_line(ct, (mline, length0, space0), (column, left_margin0, left_margin), ansi, euckr)
	return ' ' * left_margin0 + ("\n" + ' ' * left_margin).join(mline)

def multi_line(text_a, (column0, left_margin, right_margin, hanging_indent), ansi = True, euckr = False):
	assert(column0 > 0)
	assert(left_margin >= 0)
	assert(right_margin >= 0)
	column = column0 - right_margin	# column without right margin
	assert(column - left_margin >= 0)
	left_margin0 = left_margin + hanging_indent	# 1st line's left margin
	if left_margin0 < 0:
		left_margin0 = 0
	assert(column - left_margin0 >= 0)

	mline = []
	__0 = ''	# last incomplete line in mline
	length0 = 0	# length of __0
	space0 = False	# tells if there is a space between texts in text_a
			# valid only if __0 is not ''
			# respect given input;
			# if last character of i-th text and/or
			# fist one of (i+1)-th text, space0 is True;
			# otherwise False
			# in case (i+1)-th text is NULL, the test is done
			# with (i+2)-th text or so on.
	for ct in text_a:	# {
###############################################################################
# part between ###...### can be separated
# in order to deal with line-by-line input
# with minor correction for length0, space0
# (see multi_line2 and add_line)
		(c, t) = ct
		#print "|%s|" % t

		a = t.split()
		__ = ''		# line comprising words in long_line
		length = 0	# len(__)
		if length0 and len(t) and (t[0] == ' ' or t[0] == '\t' or t[0] == '\n'):
			space0 = True
		for w in a:		# {
			if euckr:
				n = len(w.encode('euckr'))
			else:
				n = len(w)

			m0 = 1
			if length0 == 0 or not space0:
				m0 = 0
			n0 = 1 if length else 0

			col = column - (left_margin if len(mline) > 0 else left_margin0)
			if length0 + m0 + length + n0 + n <= col:	# enough space
				if n0:	# or "if length"
					__ = __ + ' ' + w
				else:
					__ = __ + w
				length = length + n0 + n
			else:	# not enough space,
				# so fist aftern completing line,
				if length0 == 0 and length == 0:
					mline.append(AnsiText.color_text(w, c, ansi))
				else:
					# length0 == 0 or length == 0
					if not length:	# __0 only
						mline.append(__0)
						__0 = ''
						length0 = 0
					elif not length0:	# __ only
						mline.append(AnsiText.color_text(__, c, ansi))
						__ = ''
						length = 0
					else:	# both __0 and __
						if m0:
							mline.append(__0 + ' ' + AnsiText.color_text(__, c, ansi))
						else:
							mline.append(__0 + AnsiText.color_text(__, c, ansi))
						__0 = ''
						length0 = 0
						__ = ''
						length = 0

					__ = w
					length = n
		# end of for-loop	# }
		if length:
			m0 = 1
			if length0 == 0 or not space0:
				m0 = 0
			if m0:
				__0 = __0 + ' ' + AnsiText.color_text(__, c, ansi)
			else:
				__0 = __0 + AnsiText.color_text(__, c, ansi)
			length0 = length0 + m0 + length
		if len(t):
			space0 = t[-1] == ' '
		else:
			# inherit space0 from the "parent"
			pass
###############################################################################
	# end of for-loop	# }
	if length0:
		mline.append(__0)
	return ' ' * left_margin0 + ("\n" + ' ' * left_margin).join(mline)

"""
||||| hanging_indent
.....................
.................
...........
||||||| left_margin
"""
def make_ruler(column0, left_margin, right_margin, hanging_indent, tick = False):
	left_margin0 = left_margin
	left_margin0 = left_margin + hanging_indent
	if left_margin0 < 0:
		left_margin0 = 0

	n = column0 / 10
	if column0 % 10:
		n = n + 1
	s = ''
	if tick:
		for i in range(n):
			s = s + '%10s' % (1 + i)
		s = s[:column0] + "\n";
		s = s + ('1234567890' * n)[:column0] + "\n";
	if left_margin0 > left_margin:
		s = s + ' ' * left_margin + '|' + ' ' * (left_margin0 - left_margin - 1) + '>'
		lm = left_margin0
	elif left_margin > left_margin0:
		s = s + ' ' * left_margin0 + '<' + ' ' * (left_margin - left_margin0 - 1) + '|'
		lm = left_margin
	else:	# hanging_indent = 0 (left_margin0 = left_margin)
		s = s + ' ' * left_margin0 + '|'
		lm = left_margin
	column = column0 - right_margin
	s = s + ' ' * (column - (lm + 1) - 1) + '|' + "\n";
	return s

if __name__ == '__main__':
	text_a = [
		(
			('cyan', 'normal', False),
			'This word is very often misused. Be careful. Players do not "',
		),
		(
			('green', 'normal', True),
			'comprise',
		),
		(
			('cyan', 'normal', False),
			'" a football team, and employees do not "',
		),
		(
			('green', 'normal', True),
			'comprise',
		),
		(
			('cyan', 'normal', False),
			'" a company. Nor can a football team be said to be "',
		),
		(
			('cyan', 'normal', True),
			'comprised',
		),
		(
			('cyan', 'normal', False),
			' of" players, or a company to be "',
		),
		(
			('cyan', 'normal', True),
			'comprised',
		),
		(
			('cyan', 'normal', False),
			' of" employees. These are very common mistakes. Instead you can say that players ',
		),
		(
			('yellow', 'normal', True),
			'constitute or compose',
		),
		(
			('cyan', 'normal', False),
			' a team, and that employees ',
		),
		(
			('yellow', 'normal', False),
			'constitute or compose',
		),
		(
			('cyan', 'normal', False),
			' a company.',
		),
	]
	text = ''
	for ct in text_a:
		text = text + ct[1]
	print text

	column0, lines0 = getTerminalSize()
	left_margin = 4
	right_margin = 10
	hanging_indent = -4
	print make_ruler(column0, left_margin, right_margin, hanging_indent, tick = True),
	print multi_line(text_a, (column0, left_margin, right_margin, hanging_indent), ansi = True)
	print multi_line2(text_a, (column0, left_margin, right_margin, hanging_indent), ansi = False)
