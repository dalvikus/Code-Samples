#!/usr/bin/python

# $ dircolor -p
# ......
# Below are the color init strings for the basic file types. A color init
# string consists of one or more of the following numeric codes:
# Attribute codes:
# 00=none 01=bold 04=underscore 05=blink 07=reverse 08=concealed
# Text color codes:
# 30=black 31=red 32=green 33=yellow 34=blue 35=magenta 36=cyan 37=white
# Background color codes:
# 40=black 41=red 42=green 43=yellow 44=blue 45=magenta 46=cyan 47=white
# ......
# COLOR_TUPLE_XXX = (color_str, attr_str, background_bool)
#	color_str: ['black', 'red', 'green', 'yellow', 'blue', 'magenta', 'cyan', 'white']
#	attr_str: ['normal', 'bold', 'underline', 'blink', 'reverse', 'conceal']
# N.B. Depending on the type of terminal, some might not work; to test, use test_color function.
COLOR_TUPLE_DEFAULT = ('white', 'normal', False)
def color_text(s, color_tuple, ansi = True):
	if not ansi:
		return s
	color_table = {
		'default': COLOR_TUPLE_DEFAULT[0],	# white on black
		
		'black': 30,
		'red': 31,
		'green': 32,
		'yellow': 33,
		'blue': 34,
		'magenta': 35,
		'cyan': 36,
		'white': 37,
	}
	attr_table = {
		'default': COLOR_TUPLE_DEFAULT[1],

		'normal': '00',
		'bold': '01',
		'underline': '04',
		'blink': '05',
		'reverse': '07',
		'conceal': '08',
	}
	c = color_table['default']
	a = attr_table['default']
	b = COLOR_TUPLE_DEFAULT[2]
	if type(color_tuple) == type(()):
		n = len(color_tuple)
		if n >= 3:
			c, a, b = color_tuple[:3]
		elif n == 2:
			c, a = color_tuple[:2]
		elif n == 1:
			c = color_tuple[0]
	else:
		c = color_tuple
	if not color_table.has_key(c.lower()):
		c = color_table['default']
	color = color_table[c.lower()]
	if not attr_table.has_key(a.lower()):
		a = attr_table['default']
	attr = attr_table[a.lower()]
	if b:
		color = color + 10
	return ('[' + '%s;%dm' % (attr, color)) + s + ('[0m')

def test_color():
	for c in ['red', 'green', 'yellow', 'blue', 'magenta', 'cyan']:
		print c
		for b in [False, True]:
			print '%7s' % b,
			for a in [
				'  normal  ',
				'   bold   ',
				'underline ',
				'  blink   ',
				' reverse  ',
				' conceal  '
			]:
				print color_text('%s' % a, (c, a.strip(), b)),
			print

COLOR_TUPLE_WARN_TITLE = ('yellow', 'bold', False)
COLOR_TUPLE_WARN_TEXT = ('cyan', 'normal', False)
def warn(msg, title = 'WARN'):
	return color_text(title, COLOR_TUPLE_WARN_TITLE) + color_text(": " + msg, COLOR_TUPLE_WARN_TEXT)


if __name__ == "__main__":
	test_color()
