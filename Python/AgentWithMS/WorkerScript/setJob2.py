def setJob2(jobInstaller2):
	if jobInstaller2 == None:
		return None
#	if chkJob(jobInstaller2) == False:
#		return False
	"""
{
	'job_netstat': {
########################################################################
	'CommandID__LINK': hCommand2ByName['command_netstat'],
#	'Command': {			# Command 에 대한 정보를
#		'CommandID': 3,
#
#		'ByteStreamID__LINK': 4,
#		'EntryPoint': 'workerTypeI',
#		'ArgumentScriptID__LINK': hScript2ByName['argv_netstat'],
#		'ParseScriptID__LINK': hScript2ByName['parse_netstat'],
#		'HeartBeatInterval': 60
#	},

	'Schedule': {			# 어떤 주기로 실행할 것인지
#		......
	},

	'Rule': {			# 이벤트를 발생시키는 기준은 무엇인지
#		......
	},

	'ReturnScriptID__LINK': hScript2ByName['return_netstat'],
	'Argument__STR': 'Argument__STR: ...',
					# 데이터를 결정하는 Script 에서 사용하는 인자가 있다면

	'hTask': {			# 이러한 Task 들을 한다.
		'Sextuple0': [
			8,		# 성능데이터 값의 맵핑
			9		# 장애데이터 값의 맵핑
		],
#		......	
	}
########################################################################
	},

	# ping
	'job_ping': {
########################################################################
	'CommandID__LINK': hCommand2ByName['command_ping'],
#	'Command': {			# Command 에 대한 정보를
#		'CommandID': 3,
#
#		'ByteStreamID__LINK': 4,
#		'EntryPoint': 'workerTypeI',
#		'ArgumentScriptID__LINK': hScript2ByName['argv_ping'],
#		'ParseScriptID__LINK': hScript2ByName['parse_ping'],
#		'HeartBeatInterval': 60
#	},

	'Schedule': {			# 어떤 주기로 실행할 것인지
#		......
	},

	'Rule': {			# 이벤트를 발생시키는 기준은 무엇인지
#		......
	},

	'ReturnScriptID__LINK': hScript2ByName['return_ping'],
	'Argument__STR': 'Argument__STR: ...',
					# 데이터를 결정하는 Script 에서 사용하는 인자가 있다면

	'hTask': {			# 이러한 Task 들을 한다.
		'Sextuple0': [
			8,		# 성능데이터 값의 맵핑
			9		# 장애데이터 값의 맵핑
		],
#		......	
	}
########################################################################
	},

	# ps
	'job_ps': {
########################################################################
	'CommandID__LINK': hCommand2ByName['command_ps'],
#	'Command': {			# Command 에 대한 정보를
#		'CommandID': 3,
#
#		'ByteStreamID__LINK': 4,
#		'EntryPoint': 'workerTypeI',
#		'ArgumentScriptID__LINK': hScript2ByName['argv_ps'],
#		'ParseScriptID__LINK': hScript2ByName['parse_ps'],
#		'HeartBeatInterval': 60
#	},

	'Schedule': {			# 어떤 주기로 실행할 것인지
#		......
	},

	'Rule': {			# 이벤트를 발생시키는 기준은 무엇인지
#		......
	},

	'ReturnScriptID__LINK': hScript2ByName['return_ps'],
	'Argument__STR': 'Argument__STR: ...',
					# 데이터를 결정하는 Script 에서 사용하는 인자가 있다면

	'hTask': {			# 이러한 Task 들을 한다.
		'Sextuple0': [
			8,		# 성능데이터 값의 맵핑
			9		# 장애데이터 값의 맵핑
		],
#		......	
	}
########################################################################
	},

	# vmstat
	'job_vmstat': {
########################################################################
	'CommandID__LINK': hCommand2ByName['command_vmstat'],
#	'Command': {			# Command 에 대한 정보를
#		'CommandID': 3,
#
#		'ByteStreamID__LINK': 4,
#		'EntryPoint': 'workerTypeI',
#		'ArgumentScriptID__LINK': hScript2ByName['argv_vmstat'],
#		'ParseScriptID__LINK': hScript2ByName['parse_vmstat'],
#		'HeartBeatInterval': 60
#	},

	'Schedule': {			# 어떤 주기로 실행할 것인지
#		......
	},

	'Rule': {			# 이벤트를 발생시키는 기준은 무엇인지
#		......
	},

	'ReturnScriptID__LINK': hScript2ByName['return_vmstat'],
	'Argument__STR': 'Argument__STR: ...',
					# 데이터를 결정하는 Script 에서 사용하는 인자가 있다면

	'hTask': {			# 이러한 Task 들을 한다.
		'Sextuple0': [
			8,		# 성능데이터 값의 맵핑
			9		# 장애데이터 값의 맵핑
		],
#		......	
	}
########################################################################
	},
}
"""
	global hJob2ByID
	if not globals().has_key('hJob2ByID'):
		hJob2ByID = dict()
	global hJob2ByName
	if not globals().has_key('hJob2ByName'):
		hJob2ByName = dict()

	id = 100
	returnList = list()
	for job in jobInstaller2:
		hJob = jobInstaller2[job]

		commandID__LINK = hJob['CommandID__LINK']
		if commandID__LINK <= 0:	# 0:= Pause, <0:= Delete
			state = 'Pause' if commandID__LINK == 0 else 'Delete'
			if not hJob2ByName.has_key(job):
				print '|%s|: Marked as <%s> but not found: Ignored' % (job, state)
				continue
			print '|%s|: <%s>' % (job, state)
			old_id = hJob2ByName[job]
			returnList.append({
				'ID': hJob2ByName[job],
				'Action': 'Pause' if commandID__LINK == 0 else 'Delete'
			})
			continue

		if hJob2ByName.has_key(job):
			old_id = hJob2ByName[job]
			old_hJob = hJob2ByID[old_id]
			# Possibly update...
			isUpdateTarget = False
			isUpdateSchedule = False
			isUpdateRule = False
			isUpdateReturnScriptID = False
			if old_hJob['Target'] != hJob['Target']:
				old_hJob['Target'] = hJob['Target']
				isUpdateTarget = True
			if old_hJob['Schedule'] != hJob['Schedule']:
				old_hJob['Schedule'] = dict(hJob['Schedule'])
				isUpdateSchedule = True
			if old_hJob['Rule'] != hJob['Rule']:
				old_hJob['Rule'] = dict(hJob['Rule'])
				isUpdateRule = True
			if old_hJob['ReturnScriptID__LINK'] != hJob['ReturnScriptID__LINK']:
				old_hJob['ReturnScriptID__LINK'] = hJob['ReturnScriptID__LINK']
				isUpdateReturnScriptID = True
			if isUpdateTarget or isUpdateSchedule or isUpdateRule or isUpdateReturnScriptID:
				print '|%s|: Update' % job
				returnList.append({
					'ID': old_id,
					'Action': 'Update',
					'Detail': {
						'Target': isUpdateTarget,
						'Schedule': isUpdateSchedule,
						'Rule': isUpdateRule,
						'ReturnScriptID__LINK': isUpdateReturnScriptID
					}
				})
			else:
				print '|%s|: Nothing new: Skipped' % job

			continue

		# Find new id
		while True:
			id = id + 1	# Increase ID by 1
			if not hJob2ByID.has_key(id):
				break

		hJob2ByName[job] = id
#		hJob2ByID[id] = list([job, hJob['CommandID__LINK'], hJob['Schedule'], hJob['Rule'], hJob['ReturnScriptID__LINK'], hJob['Argument__STR'], hJob['hTask']])
		hJob2ByID[id] = dict(hJob)
		hJob2ByID[id]['Name'] = job

	return returnList
