#!/usr/bin/python
# -*- coding: utf-8 -*- from http://www.python.org/peps/pep-0263.html (http://www.python.org/dev/peps/pep-0263/)

import sys

from HTMLParser import HTMLParser
#import HTMLParser
from htmlentitydefs import name2codepoint
from time import sleep



### GHK: based on the example in Python document:
###	19.1. HTMLParser — Simple HTML and XHTML parser
### From Python document:
###	The user should subclass HTMLParser and override its methods to implement the desired behavior.
# create a subclass and override the handler methods
class myHTMLParser(HTMLParser):	#	// {
	verbose = False

	depth = -1
	### From Python document:
	###	Unlike the parser in htmllib, this parser does not check that end tags match start tags or call the end-tag handler for elements which are closed implicitly by closing an outer element.
	### GHK:
	###	so, depth is not corrent since most hypertext documents
	###	are not PERFECT; it is meanlingless to juxtapost two tag with equal depth.
	###	nevertheless, when combined with seq, depth is meaningful;

	### From Python document for HTMLParser.get_starttag_text():
	###	may be useful in dealing with HTML “as deployed” or for re-generating input with minimal changes (whitespace between attributes can be preserved, etc.)
	rawdoc = ''

	"""
	depth(actually indent): see tagset.append() in each handler
		TYPE_STARTEND_TAG
			TYPE_STARTEND_TAG
			TYPE_COMMENT
			TYPE_DATA
			TYPE_ENTITYREF
			TYPE_CHARREF
		TYPE_END_TAG
		TYPE_DECL
	"""
	TYPE_START_TAG = 0
	TYPE_END_TAG = 1
	TYPE_STARTEND_TAG = 2
	TYPE_DATA = 3
	TYPE_ENTITYREF = 4
	TYPE_CHARREF = 5
	TYPE_COMMENT = 6
	TYPE_DECL = 7
	tagset = []	# tuple of (depth, rawdata, TYPE_XXX, {name: val})
			# {name: val} optional; only for start and startend tag
	tagref = {}	# ref. of "tag"
	classref = {}	# ref. of "class" in start tag

	### From Python document:
	###	The following methods are called when data or markup elements are encountered and they are meant to be overridden in a subclass. The base class implementations do nothing (except for handle_startendtag()):
	### GHK: ten methods to be called
	def handle_starttag(self, tag, attrs):
		if self.verbose:
			print 'handle_starttag: |%s|: |%s|' % (tag, attrs)
		self.depth = self.depth + 1
		### From Python document:
		###	The tag argument is the name of the tag converted to lower case. The attrs argument is a list of (name, value) pairs containing the attributes found inside the tag’s <> brackets. The name will be translated to lower case, and quotes in the value have been removed, and character and entity references have been replaced.
		### GHK:
		###	so, to get the "rawdoc" data,
		##	use HTMLParser.get_starttag_text()
		s = self.get_starttag_text()
		assert(s)
		if self.verbose:
			print 'handle_starttag: last opened start tag:= |%s|' % s
		self.rawdoc = self.rawdoc + s

		last_index = len(self.tagset)
		# tagref
		if not self.tagref.has_key(tag):
			self.tagref[tag] = []
		self.tagref[tag].append(last_index)
		# classref
		d = {}
		for (name, val) in attrs:
			if not d.has_key(name):
				d[name] = []
			d[name].append(val)
		for key in ['class', 'id']:
			if d.has_key(key):
				assert(len(d[key]) == 1)
				class_str = d[key][0]
				if not self.classref.has_key(class_str):
					self.classref[class_str] = []
				self.classref[class_str].append(last_index)
		self.tagset.append((self.depth, s, self.TYPE_START_TAG, d, tag))
		return
	def handle_endtag(self, tag):
		if self.verbose:
			print 'handle_endtag: |%s|' % (tag)
		self.depth = self.depth - 1
		### GHK:
		###	there is no way to tell "</HTML>" from
		###	"</ HTML >" (blanks), "</HTmL>" (case)
		###	so, just use given tag
		if self.verbose:
			s = self.get_starttag_text()
			print 'handle_endtag: last opened start tag:= |%s|' % s
		rawendtag = "</%s>" % tag
		self.rawdoc = self.rawdoc + rawendtag
		self.tagset.append((self.depth + 1, rawendtag, self.TYPE_END_TAG, tag))
		return
	### From Python document:
	###	called when the parser encounters an XHTML-style empty tag (<img ... />). This method may be overridden by subclasses which require this particular lexical information; the default implementation simply calls handle_starttag() and handle_endtag().
	def handle_startendtag(self, tag, attrs):
		if self.verbose:
			print 'handle_startendtag: |%s|: |%s|' % (tag, attrs)
		s = self.get_starttag_text()
		assert(s)
		if self.verbose:
			print 'handle_startendtag: last opened start tag:= |%s|' % s
		self.rawdoc = self.rawdoc + s

		last_index = len(self.tagset)
		# tagref
		if not self.tagref.has_key(tag):
			self.tagref[tag] = []
		self.tagref[tag].append(last_index)
		# classref
		d = {}
		for (name, val) in attrs:
			if not d.has_key(name):
				d[name] = []
			d[name].append(val)
		for key in ['class', 'id']:
			if d.has_key(key):
				assert(len(d[key]) == 1)
				class_str = d[key][0]
				if not self.classref.has_key(class_str):
					self.classref[class_str] = []
				self.classref[class_str].append(last_index)
		self.tagset.append((self.depth + 1, s, self.TYPE_STARTEND_TAG, d, tag))
		return
	def handle_data(self, data):
		if self.verbose:
			print 'handle_data: |%s|' % (data)
		if self.verbose:
			s = self.get_starttag_text()
			print 'handle_data: last opened start tag:= |%s|' % s
		self.rawdoc = self.rawdoc + data
		self.tagset.append((self.depth + 1, data, self.TYPE_DATA))
		return
	### character entity reference
	### http://en.wikipedia.org/wiki/Character_entity_reference
	def handle_entityref(self, name):
		if self.verbose:
			print "handle_entityref: |%s|" % (name)
		### From Python document:
		###	This method is called to process a named character reference of the form &name; (e.g. &gt;), where name is a general entity reference (e.g. 'gt').
		### GHK:
		###	no way to tell "&gt;" from
		###	"&GT;" (case), "&gt ;" (blank)
		###	so, to recover the "rawdoc" data,
		###	use the lower case only: "&gt;"
		if self.verbose:
			s = self.get_starttag_text()
			print 'handle_entityref: last opened start tag:= |%s|' % s
		self.rawdoc = self.rawdoc + "&%s;" % name.lower()
		try:
			c = unichr(name2codepoint[name])
		except KeyError:
			sys.stderr.write('myHTMLParser::handle_entityref: KeyError: |%s|\n' % name)
			c = name
		self.tagset.append((self.depth + 1, c.encode('utf-8'), self.TYPE_ENTITYREF))
		#print "Named ent: |%s|" % c
		return
	### numeric character reference
	### http://en.wikipedia.org/wiki/Numeric_character_reference
	"""
http://en.wikipedia.org/wiki/Character_encodings_in_HTML
========================================================
Character encodings in HTML
  Character references
    Illegal characters

HTML forbids[8] the use of the characters with Universal Character Set/Unicode code points

    0 to 31, except 9, 10, and 13 (C0 control characters)
    127 (DEL character)
    128 to 159 (C1 control characters)
    55296 to 57343 (xD800–xDFFF, the UTF-16 surrogate halves)

These characters are not even allowed by reference. That is, you should not even write them as numeric character references. However, references to characters 128–159 are commonly interpreted by lenient web browsers as if they were references to the characters assigned to bytes 128–159 (decimal) in the Windows-1252 character encoding. This is in violation of HTML and SGML standards, and the characters are already assigned to higher code points, so HTML document authors should always use the higher code points. For example, for the trademark sign (™), use &#8482;, not &#153;.

The characters 9 (tab), 10 (linefeed), and 13 (carriage return) are allowed in HTML documents, but, along with 32 (space) are all considered "whitespace".[9] The "form feed" control character, which would be at 12, is not allowed in HTML documents, but is also mentioned as being one of the "white space" characters — perhaps an oversight in the specifications. In HTML, most consecutive occurrences of white space characters, except in a <pre> block, are interpreted as comprising a single "word separator" for rendering purposes. A word separator is typically rendered a single en-width space in European languages, but not in others.


http://en.wikipedia.org/wiki/Windows-1252
=========================================
Windows-1252
  Code page layout

...Differences from ISO-8859-1 are marked with thick green borders.(128-159)

According to the information on Microsoft's and the Unicode Consortium's websites, positions 81, 8D, 8F, 90, and 9D are unused; however, the Windows API MultiByteToWideChar maps these to the corresponding C1 control codes.

The euro character at position 80 was not present in earlier versions of this code page, nor were the S, s, Z, and z with caron (háček).


http://www.w3.org/TR/REC-html40/sgml/entities.html
==================================================
24 Character entity references in HTML 4
  24.4 Character entity references for markup-significant and internationalization characters

Entities have also been added for the remaining characters occurring in CP-1252 which do not occur in the HTMLlat1 or HTMLsymbol entity sets. These all occur in the 128 to 159 range within the CP-1252 charset. These entities permit the characters to be denoted in a platform-independent manner.

<!-- C0 Controls and Basic Latin -->
<!ENTITY quot    CDATA "&#34;"   -- quotation mark = APL quote,
                                    U+0022 ISOnum -->
<!ENTITY amp     CDATA "&#38;"   -- ampersand, U+0026 ISOnum -->
<!ENTITY lt      CDATA "&#60;"   -- less-than sign, U+003C ISOnum -->
<!ENTITY gt      CDATA "&#62;"   -- greater-than sign, U+003E ISOnum -->

<!-- Latin Extended-A -->
	<!ENTITY OElig   CDATA "&#338;"  -- latin capital ligature OE,
                                    U+0152 ISOlat2 -->
	<!ENTITY oelig   CDATA "&#339;"  -- latin small ligature oe, U+0153 ISOlat2 -->
<!-- ligature is a misnomer, this is a separate character in some languages -->
	<!ENTITY Scaron  CDATA "&#352;"  -- latin capital letter S with caron,
                                    U+0160 ISOlat2 -->
	<!ENTITY scaron  CDATA "&#353;"  -- latin small letter s with caron,
                                    U+0161 ISOlat2 -->
	<!ENTITY Yuml    CDATA "&#376;"  -- latin capital letter Y with diaeresis,
                                    U+0178 ISOlat2 -->

<!-- Spacing Modifier Letters -->
	<!ENTITY circ    CDATA "&#710;"  -- modifier letter circumflex accent,
                                    U+02C6 ISOpub -->
	<!ENTITY tilde   CDATA "&#732;"  -- small tilde, U+02DC ISOdia -->

<!-- General Punctuation -->
<!ENTITY ensp    CDATA "&#8194;" -- en space, U+2002 ISOpub -->
<!ENTITY emsp    CDATA "&#8195;" -- em space, U+2003 ISOpub -->
<!ENTITY thinsp  CDATA "&#8201;" -- thin space, U+2009 ISOpub -->
<!ENTITY zwnj    CDATA "&#8204;" -- zero width non-joiner,
                                    U+200C NEW RFC 2070 -->
<!ENTITY zwj     CDATA "&#8205;" -- zero width joiner, U+200D NEW RFC 2070 -->
<!ENTITY lrm     CDATA "&#8206;" -- left-to-right mark, U+200E NEW RFC 2070 -->
<!ENTITY rlm     CDATA "&#8207;" -- right-to-left mark, U+200F NEW RFC 2070 -->
	<!ENTITY ndash   CDATA "&#8211;" -- en dash, U+2013 ISOpub -->
	<!ENTITY mdash   CDATA "&#8212;" -- em dash, U+2014 ISOpub -->
	<!ENTITY lsquo   CDATA "&#8216;" -- left single quotation mark,
                                    U+2018 ISOnum -->
	<!ENTITY rsquo   CDATA "&#8217;" -- right single quotation mark,
                                    U+2019 ISOnum -->
	<!ENTITY sbquo   CDATA "&#8218;" -- single low-9 quotation mark, U+201A NEW -->
	<!ENTITY ldquo   CDATA "&#8220;" -- left double quotation mark,
                                    U+201C ISOnum -->
	<!ENTITY rdquo   CDATA "&#8221;" -- right double quotation mark,
                                    U+201D ISOnum -->
	<!ENTITY bdquo   CDATA "&#8222;" -- double low-9 quotation mark, U+201E NEW -->
	<!ENTITY dagger  CDATA "&#8224;" -- dagger, U+2020 ISOpub -->
	<!ENTITY Dagger  CDATA "&#8225;" -- double dagger, U+2021 ISOpub -->
	<!ENTITY permil  CDATA "&#8240;" -- per mille sign, U+2030 ISOtech -->
	<!ENTITY lsaquo  CDATA "&#8249;" -- single left-pointing angle quotation mark,
                                    U+2039 ISO proposed -->
<!-- lsaquo is proposed but not yet ISO standardized -->
	<!ENTITY rsaquo  CDATA "&#8250;" -- single right-pointing angle quotation mark,
                                    U+203A ISO proposed -->
<!-- rsaquo is proposed but not yet ISO standardized -->
	<!ENTITY euro   CDATA "&#8364;"  -- euro sign, U+20AC NEW -->


http://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WindowsBestFit/bestfit1252.txt
====================================================================================
......
0x80	0x20ac	;Euro Sign
0x81	0x0081
0x82	0x201a	;Single Low-9 Quotation Mark
0x83	0x0192	;Latin Small Letter F With Hook
0x84	0x201e	;Double Low-9 Quotation Mark
0x85	0x2026	;Horizontal Ellipsis
0x86	0x2020	;Dagger
0x87	0x2021	;Double Dagger
0x88	0x02c6	;Modifier Letter Circumflex Accent
0x89	0x2030	;Per Mille Sign
0x8a	0x0160	;Latin Capital Letter S With Caron
0x8b	0x2039	;Single Left-Pointing Angle Quotation Mark
0x8c	0x0152	;Latin Capital Ligature Oe
0x8d	0x008d
0x8e	0x017d	;Latin Capital Letter Z With Caron
0x8f	0x008f
0x90	0x0090
0x91	0x2018	;Left Single Quotation Mark
0x92	0x2019	;Right Single Quotation Mark
0x93	0x201c	;Left Double Quotation Mark
0x94	0x201d	;Right Double Quotation Mark
0x95	0x2022	;Bullet
0x96	0x2013	;En Dash
0x97	0x2014	;Em Dash
0x98	0x02dc	;Small Tilde
0x99	0x2122	;Trade Mark Sign
0x9a	0x0161	;Latin Small Letter S With Caron
0x9b	0x203a	;Single Right-Pointing Angle Quotation Mark
0x9c	0x0153	;Latin Small Ligature Oe
0x9d	0x009d
0x9e	0x017e	;Latin Small Letter Z With Caron
0x9f	0x0178	;Latin Capital Letter Y With Diaeresis
......

0x81	0x0081
0x8d	0x008d
0x8e	0x017d	;Latin Capital Letter Z With Caron
0x8f	0x008f
0x90	0x0090
0x9d	0x009d
0x9e	0x017e	;Latin Small Letter Z With Caron
"""
	### http://en.wikipedia.org/wiki/Character_encodings_in_HTML
	### http://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WindowsBestFit/bestfit1252.txt
	bestfit1252 = {
		0x80:	0x20ac,	#	;Euro Sign
		0x81:	0x0081,	#
		0x82:	0x201a,	#	;Single Low-9 Quotation Mark
		0x83:	0x0192,	#	;Latin Small Letter F With Hook
		0x84:	0x201e,	#	;Double Low-9 Quotation Mark
		0x85:	0x2026,	#	;Horizontal Ellipsis
		0x86:	0x2020,	#	;Dagger
		0x87:	0x2021,	#	;Double Dagger
		0x88:	0x02c6,	#	;Modifier Letter Circumflex Accent
		0x89:	0x2030,	#	;Per Mille Sign
		0x8a:	0x0160,	#	;Latin Capital Letter S With Caron
		0x8b:	0x2039,	#	;Single Left-Pointing Angle Quotation Mark
		0x8c:	0x0152,	#	;Latin Capital Ligature Oe
		0x8d:	0x008d,	#
		0x8e:	0x017d,	#	;Latin Capital Letter Z With Caron
		0x8f:	0x008f,	#
		0x90:	0x0090,	#
		0x91:	0x2018,	#	;Left Single Quotation Mark
		0x92:	0x2019,	#	;Right Single Quotation Mark
		0x93:	0x201c,	#	;Left Double Quotation Mark
		0x94:	0x201d,	#	;Right Double Quotation Mark
		0x95:	0x2022,	#	;Bullet
		0x96:	0x2013,	#	;En Dash
		0x97:	0x2014,	#	;Em Dash
		0x98:	0x02dc,	#	;Small Tilde
		0x99:	0x2122,	#	;Trade Mark Sign
		0x9a:	0x0161,	#	;Latin Small Letter S With Caron
		0x9b:	0x203a,	#	;Single Right-Pointing Angle Quotation Mark
		0x9c:	0x0153,	#	;Latin Small Ligature Oe
		0x9d:	0x009d,	#
		0x9e:	0x017e,	#	;Latin Small Letter Z With Caron
		0x9f:	0x0178,	#	;Latin Capital Letter Y With Diaeresis
	}
	def handle_charref(self, name):
		if self.verbose:
			print "handle_charref: |%s|" % (name)
		### From Python document:
		###	This method is called to process decimal and hexadecimal numeric character references of the form &#NNN; and &#xNNN;. For example, the decimal equivalent for &gt; is &#62;, whereas the hexadecimal is &#x3E;; in this case the method will receive '62' or 'x3E'.
		### GHK: the same is true as handle_entityref:
		###	so, to recover the "rawdoc" data,
		###	use these: "&62;" or "&x3E" (x followed by upper case)
		if self.verbose:
			s = self.get_starttag_text()
			print 'handle_charref: last opened start tag:= |%s|' % s
		x = name.startswith('x')
		if x:
			n = int(name[1:], 16)
		else:
			n = int(name)
		bestfit1252 = self.bestfit1252
		if bestfit1252.has_key(n):
			if self.verbose:
				sys.stderr.write('WARN: CP-1252(AKA WINDOWS-1252) 0x80-0x9F(128-159) -> UNICODE\n')
			n = bestfit1252[n]
		t = ('%X' if x else '%d') % n
		self.rawdoc = self.rawdoc + "&#%s;" % t
		c = unichr(n).encode('utf-8')
		self.tagset.append((self.depth + 1, c, self.TYPE_CHARREF))
		return
	def handle_comment(self, comment):
		if self.verbose:
			print 'handle_comment: |%s|' % (comment)
		if self.verbose:
			s = self.get_starttag_text()
			print 'handle_comment: last opened start tag:= |%s|' % s
		rawcomment = "<!--%s-->" % comment
		self.rawdoc = self.rawdoc + rawcomment
		self.tagset.append((self.depth + 1, rawcomment, self.TYPE_COMMENT))
		return
	def handle_decl(self, decl):
		if self.verbose:
			print 'handle_decl: |%s|' % (decl)
		if self.verbose:
			s = self.get_starttag_text()
			print 'handle_decl: last opened start tag:= |%s|' % s
		rawdecl = "<!%s>" % decl
		self.rawdoc = self.rawdoc + rawdecl
		self.tagset.append((self.depth, rawdecl, self.TYPE_DECL))
		return
	### GHK:
	###	don't know what this is exactly,
	###	so do not handle this call
	def handle_pi(self, data):
		if self.verbose:
			print 'handle_pi: |%s|' % (data)
		if self.verbose:
			s = self.get_starttag_text()
			print 'handle_pi: last opened start tag:= |%s|' % s
		return
	### GHK: do not handle this call
	def unknown_decl(self, data):
		if self.verbose:
			print 'unknown_decl: |%s|' % (data)
		if self.verbose:
			s = self.get_starttag_text()
			print 'unknown_decl: last opened start tag:= |%s|' % s
		return

	def reset(self):
		self.rawdoc = ''
		self.tagset = []
		self.tagref = {}
		self.classref = {}
		HTMLParser.reset(self)
