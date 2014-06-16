def setByteStream2(byteStreamInstaller2):
	if byteStreamInstaller2 == None:
		return None

	"""
IN: hByteStream =
{
	# name: [entryPoint, filename]
	'bs_workerTypeI': ['workerTypeI', 'libworkerTypeI.so'],
	'bs_workerTypeII': ['workerTypeII', 'libworkerTypeII.so'],
}

OUT:
hByteStream2ByID = {
#	ID: [name, entryPoint, md5sum, byteStream]
	101L: ['bs_workerTypeI', 'workerTypeI', '...', '0x...'],
	102L: ['bs_workerTypeII', 'workerTypeII', '...', 'ox...']
}
hByteStream2ByName = {
#	name: ID
	'bs_workerTypeI': 101L,
	'bs_workerTypeII': 102L
}
"""

	# Reset all...
	global	hByteStream2ByID
	if not globals().has_key('hByteStream2ByID'):
		hByteStream2ByID = dict()
	global	hByteStream2ByName
	if not globals().has_key('hByteStream2ByName'):
		hByteStream2ByName = dict()

	returnList = list()	# [ID]:= Delete
				# [ID, True, True]|[ID, True, False]|[ID, False, True]:= Update
	id = 100	# Starting ID...
	for key in byteStreamInstaller2:
		bsRawList = byteStreamInstaller2[key]		# [entryPoint, filename]

		if bsRawList[0] == None:
			# Possibly delete...
			if not hByteStream2ByName.has_key(key):
				print '|%s|: Marked as delete but no match' % key
				continue

			old_id = hByteStream2ByName[key]
			if old_id != None:
				del hByteStream2ByName[key]
				del hByteStream2ByID[old_id]
				returnList.append({
					'ID': old_id,
					'Action': 'Delete'
				})
				print '|%s|(ID = %d): Deleted' % (key, old_id)
			else:
				print '|%s|: Marked as delete but no match' % key

			continue

		f = open(bsRawList[1], 'rb')	# Byte-stream
		byteStream = ''
		for _ in f:
			byteStream = byteStream + _
		f.close()
		import md5
		m = md5.new()
		m.update(byteStream)
		mh = m.hexdigest()
#		print mh

		if hByteStream2ByName.has_key(key):
			isUpdateByteStream = False

			# Possibly update...
			old_id = hByteStream2ByName[key]
			old_bsDict = hByteStream2ByID[old_id]	# {'Name': '...', 'Entry Point': '...', 'MD5': '...', 'Byte Stream': 0x...}

			if old_bsDict['MD5'] != mh:
				isUpdateByteStream = True
				old_bsDict['MD5'] = mh
				old_bsDict['Byte Stream'] = byteStream

			if isUpdateByteStream:
				returnList.append({
					'ID': old_id,
					'Action': 'Update',
				})
				print '|%s|(ID = %d): Updated' % (key, old_id)
			else:
				print '|%s|: Nothing new: Skipped' % key

			continue

		# Find new id
		while True:
			id = id + 1	# Increase ID by 1
			if not hByteStream2ByID.has_key(id):
				break

		hByteStream2ByName[key] = id
		# ID: [name, entryPoint, md5sum, byteStream]
		hByteStream2ByID[id] = {
			'Name': key,
			'Entry Point': bsRawList[0],
			'MD5': mh,
			'Byte Stream': byteStream

		}

	return returnList
