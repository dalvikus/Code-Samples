def setSysInfo():
	sysInfo = dict()

	from os import popen
	f = popen("uname -a")
	if f == None:
		return None
	l = f.readline()

	import re
	m = re.compile('^(\S+)\s+(\S+)\s+(\S+).*$').match(l)
	if m == None:
		return None

	sysInfo['systemname'] = m.group(1)
	sysInfo['nodename'] = m.group(2)
	sysInfo['version'] = m.group(3)

	return sysInfo
