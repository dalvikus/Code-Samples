#!/usr/bin/python
# -*- coding: utf-8 -*- from http://www.python.org/peps/pep-0263.html (http://www.python.org/dev/peps/pep-0263/)

TABSIZE = 2

def print_dict(x, indent_str = '', verbose = False):
    maxlen = -1
    for key in x.keys():
        fmt = '%s'
        if type(key) == type(str()):
            fmt = '|%s|'
        n = len(fmt % key)
        if n > maxlen:
            maxlen = n
    maxlen = maxlen + 2 # ': '
    if verbose:
        print maxlen

    a = []
    for key in x.keys():
        fmt = '%s'
        if type(key) == type(str()):
            fmt = '|%s|'
        key_str = fmt % key
        n = len(key_str)
        key_str = key_str + ': '
        key_str = key_str + ' ' * (maxlen - (n + 2))

        val = x[key]
        if type(val) == type(dict()):
            #val_str = '%s' % type(dict())
            i_str = indent_str + ' ' * TABSIZE + ' ' * maxlen
            s = print_dict(val, i_str, verbose)
            val_str = s
        elif type(val) == type(list()):
            #val_str = type(list())
            i_str = indent_str + ' ' * TABSIZE + ' ' * maxlen
            s = print_list(val, i_str, verbose, isTuple = False)
            val_str = s
        elif type(val) == type(set()):
            val_str = '%s' % type(set())
        elif type(val) == type(tuple()):
            #val_str = '%s' % type(tuple())
            i_str = indent_str + ' ' * TABSIZE + ' ' * maxlen
            s = print_list(val, i_str, verbose, isTuple = True)
            val_str = s
        else:
            val_str = '"%s"' % val
        a.append(' ' * TABSIZE + key_str + val_str)
    s = ''
    if len(x.keys()):
        s = indent_str + ('\n' + indent_str).join(a) + '\n'
    return '{' + '\n' + s + indent_str + '}'

def print_list(x, indent_str = '', verbose = False, isTuple = False):
    a = []
    elm_indent_str = indent_str + ' ' * TABSIZE
    for e in x:
        if type(e) == type(dict()):
            i_str = elm_indent_str
            s = print_dict(e, i_str)
            val_str = s#[len(i_str):]
            #val_str = '%s' % type(dict())
        elif type(e) == type(list()):
            i_str = elm_indent_str
            s = print_list(e, i_str, isTuple = False)
            val_str = s#[len(i_str):]
            #val_str = '%s' % type(list())
        elif type(e) == type(set()):
            val_str = '%s' % type(set())
        elif type(e) == type(tuple()):
            i_str = elm_indent_str
            s = print_list(e, i_str, isTuple = True)
            val_str = s#[len(i_str):]
            #val_str = '%s' % type(tuple())
        else:
            val_str = '"%s"' % e
        a.append(val_str)
    s = ''
    if len(a):
        s = elm_indent_str + ('\n' + elm_indent_str).join(a) + '\n'
    if verbose:
        print 'len = %d(%d)' % (len(indent_str), len(a))
    left = '(' if isTuple else '['
    right = ')' if isTuple else ']'
    return left + '\n' + s + indent_str + right

