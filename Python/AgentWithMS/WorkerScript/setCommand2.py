def setCommand2(commandInstaller2):
	if commandInstaller2 == None:
		return None
	"""
#IN: hCommand =
{
	'command_netstat': {
		'ByteStreamID__LINK': hByteStream2ByName['bs_workerTypeII'],
		'ArgumentScriptID__LINK': hScript2ByName['argv_netstat'],
		'ParseScriptID__LINK': hScript2ByName['parse_netstat'],
		'HeartBeatInterval': 60
	},
	'command_ping': {
		'ByteStreamID__LINK': hByteStream2ByName['bs_workerTypeI'],
		'ArgumentScriptID__LINK': hScript2ByName['argv_ping'],
		'ParseScriptID__LINK': hScript2ByName['parse_ping'],
		'HeartBeatInterval': 60
	},
	'command_ps': {
		'ByteStreamID__LINK': hByteStream2ByName['bs_workerTypeII'],
		'ArgumentScriptID__LINK': hScript2ByName['argv_ps'],
		'ParseScriptID__LINK': hScript2ByName['parse_ps'],
		'HeartBeatInterval': 60
	},
	'command_vmstat': {
		'ByteStreamID__LINK': hByteStream2ByName['bs_workerTypeI'],
		'ArgumentScriptID__LINK': hScript2ByName['argv_vmstat'],
		'ParseScriptID__LINK': hScript2ByName['parse_vmstat'],
		'HeartBeatInterval': 60
	},
}

#OUT:
hCommand2ByID = {
	101L: ['command_netstat', 101, 101, 111, 60],	# [command, ByteStreamID, ArgumentScriptID, ParseScriptID, HeartBeatInterval]
	......
}
hCommand2ByName = {
	'command_netstat': 101L,
	......
}
"""

	global hCommand2ByName
	if not globals().has_key('hCommand2ByName'):
		hCommand2ByName = dict()
	global hCommand2ByID
	if not globals().has_key('hCommand2ByID'):
		hCommand2ByID = dict()

	id = 100
	returnList = list()
	for cmd in commandInstaller2:
		hCmd = commandInstaller2[cmd]
		byteStreamID__LINK = hCmd['ByteStreamID__LINK']
		if byteStreamID__LINK <= 0:	# 0:= Pause, <0:= Delete
			state = 'Pause' if byteStreamID__LINK == 0 else 'Delete'
			if not hCommand2ByName.has_key(cmd):
				print '|%s|: Marked as <%s> but not found: Ignored' % (cmd, state)
				continue
			print '|%s|: <%s>' % (cmd, state)
			old_id = hCommand2ByName[cmd]
			returnList.append({
				'ID': hCommand2ByName[cmd],
				'Action': 'Pause' if byteStreamID__LINK == 0 else 'Delete'
			})
			continue

		if hCommand2ByName.has_key(cmd):
			old_id = hCommand2ByName[cmd]
			old_hCmd = hCommand2ByID[old_id]
			# Possibly update...
			isUpdateByteStreamID = False
			isUpdateArgumentScriptID = False
			isUpdateParseScriptID = False
			if old_hCmd['ByteStreamID__LINK'] != hCmd['ByteStreamID__LINK']:
				old_hCmd['ByteStreamID__LINK'] = hCmd['ByteStreamID__LINK']
				isUpdateByteStreamID = True
			if old_hCmd['ArgumentScriptID__LINK'] != hCmd['ArgumentScriptID__LINK']:
				old_hCmd['ArgumentScriptID__LINK'] = hCmd['ArgumentScriptID__LINK']
				isUpdateArgumentScriptID = True
			if old_hCmd['ParseScriptID__LINK'] != hCmd['ParseScriptID__LINK']:
				old_hCmd['ParseScriptID__LINK'] = hCmd['ParseScriptID__LINK']
				isUpdateParseScriptID = True
			if isUpdateByteStreamID or isUpdateArgumentScriptID or isUpdateParseScriptID:
				print '|%s|: Update' % cmd
				returnList.append({
					'ID': old_id,
					'Action': 'Update',
					'Detail': {
						'ByteStreamID__LINK': isUpdateByteStreamID,
						'ArgumentScriptID__LINK': isUpdateArgumentScriptID,
						'ParseScriptID__LINK': isUpdateParseScriptID
					}
				});
			else:
				print '|%s|: Nothing new: Skipped' % cmd

			continue

		# Find new id
		while True:
			id = id + 1	# Increase ID by 1
			if not hCommand2ByID.has_key(id):
				break

		hCommand2ByName[cmd] = id
#		hCommand2ByID[id] = list([cmd, hCmd['ByteStreamID__LINK'], hCmd['ArgumentScriptID__LINK'], hCmd['ParseScriptID__LINK'], hCmd['HeartBeatInterval']])
		hCommand2ByID[id] = dict(hCmd)
		hCommand2ByID[id]['Name'] = cmd

	return returnList