# end of class myHTMLParser		// }


class analhtml(myHTMLParser):	#	// {

	classindexref = {}
	def reset(self):
		self.classindexref = {}
		myHTMLParser.reset(self)

	def get_index_depth(self, class_name):
		assert(class_name)
		name = class_name
		refonly = self.classref
		if class_name[0] == '<' and class_name[-1] == '>':	# tag
			name = class_name[1:-1]
			refonly = self.tagref
		if self.classindexref.has_key(name):
			return self.classindexref[name]
		if not refonly.has_key(name):
			return []
		a = []
		for start_index in refonly[name]:
			(depth, rawtagdata, type_xxx) = self.tagset[start_index][:3]
			end_index = -1
			#print start_index
			done = False
			for i in range(start_index, len(self.tagset)):
				d = self.tagset[i][0]
				if done and d == depth:
					break
				done = True
			end_index = i
			#print end_index
			a.append((start_index, end_index))
		self.classindexref[name] = a
		return a

	"""
	struct:
		class_name_a[0]
			class_name_a[1]
				......
					class_name_a[-1]

	purpose:
		get indices of class_name_a[-1] in a hierarchy

	return:
		(start_index, end_index) on success
		None on failure
	"""
	def get_indices(self, class_name_a, warn = False):	#	// {
		assert(type(class_name_a) == type([]))
		assert(len(class_name_a))
		ra = []
		try:
			aa = []
			for class_name in class_name_a:
				a = self.get_index_depth(class_name)
				if not len(a):
					return []
				aa.append(a)
			##print aa
			"""
		--------         --------		(si, ei)
			 ------   ------
		..................................
		--  --   --        -- --    --		(start_index, end_index)

			assuming that:
				0) for each (start_index, end_index),
				   there is only one (si, ei)
				1) si < ei
				2) start_index < end_index
			"""
			ra = []
			for x in aa[-1]:
				(start_index, end_index) = x
				#print '(start_index, end_index) = %s' % (x,)
				a = []
				for i in range(len(aa) - 1):
					#print 'i = %d' % i
					found = False
					for k in aa[i]:
						(si, ei) = k
						#print '(si, ei) = %s' % (k,)
						if si > end_index:
							# out of range
							break
						if ei < start_index:
							# try next (si, ei)
							continue
						found = True
						a.append((si, ei))
						break
					if not found:
						# try next (start_index, end_index)
						a = []
						break
				if len(aa) > 1 and len(a) == 0:
					# try next (start_index, end_index)
					continue
				a.append(x)
				ra.append(a)
			##print 'ra: %s' % ra
			return ra
		finally:
			if warn:
				if not len(ra):
					sys.stderr.write('WARN: not found (start_index, end_index) for the hierarchy:\n')
					for i in range(len(class_name_a)):
						sys.stderr.write('WARN:\t\t%s"%s"\n' % (' ' * 2 * i, class_name_a[i]))
				if len(ra) > 1:
					sys.stderr.write("WARN: two or more found (start_index, end_index) for the hierarchy:\n")
					for i in range(len(class_name_a)):
						sys.stderr.write('WARN:\t\t%s"%s"\n' % (' ' * 2 * i, class_name_a[i]))
					for x in ra:
						sys.stderr.write('WARN:\t%s\n' % (x,))
	# end of get_indices						// }
	def get_text(self, (start_index, end_index)):
		assert(start_index < end_index)
		text = ''
		###a = []
		for i in range(start_index, end_index + 1):
			(depth, rawtagdata, type_xxx) = self.tagset[i][:3]
			if type_xxx == self.TYPE_DATA or type_xxx == self.TYPE_ENTITYREF or type_xxx == self.TYPE_CHARREF:
				###a.append(rawtagdata)
				#print 'get_text: rawtagdata: |%s|' % (rawtagdata,)
				text = text + rawtagdata
		###print a
		return text.strip()

	### GHK
	#	check with class_name = "di" (word itself)
	#	and class_name = "result-list" (more results)
	def walk(self, class_name):	#	// {
		assert(class_name)
		#print self.tagset
		"""
		for i in range(len(self.tagset)):
			print 'tagset[%d]:= |%s|' % (i, self.tagset[i])
		"""
		#print self.classref
		if class_name[0] == '<' and class_name[-1] == '>':
			di_index_a = self.tagref[class_name[1:-1]]
		else:
			di_index_a = self.classref[class_name]
		#print di_index_a
		assert(di_index_a)
		assert(len(di_index_a))
		# called with each di_index in di_index_a
		# after this local def.
		def f(di_index):
			di_depth = self.tagset[di_index][0]
			done = False
			for i in range(di_index, len(self.tagset)):
				(depth, rawtagdata, type_xxx) = self.tagset[i][:3]
				data = None
				if type_xxx == self.TYPE_START_TAG or \
				   type_xxx == self.TYPE_END_TAG or \
				   type_xxx == self.TYPE_STARTEND_TAG or \
				   type_xxx == self.TYPE_COMMENT or \
				   type_xxx == self.TYPE_DECL:
					indent = depth
					data = rawtagdata
				else:	# TYPE_DATA, TYPE_ENTITYREF, TYPE_CHARREF
					indent = depth
					data = rawtagdata.strip()
					# |(12, '\xc2\xa0\n      ', 3)|
					#data = rawtagdata.strip(' \t\r\n\xc2\xa0')
					#data = rawtagdata
				#print '%d:\t%s|%s|' % (i, ' ' * indent, rawtagdata),
				indent = indent - di_depth
				if data:
					# 12 enough?
					"""
UnicodeDecodeError: 'ascii' codec can't decode byte 0xeb in position 0: ordinal not in range(128)

UnicodeEncodeError: 'ascii' codec can't encode character u'\xa9' in position 0: ordinal not in range(128)
"""
					print '%-12s%s|%s|\n' % (str("%d(%d)") % (i, type_xxx), ' ' * 2 * indent, data)
					#sys.stdout.write('%s|%s|\n' % (' ' * indent, data))
				if done and depth == di_depth:
					break
				done = True
			return
		for di_index in di_index_a:
			print '-' * 80
			f(di_index)
		return
	# end of walk				// }

	# node
	# start of check_node	// {
	def check_node0(self, (start_index, end_index), depth = 0, trace = (False, 0)):
		tagset = self.tagset
		if trace[0]:
			ndot = 0
		x = []
		a = []	# list of (depth, start tag)
		i = start_index
		while i <= end_index:
			nodeinfo = tagset[i]
			x.append(nodeinfo)
			(depth, raw, t) = nodeinfo[:3]
			if t == self.TYPE_START_TAG:
				# check depth
				if False and len(a) and depth != a[-1][0] + 1:
					if trace[0]:
						sys.stderr.write('\r%s\r' % (' ' * ndot))
					sys.stderr.write('FATAL: depth: not continuous\n')
					return False
				tag = nodeinfo[4]
				n = len(tag) + 2
				if trace[0]:
					sys.stderr.write('<%s>' % tag)
					sleep(trace[1] * 0.001)
					ndot = ndot + n
				a.append((depth, tag))
			elif t == self.TYPE_END_TAG:
				tag = nodeinfo[3]
				if not len(a) or a[-1][1] != tag:#a[-1] != (depth, tag):
					if trace[0]:
						sys.stderr.write('\r%s\r' % (' ' * ndot))
					sys.stderr.write('FATAL: unmatched tag: no open tag: |%s|\n' % tag)
					print '\n'.join(['%s' % str(_) for _ in x])
					x = []
					import aligntext
					print '\n'.join(['%s' % aligntext.color_text(_[1], ('blue', 'normal', False)) for _ in a])
					print '%s' % aligntext.color_text('FATAL: unmatched tag: no open tag: |%s|' % tag, ('red', 'normal', False))
					i = i + 1
					continue
					return False
				n = len(tag) + 2
				if trace[0]:
					sys.stderr.write('[D [D' * n)
					sleep(trace[1] * 0.001)
					ndot = ndot - n
				a.pop()
			i = i + 1
		if len(a):
			if trace[0]:
				sys.stderr.write('\r%s\r' % (' ' * ndot))
			sys.stderr.write('FATAL: unmatched tag: no close tag: |%s|\n' % a[-1][1])
		return len(a) == 0
	# end of check_node	// }
	# start of check_node	// {
	# very strict
	def check_node(self, (start_index, end_index), depth = 0, trace = (False, 0)):
		tagset = self.tagset
		if trace[0]:
			ndot = 0
		x = []
		a = []	# list of (depth, start tag)
		i = start_index
		while i <= end_index:
			nodeinfo = tagset[i]
			x.append(nodeinfo)
			(depth, raw, t) = nodeinfo[:3]
			if t == self.TYPE_START_TAG:
				# check depth
				if len(a) and depth != a[-1][0] + 1:
					if trace[0]:
						sys.stderr.write('\r%s\r' % (' ' * ndot))
					sys.stderr.write('FATAL: depth: not continuous\n')
					return False
				tag = nodeinfo[4]
				n = len(tag) + 2
				if trace[0]:
					sys.stderr.write('<%s>' % tag)
					sleep(trace[1] * 0.001)
					ndot = ndot + n
				a.append((depth, tag))
			elif t == self.TYPE_END_TAG:
				tag = nodeinfo[3]
				if not len(a) or a[-1] != (depth, tag):
					if trace[0]:
						sys.stderr.write('\r%s\r' % (' ' * ndot))
					sys.stderr.write('FATAL: unmatched tag: no open tag: |%s|\n' % tag)
					return False
				n = len(tag) + 2
				if trace[0]:
					sys.stderr.write('[D [D' * n)
					sleep(trace[1] * 0.001)
					ndot = ndot - n
				a.pop()
			i = i + 1
		if len(a):
			if trace[0]:
				sys.stderr.write('\r%s\r' % (' ' * ndot))
			sys.stderr.write('FATAL: unmatched tag: no close tag: |%s|\n' % a[-1][1])
		return len(a) == 0
	# end of check_node	// }
	NODE_TAG = 0		#TYPE_START_TAG = 0
				#TYPE_END_TAG = 1
				#TYPE_STARTEND_TAG = 2
	NODE_DATA = 3		# TYPE_DATA = 3
	NODE_ENTITYREF = 4	# TYPE_ENTITYREF = 4
	NODE_CHARREF = 5	# TYPE_CHARREF = 5
	NODE_COMMENT = 6	# TYPE_COMMENT = 6
	NODE_DECL = 7		# TYPE_DECL = 7
	# start of set_node_table	// {
	def set_node_table(self, (start_index, end_index), node_table, parent_node_index = -1):
		assert(self.check_node((start_index, end_index)))#, trace = (True, 100)))
		tagset = self.tagset
		node_table_index_a = []	# list of index of children
		i = start_index
		while i <= end_index:
			nodeinfo = tagset[i]
			(depth, raw, t) = nodeinfo[:3]
			# set "dic"(dictionary) for attributes
			if t == self.TYPE_STARTEND_TAG or t == self.TYPE_START_TAG:
				dic = {}
				d = nodeinfo[3]
				for key in d.keys():
					val_a = d[key]	# list
					dic[key] = val_a[-1]	# last one

			if t == self.TYPE_START_TAG:
				# find matched TYPE_END_TAG
				k = i + 1
				while k <= end_index:
					if tagset[k][0] == depth:	# insured by check_node
						# found
						break
					k = k + 1
				node = {
					'type': self.NODE_TAG,
					'tag': nodeinfo[4],
					'raw': raw,
					'dic': dic,
					'parent': parent_node_index,
					'children': None,	# to be filled after calling set_node_table
					'nodes': None,		# ditto
				}
				node_index = len(node_table)
				node_table.append(node)
				node_table_index_a.append(node_index)
				child_index_a = self.set_node_table((i + 1, k - 1), node_table, node_index)
				node['nodes'] = child_index_a
				a = []
				for idx in child_index_a:
					nd = node_table[idx]
					if nd['type'] == self.NODE_TAG:
						a.append(idx)
				node['children'] = a
				i = k + 1
				continue
			if t == self.TYPE_STARTEND_TAG:
				node = {
					'type': self.NODE_TAG,
					'tag': nodeinfo[4],
					'raw': raw,
					'dic': dic,
					'children': [],	# no children
					'nodes': [],	# no nodes
				}
			else:	# TYPE_DATA, TYPE_CHARREF, TYPE_ENTITYREF
				# TYPE_DECL
				# TYPE_COMMENT
				node = {
					'type': t,
					'val': raw,
				}
			i = i + 1
			#print node
			unite = False
			if len(node_table):
				if parent_node_index == node_table[-1]['parent'] and node['type'] == self.NODE_DATA and node_table[-1]['type'] == node['type']:
					print parent_node_index, node_table[-1]['parent']
					unite = True
				# other cases...
			if unite:
				node_table[-1]['val'] = node_table[-1]['val'] + node['val']
			else:
				node_table_index_a.append(len(node_table))
				node['parent'] = parent_node_index
				node_table.append(node)
		return node_table_index_a
	# end of set_node_table		// }
	# start of walk_node_table	// {
	def walk_node_table(self, node_table, parent_node_index):
		def write_text_node(node_type, node_val, indent_str):
			a = []
			if False:
				pass
			elif node_type == self.NODE_DATA:
				type_str = 'NODE_DATA'
			elif node_type == self.NODE_ENTITYREF:
				type_str = 'NODE_ENTITYREF'
			elif node_type == self.NODE_CHARREF:
				type_str = 'NODE_CHARREF'
			elif node_type == self.NODE_COMMENT:
				type_str = 'NODE_COMMENT'
			elif node_type == self.NODE_DECL:
				type_str = 'NODE_DECL'
			else:
				assert(False)
			a.append(indent_str + '{')
			a.append(indent_str + '  type: %s' % type_str)
			a.append(indent_str + '  val: |%s|' % node_val)
			a.append(indent_str + '},')
			return a

		a = []
		for node_index in range(len(node_table)):
			node = node_table[node_index]
			if node['parent'] != parent_node_index:
				continue
			nt = node['type']
			if nt == self.NODE_TAG:
				a.append('{')
				a.append('  type: NODE_TAG')
				a.append('  val: {')
				a.append('    tag: |%s|' % node['tag'])
				a.append('    raw: |%s|' % node['raw'])
				a.append('    dic: {')
				dic = node['dic']
				for key in dic.keys():
					a.append('      |%s|: |%s|' % (key, dic[key]))
				a.append('    }')
				"""
				a.append('    children: [')
				r = self.walk_node_table(node_table, node_index)
				for e in r:
					a.append('      %s' % e)
				a.append('    ]')
				"""
				a.append('    nodes: [')
				for i in node['nodes']:
					nd = node_table[i]
					nt = nd['type']
					if nt == self.NODE_TAG:
						a.append('      child: {')
						r = self.walk_node_table(node_table, node_index)
						for e in r:
							a.append('        %s' % e)
						a.append('      },')
					else:
						r = write_text_node(nt, nd['val'], indent_str = '      ')
						for e in r:
							a.append(e)
						continue
						if False:
							pass
						elif nt == self.NODE_DATA:
							ts = 'NODE_DATA'
						elif nt == self.NODE_ENTITYREF:
							ts = 'NODE_ENTITYREF'
						elif nt == self.NODE_CHARREF:
							ts = 'NODE_CHARREF'
						elif nt == self.NODE_COMMENT:
							ts = 'NODE_COMMENT'
						elif nt == self.NODE_DECL:
							ts = 'NODE_DECL'
						else:
							assert(False)
						a.append('      {')
						a.append('        type: %s' % ts)
						a.append('        val: |%s|' % nd['val'])
						a.append('      },')
				a.append('    ]')
				a.append('  }')
				a.append('},')
			else:
				r = write_text_node(nt, node['val'], indent_str = '')
				for e in r:
					a.append(e)
				continue
				if False:
					pass
				elif nt == self.NODE_DATA:
					ts = 'NODE_DATA'
				elif nt == self.NODE_ENTITYREF:
					ts = 'NODE_ENTITYREF'
				elif nt == self.NODE_CHARREF:
					ts = 'NODE_CHARREF'
				elif nt == self.NODE_COMMENT:
					ts = 'NODE_COMMENT'
				elif nt == self.NODE_DECL:
					ts = 'NODE_DECL'
				else:
					assert(False)
				a.append('{')
				a.append('  type: %s' % ts)
				a.append('  val: |%s|' % node['val'])
				a.append('},')
		return a
	# end of walk_node_table	// }
