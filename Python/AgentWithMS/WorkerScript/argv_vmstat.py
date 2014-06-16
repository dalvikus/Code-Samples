def argv_vmstat():
	arg0 = 'vmstat -n 1' if hSysInfo['systemname'] == 'Linux' else 'vmstat -n 1 1000000'
	arg1 = 2
	return [arg0, arg1]
