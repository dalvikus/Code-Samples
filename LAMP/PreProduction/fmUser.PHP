<?PHP
/*
 * 2005/12/11 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 상수들을 설정한다.
 */

	// 직위
	define("POSITION_LEADER", 1);	// 팀장
	define("POSITION_WORKER", 2);	// 팀원
	define("POSITION_VISITOR", 3);	// 방문자
	// 계정 상태
	define('IDSTAT_BLOCK', 0);	// 중지: 계정이 일지적으로 정지된 상태
	define('IDSTAT_INUSE', 1);	// 사용중
?>
<?PHP
//phpinfo();
	// Connecting, selecting database
	$link = mysql_connect('localhost', 'root', 'MySQL-4.1.14') or die('Could not connect: ' . mysql_error());
	mysql_select_db('PREPRODUCTION') or die('Could not select database');
	if (isset($_POST["PHP_EVAL_CODE"])) {
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML>
<HEAD>
<TITLE>사용자 관리</TITLE>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; CHARSET=euc-kr">
</HEAD>
<BODY>
<PRE>
<?PHP
?>
</PRE>
<SCRIPT LANGUAGE="JavaScript1.3">
	location.replace("fmUser.PHP");
</SCRIPT>
</BODY>
</HTML>
<?PHP
	} else {
if (1) {
		global	$JavaScriptEvalCode;
		$JavaScriptEvalCode = '';	// JavaScript가 해석할 문자열

		$query = 'SELECT T1.GROUP_ID, T2.USER_ID, NAME FROM TGROUP AS T1, TGROUP_USER AS T2 WHERE T1.GROUP_ID > 0 AND T1.GROUP_ID = T2.GROUP_ID ORDER BY T1.GROUP_ID, T2.USER_ID';
		if (!($result = mysql_query($query))) {
			$message = '';
			$message .= 'errno: |' . mysql_errno() . '|' . "\n";
			$message .= 'error: |' . mysql_error() . '|' . "\n";
			$message .= 'query: |' . $query . '|' . "\n";
			die($message);
		}
		$aGroup = array();
		while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
			$groupId = $row["GROUP_ID"];
			if (!isset($aGroup[$groupId])) {
				$aGroup[$groupId] = array(
					'id' => $groupId,
					'name' => $row["NAME"],
					'aUserId' => array()
				);
			}
			array_push($aGroup[$groupId]['aUserId'], $row["USER_ID"]);
		}
		// 사용한 결과를 반환한다.
		mysql_free_result($result);
//		print_r($aGroup);
		$a = array();	// JavaScript가 eval할 objGroup에 대한 정의들로 이루어진 배열
		foreach ($aGroup as $oGroup) {
			// function objGroup(id, name, aUserIdByVal);
			array_push($a,
				"\n" .
				"\t" . 'new objGroup(' .
					$oGroup['id'] . ', ' .
					'"' .  $oGroup['name'] . '"' . ', ' .
					'[' . join(', ', $oGroup['aUserId']) . ']' .
				')'
			);
		}
		$JavaScriptEvalCode .=
		'gaGroup = [' . join(',', $a) . "\n" .
		']' . "\n";

		$query = 'SELECT T1.USER_ID, T2.GROUP_ID, NICKNAME, FULLNAME, POSITION, STATUS FROM TUSER AS T1, TGROUP_USER AS T2 WHERE T1.USER_ID > 0 AND T1.USER_ID = T2.USER_ID ORDER BY T1.USER_ID, T2.GROUP_ID';
		if (!($result = mysql_query($query))) {
			$message = '';
			$message .= 'errno: |' . mysql_errno() . '|' . "\n";
			$message .= 'error: |' . mysql_error() . '|' . "\n";
			$message .= 'query: |' . $query . '|' . "\n";
			die($message);
		}
		$aUser = array();
		while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
			$userId = $row["USER_ID"];
			if (!isset($aUser[$userId])) {
				$aUser[$userId] = array(
					'id' => $userId,
					'nickname' => $row["NICKNAME"],
					'fullname' => $row["FULLNAME"],
					'position' => $row["POSITION"],
					'status' => $row["STATUS"],
					'aGroupId' => array()
				);
			}
			array_push($aUser[$userId]['aGroupId'], $row["GROUP_ID"]);
		}
		// 사용한 결과를 반환한다.
		mysql_free_result($result);
//		print_r($aUser);
		$a = array();	// JavaScript가 eval할 objUser에 대한 정의들로 이루어진 배열
		foreach ($aUser as $oUser) {
			// function objUser(id, nickname, fullname, position, status, aGroupIdByVal);
			array_push($a,
				"\n" .
				"\t" .
				'new objUser(' .
					$oUser['id'] . ', ' .
					'"' . $oUser['nickname'] . '"' . ', ' .
					'"' . $oUser['fullname'] . '"' . ', ' .
					$oUser['position'] . ', ' .
					$oUser['status'] . ', ' .
					'[' . join(', ', $oUser['aGroupId']) . ']' .
				')'
			);
		}
		$JavaScriptEvalCode .= "\n" .
		'gaUser = [' . join(',', $a) . "\n" .
		']' . "\n";

		// JavaScript에서 문자열로 받아들여야 하므로 다음과 같은 변환을 한다.
		// (\) -> (\\), (") -> (\")
//		echo '|' . $JavaScriptEvalCode . '|' . "\n";
		$JavaScriptEvalCode = preg_replace('/([\\\"])/', "\\\\$1", $JavaScriptEvalCode);
		// 줄바꿈도 변환한다.
		$JavaScriptEvalCode = preg_replace('/\n/', '\\\\n', $JavaScriptEvalCode);
