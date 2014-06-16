{
	'Type I - vmstat': {
		'WORKER_MODULE_BYTE_STREAM_LINK': 'workerTypeI',
		'ARGV': argv_vmstat(),
		'WORKER_SCRIPT_LINK': 'script_vmstat'
	},
	'Type I - ping': {
		'WORKER_MODULE_BYTE_STREAM_LINK': 'workerTypeI',
		'ARGV': argv_ping(),
		'WORKER_SCRIPT_LINK': 'script_ping'
	},

	'Type II - ps': {
		'WORKER_MODULE_BYTE_STREAM_LINK': 'workerTypeII',
		'ARGV': argv_ps(),
		'WORKER_SCRIPT_LINK': 'script_ps'
	},
	'Type II - netstat': {
		'WORKER_MODULE_BYTE_STREAM_LINK': 'workerTypeII',
		'ARGV': argv_netstat(),
		'WORKER_SCRIPT_LINK': 'script_netstat'
	},
}
