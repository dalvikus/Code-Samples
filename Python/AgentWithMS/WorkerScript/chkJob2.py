def chkJob2(job2):
	if job2 == None:
		print 'Job is None'
		return False
	if type(job2) != type({}):
		print 'job2 is NOT a dictionary'
		return False
	if job2.has_key('JobID') == False:
		print 'No Key:= |JobID|'
		return False
	if not job2.has_key('CommandID__LINK') and not job2.has_key('Command'):
		print 'No Key:= |CommandID__LINK| nor |Command|'
		return False
	if job2.has_key('Command'):
		if type(job2['Command']) != type({}):
			return False

		validCommandKeyList = [
			'CommandID',
			'ByteStreamID__LINK',
			'EntryPoint',
			'ArgumentScriptID__LINK',
			'ParseScriptID__LINK',
			'HeartBeatInterval'
		];
		if set(job2['Command'].keys()) != set(validCommandKeyList):
			return False
	else:
		return False

	return True