//		echo '|' . $JavaScriptEvalCode . '|' . "\n";
}
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML>
<HEAD>
<TITLE>fmUser.PHP</TITLE>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; CHARSET=euc-kr">
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/12/18 (KST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * "반영하기"버튼을 구현한다.
 */

	function fnSubmit() {
		for (var i = 0; i < gaGroup.length; ++i) {
		}
		for (var i = 0; i < gaUser.length; ++i) {
		}
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/12/15 (KST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * "더하기", "고치기", "지우기" 버튼에 대한 이벤트핸들러를 구현한다.
 */

	function fnInsertGroup() {
		var	groupName = txtGroupName.value;
		if (groupName == '') {
			alert('그룹 이름을 정해야 합니다');
			return;
		}

		var	index = -1;	// 이 GROUP이 추가된 gaGroup에서의 위치
		var	rowIndex = gGroup.rowIndex;	// 이 GROUP이 표시될 행을 말해준다.
		for (var i = 0; i < gaGroup.length; ++i) {
			var	oGroup = gaGroup[i];
			if (oGroup.rowIndex == -1) {	// 삭제된 GROUP은
				// 건너뛴다.
				continue;
			}

			if (oGroup.name == groupName) {
				alert('같은 이름("' + groupName + '")을 가진 GROUP이 ' + (1 + oGroup.rowIndex) + '번째 행에 존재합니다.' + '\n' + '다른 이름을 사용하세요.');
				return;
			}
			if (rowIndex == gaGroup[i].rowIndex) {
				index = i;
			}
		}
		if (rowIndex == -1) {	// 처음 추가되는 GROUP이라면
			index = 0;
			rowIndex = 0;
		}
		// 새로운 GROUP은 gaGroup의 index번째 성분으로,
		// GROUP목록의 rowIndex번째 행에 표시된다.

		// GROUP목록 중에서 .rowIndex가 rowIndex보다 크거나 같은 GROUP의 .rowIndex값을 1씩 증가시킨다.
		// 이 때 웹에서 새로 만든 GROUP이라면 id도 적절히 바꾼다.
		for (var i = 0; i < gaGroup.length; ++i) {
			var	oGroup = gaGroup[i];
			if (oGroup.rowIndex == -1) {	// 삭제된 GROUP이라면 건너뛴다.
				continue;
			}

			if (oGroup.rowIndex >= rowIndex) {
				++oGroup.rowIndex;
				if (oGroup.id < 0) {
					--oGroup.id;
				}
			}
		}

		// USER목록 중에서 각 USER가 속한 GROUP에 대한 정보가 rowIndex보다 크거나 같다면
		// 즉 새로 만든 GROUP에 속한다면,
		// 그 값도 바꾼다.
		for (var i = 0; i < gaUser.length; ++i) {
			var	oUser = gaUser[i];
			if (oUser.rowIndex == -1) {	// 삭제된 USER라면
				// 건너뛴다.
				continue;
			}

			for (var k = 0; k < oUser.aGroupId.length; ++k) {
				if (oUser.aGroupId[k] <= -rowIndex - 1) {
					oUser.aGroupId.splice(k, 1, -rowIndex - 1 - 1);
				}
			}
		}
		for (var k = 0; k < gUser.aGroupId.length; ++k) {
			if (gUser.aGroupId[k] <= -rowIndex - 1) {
				gUser.aGroupId.splice(k, 1, -rowIndex - 1 - 1);
			}
		}

		gGroup.id = -rowIndex - 1;
		gGroup.name = groupName;
		gGroup.aUserId = [];
		var	aNickname = [];
		var	aSelectedgaUserIndex = [];
		for (var i = 0; i < sltUser.options.length; ++i) {
			var	oOp = sltUser.options[i];
			var	a = oOp.value.split(':');
			var	iUser = a[0];
			var	oUser = gaUser[iUser];	// gaUser[iUser]에 해당하는 USER가
			// a[1]은 이 GROUP에 속해있었는 지를 말해준다.
			var	selected = oOp.selected;
			oOp.value = iUser + (selected ? ':1' : ':0');
			if (selected) {
				// 이 USER는 GROUP에 추가한다.
				var	userId = oUser.id;
				gGroup.aUserId.push(userId);
				aNickname.push(oOp.text);
				aSelectedgaUserIndex.push(iUser);
				// USER의 GROUP목록에 이 GROUP을 추가한다.
				oUser.aGroupId.push(gGroup.id);
				if (userId == gUserId) {
					gUser.aGroupId.push(gGroup.id);
				}
			}
		}
		gGroup.rowIndex = rowIndex;

		// gaGroup의 index번째에 gGroup을 추가한다.
		gaGroup.splice(index, 0, new objGroup(gGroup.id, gGroup.name, gGroup.aUserId));
		// 새로운 GROUP은 목록의 rowIndex행에 나타난다.
		gaGroup[index].rowIndex = rowIndex;

		// GROUP목록의 rowIndex번째 행에 표시한다.
if (1) {
		var	oTr = tblGroupList.insertRow(1 + rowIndex);	// 머리행은 건너뛴다.
		if (!oTr) {
			alert('tblGroupList.insertRow(): Failure');
			return false;
		}

		// 선택
		var	oTd = oTr.insertCell();
		if (!oTd) {
			alert('.insertCell: Failure');
			tblGroupList.deleteRow(oTr.rowIndex);
			return false;
		}
		oTd.align = "CENTER";
		oTd.innerHTML = '<INPUT TYPE="RADIO" NAME="GROUPROWINDEX" onClick="fnGroupRadioOnClick(this);">';

		// 이름
		var	oTd = oTr.insertCell();
		if (!oTd) {
			alert('.insertCell: Failure');
			tblGroupList.deleteRow(oTr.rowIndex);
			return false;
		}
		oTd.align = "CENTER";
		oTd.innerHTML = gGroup.name;

		// 이 GROUP에 속한 USER들의 아이디
		var	oTd = oTr.insertCell();
		if (!oTd) {
			alert('.insertCell: Failure');
			tblGroupList.deleteRow(oTr.rowIndex);
			return false;
		}
		oTd.align = "LEFT";
		oTd.innerHTML = aNickname.join(', ');
}
		// 그 행을 가리키는 RADIO버튼을 CHECKED상태로 만든다.
		var	oRadio = tblGroupList.rows[1 + rowIndex].children[0].children[0];
		oRadio.checked = true;

		fnDisplayUser();

		// USER목록에서 USER가 속한 GROUP의 이름을 갱신한다.
if (1) {
		for (var i = 0; i < aSelectedgaUserIndex.length; ++i) {
			var	oUser = gaUser[aSelectedgaUserIndex[i]];
			var	aGroupName = [];
			for (var iSelect = 0; iSelect < sltGroup.options.length; ++iSelect) {
				var	a = sltGroup.options[iSelect].value.split(':');
				var	oGroup = gaGroup[a[0]];
				for (var k = 0; k < oUser.aGroupId.length; ++k) {
					if (oGroup.id == oUser.aGroupId[k]) {
						aGroupName.push(oGroup.name);
						break;
					}
				}
			}
			tblUserList.rows[1 + oUser.rowIndex].children[5].innerHTML = aGroupName.join(', ');
		}
}
	}
	function fnUpdateGroup() {
		var	groupName = txtGroupName.value;
		if (groupName == '') {
			alert('그룹 이름을 정해야 합니다');
			return;
		}

		var	rowIndex = gGroup.rowIndex;
		var	iGroup;	// gaGroup중에서 GROUP목록의 rowIndex번째 행에 해당하는 objGroup에 대한 첨자
		for (var i = 0; i < gaGroup.length; ++i) {
			var	oGroup = gaGroup[i];
			if (oGroup.rowIndex == -1) {	// 삭제된 GROUP은
				// 건너뛴다.
				continue;
			}

			if (oGroup.id != gGroup.id && oGroup.name == groupName) {
				alert('같은 이름("' + groupName + '")을 가진 GROUP이 ' + (1 + oGroup.rowIndex) + '번째 행에 존재합니다.' + '\n' + '다른 이름을 사용하세요.');
				return;
			}
			if (rowIndex == oGroup.rowIndex) {
				iGroup = i;
			}
		}

		if (iGroup === undefined) {
			alert('GROUP목록의 ' + (1 + rowIndex) + '번째 행에 해당하는 GROUP을 찾을 수 없다');
			return;
		}
		// gaGroup의 iGroup번째 성분의 정보를 변경한다.

		var	aaNewGroupId = [];	// 이 GROUP에서 제외된 USER가 속하는 새로운 GROUP목록들로 이루어진 배열
		var	aAddUserId = [];	// USER의 GROUP목록에 이 GROUP이 새롭게 추가되는 USER목록
		var	aUserId = [];
		var	aNickname = [];
		for (var i = 0; i < sltUser.options.length; ++i) {
			var	oOp = sltUser.options[i];
			var	a = oOp.value.split(':');
			var	iUser = a[0];
			var	oUser = gaUser[iUser];	// gaUser[iUser]에 해당하는 USER가
			var	bSelected = oOp.selected;
			// a[1]은 이 GROUP에 속해있었는 지를 말해준다.
			if (!bSelected && a[1] == '1') {	// 이 GROUP에서 USER가 제외되었다.
				// 이 USER가 속한 GROUP이 있는 지 알아본다.
				// 없다면 이 GROUP에서 해당 USER는 제외될 수 없다.
				var	aGroupId = [];	// USER가 속할 GROUP의 ID로 이루어진 배열
				for (var k = 0; k < oUser.aGroupId.length; ++k) {
					if (oUser.aGroupId[k] == gGroup.id) {
						// 이 GROUP에서는 제외될 것이므로 이 GROUP은 갯수에 포함하지 않는다.
					} else {
						aGroupId.push(oUser.aGroupId[k]);
					}
				}
				if (!aGroupId.length) {
					alert('이 GROUP("' + gGroup.name + '")은 USER("' + oUser.nickname + '")가 속한 유일한 GROUP이므로 이 USER는 GROUP에서 제외될 수 없다');
					return;
				}
				// USER가 속한 GROUP의 ID를 변경한다.
				aaNewGroupId[iUser] = aGroupId;
				if (gUser.id == oUser.id) {
					gUser.aGroupId = aGroupId;
				}
			}
			if (bSelected && a[1] == '0') {	// 이 GROUP에 새롭게 추가되었다.
				// 이 USER의 GROUP목록에 이 GROUP을 추가한다.
				aAddUserId[iUser] = 1;
				if (gUser.id == oUser.id) {
					gUser.aGroupId.push(gGroup.id);
				}
			}
			if (oOp.selected) {	// 이 GROUP에 새로운 USER가 추가되었다.
				// 이 USER는 GROUP에 추가한다.
				aUserId.push(oUser.id);
				aNickname.push(oOp.text);
			}
			oOp.value = iUser + ':' + (bSelected ? '1' : '0');
		}
		// 제외된 USER들에서, 각 USER가 속한 GROUP의 ID를 변경한다.
		for (var iUser = 0; iUser < gaUser.length; ++iUser) {
			var	oUser = gaUser[iUser];
			var	userId = oUser.id;
			if (oUser.rowIndex == -1) {	// 표시할 수 없는 USER는 제외한다.
				continue;
			}

			if (
				aaNewGroupId[iUser] === undefined &&	// USER의 GROUP목록에 바뀌지도 않았고,
				aAddUserId[iUser] === undefined		// USER의 GROUP목록에 현재 GROUP을 추가하지도 않은 경우
			) {
				continue;
			}

			// USER의 GROUP목록을 변경한다.
			if (aaNewGroupId[iUser] !== undefined) {	// USER의 GROUP목록이 바뀌었다.
				oUser.aGroupId = aaNewGroupId[iUser];
			} else if (aAddUserId[iUser] !== undefined) {	// USER의 GROUP목록에 현재 GROUP을 추가한다.
				oUser.aGroupId.push(gGroup.id);
			}
		}

		gaGroup[iGroup].name = groupName;
		gGroup.name = groupName;
		tblGroupList.rows[1 + rowIndex].children[1].innerHTML = groupName;
		gaGroup[iGroup].aUserId = aUserId;
		gGroup.aUserId = aUserId;
		tblGroupList.rows[1 + rowIndex].children[2].innerHTML = aNickname.join(', ');

		fnDisplayUser();

		// 변경된 GROUP목록을 표시한다.
		for (var iUser = 0; iUser < gaUser.length; ++iUser) {
			var	oUser = gaUser[iUser];
			var	userId = oUser.id;
			if (oUser.rowIndex == -1) {	// 표시할 수 없는 USER는 제외한다.
				continue;
			}

			var	bSelected = false;
			for (var i = 0; i < aUserId.length; ++i) {
				if (userId == aUserId[i]) {
					bSelected = true;
					break;
				}
			}
			if (
				!bSelected &&
				aaNewGroupId[iUser] === undefined &&	// USER의 GROUP목록에 바뀌지도 않았고,
				aAddUserId[iUser] === undefined		// USER의 GROUP목록에 현재 GROUP을 추가하지도 않은 경우
			) {
				continue;
			}

			var	aGroupName = [];
			for (var iGroupRow = 0; iGroupRow < sltGroup.options.length; ++iGroupRow) {
				var	oOp = sltGroup.options[iGroupRow];
				var	a = oOp.value.split(':');
				for (var i = 0; i < oUser.aGroupId.length; ++i) {
					if (gaGroup[a[0]].id == oUser.aGroupId[i]) {
						aGroupName.push(oOp.text);
					}
				}
			}
			tblUserList.rows[1 + oUser.rowIndex].children[5].innerHTML = aGroupName.join(', ');
		}
	}
	function fnDeleteGroup() {
		// USER중에서, 삭제될려는 GROUP이 USER가 속한 유일한 GROUP인지 확인한다.
		var	aModified = [];	// aModified[i]가 undefined가 아니라면,
					// i번째 USER가 속한 GROUP에서 지우려는 GROUP이 제외된 GROUP들의
					// ID들로 이루어진 배열이다.
		for (var i = 0; i < gaUser.length; ++i) {
			var	oUser = gaUser[i];
			if (oUser.rowIndex == -1) {	// 이미 삭제된 USER는
				// 건너뛴다.
				continue;
			}

			var	aGroupId = [];
			var	bModified = false;	// USER가 속한 GROUP에 지우려는 GROUP이 속했는 지를 말해준다.
			for (var k = 0; k < oUser.aGroupId.length; ++k) {
				var	groupId = oUser.aGroupId[k];
				if (gGroup.id == groupId) {
					bModified = true;
				} else {
					aGroupId.push(groupId);
				}
			}
			if (aGroupId.length == 0) {	// 지우려는 GROUP이 USER가 속한 GROUP이다.
				alert('지우려는 GROUP("' + gGroup.name + '")은 USER("' + oUser.nickname + '")이 속한 유일한 GROUP이므로 지울 수 없습니다');
				return;
			}
			if (bModified) {	// USER가 속한 GROUP목록에 변화가 있다.
				aModified[i] = aGroupId;
			}
		}

		// gaGroup에서 해당 GROUP을 삭제하거나(새로 만든 GROUP의 경우),
		// 삭제했다고(DB에서 가져온 GROUP) 표시한다.
		var	rowIndex = gGroup.rowIndex;	// 이 GROUP이 표시된 행을 말해준다.
		var	iGroup;		// gaGroup에서 몇 번째 성분인지를 말해준다.
		for (var i = 0; i < gaGroup.length; ++i) {
			if (rowIndex == gaGroup[i].rowIndex) {
				iGroup = i;
			}
		}
		if (iGroup === undefined) {
			alert(rowIndex + '번째 행에 해당하는 GROUP을 찾을 수 없다');
			return;
		}
		if (gGroup.id < 0) {	// 웹에서 새로 만든 GROUP이라면
			// 해당 GROUP을 삭제한다.
			gaGroup.splice(iGroup, 1);
		} else {		// DB에서 가져온 GROUP이라면
			// 삭제했다고 표시한다.
			gaGroup[iGroup].rowIndex = -1;
		}
		// GROUP목록에서 해당 GROUP행을 삭제한다.
		tblGroupList.deleteRow(1 + rowIndex);

		// USER중에서 USER가 속한 GROUP목록에 변화가 있다면 GROUP목록을 수정한다.
		for (var i = 0; i < gaUser.length; ++i) {
			if (aModified[i] === undefined) {	// 변화가 없는 USER는
				// 건너뛴다.
				continue;
			}
			gaUser[i].aGroupId = aModified[i];
		}

		// GROUP목록에서 .rowIndex가 rowIndex보다 큰 GROUP의 .rowIndex값을 1만큼 감소시킨다.
		for (var i = 0; i < gaGroup.length; ++i) {
			var	oGroup = gaGroup[i];
			if (oGroup.rowIndex == -1) {	// 삭제된 GROUP은
				// 건너뛴다.
				continue;
			}

			if (oGroup.rowIndex > rowIndex) {
				--oGroup.rowIndex;
			}
		}

		// USER가 속한 목록중에서 rowIndex와 관련된 ID의 경우(.id < 0),
		// 같은 과정을 밟는다.
		for (var i = 0; i < gaUser.length; ++i) {
			var	oUser = gaUser[i];
			if (oUser.rowIndex == -1) {	// 이미 삭제된 USER는
				// 건너뛴다.
				continue;
			}

			if (oUser.id > 0) {	// rowIndex와 관련이 없다.
				continue;
			}

			if (oUser.id < -rowIndex - 1) {
				++oUser.id;
			}
		}

		// 새로운 GROUP을 표시한다.
		if (rowIndex == sltGroup.options.length - 1) {	// 제일 마지막 GROUP이였다면
			--rowIndex;
		}
		var	groupId;
		// rowIndex에 해당하는 GROUP을 찾는다.
		for (var i = 0; i < gaGroup.length; ++i) {
			var	oGroup = gaGroup[i];
			if (oGroup.rowIndex == -1) {	// 삭제된 GROUP은
				// 건너뛴다.
				continue;
			}

			if (oGroup.rowIndex == rowIndex) {
				groupId = oGroup.id;
				break;
			}
		}
		gGroupId = groupId === undefined ? -1 : groupId;
		// gGroupId에 해당하는 gGroup를 설정한다.
		fnSetGroup();
		// gGroup의 정보를 표시한다.
		fnDisplayGroup();

		fnDisplayUser();
		// USER가 속한 GROUP목록에 대한 표시를 변경한다.
		for (var i = 0; i < gaUser.length; ++i) {
			var	oUser = gaUser[i];
			if (aModified[i] === undefined) {	// GROUP목록에 변화가 없는 USER는
				// 건너뛴다.
				continue;
			}

			var	aGroupName = [];
			for (var j = 0; j < sltGroup.options.length; ++j) {
				var	oOp = sltGroup.options[j];
				var	a = oOp.value.split(':');
				var	iGroup = a[0];
				for (var k = 0; k < oUser.aGroupId.length; ++k) {
					if (oUser.aGroupId[k] == gaGroup[iGroup].id) {
						aGroupName.push(gaGroup[iGroup].name);
						break;
					}
				}
			}
			tblUserList.rows[1 + oUser.rowIndex].children[5].innerHTML = aGroupName.join(', ');
		}
	}

	function fnInsertUser() {
	}
	function fnUpdateUser() {
		alert(gUser.rowIndex);
	}
	function fnDeleteUser() {
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/12/11 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 상수들을 설정한다.
 */

	// 직위
	var	cPOSITION_LEADER	= <?PHP echo POSITION_LEADER ?>;
	var	cPOSITION_WORKER	= <?PHP echo POSITION_WORKER ?>;
	var	cPOSITION_VISITOR	= <?PHP echo POSITION_VISITOR ?>;
	// 계정 상태
	var	cIDSTAT_BLOCK		= <?PHP echo IDSTAT_BLOCK ?>;
	var	cIDSTAT_INUSE		= <?PHP echo IDSTAT_INUSE ?>;
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/12/11 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * gGroup(gUser)에 대한 정보를 표시한다.
 */

	// gGroup에 대한 정보를 표시한다.
	function fnDisplayGroup() {
		// gGroup에 해당하는 행이 선택한다.
		// (정책) 해당하는 행이 없다면, 표시가능한 제일 마지막 행을 선택한다.
		var	rowIndex = gGroup.rowIndex;
		for (var iGroup = 0; iGroup < gaGroup.length; ++iGroup) {
			var	oGroup = gaGroup[iGroup];
			if (oGroup.rowIndex == -1) {	// 이미 삭제된 GROUP은 건너뛴다.
				// oGroup.rowIndex = -1
				continue;
			}

			var	oRadio = tblGroupList.rows[1 + oGroup.rowIndex].children[0].children[0];
			oRadio.checked = rowIndex == -1 || rowIndex == oGroup.rowIndex;
		}

		txtGroupName.value = gGroup.name;

		// USER들의 모든 목록을 SELECT에 표시하고,
		// gGroup에 속하는 USER들을 SELECTED상태로 만든다.
		fnAddOptionsToSltUser();
	}
	// USER들의 모든 목록을 SELECT에 표시하고,
	// gGroup에 속하는 USER들을 SELECTED상태로 만든다.
	function fnAddOptionsToSltUser() {
		// USER목록에 나타난 순서대로 gaUser에 대한 첨자의 배열을 만든다.
		var	aUserIndexByRow = [];
		for (var iUser = 0; iUser < gaUser.length; ++iUser) {
			var	rowIndex = gaUser[iUser].rowIndex;
			if (rowIndex >= 0) {	// USER목록에 표시된 USER에 한해서
				aUserIndexByRow.splice(rowIndex, 0, iUser);
			}
		}

		// 이 배열을 표시한다.
		sltUser.options.length = 0;	// 초기화한다.
		for (var rowIndex = 0; rowIndex < aUserIndexByRow.length; ++rowIndex) {
			var	oOp = document.createElement("OPTION");
			var	iUser = aUserIndexByRow[rowIndex];
			oOp.text = gaUser[iUser].nickname;
			var	bSelected = false;
			for (var iGroup = 0; iGroup < gGroup.aUserId.length; ++iGroup) {
				if (gaUser[iUser].id == gGroup.aUserId[iGroup]) {
					bSelected = true;
					break;
				}
			}
			oOp.value = iUser + (bSelected ? ':1' : ':0');
			oOp.selected = bSelected;
			sltUser.options.add(oOp);
		}
	}

	// gUser에 대한 정보를 표시한다.
	function fnDisplayUser() {
		// gUser에 해당하는 행이 선택한다.
		// (정책) 해당하는 행이 없다면, 표시가능한 제일 마지막 행을 선택한다.
		var	rowIndex = gUser.rowIndex;
		for (var iUser = 0; iUser < gaUser.length; ++iUser) {
			var	oUser = gaUser[iUser];
			if (oUser.rowIndex == -1) {	// 이미 삭제된 USER는 건너뛴다.
				// oUser.rowIndex = -1
				continue;
			}

			var	oRadio = tblUserList.rows[1 + oUser.rowIndex].children[0].children[0];
			oRadio.checked = rowIndex == -1 || rowIndex == oUser.rowIndex;
		}

		txtNickname.value = gUser.nickname;
		txtPassword.value = gUser.password;
		txtFullname.value = gUser.fullname;
		// idPosition에 있는 RADIO버튼 중에서,
		// gUser.position에 해당하는 값을 가진 RADIO버튼을 CHECKED상태로 만든다.
		for (var i = 0; i < idPosition.children.length; ++i) {
			idPosition.children[i].checked = idPosition.children[i].value == gUser.position;
		}
		// idStatus에 있는 RADIO버튼 중에서,
		// gUser.status에 해당하는 값을 가진 RADIO버튼을 CHECKED상태로 만든다.
		for (var i = 0; i < idStatus.children.length; ++i) {
			idStatus.children[i].checked = idStatus.children[i].value == gUser.status;
		}

		// GROUP들의 모든 목록을 SELECT에 표시하고,
		// gUser가 속하는 GROUP들을 SELECTED상태로 만든다.
		fnAddOptionsToSltGroup();
	}
	// GROUP들의 모든 목록을 SELECT에 표시하고,
	// gUser가 속하는 GROUP들을 SELECTED상태로 만든다.
	function fnAddOptionsToSltGroup() {
		// GROUP목록에 나타난 순서대로 gaGroup에 대한 첨자의 배열을 만든다.
		var	aGroupIndexByRow = [];
		for (var iGroup = 0; iGroup < gaGroup.length; ++iGroup) {
			var	rowIndex = gaGroup[iGroup].rowIndex;
			if (rowIndex >= 0) {	// GROUP목록에 표시된 GROUP에 한해서
				aGroupIndexByRow.splice(rowIndex, 0, iGroup);
			}
		}

		// 이 배열을 표시한다.
		sltGroup.options.length = 0;	// 초기화한다.
		for (var rowIndex = 0; rowIndex < aGroupIndexByRow.length; ++rowIndex) {
			var	oOp = document.createElement("OPTION");
			var	iGroup = aGroupIndexByRow[rowIndex];
			oOp.text = gaGroup[iGroup].name;
			var	bSelected = false;
			for (var iUser = 0; iUser < gUser.aGroupId.length; ++iUser) {
				if (gaGroup[iGroup].id == gUser.aGroupId[iUser]) {
					bSelected = true;
					break;
				}
			}
			oOp.value = iGroup + (bSelected ? ':1' : ':0');
			oOp.selected = bSelected;
			sltGroup.options.add(oOp);
		}
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/12/11 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * GROUP(UESR)목록을 만들고, gGroupId(gUserId)에 해당하는 gGroup(gUser)를 설정한다.
 */

	// gaGroup에 속하는 GROUP들은 GROUP목록에 표시한다.
	function fnListGroup() {
		for (var iGroup = 0; iGroup < gaGroup.length; ++iGroup) {
			var	oGroup = gaGroup[iGroup];
			if (oGroup.bDeleted) {	// 이미 삭제된 GROUP은 건너뛴다.
				// oGroup.rowIndex = -1
				continue;
			}

			var	oTr = tblGroupList.insertRow(1 + oGroup.rowIndex);	// 머리행은 건너뛴다.
			if (!oTr) {
				alert('tblGroupList.insertRow(): Failure');
				return false;
			}

			// 선택
			var	oTd = oTr.insertCell();
			if (!oTd) {
				alert('.insertCell: Failure');
				tblGroupList.deleteRow(oTr.rowIndex);
				return false;
			}
			oTd.align = "CENTER";
			oTd.innerHTML = '<INPUT TYPE="RADIO" NAME="GROUPROWINDEX" VALUE="' + iGroup + '" onClick="fnGroupRadioOnClick(this);">';

			// 이름
			var	oTd = oTr.insertCell();
			if (!oTd) {
				alert('.insertCell: Failure');
				tblGroupList.deleteRow(oTr.rowIndex);
				return false;
			}
			oTd.align = "CENTER";
			oTd.innerHTML = oGroup.name;

			// 이 GROUP에 속한 USER들의 아이디
			var	oTd = oTr.insertCell();
			if (!oTd) {
				alert('.insertCell: Failure');
				tblGroupList.deleteRow(oTr.rowIndex);
				return false;
			}
			oTd.align = "LEFT";
			var	aUserNickname = [];
			for (var i = 0; i < oGroup.aUserId.length; ++i) {
				var	userId = oGroup.aUserId[i];
				for (k = 0; k < gaUser.length; ++k) {
					if (
						(userId > 0 && userId == gaUser[k].id) ||
							// userId는 기존 USER의 USER_ID이고,
							// 그 USER_ID가 일치하거나
						(userId < 0 && (-userId - 1) == gaUser[k].rowIndex)
							// userId는 새로 만든 USER목록의 (-userId - 1)번째 USER에 해당하고,
							// 그 행이 일치하는 경우
					) {
						aUserNickname.push(gaUser[k].nickname);
						break;
					}
				}
			}
			oTd.innerHTML = aUserNickname.join(', ');
		}
	}
	// GROUP목록에서 RADIO버튼이 선택되었을 때 호출되는 이벤트핸들러이다.
	function fnGroupRadioOnClick(oRadio) {
		var	rowIndex = oRadio.parentElement.parentElement.rowIndex - 1;	// 머리행은 제외한다.
		var	groupId;
		for (var i = 0; i < gaGroup.length; ++i) {
			if (rowIndex == gaGroup[i].rowIndex) {
				groupId = gaGroup[i].id;
				break;
			}
		}
		if (groupId === undefined) {
			alert('GROUP목록에서 ' + (1 + rowIndex) + '행에 해당하는 GROUP를 찾을 수 없다');
		}
		gGroupId = groupId;

		// gGroupId에 해당하는 gGroup를 설정한다.
		fnSetGroup();

		// gGroup의 정보를 표시한다.
		fnDisplayGroup();
	}
	// gGroupId에 해당하는 gGroup를 설정한다.
	function fnSetGroup() {
		// gGroupId의 유효성을 검사하고,
		// gGroupId에 해당하는 objGroup을 gGroup에 "복사"한다.
		var	index;	// gGroup에 해당하는 gaGroup배열에서의 첨자
		for (var i = 0; i < gaGroup.length; ++i) {
			if (gGroupId === gaGroup[i].id) {
				index = i;
				break;
			}
		}
		if (index === undefined) {
			// gGroupId에 해당하는 objGroup이 없다.
			// (정책) 제일 마지막 표시가능한 GROUP을 기본으로 한다.
			for (var i = gaGroup.length - 1; i >= 0; --i) {
				if (gaGroup[i].rowIndex != -1) {
					// gGroupId를 새로 설정한다.
					gGroupId = gaGroup[i].id;
					index = i;
					break;
				}
			}
			if (index == undefined) {
				// 표시가능한 GROUP이 하나도 없다.
				gGroupId = 0;
			}
		}

		if (index === undefined) {
			// 표시가능한 어떠한 GROUP도 없다.
			btnUpdateGroup.disabled = true;
			btnDeleteGroup.disabled = true;

			// gGroup에는 새로운 GROUP에 대한 정보를 담는다.
			gGroup = new objGroup(0, '그룹 이름', []);

			// 이 GROUP은 목록의 어떤 행과도 관련이 없다.
			gGroup.rowIndex = -1;
		} else {
			btnUpdateGroup.disabled = false;
			btnDeleteGroup.disabled = false;

			// gGroupId에 해당하는 objGroup을 찾았다.
			// 그 GROUP의 정보를 gGroup에 복사한다.
			gGroup = new objGroup(gaGroup[index].id, gaGroup[index].name, gaGroup[index].aUserId);

			// 이 GROUP은 목록에서, gGroupId에 해당하는 objGroup을 내용을 복사한 것이다.
			gGroup.rowIndex = gaGroup[index].rowIndex;
		}
	}

	// gaUser에 속하는 USER들은 USER목록에 표시한다.
	function fnListUser() {
		for (var iUser = 0; iUser < gaUser.length; ++iUser) {
			var	oUser = gaUser[iUser];
			if (oUser.bDeleted) {	// 이미 삭제된 USER는 건너뛴다.
				// oUser.rowIndex = -1
				continue;
			}

			var	oTr = tblUserList.insertRow(1 + oUser.rowIndex);	// 머리행은 건너뛴다.
			if (!oTr) {
				alert('tblUserList.insertRow(): Failure');
				return false;
			}

			// 선택
			var	oTd = oTr.insertCell();
			if (!oTd) {
				alert('.insertCell: Failure');
				tblUserList.deleteRow(oTr.rowIndex);
				return false;
			}
			oTd.align = "CENTER";
			oTd.innerHTML = '<INPUT TYPE="RADIO" NAME="USERROWINDEX" VALUE="' + iUser + '" onClick="fnUserRadioOnClick(this);">';

			// 아이디
			var	oTd = oTr.insertCell();
			if (!oTd) {
				alert('.insertCell: Failure');
				tblUserList.deleteRow(oTr.rowIndex);
				return false;
			}
			oTd.align = "CENTER";
			oTd.innerHTML = oUser.nickname;

			// 이름
			var	oTd = oTr.insertCell();
			if (!oTd) {
				alert('.insertCell: Failure');
				tblUserList.deleteRow(oTr.rowIndex);
				return false;
			}
			oTd.align = "CENTER";
			oTd.innerHTML = oUser.fullname == '' ? '&nbsp;' : oUser.fullname;

			// 직위
			var	oTd = oTr.insertCell();
			if (!oTd) {
				alert('.insertCell: Failure');
				tblUserList.deleteRow(oTr.rowIndex);
				return false;
			}
			oTd.align = "CENTER";
			oTd.innerHTML = oUser.position == cPOSITION_LEADER ? '팀장' : '팀원';

			// 계정상태
			var	oTd = oTr.insertCell();
			if (!oTd) {
				alert('.insertCell: Failure');
				tblUserList.deleteRow(oTr.rowIndex);
				return false;
			}
			oTd.align = "CENTER";
			oTd.innerHTML = oUser.status == cIDSTAT_INUSE ? '사용중' : '일시정지';

			// 이 USER가 속한 GROUP들의 이름
			var	oTd = oTr.insertCell();
			if (!oTd) {
				alert('.insertCell: Failure');
				tblUserList.deleteRow(oTr.rowIndex);
				return false;
			}
			oTd.align = "LEFT";
			var	aGroupName = [];
			for (var i = 0; i < oUser.aGroupId.length; ++i) {
				var	groupId = oUser.aGroupId[i];
				for (k = 0; k < gaGroup.length; ++k) {
					if (
						(groupId > 0 && groupId == gaGroup[k].id) ||
							// groupId는 기존 GROUP의 GROUP_ID이고,
							// 그 GROUP_ID가 일치하거나
						(groupId < 0 && (-groupId - 1) == gaGroup[k].rowIndex)
							// groupId는 새로 만든 GROUP목록의 (-groupId - 1)번째 GROUP에 해당하고,
							// 그 행이 일치하는 경우
					) {
						aGroupName.push(gaGroup[k].name);
						break;
					}
				}
			}
			oTd.innerHTML = aGroupName.join(', ');
		}
	}
	// USER목록에서 RADIO버튼이 선택되었을 때 호출되는 이벤트핸들러이다.
	function fnUserRadioOnClick(oRadio) {
		var	rowIndex = oRadio.parentElement.parentElement.rowIndex - 1;	// 머리행은 제외한다.
		var	userId;
		for (var i = 0; i < gaUser.length; ++i) {
			if (rowIndex == gaUser[i].rowIndex) {
				userId = gaUser[i].id;
				break;
			}
		}
		if (userId === undefined) {
			alert('USER목록에서 ' + (1 + rowIndex) + '행에 해당하는 USER를 찾을 수 없다');
		}
		gUserId = userId;

		// gUserId에 해당하는 gUser를 설정한다.
		fnSetUser();

		// gUser의 정보를 표시한다.
		fnDisplayUser();
	}
	// gUserId에 해당하는 gUser를 설정한다.
	function fnSetUser() {
		// gUserId의 유효성을 검사하고,
		// gUserId에 해당하는 objUser을 gUser에 "복사"한다.
		var	index;	// gUser에 해당하는 gaUser배열에서의 첨자
		for (var i = 0; i < gaUser.length; ++i) {
			if (gUserId === gaUser[i].id) {
				index = i;
				break;
			}
		}
		if (index === undefined) {
			// gUserId에 해당하는 objUser이 없다.
			// (정책) 제일 마지막 표시가능한 USER를 기본으로 한다.
			for (var i = gaUser.length - 1; i >= 0; --i) {
				if (gaUser[i].rowIndex != -1) {
					// gUserId를 새로 설정한다.
					gUserId = gaUser[i].id;
					index = i;
					break;
				}
			}
			if (index == undefined) {
				// 표시가능한 USER가 하나도 없다.
				gUserId = 0;
			}
		}

		if (index === undefined) {
			// 표시가능한 어떠한 USER도 없다.
			btnUpdateUser.disabled = true;
			btnDeleteUser.disabled = true;

			// gUser에는 새로운 USER에 대한 정보를 담는다.
			gUser = new objUser(0, '아이디', '전체 이름', cPOSITION_WORKER, cIDSTAT_INUSE, []);

			// 이 USER은 목록의 어떤 행과도 관련이 없다.
			gUser.rowIndex = -1;
		} else {
			btnUpdateUser.disabled = false;
			btnDeleteUser.disabled = false;

			// gUserId에 해당하는 objUser을 찾았다.
			// 그 USER의 정보를 gUser에 복사한다.
			gUser = new objUser(gaUser[index].id, gaUser[index].nickname, gaUser[index].fullname, gaUser[index].position, gaUser[index].status, gaUser[index].aGroupId);

			// 이 USER은 목록에서, gUserId에 해당하는 objUser을 내용을 복사한 것이다.
			gUser.rowIndex = gaUser[index].rowIndex;
		}
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/12/11 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 웹 초기화과정을 수행한다.
 */

	var	gaVarName = new Array(
		'gGroupId',
		'gUserId'
	);
	function fnSetCookie() {
		var	aVarVal = new Array(gaVarName.length);
		var	cookie = '';
		for (var i = 0; i < gaVarName.length; ++i) {
			aVarVal[i] = eval(gaVarName[i]);
		}
		document.cookie = 'User.Var=' + aVarVal;
	}
	function fnGetCookie() {
		var	aCookie = document.cookie.split("; ");
//		alert("# of cookie(s) = " + aCookie.length);
		for (var i = 0; i < aCookie.length; ++i) {
			var	aNameVal = aCookie[i].split("=");
			if (aNameVal.length != 2) {
//				alert('Invalid cookie: "' + aCookie[i] + '"');
				continue;
			}
//			alert('NAME = "' + aNameVal[0] + '", VALUE = "' + aNameVal[1] + '"');
			if (aNameVal[0] == "User.Var") {
				var	aVarVal = aNameVal[1].split(",");
				for (var k = 0; k < gaVarName.length; ++k) {
					if (aVarVal[k] != "") {
						eval(gaVarName[k] + ' = ' + aVarVal[k]);
					}
				}
			}
			if (aNameVal[0] == "User") {
				var	aUser = unescape(aNameVal[1]).split(":");
				gUserId = parseInt(aUser[0]);
			}
		}
	}

	function fnSetData() {
		var	JavaScriptEvalCode = "<?PHP echo $JavaScriptEvalCode ?>";
//		alert(JavaScriptEvalCode);
		eval(JavaScriptEvalCode);

		// 각 오브젝트가 해당 목록의 몇 번째 행에 위치할 지를 설정한다.
		for (var i = 0; i < gaGroup.length; ++i)
			gaGroup[i].rowIndex = i;
		for (var i = 0; i < gaUser.length; ++i)
			gaUser[i].rowIndex = i;
	}

	function fnSetVar() {
		// gGroupId: number
		gGroupId = parseInt(gGroupId);
		if (isNaN(gGroupId)) {		// 정의되지 않았거나 숫자가 아닌 경우
			// (정책) 기본값은 0으로 GROUP_ID가 설정되지 않은 것으로 한다.
			gGroupId = 0;
		}
		// gGroupId의 유효성 검사는 fnSetGroup함수에서 이루어진다.
		// gUserId: number
		gUserId = parseInt(gUserId);
		if (isNaN(gUserId)) {		// 정의되지 않았거나 숫자가 아닌 경우
			// (정책) 기본값은 0으로 USER_ID가 설정되지 않은 것으로 한다.
			gUserId = 0;
		}
		// gUserId의 유효성 검사는 fnSetUser함수에서 이루어진다.
	}

	window.onload = fnOnLoad;
	window.onunload = fnOnUnload;
	function fnOnLoad() {
		fnGetCookie();

		fnSetData();

		// 설정된 전역변수가 없다면 전역변수를 정책에 따른 기본값으로 설정하고,
		// 있다면 설정된 데이터를 바탕으로 다시 확인한다.
		fnSetVar();

		// gaGroup에 속하는 GROUP들은 GROUP목록에 표시한다.
		fnListGroup();
		// gGroupId에 해당하는 gGroup를 설정하고,
		// gGroup에 대한 정보를 표시한다.
		fnSetGroup();
		// gGroup에 대한 정보를 표시한다.
		fnDisplayGroup();

		// gaUser에 속하는 USER들은 USER목록에 표시한다.
		fnListUser();
		// gUserId에 해당하는 gUser를 설정하고,
		// gUser에 대한 정보를 표시한다.
		fnSetUser();
		// gUser에 대한 정보를 표시한다.
		fnDisplayUser();
	}
	function fnOnUnload() {
		fnSetCookie();
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/12/10 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 오브젝트를 정의한다.
 */
	// 오브젝트로 이루어진 배열에서,
	// 오브젝트를 특징짓는 ID를 통해서
	// 배열에서의 위치를 쉽게 찾도록 한다.
	// 일종의 HASH이다.
	function objHashIndex(id, index) {
		this.id = id;
		this.index = index;
	}

	function objGroup(id, name, aUserIdByVal) {
		this.id = id;	// > 0: DB에서 가져왔다, 0: 새로 만들었다
		this.name = name;
		this.aUserId = [];
		for (var i = 0; i < aUserIdByVal.length; ++i) {
			// userId가 0보다 크면 DB의 USER_ID이다.
			// userId가 0보다 작으면 USER가 새로 추가되었고,
			// 그 USER의 정보는 USER목록의 (-userId - 1)번째에 표시되어있다는 것을 뜻한다.
			this.aUserId.push(aUserIdByVal[i]);
		}

		if (id) {	// DB에서 가져왔다.
			// 비교하기 위하여 원본을 저장한다.
			this.bDeleted = false;	// DB에서 가져온 정보는 웹에서 지울 수가 없다.
						// 지웠다는 표시만 하고 나중에 서버로 이 정보를 보내서 서버에서 지우게 한다.
			this._name = name;
			this._aUserId = [];
			for (var i = 0; i < aUserIdByVal.length; ++i)
				this._aUserId.push(aUserIdByVal[i]);
		}

		// GROUP목록에서 몇 번째 행에 표시되는 지를 말해준다.
		// 초기화과정에서 설정한다.
		this.rowIndex = -1;	// 기본값은 제일 마지막이다.
	}
	var	gaGroup;
	var	gGroupId;
	var	gGroup;

	function objUser(id, nickname, fullname, position, status, aGroupIdByVal) {
		this.id = id;	// > 0: DB에서 가져왔다, 0: 새로 만들었다
		this.nickname = nickname;
		this.fullname = fullname;
		this.position = position;
		this.status = status;
		this.aGroupId = [];
		for (var i = 0; i < aGroupIdByVal.length; ++i) {
			// groupId가 0보다 크면 DB의 GROUP_ID이다.
			// groupId가 0보다 작으면 GROUP이 새로 추가되었고,
			// 그 GROUP의 정보는 GROUP목록의 (-groupId - 1)번째에 표시되어있다는 것을 뜻한다.
			this.aGroupId.push(aGroupIdByVal[i]);
		}

		// DB에서 정보를 가져올 때, password는 가져오지 않는다.
		// 왜냐하면 DB에 저정된 password는 sha1으로 해시처리된 정보이기때문에,
		// 가져와서 확인하거나 보여줄 수가 없기 때문이다.
		// 대신 password의 기본값은 ''으로 해서,
		// 이 값이 ''이 아니면 DB에 있는 password값을 변경하는 것으로 한다.
		// id = 0인 경우에는 이 값을 무조건 DB에 저장한다.
		this.password = '';

		if (id) {	// DB에서 가져왔다.
			// 비교하기 위하여 원본을 저장한다.
			this.bDeleted = false;	// DB에서 가져온 정보는 웹에서 지울 수가 없다.
						// 지웠다는 표시만 하고 나중에 서버로 이 정보를 보내서 서버에서 지우게 한다.

			this._nickname = nickname;
			this._fullname = fullname;
			this._position = position;
			this._status = status;
			this._aGroupId = [];
			for (var i = 0; i < aGroupIdByVal.length; ++i)
				this._aGroupId.push(aGroupIdByVal[i]);
		}

		// USER목록에서 몇 번째 행에 표시되는 지를 말해준다.
		// 초기화과정에서 설정한다.
		this.rowIndex = -1;	// 기본값은 제일 마지막이다.
	}
	var	gaUser;
	var	gUserId;
	var	gUser;
</SCRIPT>
</HEAD>
<BODY>
<P><INPUT TYPE="BUTTON" VALUE="반영하기" onClick="fnSubmit();"></P>

<P>그룹 관리</P>
<DIV ALIGN="CENTER">
<TABLE WIDTH="85%" BORDER="1" CELLSPACING="0" CELLPADDING="0"><TBODY>
<TR><TD WIDTH="100%"><TABLE ID="tblGroupList" WIDTH="100%" BORDER="1" FRAME="VOID" CELLSPACING="0" CELLPADDING="0" STYLE="border: solid 2px red;"><TBODY>
	<TR>
	<TH>선택</TH>
	<TH WIDTH="20%">그룹</TH>
	<TH WIDTH="80%">유저</TH>
	</TR>
</TBODY></TABLE></TD></TR>
<TR><TD WIDTH="100%"><TABLE WIDTH="100%" BORDER="1" CELLSPACING="0" CELLPADDING="0" STYLE="border: solid 2px blue;"><TBODY>
	<TR><TD WIDTH="100%"><TABLE WIDTH="100%" BORDER="1" FRAME="VOID" CELLSPACING="0" CELLPADDING="0"><TBODY>
		<TR>
			<TD WIDTH="20%" ALIGN="RIGHT">그룹</TD>
			<TD WIDTH="80%"><INPUT ID="txtGroupName" TYPE="TEXT" NAME="GROUPNAME" VALUE=""></TD>
		</TR>
		<TR>
			<TD WIDTH="20%" ALIGN="RIGHT">유저</TD>
			<TD WIDTH="80%"><SELECT ID="sltUser" MULTIPLE SIZE="4" STYLE="width: 200px;"></SELECT></TD>
		</TR>
	</TBODY></TABLE></TD></TR>
	<TR><TD WIDTH="100%"><TABLE WIDTH="100%" BORDER="0" CELLSPACING="0" CELLPADDING="0"><TBODY>
		<TR><TD ALIGN="CENTER"><INPUT TYPE="BUTTON" VALUE="더하기" onClick="fnInsertGroup();">&nbsp;&nbsp;&nbsp;&nbsp;<INPUT ID="btnUpdateGroup" TYPE="BUTTON" VALUE="고치기" onClick="fnUpdateGroup();">&nbsp;&nbsp;&nbsp;&nbsp;<INPUT ID="btnDeleteGroup" TYPE="BUTTON" VALUE="지우기" onClick="fnDeleteGroup();"></TD></TR>
	</TBODY></TABLE></TD></TR>
</TBODY></TABLE></TD></TR>
</TBODY></TABLE>
</DIV>

<P>유저 관리</P>
<DIV ALIGN="CENTER">
<TABLE WIDTH="85%" BORDER="1" CELLSPACING="0" CELLPADDING="0"><TBODY>
<TR><TD WIDTH="100%"><TABLE ID="tblUserList" WIDTH="100%" BORDER="1" FRAME="VOID" CELLSPACING="0" CELLPADDING="0" STYLE="border: solid 2px red;"><TBODY>
	<TR>
	<TH>선택</TH>
	<TH WIDTH="15%">아이디</TH>
	<TH WIDTH="25%">이름</TH>
	<TH WIDTH="10%">직위</TH>
	<TH WIDTH="10%">상태</TH>
	<TH WIDTH="40%">그룹</TH>
	</TR>
</TBODY></TABLE></TD></TR>
<TR><TD WIDTH="100%"><TABLE WIDTH="100%" BORDER="1" CELLSPACING="0" CELLPADDING="0" STYLE="border: solid 2px blue;"><TBODY>
	<TR><TD WIDTH="100%"><TABLE WIDTH="100%" FRAME="VOID" BORDER="1" CELLSPACING="0" CELLPADDING="0"><TBODY>
		<TR>
			<TD WIDTH="20%" ALIGN="RIGHT">아이디</TD>
			<TD WIDTH="80%"><INPUT ID="txtNickname" TYPE="TEXT" NAME="NICKNAME" VALUE=""></TD>
		</TR>
		<TR>
			<TD WIDTH="20%" ALIGN="RIGHT">암호</TD>
			<TD WIDTH="80%"><INPUT ID="txtPassword" TYPE="PASSWORD" NAME="PASSWORD" VALUE=""></TD>
		</TR>
		<TR>
			<TD WIDTH="20%" ALIGN="RIGHT">이름</TD>
			<TD WIDTH="80%"><INPUT ID="txtFullname" TYPE="TEXT" NAME="FULLNAME" VALUE=""></TD>
		</TR>
		<TR>
			<TD WIDTH="20%" ALIGN="RIGHT">직위</TD>
			<TD ID="idPosition" WIDTH="80%"><INPUT TYPE="RADIO" NAME="POSITION" VALUE="<?PHP echo POSITION_LEADER ?>">팀장&nbsp;&nbsp;<INPUT TYPE="RADIO" NAME="POSITION" VALUE="<?PHP echo POSITION_WORKER ?>">팀원</TD>
		</TR>
		<TR>
			<TD WIDTH="20%" ALIGN="RIGHT">계정상태</TD>
			<TD ID="idStatus" WIDTH="80%"><INPUT TYPE="RADIO" NAME="STATUS" VALUE="<?PHP echo IDSTAT_BLOCK ?>">일시정지&nbsp;&nbsp;<INPUT TYPE="RADIO" NAME="STATUS" VALUE="<?PHP echo IDSTAT_INUSE ?>">사용중&nbsp;&nbsp;</TD>
		</TR>
		<TR>
			<TD WIDTH="20%" ALIGN="RIGHT">그룹</TD>
			<TD WIDTH="80%"><SELECT ID="sltGroup" MULTIPLE SIZE="4" STYLE="width: 200px;"></SELECT></TD>
		</TR>
	</TBODY></TABLE></TD></TR>
	<TR><TD WIDTH="100%"><TABLE WIDTH="100%" BORDER="0" CELLSPACING="0" CELLPADDING="0"><TBODY>
		<TR><TD ALIGN="CENTER"><INPUT TYPE="BUTTON" VALUE="더하기" onClick="fnInsertUser();">&nbsp;&nbsp;&nbsp;&nbsp;<INPUT ID="btnUpdateUser" TYPE="BUTTON" VALUE="고치기" onClick="fnUpdateUser();">&nbsp;&nbsp;&nbsp;&nbsp;<INPUT ID="btnDeleteUser" TYPE="BUTTON" VALUE="지우기" onClick="fnDeleteUser();"></TD></TR>
	</TBODY></TABLE></TD></TR>
</TBODY></TABLE></TD></TR>
</TBODY></TABLE>
</DIV>
</BODY>
</HTML>
<?PHP
	}
	// Closing connection
	mysql_close($link);
?>