# end of class analhtml			// }


if __name__ == '__main__':
	ah = analhtml()

	f = open('all_opt_out')
	s = ''.join(f.readlines())
	f.close()
	ah.reset()
	ah.feed(s)
	ah.check_node0((56,845), trace = (True, 1))	# ok
	sys.exit()
	ah.check_node0((78,109), trace = (True, 1))	# ok
	sys.exit()
	ah.check_node0((110,265), trace = (True, 1))	# ok
	ah.check_node0((266,845), trace = (True, 1))
	"""
	for i in range(len(ah.tagset)):
		print '%04d:' % i,
		print ah.tagset[i]
	sys.exit()
	"""
	c = "wrapper responsive_container"
	c = []
	#c.append("cdo-dblclick-area")
	c.append("cdo-footer")
	#c.append("responsive_row")
	#c.append("responsive_row responsive_hide_on_smartphone")# ok
	#c.append("responsive_row")	# ok
	r = ah.get_indices(c)
	print r
	(si, ei) = r[0][-1]
	for i in range(si, ei + 1):
		print ah.tagset[i]
	sys.exit()
	ah.check_node0(r[1][-1], trace = (True, 100))
	'''
	r = ah.get_indices(['cdo-dblclick-area'])
	r = ah.get_indices(["responsive_cell_center"])
	r = ah.get_indices(["responsive_cell_left responsive_hide_on_smartphone"])
	print r
	print r[0][0]
	ah.check_node0(r[0][0], trace = (True, 1))
'''
	ah.check_node0((0, len(ah.tagset) - 1), trace = (True, 10))
	sys.exit()
	
	f = open('nut_2')
	s = ''.join(f.readlines())
	f.close()
	ah.reset()
	ah.feed(s)
	ah.walk('<html>')
	ra = ah.get_indices(['di-body', 'def-head', 'def'])
	print ra
	for r in ra:
		print r[-1]
		rt = ah.get_text(r[-1])
		print rt
	f = open('nut_3')
	s = ''.join(f.readlines())
	f.close()
	ah.reset()
	ah.feed(s)
	#ah.walk('<html>')
	ra = ah.get_indices(['di-body', 'def-head', 'def'])
	print ra
	for r in ra:
		print r[-1]
		rt = ah.get_text(r[-1])
		print rt
	mh = myHTMLParser()
	for i in range(100):
		print 'i = %d' % i
		f = open('nut_3')
		s = ''.join(f.readlines())
		f.close()
		#ah = analhtml()
		#ah.reset()
		#ah.feed(s)
		mh.reset()
		mh.feed(s)
		mh.close()
		continue
		#ah.walk('<html>')
		ra = ah.get_indices(['di-body', 'def-head', 'def'])
		print ra
		for r in ra:
			print r[-1]
			rt = ah.get_text(r[-1])
			print rt