if __name__ == "__main__":
    """
    #f = open('/home/icentrik/Desktop/bookmarks-2013-06-19.json')
    try:
        f = open('./bookmarks-2013-06-19.json')
        s = ''.join(f.readlines())
        print s
        null=None
        x = eval(s)
        #print type(x)
        #for y in x:
        #   print '%s: %s' % (y, type(x[y]))
        print ' ' * 2 + print_dict(x, indent_str = ' ' * 2)
    except:
        import sys
        print sys.exc_info()
        pass
    print ' ' * 2 + print_list([1, 2, 3], indent_str = ' ' * 2)
    print ' ' * 2 + print_dict({1: 'a', 2:'b'}, indent_str = ' ' * 2)
    x = {
        1: [{'x': 'a', 'y': 1}, {'y': [],}],
        2: [3, 4, {5: [1]}]
    }
    print ' ' * 2 + print_dict(x, indent_str = ' ' * 2)
"""
    import sys
    json_str = "([('sycophantic', '\xcb\x8cs\xc9\xaak.\xc9\x99\xcb\x88f\xc3\xa6n.t\xc9\xaak', (None, None, None, 'http://dictionary.cambridge.org/media/british/us_pron_ogg/e/eus/eus74/eus74458.ogg'), 'adjective', None, [('(of a person or of behaviour) praising people in authority in a way that is not sincere, usually in order to get some advantage from them:', ['There was sycophantic laughter from the audience at every one of his terrible jokes.'])]), ('sycophancy', '\xcb\x88s\xc9\xaak.\xc9\x99.f\xc9\x99n.si', (None, None, None, 'http://dictionary.cambridge.org/media/british/us_pron_ogg/c/cus/cus02/cus02014.ogg'), 'noun', None, []), ('sycophant', '\xcb\x88s\xc9\xaak.\xc9\x99.f\xc3\xa6nt', (None, None, None, 'http://dictionary.cambridge.org/media/british/us_pron_ogg/e/eus/eus74/eus74457.ogg'), 'noun', None, [(None, ['The prime minister is surrounded by sycophants.'])])], [])"
    json_str = r'{"doc": [[{"head": {"word": "word", "gw": "", "pos": ["n"], "pron": ["", "", "http://dictionary.cambridge.org/media/american-english/us_pron/w/wor/word_/word.mp3", "http://dictionary.cambridge.org/media/american-english/us_pron_ogg/w/wor/word_/word.ogg"], "ipa": ["wɜrd"]}, "body": [{"def": "a single unit of language that has meaning and can be spoken or written:", "example": ["The word \"environment\" means different things to different people.", "She spoke so fast I couldn’t understand a word (= anything she said)."]}]}, {"head": {"word": "worded", "gw": "", "pos": ["adj"], "pron": ["", "", "http://dictionary.cambridge.org/media/american-english/us_pron/w/wor/worde/worded.mp3", "http://dictionary.cambridge.org/media/american-english/us_pron_ogg/w/wor/worde/worded.ogg"], "ipa": ["ˈwɜrd·əd"]}, "body": [{"def": "", "example": ["a strongly worded letter"]}]}]], "more": [{"word": "word", "pos": "n", "gw": "BRIEF STATEMENT", "href": "http://dictionary.cambridge.org/dictionary/american-english/word_2", "arl": 1}, {"word": "word", "pos": "n", "gw": "NEWS", "href": "http://dictionary.cambridge.org/dictionary/american-english/word_3", "arl": 1}, {"word": "word", "pos": "n", "gw": "PROMISE", "href": "http://dictionary.cambridge.org/dictionary/american-english/word_4", "arl": 1}, {"word": "word", "pos": "n", "gw": "ORDER", "href": "http://dictionary.cambridge.org/dictionary/american-english/word_5", "arl": 1}, {"word": "word", "pos": "v", "gw": "LANGUAGE UNIT", "href": "http://dictionary.cambridge.org/dictionary/american-english/word_6", "arl": 1}, {"word": "code word", "pos": "n", "gw": "", "href": "http://dictionary.cambridge.org/dictionary/american-english/code-word", "arl": 2}, {"word": "swear word", "pos": "n", "gw": "", "href": "http://dictionary.cambridge.org/dictionary/american-english/swear-word", "arl": 2}, {"word": "word processing", "pos": "n", "gw": "", "href": "http://dictionary.cambridge.org/dictionary/american-english/word-processing", "arl": 2}, {"word": "word processor", "pos": "n", "gw": "", "href": "http://dictionary.cambridge.org/dictionary/american-english/word-processor", "arl": 2}, {"word": "four-letter word", "pos": "n", "gw": "", "href": "http://dictionary.cambridge.org/dictionary/american-english/four-letter-word", "arl": 2}, {"word": "breathe a word", "pos": "idiom", "gw": "", "href": "http://dictionary.cambridge.org/dictionary/american-english/breathe-a-word", "arl": 7}, {"word": "man of his word", "pos": "idiom", "gw": "", "href": "http://dictionary.cambridge.org/dictionary/american-english/man-of-his-word", "arl": 7}, {"word": "the word is", "pos": "idiom", "gw": "", "href": "http://dictionary.cambridge.org/dictionary/american-english/the-word-is", "arl": 7}, {"word": "word for word", "pos": "idiom", "gw": "", "href": "http://dictionary.cambridge.org/dictionary/american-english/word-for-word", "arl": 7}, {"word": "word of mouth", "pos": "idiom", "gw": "", "href": "http://dictionary.cambridge.org/dictionary/american-english/word-of-mouth", "arl": 7}, {"word": "say a word, at breathe a word", "pos": "idiom", "gw": "", "href": "http://dictionary.cambridge.org/dictionary/american-english/breathe-a-word", "arl": 7}, {"word": "woman of her word, at man of his word", "pos": "idiom", "gw": "", "href": "http://dictionary.cambridge.org/dictionary/american-english/man-of-his-word", "arl": 7}, {"word": "a household word, at a household name", "pos": "idiom", "gw": "", "href": "http://dictionary.cambridge.org/dictionary/american-english/a-household-name", "arl": 7}, {"word": "true to your word", "pos": "idiom", "gw": "", "href": "http://dictionary.cambridge.org/dictionary/american-english/true-to-your-word", "arl": 7}, {"word": "get a word in edgewise", "pos": "idiom", "gw": "", "href": "http://dictionary.cambridge.org/dictionary/american-english/get-a-word-in-edgewise", "arl": 7}, {"word": "have the final word on something, at have the last word on something", "pos": "idiom", "gw": "", "href": "http://dictionary.cambridge.org/dictionary/american-english/have-the-last-word-on-something", "arl": 7}, {"word": "the last word in something", "pos": "idiom", "gw": "", "href": "http://dictionary.cambridge.org/dictionary/american-english/the-last-word-in-something", "arl": 7}, {"word": "take someone’s word for it", "pos": "idiom", "gw": "", "href": "http://dictionary.cambridge.org/dictionary/american-english/take-someone-s-word-for-it", "arl": 7}, {"word": "have the last word on something", "pos": "idiom", "gw": "", "href": "http://dictionary.cambridge.org/dictionary/american-english/have-the-last-word-on-something", "arl": 7}, {"word": "put in a good word for someone", "pos": "idiom", "gw": "", "href": "http://dictionary.cambridge.org/dictionary/american-english/put-in-a-good-word-for-someone", "arl": 7}]}'
    json_str = ''.join(sys.stdin.readlines())
    d = eval(json_str)
    #print d
    #json_dic = json.loads(json_str)
    #print json_dic
    if type(d) == type(dict()):
        print ' ' * 2 + print_dict(d, indent_str = ' ' * 2)
    elif type(d) == type(list()):
        print ' ' * 2 + print_list(d, indent_str = ' ' * 2, isTuple = False)
    elif type(d) == type(tuple()):
        print ' ' * 2 + print_list(d, indent_str = ' ' * 2, isTuple = True)
    else:
        assert(False), "%s: not implemented yet" % type(d)
