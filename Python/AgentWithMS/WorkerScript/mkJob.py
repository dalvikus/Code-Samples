def mkJob(scriptID__SET, byteStreamID__SET, commandID__SET, jobID):
#	print 'ScriptID__SET: |%s|' % scriptID__SET
#	print 'ByteStreamID__SET: |%s|' % byteStreamID__SET
#	print 'CommandID__SET: |%s|' % commandID__SET
	if jobID == None:
		print 'Warning: jobID = None'
		return None

	global	hJob2ByID
	if type(hJob2ByID) != type({}):
		print 'Error: hJob2ByID: Undefined or...: type(hJob2ByID) = |%s|' % type(hJob2ByID)
		return None
	if not hJob2ByID.has_key(jobID):
		print 'Warning: hJob2ByID: No jobID = %d: |%s|' % (jobID, hJob2ByID)
		return None

	# 수정할 것이므로 복사해서 사용한다.
	hJob = dict(hJob2ByID[jobID])

	global hAgent
	if not globals().has_key('hAgent'):
		hAgent = dict()
	target = hJob['Target']
##	print 'Target = |%s|' % target
# Paremeter 를 통해 각 SET 를 내려받는다.
####	if not hAgent.has_key(target):
####		hAgent[target] = {
####			# Downloaded sets...
####			# Agent 가 이미 내려받은 Command, Byte Stream, Script 의 집합
####			# "JobID__LIST" 에 있는 Job 으로부터
####			# Agent 에게 내려줄 "Pending Job" 을 구성할 때 사용된다.
####			'CommandID__SET': set(),
####			'ScriptID__SET': set(),
####		}

	# 어떤 Script 들이 Job 의 동작에 필요한 지 알아본다.
	hScriptList = list()
	# Script 는 크게 세 부분에서 필요하다.
	# Job 자체의 동작과 관련된 Script 가 있다; Parse Script 가 만드는 구조를 참고하여,
	# 원하는 정보를 모아 그 결과를 알려주는 Script 이다.
	# 반드시 존재한다.
	hScriptList.append(hScript2ByID[hJob['ReturnScriptID__LINK']])
	# Job 에 필요한 정보를 수집하는 Command 의 실행과 관련된 두 Script 가 있다.
	#	Argument Script: Command 의 인자를 알려주는 Script
	#	Parse Script: Command 의 결과를 특정 구조로 만들어주는 Script
	# Job 의 동작에 필요한 Command 가 실행 중인지 알아본다.
	commandID = hJob['CommandID__LINK']
	# 실행 중이라면 다음 두 값은 None 이다.
	# 그렇지 않다면 적절한 값을 가진다.
	hCommand = None
	byteStreamID = None
####	if not commandID in hAgent[target]['CommandID__SET']:
	if not commandID in commandID__SET:
		# 수정할 것이므로 복사해서 사용한다.
		hCommand = dict(hCommand2ByID[commandID])
		byteStreamID = hCommand['ByteStreamID__LINK']
		#	1. "CommandID" 키에 해당하는 item 을 추가한다.
		#	2. "hJob" 키에 해당하는 item 을 추가한다.
		hCommand['CommandID'] = commandID
		hCommand['hJob'] = dict()

		# Command 의 실행과 관련된 두 Script 도 내려보내야 한다.
		hScriptList.append(hScript2ByID[hCommand['ArgumentScriptID__LINK']])
		hScriptList.append(hScript2ByID[hCommand['ParseScriptID__LINK']])
##	print 'hCommand:= |%s|' % hCommand
	if byteStreamID in byteStreamID__SET:
		byteStreamID = None
##	print 'byteStreamID = %s' % byteStreamID

	# 사용되는 Script 의 동작에 필요한 모든 Script 의 목록을 얻는다.
	# 제일 안쪽에 있는 것을 먼저 등록한다. 즉 먼저 내려받게한다.
	requiredScriptIDList = list()
	for hScript in hScriptList:
		for id in hScript['ExpandedRequiredIDList']:
####			if id in hAgent[target]['ScriptID__SET']:
			if id in scriptID__SET:
				# 이미 등록되었다.
				continue

			if not id in requiredScriptIDList:
				requiredScriptIDList.append(id)
			else:
#				print 'id = %s: Skipped' % id
				pass
		# 직접 적으로 사용하는 Script ID 도 등록한다.
		hScriptID = hScript2ByName[hScript['Name']]
		if not hScriptID in scriptID__SET and not hScriptID in requiredScriptIDList:
			requiredScriptIDList.append(hScriptID)
##	print 'Required Script ID List:= |%s|' % requiredScriptIDList
##	print 'hJob:= |%s|' % hJob

	return {
		'RequiredScriptIDList': requiredScriptIDList,
		'ByteStreamID': byteStreamID,
		'hCommand': hCommand,
		'hJob': hJob,
	}
