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
