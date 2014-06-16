def setScriptTree2(hScriptTree2):
	"""
// setScriptTree.py 의 있는 변수의 이름과 일치해야 한다.
const char*	CMgmtSrv::SCRIPT2_BY_ID			= "hScript2ByID";
const char*	CMgmtSrv::SCRIPT2_BY_NAME		= "hScript2ByNAME";
const char*	CMgmtSrv::BYTE_STREAM2_BY_ID		= "hByteStream2ByID";
const char*	CMgmtSrv::BYTE_STREAM2_BY_NAME		= "hByteStream2ByName";
const char*	CMgmtSrv::COMMAND2_BY_ID		= "hCommand2ByID";
const char*	CMgmtSrv::COMMAND2_BY_NAME		= "hCommand2ByName";
const char*	CMgmtSrv::JOB2_BY_ID			= "hJob2ByID";
const char*	CMgmtSrv::JOB2_BY_NAME			= "hJob2ByName";
"""
	def findDelIDList(del_id, delIDList):
		global hScript2ByID
		del_valDict = hScript2ByID[del_id]
		if del_valDict['ExpandedDependsOnIDSet']:	# del_id depends on...
			# Skip
			return

		# Do cleanup depending on del_valDict['TypeBlob']:
		#	delete function, delete variable
		delIDList.append({'ID': del_id, 'Action': 'Delete'});
		for id in del_valDict['ExpandedRequiredIDList']:
			findDelIDList(id, delIDList)
		pass

	def findRequiredIDList(id, reqIDList):
		global hScript2ByID
		# Check if id require id_.
		for id_ in hScript2ByID[id]['ReqIDSet']:
			if id_ in reqIDList:	# Already done
				# Skip
				continue

			# The deeper comes first.
			findRequiredIDList(id_, reqIDList)
			# The shallower next.
			reqIDList.append(id_)
##			reqIDList.append('%d(|%s|)' % (id_, hScript2ByID[id_]['Name']))

	# Check if id depends on id_.
	def findDepIDSet(id, id_, depIDSet):
		if id_ == id:	# Itself
			# Skip
			return
		if id_ in depIDSet:	# Already added
			# Skip
			return

		global hScript2ByID
		# Check if ReqIDSet of id_ has id.
		for id__ in hScript2ByID[id_]['ReqIDSet']:
			if id__ == id:	# id_ requires id; id depends on id_
				# Found
				depIDSet.add(id_)
#				depIDSet.add('%d(|%s|)' % (id_, hScript2ByID[id_]['Name']))
				break
			else:
				findDepIDSet(id, id__, depIDSet)

	if hScriptTree2 == None:
		return None
	"""
IN: hScript2 =
{
	# keyName:= name of var or func
	#	source filename = keyName + ".py"
				# varNameType
				#	None:= delete
				#	'...':= var = eval('...')
				#	(None,):= function
				#	('...',):= var = eval(compile('...', 'as...', 'exec'))
						# Dependency list
	'chkJob': 		[(None,),		[]],
	'setJob': 		[(None,),		['chkJob']],
	'setSysInfo':		[('hSysInfo',),		[]],		# Set hSysInfo
	'setWorker':		['hWorker',		['argv_netstat', 'argv_ping', 'argv_ps', 'argv_vmstat']],
									# Set hWorker
#	'jobInstaller':		['hJob',		[]],
#	'setScriptTree':	[(None,),		[]],
#	'scriptTreeInstaller':	['hScriptTree2',		[]],


	# Parse...
	'parse_netstat':	[(None,),		[]],
	'parse_ping':		[(None,),		[]],
	'parse_ps':		[(None,),		[]],
	'parse_vmstat':		[(None,),		[]],

	# Argument...
	'argv_netstat':		[(None,),		[]],	# Use hSysInfo
	'argv_ping':		[(None,),		[]],
	'argv_ps':		[(None,),		[]],
	'argv_vmstat':		[(None,),		['setSysInfo']],

	# Return...
	'return_netstat':	[(None,),		['setWorker']],		# Use hWorker
	'return_ping':		[(None,),		[]],
	'return_ps':		[(None,),		[]],
	'return_vmstat':	[(None,),		[]],
}

OUT:
hScript2ByID = {
#	SCRIPT__ID: [keyName, scriptType, DirectlyRequiredSetByID, source],
	100L: ['chkJob',	(None,),	set([]),	'def chkJob...'],
	101L: ['setJob',	(None,),	set([100L]),	'def setJob...'],
	102L: ['setSysInfo',	('hSysInfo',),	set([]),	'def setSysInfo...'],	# hSysInfo = setSysInfo();
	103L: ['jobInstaller',	'hJob',		set([]),	'...'],				# hJob = eval('...')
	......
}
hScript2ByName = {
	'keyName': SCRIPT__ID,
	......
}
"""
	global hScript2ByID
	if not globals().has_key('hScript2ByID'):
		hScript2ByID = dict()
