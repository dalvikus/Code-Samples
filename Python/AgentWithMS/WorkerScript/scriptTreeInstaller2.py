# LOAD__SCRIPT_FUNCTION "setScriptTree2" "setScriptTree2.py" 1
# LOAD__INSTALLER "setScriptTree2" "scriptTreeInstaller2.py"
# hScriptTree =
{
	# keyName:= name of var or func
	#	source filename = keyName + ".py"
				# varName
				#	None:= delete
				#	'...':= var = eval('...')
				#	(None,):= function
				#	('...',):= var = eval(compile('...', 'as...', 'exec'))
						# Require List
# 네 종류가 있다.
#	1. 주어진 이름의 함수를 등록한다.
#		(None,):= 주어진 이름의 함수를 등록
#	2. Script 의 내용을 인자로 사용하여 특정 함수를 호출하는 경우
#		(None, 'varName'):= Script 내용을 특정 변수(varName)로 설정
#	3. 주어진 이름의 함수를 실행하여 그 결과를 주어진 이름의 변수로 설정한다.
#		('hSysInfo',()):= 주어진 이름의 함수를 실행하여, 그 결과를 hSysInfo 에 넣는다.
#		이 때 인자는 () 를 사용하여 함수를 호출한다. 없더라도 반드시 ()은 사용해야 한다.
#	4. Script 의 내용을 특정 변수로 설정한다.
#		('setJob',):= Script 의 내용을 인자로 사용하여 "setJob" 이라는 함수를 호출한다.
#		이 때 추가적으로 사용하는 인자가 있을 수도 있다.
#		('setJob', 'a', 'b') 라면 'a', 'b' 가 추가인자로 사용된다.

	'setSysInfo':		[('hSysInfo', ()),	[]],		# Set hSysInfo

	# Parse...
	'parse_netstat':	[(None,),		[]],
	'parse_ping':		[(None,),		[]],
	'parse_ps':		[(None,),		[]],
	'parse_vmstat':		[(None,),		[]],

	# Argument...
	'argv_netstat':		[(None,),		[]],
	'argv_ping':		[(None,),		[]],
	'argv_ps':		[(None,),		[]],
	'argv_vmstat':		[(None,),		['setSysInfo']],	# Use hSysInfo

	# Return...
	'return_netstat':	[(None,),		['setWorker']],		# Use hWorker
	'return_ping':		[(None,),		[]],
	'return_ps':		[(None,),		[]],
	'return_vmstat':	[(None,),		[]],



	'setWorker':		[(None, 'hWorker'),	['argv_netstat', 'argv_ping', 'argv_ps', 'argv_vmstat']],
									# Set hWorker

	'setByteStream2':	[(None,),		[]],
	'byteStreamInstaller2':	[('setByteStream2',),	['setByteStream2']],

	'setCommand2':		[(None,),		[]],
	'commandInstaller2':	[('setCommand2',),	['setCommand2', 'byteStreamInstaller2']],

	'chkJob2': 		[(None,),		[]],
	'setJob2': 		[(None,),		['chkJob2']],
	'jobInstaller2':	[('setJob2',),		['setJob2', 'commandInstaller2']],

	'mkJob':		[(None,),		[]],
}
