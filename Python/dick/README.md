Brief Description
-----------------
It customized the online dictionary (http://dictionary.cambridge.org).

Long Description
----------------
The dictionary is essential to me as a foreigner;
especially to me as a programmer, I would like to customize it in two aspects:
1) I as a programmer prefer to use a terminal in a linux environment rather than a browser.
2) Moreover, the web page itself contains redundant stuffs, mainly advertisements.
To fulfull these two requirements, the "main" (without advertisement) web page
is fetcted by "wget" and/or "curl", and parsed to extract "key" information: word(s),
definition, example sentences with pronunciation.
This information is backed by Sqlite database to minimize the network overload.

The online dictionaries were researched. Many were too verbose and hard to find the appropriate meaning of word that I liked to look up. One (cambridge) was selected carefully because of its simplicity and clearance in definition, especially with "guide word" feature; also, I found the example sentences are, albeit succinct, very helpful.

Class Hierarchy
---------------
    analhtml
      myHTMLParser
        HTMLParser (imported from Python)
        
    Cambridge
      webdic
        analhtml
        
    Merriam_Webster
      webdic
        analhtml
        
    English_Korean
    
    Word_Smart
    
    dictionary
      Cambridge, Merriam_Webster, English_Korean, Word_Smart

Note
----
1. Cambridge dictionary is strict in sense that only real-in-use words are found: ashamed (not ashame) [adjective only],
auspices (not auspice) [plural only], etc., so sometimes it use Merriam-Webster dictionary for spelling check and pronunciation.
2. There are two additional local dictionaries: 1) words for "higher" level called "Word Smar", 2) English-Korean. On demand, we can use it as an option.

3. Compare two snapshots: online.png (web itself), offline.png (customized)