#	hScript2ByID = dict()
	global hScript2ByName
	if not globals().has_key('hScript2ByName'):
		hScript2ByName = dict()
#	hScript2ByName = dict()

	id = 100	# Starting ID
	returnList = list()
	for keyName in hScriptTree2:
		valList = hScriptTree2[keyName]		# [varNameType, [DirectlyRequiredListByName]]

		if valList[0] == None:
			# Possibly delete...
			if not hScript2ByName.has_key(keyName):
				print 'Warning: |%s| marked <Delete> but there is no |%s| in hScript2ByName: Ignored' % (keyName, keyName)
				continue
			del_id = hScript2ByName[keyName]
			del_valDict = hScript2ByID[del_id]
			# Check if it is safe to delete ID.
#			print '|%s|: |%s|, |%s|' % (keyName, del_valDict['TypeBlob'], del_valDict['ExpandedDependsOnIDSet'])
			if del_valDict['ExpandedDependsOnIDSet']:
				print 'Error: |%s| mark <Delete> but it depends on |%s|: Reset to |%s|' % (
					keyName,
					del_valDict['ExpandedDependsOnIDSet'],
					del_valDict['TypeBlob']
				)
				valList[0] = del_valDict['TypeBlob']	# Reset
			else:
				print '|%s|: Expanded Required ID set:= |%s|' % (keyName, del_valDict['ExpandedRequiredIDList'])
				delIDList = list()
				findDelIDList(del_id, delIDList)
				returnList = returnList + delIDList
				continue

		# load source through compile('...', 'as |' + name + '|', 'eval' or 'exec')
		source = ''
		f = open('WorkerScript/' + keyName + '.py')
		for _ in f:
			source = source + _
		f.close()

		import md5
		m = md5.new()
		m.update(source)
		mh = m.hexdigest()

		if hScript2ByName.has_key(keyName):
			# Possibly update...
			old_id = hScript2ByName[keyName]
			old_valDict = hScript2ByID[old_id]

			isUpdateSource = False
			if old_valDict['MD5'] != mh:
				old_valDict['MD5'] = mh
				old_valDict['Source'] = source
				old_valDict['EvalDone'] = False
				isUpdateSource = True

			if isUpdateSource:
				returnList.append({
					'ID': old_id,
					'Action': 'Update',
					'Detail': {
						'Source': isUpdateSource
					}
				})
				print '|%s|(ID = %d): Updated' % (keyName, old_id)
			else:
				print '|%s|: Nothing new: Skipped' % (keyName)

			continue

		# Find new id
		while True:
			id = id + 1	# Increase ID by 1
			if not hScript2ByID.has_key(id):
				break

		hScript2ByName[keyName] = id
		varNameType = valList[0]		# (None,)|(None, '...')|('...', ...)|('...', ())
		valDict = hScript2ByID[id] = {'Name': keyName, 'TypeBlob': varNameType}
		valDict['MD5'] = mh
		valDict['Source'] = source
		# For Required Search...
		valDict['EvalDone'] = False

	# Fill [DirectlyRequiredSetByID]
#	print hScript2ByName
#	print hScript2ByID
	for id in hScript2ByID:
		valDict = hScript2ByID[id]

		name = valDict['Name']
		# Find [DirectlyRequiredSetByID]
		reqIDSet = set()
		for requireVarName in hScriptTree2[name][1]:
			# find ID for requireVarName in hScript2ByName
			requireID = hScript2ByName[requireVarName]
			if requireID == None:
				print 'Error: |%s| in hScriptTree2[|%s|] = |%s|: There is |%s| in hScriptTree2' % (
					requireVarName, name, hScriptTree2[name][1], name
				)
				continue

			reqIDSet.add(requireID)
		# Append [DirectlyRequiredSetByID] to valDict
		valDict['ReqIDSet'] = reqIDSet
#	print hScript2ByID

	# Fill |ExpandedDependsOnIDSet|.
	for id in hScript2ByID:
		reqIDList = list()
		findRequiredIDList(id, reqIDList)
		hScript2ByID[id]['ExpandedRequiredIDList'] = reqIDList
#		print 'id = %d(|%s|) requires %s' % (id, hScript2ByID[id]['Name'], reqIDList)

	# Fill |ExpandedDependsOnIDSet|.
	for id in hScript2ByID:
		depIDSet = set()
		for id_ in hScript2ByID:
			findDepIDSet(id, id_, depIDSet)
		hScript2ByID[id]['ExpandedDependsOnIDSet'] = depIDSet
#		print 'id = %d(|%s|) depends on %s' % (id, hScript2ByID[id]['Name'], depIDSet)

	return returnList
