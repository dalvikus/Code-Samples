{
	'job_netstat': {
########################################################################
	'Target': 'SunOS',

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
	'Target': 'SunOS',

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
	# ping
	'job_ping2': {
	'Target': 'SunOS',

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
	'Target': 'SunOS',

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
	'Target': 'HP-UX',

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
