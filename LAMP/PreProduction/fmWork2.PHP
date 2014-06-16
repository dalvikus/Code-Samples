<?PHP
/*
 * 2005/12/28 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 상수들을 설정한다.
 */

	// ->workType
	define("WORK_DESIGN", 1);	// 디자인 작업
	define("WORK_STORYBOARD", 2);	// 스토리보드 작업
	// -> designType
	define("DESIGN_CONCEPT", 1);	// 컨셉 디자인
	define("DESIGN_DETAIL", 2);	// 구체적인 디자인
	// ->targetType
	define("TARGET_CHR", 1);	// 인물
	define("TARGET_BG", 2);		// 배경
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
<TITLE>fmWork2.PHP</TITLE>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; CHARSET=euc-kr">
</HEAD>
<BODY>
<PRE>
<?PHP
	$aWorkIdDate = split(',', $_POST['PHP_EVAL_CODE']);
	foreach ($aWorkIdDate as $workIdDate) {
		$a = split(':', $workIdDate);
		$b = split('-', $a[1]);
		$b[0] = join('-', split('/', $b[0]));
		$b[1] = join('-', split('/', $b[1]));
		echo 'WORK_ID(' . $a[0] . '): ' . $b[0] . '~' . $b[1] . "\n";
		$query = sprintf(
			"UPDATE TWORK SET DATE_BEGIN = '%s', DATE_END = '%s' WHERE WORK_ID = %d",
			$b[0], $b[1],
			$a[0]
		);
		echo 'query: |' . $query . '|' . "\n";
		if (!mysql_query($query)) {
			$message = '';
			$message .= 'errno: |' . mysql_errno() . '|' . "\n";
			$message .= 'error: |' . mysql_error() . '|' . "\n";
			$message .= 'query: |' . $query . '|' . "\n";
			die($message);
		}
	}
?>
</PRE>
<SCRIPT LANGUAGE="JavaScript1.3">
	location.replace("fmWork2.PHP");
</SCRIPT>
</BODY>
</HTML>
<?PHP
	} else {
if (1) {
/*
// 1. 디자인 작업
// 작업 이미지가 없는 경우
SELECT T2.SHOW_ID, T2.NAME AS SHOW_NAME, T2.DATE_BEGIN AS SHOW_BEGIN_DATE, T2.DATE_END AS SHOW_END_DATE, T3.SCENE_ID, T3.NAME AS SCENE_NAME, T4.TYPE AS TARGET_TYPE, T4.NAME AS TARGET_NAME, T1.TYPE AS DESIGN_TYPE, DESIGN_ID, T5.DATE_BEGIN AS WORK_BEGIN_DATE, T5.DATE_END AS WORK_END_DATE FROM TDESIGN AS T1, TSHOW AS T2, TSCENE AS T3, TTARGET AS T4, TWORK AS T5 WHERE T1.DESIGN_ID = T5.WORK_ID AND T1.SHOW_ID = T2.SHOW_ID AND T1.SCENE_ID = T3.SCENE_ID AND T1.TARGET_ID = T4.TARGET_ID AND IMAGE_ID = 0 ORDER BY SHOW_ID;

// 작업 이미지가 있는 경우
SELECT T3.SHOW_ID, T3.NAME AS SHOW_NAME, T3.DATE_BEGIN AS SHOW_BEGIN_DATE, T3.DATE_END AS SHOW_END_DATE, T4.SCENE_ID, T4.NAME AS SCENE_NAME, T5.TYPE AS TARGET_TYPE, T5.NAME AS TARGET_NAME, T1.TYPE AS DESIGN_TYPE, DESIGN_ID, T7.DATE_BEGIN AS WORK_BEGIN_DATE, T7.DATE_END AS WORK_END_DATE, GROUP_CONCAT(T2.USER_ID ORDER BY T2.USER_ID) AS USER_IDs, GROUP_CONCAT(T6.NICKNAME ORDER BY T2.USER_ID) AS NICKNAMEs, GROUP_CONCAT(REVISION ORDER BY T2.USER_ID, REVISION DESC) AS REVISIONs, GROUP_CONCAT(FILENAME_UUID ORDER BY T2.USER_ID, REVISION DESC) AS FILENAME_UUIDs FROM TDESIGN AS T1, TWORK_IMAGE AS T2, TSHOW AS T3, TSCENE AS T4, TTARGET AS T5, TUSER AS T6, TWORK AS T7 WHERE T1.DESIGN_ID = T7.WORK_ID AND T1.SHOW_ID = T3.SHOW_ID AND T1.SCENE_ID = T4.SCENE_ID AND T1.TARGET_ID = T5.TARGET_ID AND T2.USER_ID = T6.USER_ID AND T1.IMAGE_ID = T2.IMAGE_ID GROUP BY DESIGN_ID ORDER BY SHOW_ID;

// 2. 스토리보드 작업
// 작업 이미지가 없는 경우
SELECT T4.SHOW_ID, T4.NAME AS SHOW_NAME, T4.DATE_BEGIN AS SHOW_BEGIN_DATE, T4.DATE_END AS SHOW_END_DATE, T5.SCENE_ID, T5.NAME AS SCENE_NAME, T1.STORYBOARD_ID, T6.DATE_BEGIN AS WORK_BEGIN_DATE, T6.DATE_END AS WORK_END_DATE, T2.SERIAL FROM TSTORYBOARD AS T1, TCUT AS T2, TSHOW AS T4, TSCENE AS T5, TWORK AS T6 WHERE T1.STORYBOARD_ID = T6.WORK_ID AND T1.SCENE_ID = T5.SCENE_ID AND T5.SHOW_ID = T4.SHOW_ID AND T1.STORYBOARD_ID = T2.STORYBOARD_ID AND IMAGE_ID = 0 ORDER BY SHOW_ID;

// 작업 이미지가 있는 경우
SELECT T4.SHOW_ID, T4.NAME AS SHOW_NAME, T4.DATE_BEGIN AS SHOW_BEGIN_DATE, T4.DATE_END AS SHOW_END_DATE, T5.SCENE_ID, T5.NAME AS SCENE_NAME, T1.STORYBOARD_ID, T6.DATE_BEGIN AS WORK_BEGIN_DATE, T6.DATE_END AS WORK_END_DATE, T2.SERIAL, GROUP_CONCAT(FILENAME_UUID ORDER BY REVISION DESC) AS FILENAME_UUIDs FROM TSTORYBOARD AS T1, TCUT AS T2, TWORK_IMAGE AS T3, TSHOW AS T4, TSCENE AS T5, TWORK AS T6 WHERE T1.STORYBOARD_ID = T6.WORK_ID AND T1.SCENE_ID = T5.SCENE_ID AND T5.SHOW_ID = T4.SHOW_ID AND T1.STORYBOARD_ID = T2.STORYBOARD_ID AND T2.IMAGE_ID = T3.IMAGE_ID GROUP BY T2.IMAGE_ID ORDER BY SHOW_ID;
 */
		global	$JavaScriptEvalCode;
		$JavaScriptEvalCode = '';	// JavaScript가 해석할 문자열

		// 1. 디자인 작업에 대한 정보를 모은다.
		// 1.1. 작업에 대한 결과 이미지가 없는 경우
		//	단지, 어떤 작업이 있는 지만을 알 수 있다.
		//	해당 작업을 진행하는 작업자에 대한 정보를 없다.
		$query = 'SELECT T2.SHOW_ID, T2.NAME AS SHOW_NAME, T2.DATE_BEGIN AS SHOW_BEGIN_DATE, T2.DATE_END AS SHOW_END_DATE, T3.SCENE_ID, T3.NAME AS SCENE_NAME, T4.TYPE AS TARGET_TYPE, T4.NAME AS TARGET_NAME, T1.TYPE AS DESIGN_TYPE, DESIGN_ID, T5.DATE_BEGIN AS WORK_BEGIN_DATE, T5.DATE_END AS WORK_END_DATE FROM TDESIGN AS T1, TSHOW AS T2, TSCENE AS T3, TTARGET AS T4, TWORK AS T5 WHERE T1.DESIGN_ID = T5.WORK_ID AND T1.SHOW_ID = T2.SHOW_ID AND T1.SCENE_ID = T3.SCENE_ID AND T1.TARGET_ID = T4.TARGET_ID AND IMAGE_ID = 0 ORDER BY SHOW_ID';
		if (!($result = mysql_query($query))) {
			$message = '';
			$message .= 'errno: |' . mysql_errno() . '|' . "\n";
			$message .= 'error: |' . mysql_error() . '|' . "\n";
			$message .= 'query: |' . $query . '|' . "\n";
			die($message);
		}
		$aaWork2 = array();	// JavaScript의 objaWork2에 해당한다.
		while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
			$showId = $row['SHOW_ID'];
			if (!isset($aaWork2[$showId])) {
				$aaWork2[$showId] = array(	// JavaScript의 objaWork2에 해당한다.
					'showId' => $showId,
					'showName' => $row['SHOW_NAME'],
					'showBeginDate' => $row['SHOW_BEGIN_DATE'],
					'showEndDate' => $row['SHOW_END_DATE'],
					'aWork2' => array()	// 각 성분이 JavaScript의 objWork2에 해당한다.
				);
			}
			// $aaWork2[$showId]['aWork2']배열에 각 작업에 대한 정보를 저장한다.
			// 먼저 작업의 이름은 결정한다.
			$targetType = $row['TARGET_TYPE'];
			$targetName = ($targetType == TARGET_CHR ? '인물' : '배경') . '("' . $row['TARGET_NAME'] . '")';
			$workName = '';
			if ($row['SCENE_ID']) {
				$workName = '특정 장면("' . $row['SCENE_NAME'] . '")에' . ($targetType == TARGET_CHR ? ' 등장하는 ' : '서 사용되는 ') . $targetName . '에 대한 ';
			} else {
				$workName = '작품 전체에' . ($targetType == TARGET_CHR ? ' 등장하는 ' : '서 사용되는 ') . $targetName . '에 대한 ';
			}
			$workName .= ($row['DESIGN_TYPE'] == DESIGN_CONCEPT ? '컨셉' : '구체적인') . ' 디자인';
			array_push($aaWork2[$showId]['aWork2'],
				// JavaScript의 objaWork2에 해당한다.
				array(
					'workId' => $row['DESIGN_ID'],
					'workType' => WORK_DESIGN,
					'workName' => $workName,
					'workBeginDate' => $row['WORK_BEGIN_DATE'],
					'workEndDate' => $row['WORK_END_DATE'],
					'aWorkImage' => array()		// JavaScript의 objWorkImage에 해당한다.
				)
			);
		}
		// 사용한 결과를 반환한다.
		mysql_free_result($result);
//		print_r($aaWork2);

		// 1.2. 작업에 대한 결과 이미지가 있는 경우
		//	작업 결과가 있는 작업자만 표시된다.
		$query = 'SELECT T3.SHOW_ID, T3.NAME AS SHOW_NAME, T3.DATE_BEGIN AS SHOW_BEGIN_DATE, T3.DATE_END AS SHOW_END_DATE, T4.SCENE_ID, T4.NAME AS SCENE_NAME, T5.TYPE AS TARGET_TYPE, T5.NAME AS TARGET_NAME, T1.TYPE AS DESIGN_TYPE, DESIGN_ID, T7.DATE_BEGIN AS WORK_BEGIN_DATE, T7.DATE_END AS WORK_END_DATE, GROUP_CONCAT(T2.USER_ID ORDER BY T2.USER_ID) AS USER_IDs, GROUP_CONCAT(T6.NICKNAME ORDER BY T2.USER_ID) AS NICKNAMEs, GROUP_CONCAT(REVISION ORDER BY T2.USER_ID, REVISION DESC) AS REVISIONs, GROUP_CONCAT(FILENAME_UUID ORDER BY T2.USER_ID, REVISION DESC) AS FILENAME_UUIDs FROM TDESIGN AS T1, TWORK_IMAGE AS T2, TSHOW AS T3, TSCENE AS T4, TTARGET AS T5, TUSER AS T6, TWORK AS T7 WHERE T1.DESIGN_ID = T7.WORK_ID AND T1.SHOW_ID = T3.SHOW_ID AND T1.SCENE_ID = T4.SCENE_ID AND T1.TARGET_ID = T5.TARGET_ID AND T2.USER_ID = T6.USER_ID AND T1.IMAGE_ID = T2.IMAGE_ID GROUP BY DESIGN_ID ORDER BY SHOW_ID';
		if (!($result = mysql_query($query))) {
			$message = '';
			$message .= 'errno: |' . mysql_errno() . '|' . "\n";
			$message .= 'error: |' . mysql_error() . '|' . "\n";
			$message .= 'query: |' . $query . '|' . "\n";
			die($message);
		}
		while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
			$showId = $row['SHOW_ID'];
			if (!isset($aaWork2[$showId])) {
				$aaWork2[$showId] = array(	// JavaScript의 objaWork2에 해당한다.
					'showId' => $showId,
					'showName' => $row['SHOW_NAME'],
					'showBeginDate' => $row['SHOW_BEGIN_DATE'],
					'showEndDate' => $row['SHOW_END_DATE'],
					'aWork2' => array()	// 각 성분이 JavaScript의 objWork2에 해당한다.
				);
			}
			// $aaWork2[$showId]['aWork2']배열에 각 작업에 대한 정보를 저장한다.
			// 먼저 작업의 이름은 결정한다.
			$targetType = $row['TARGET_TYPE'];
			$targetName = ($targetType == TARGET_CHR ? '인물' : '배경') . '("' . $row['TARGET_NAME'] . '")';
			$workName = '';
			if ($row['SCENE_ID']) {
				$workName = '특정 장면("' . $row['SCENE_NAME'] . '")에' . ($targetType == TARGET_CHR ? ' 등장하는 ' : '서 사용되는 ') . $targetName . '에 대한 ';
			} else {
				$workName = '작품 전체에' . ($targetType == TARGET_CHR ? ' 등장하는 ' : '서 사용되는 ') . $targetName . '에 대한 ';
			}
			$workName .= ($row['DESIGN_TYPE'] == DESIGN_CONCEPT ? '컨셉' : '구체적인') . ' 디자인';
			array_push($aaWork2[$showId]['aWork2'],
				// JavaScript의 objaWork2에 해당한다.
				array(
					'workId' => $row['DESIGN_ID'],
					'workType' => WORK_DESIGN,
					'workName' => $workName,
					'workBeginDate' => $row['WORK_BEGIN_DATE'],
					'workEndDate' => $row['WORK_END_DATE'],
					'aWorkImage' => array()		// JavaScript의 objWorkImage에 해당한다.
				)
			);
			// 여기까지는 1.1.과 같다.

			// 방금 넣은 작업의 'aWorkImage'배열에 작업 이미지에 대한 결과를 담는다.
			$refWork = &$aaWork2[$showId]['aWork2'][count($aaWork2[$showId]['aWork2']) - 1];
/*
USER_IDs: 1,2,2
NICKNAMEs: calani98,coo03,coo03
REVISIONs: 1,2,1
FILENAME_UUIDs: 726e905604b6614f3eb7a854aa39cbce5e076972,7c84a5d2634ed74794979b4386af46ed53dc9659,b6be7bede17ae11ade23993a3d5f7ba270b2ecb0
 */
			$aUserId = split(',', $row['USER_IDs']);
			$aNickname = split(',', $row['NICKNAMEs']);
			$aRevision = split(',', $row['REVISIONs']);
			$aFilenameUuid = split(',', $row['FILENAME_UUIDs']);
			$lastUserId = 0;
			foreach ($aUserId as $i => $userId) {
				if ($userId == $lastUserId) {
					// 같은 USER의 다른 REVISION에 해당한다.
					// 가장 최근의 REVISION만을 보여주므로 이 경우는 건너뛴다.
					continue;
				}
				// 다른 USER의 최근 REVISION이다.
				array_push($refWork['aWorkImage'],
					// JavaScript의 objWorkImage에 해당한다.
					array(
						'nickname' => $aNickname[$i],
						'aFilenameUuid' => array('"' . $aFilenameUuid[$i] . '"')
					)
				);
				$lastUserId = $userId;
			}
		}
		// 사용한 결과를 반환한다.
		mysql_free_result($result);
//		print_r($aaWork2);

		// 2. 스토리보드 작업에 대한 정보를 모은다.
		// 2.1. 작업에 대한 결과 이미지가 없는 경우
		$query = 'SELECT T4.SHOW_ID, T4.NAME AS SHOW_NAME, T4.DATE_BEGIN AS SHOW_BEGIN_DATE, T4.DATE_END AS SHOW_END_DATE, T5.SCENE_ID, T5.NAME AS SCENE_NAME, T1.STORYBOARD_ID, T6.DATE_BEGIN AS WORK_BEGIN_DATE, T6.DATE_END AS WORK_END_DATE, T2.SERIAL FROM TSTORYBOARD AS T1, TCUT AS T2, TSHOW AS T4, TSCENE AS T5, TWORK AS T6 WHERE T1.STORYBOARD_ID = T6.WORK_ID AND T1.SCENE_ID = T5.SCENE_ID AND T5.SHOW_ID = T4.SHOW_ID AND T1.STORYBOARD_ID = T2.STORYBOARD_ID AND IMAGE_ID = 0 ORDER BY SHOW_ID';
		if (!($result = mysql_query($query))) {
			$message = '';
			$message .= 'errno: |' . mysql_errno() . '|' . "\n";
			$message .= 'error: |' . mysql_error() . '|' . "\n";
			$message .= 'query: |' . $query . '|' . "\n";
			die($message);
		}
		while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
			$showId = $row['SHOW_ID'];
			if (!isset($aaWork2[$showId])) {
				$aaWork2[$showId] = array(	// JavaScript의 objaWork2에 해당한다.
					'showId' => $showId,
					'showName' => $row['SHOW_NAME'],
					'showBeginDate' => $row['SHOW_BEGIN_DATE'],
					'showEndDate' => $row['SHOW_END_DATE'],
					'aWork2' => array()	// 각 성분이 JavaScript의 objWork2에 해당한다.
				);
			}
			// $aaWork2[$showId]['aWork2']배열에 각 작업에 대한 정보를 저장한다.
			$storyboardId = $row['STORYBOARD_ID'];
			// $storyboardId에 해당하는 스토리보드 작업이 있는 지 알아본다.
			$index = -1;
			foreach ($aaWork2[$showId]['aWork2'] as $i => $oWork2) {
				if ($oWork2['workId'] == $storyboardId) {
					$index = $i;
//					break;
				}
			}
			if ($index == -1) {
				// 먼저 작업의 이름은 결정한다.
				$workName = '장면("' . $row['SCENE_NAME'] . '")에 대한 스토리보드';
				array_push($aaWork2[$showId]['aWork2'],
					// JavaScript의 objaWork2에 해당한다.
					array(
						'workId' => $row['STORYBOARD_ID'],
						'workType' => WORK_STORYBOARD,
						'workName' => $workName,
						'workBeginDate' => $row['WORK_BEGIN_DATE'],
						'workEndDate' => $row['WORK_END_DATE'],
						'aWorkImage' => array()		// JavaScript의 objWorkImage에 해당한다.
					)
				);
				$index = count($aaWork2[$showId]['aWork2']) - 1;
			}

			$refWork = &$aaWork2[$showId]['aWork2'][$index];
			if (!isset($refWork['aWorkImage'][0])) {
				$refWork['aWorkImage'][0] = array(
					'nickname' => 'hello, world: 스토리보드',
					'aFilenameUuid' => array()
				);
			}
			$refaImage = &$refWork['aWorkImage'][0]['aFilenameUuid'];
//			$refaImage[$row['SERIAL'] - 1] = 'undefined';
			array_splice($refaImage, $row['SERIAL'] - 1, 0, 'undefined');
		}
		// 사용한 결과를 반환한다.
		mysql_free_result($result);
//		print_r($aaWork2);

		// 2.2. 작업에 대한 결과 이미지가 있는 경우
		$query = 'SELECT T4.SHOW_ID, T4.NAME AS SHOW_NAME, T4.DATE_BEGIN AS SHOW_BEGIN_DATE, T4.DATE_END AS SHOW_END_DATE, T5.SCENE_ID, T5.NAME AS SCENE_NAME, T1.STORYBOARD_ID, T6.DATE_BEGIN AS WORK_BEGIN_DATE, T6.DATE_END AS WORK_END_DATE, T2.SERIAL, GROUP_CONCAT(FILENAME_UUID ORDER BY REVISION DESC) AS FILENAME_UUIDs FROM TSTORYBOARD AS T1, TCUT AS T2, TWORK_IMAGE AS T3, TSHOW AS T4, TSCENE AS T5, TWORK AS T6 WHERE T1.STORYBOARD_ID = T6.WORK_ID AND T1.SCENE_ID = T5.SCENE_ID AND T5.SHOW_ID = T4.SHOW_ID AND T1.STORYBOARD_ID = T2.STORYBOARD_ID AND T2.IMAGE_ID = T3.IMAGE_ID GROUP BY T2.IMAGE_ID ORDER BY SHOW_ID';
		if (!($result = mysql_query($query))) {
			$message = '';
			$message .= 'errno: |' . mysql_errno() . '|' . "\n";
			$message .= 'error: |' . mysql_error() . '|' . "\n";
			$message .= 'query: |' . $query . '|' . "\n";
			die($message);
		}
		while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
			$showId = $row['SHOW_ID'];
			if (!isset($aaWork2[$showId])) {
				$aaWork2[$showId] = array(	// JavaScript의 objaWork2에 해당한다.
					'showId' => $showId,
					'showName' => $row['SHOW_NAME'],
					'showBeginDate' => $row['SHOW_BEGIN_DATE'],
					'showEndDate' => $row['SHOW_END_DATE'],
					'aWork2' => array()	// 각 성분이 JavaScript의 objWork2에 해당한다.
				);
			}
			// $aaWork2[$showId]['aWork2']배열에 각 작업에 대한 정보를 저장한다.
			$storyboardId = $row['STORYBOARD_ID'];
			// $storyboardId에 해당하는 스토리보드 작업이 있는 지 알아본다.
			$index = -1;
			foreach ($aaWork2[$showId]['aWork2'] as $i => $oWork2) {
				if ($oWork2['workId'] == $storyboardId) {
					$index = $i;
//					break;
				}
			}
			if ($index == -1) {
				// 먼저 작업의 이름은 결정한다.
				$workName = '장면("' . $row['SCENE_NAME'] . '")에 대한 스토리보드';
				array_push($aaWork2[$showId]['aWork2'],
					// JavaScript의 objaWork2에 해당한다.
					array(
						'workId' => $row['STORYBOARD_ID'],
						'workType' => WORK_STORYBOARD,
						'workName' => $workName,
						'workBeginDate' => $row['WORK_BEGIN_DATE'],
						'workEndDate' => $row['WORK_END_DATE'],
						'aWorkImage' => array()		// JavaScript의 objWorkImage에 해당한다.
					)
				);
				$index = count($aaWork2[$showId]['aWork2']) - 1;
			}

			$refWork = &$aaWork2[$showId]['aWork2'][$index];
			if (!isset($refWork['aWorkImage'][0])) {
				$refWork['aWorkImage'][0] = array(
					'nickname' => 'hello, world: 스토리보드',
					'aFilenameUuid' => array()
				);
			}
			$refaImage = &$refWork['aWorkImage'][0]['aFilenameUuid'];
			// 여기까지는 2.1.과 같다.

			$aFilenameUuid = split(',', $row['FILENAME_UUIDs']);
			// 가장 최근의 리비젼만 보여준다.
//			$refaImage[$row['SERIAL'] - 1] = $aFilenameUuid[0];
			array_splice($refaImage, $row['SERIAL'] - 1, 0, '"' . $aFilenameUuid[0] . '"');
		}
		// 사용한 결과를 반환한다.
		mysql_free_result($result);
//		print_r($aaWork2);

		// 지금까지 모은 작업을 대한 정보를 JavaScript가 해석할 코드로 만든다.
		$aa = array();
		foreach ($aaWork2 as $showId => $oaWork2) {
			$a = array();
			foreach ($oaWork2['aWork2'] as $oWork2) {
				$aWorkImage = array();
				foreach ($oWork2['aWorkImage'] as $oWorkImage) {
					array_push($aWorkImage,
						"\n" .
						"\t\t\t\t\t" . 'new objWorkImage(' . "\n" .
						"\t\t\t\t\t\t" . '"' . $oWorkImage['nickname'] . '"' . ',' . "\n" .
						"\t\t\t\t\t\t" . '[' . join(', ', $oWorkImage['aFilenameUuid']) . ']' . "\n" .
						"\t\t\t\t\t" . ')'
					);
				}
				array_push($a,
					"\n" .
					"\t\t\t" . 'new objWork2(' . "\n" .
					"\t\t\t\t" . $oWork2['workId'] . ',' . "\n" .
					"\t\t\t\t" . $oWork2['workType'] . ',' . "\n" .
					"\t\t\t\t" . '"' . preg_replace('/([\\\"])/', "\\\\$1", $oWork2['workName']) . '",' . "\n" .
					"\t\t\t\t" . 'new Date("' . join('/', split('-', $oWork2['workBeginDate'])) . '"),' . "\n" .
					"\t\t\t\t" . 'new Date("' . join('/', split('-', $oWork2['workEndDate'])) . '"),' . "\n" .
					"\t\t\t\t" . '[' . join(',', $aWorkImage) . "\n" .
					"\t\t\t\t" . ']' . "\n" .
					"\t\t\t" . ')'
				);
			}
			array_push($aa,
				"\n" .
				"\t" . 'new objaWork2(' . "\n" .
				"\t\t" . $oaWork2['showId'] . ',' . "\n" .
				"\t\t" . '"' . $oaWork2['showName'] . '"' . ',' . "\n" .
				"\t\t" . 'new Date("' . join('/', split('-', $oaWork2['showBeginDate'])) . '"),' . "\n" .
				"\t\t" . 'new Date("' . join('/', split('-', $oaWork2['showEndDate'])) . '"),' . "\n" .
				"\t\t" . '[' . join(',', $a) . "\n" .
				"\t\t" . ']' . "\n" .
				"\t" . ')'
			);
		}
		$JavaScriptEvalCode =
		'gaWork2 = [' . join(',', $aa) . "\n" .
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
<TITLE>fmWork2.PHP</TITLE>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; CHARSET=euc-kr">
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/12/18 (KST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 *
 */
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/12/15 (KST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 *
 */
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2006/1/2 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 작업 날짜가 바뀌었다면 이를 반영한다.
 */

	function fnSubmit() {
		var	a = [];
		for (var i = 0; i < gaWork2.length; ++i) {
			for (k = 0; k < gaWork2[i].aWork2.length; ++k) {
				var	oWork2 = gaWork2[i].aWork2[k];
				if (fnGetDateString(oWork2._workBeginDate) == fnGetDateString(oWork2.workBeginDate) && fnGetDateString(oWork2._workEndDate) == fnGetDateString(oWork2.workEndDate)) {
					continue;
				}
//				alert('WORK_ID(' + oWork2.workId + '): "' + oWork2.workName + '"');
				a.push(oWork2.workId + ':' + fnGetDateString(oWork2.workBeginDate) + '-' + fnGetDateString(oWork2.workEndDate));
			}
		}
		if (a.length == 0) {
			alert('바뀐 것이 하나도 없다');
			return;
		}

		// 데이터를 서버로 보내는 데 필요한 FORM을 만든다.
		var	oFORM = document.createElement('<FORM METHOD="POST" ACTION="fmWork2.PHP" ENCTYPE="APPLICATION/X-WWW-FORM-URLENCODED">');
		document.appendChild(oFORM);

		// .PHP에서 받아서 eval할 PHP코드를 포함하는 데이터를 만드는 데 필요한 TEXTAREA를 만든다.
		// 일반적으로 PHP코드는 여러 줄에 걸쳐있으므로 TEXTAREAD를 사용한다.
		var	oINPUT = document.createElement('<TEXTAREA NAME="PHP_EVAL_CODE">');
		oINPUT.value = a.join(',');
		oFORM.appendChild(oINPUT);

		oFORM.submit();
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/12/30 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 */
	// 작품목록에서 특정 작품을 선택했을 때 발생하는 이벤트를 처리하는 핸들러이다.
	function fnShowSelectOnChange() {
		gShowId = sltShow.options[sltShow.selectedIndex].value;
		if (!fnInit(gShowId)) {
			alert('fnInit(): Failure');
		}
	}

	function fnMakeWorkScheduleTable() {
		for (i = tblWork2.rows.length - 1; i > 0; --i) {
			tblWork2.deleteRow(i);
		}

//alert(tblWork2.clientWidth + ', ' + (oWork.clientWidth + oWorkPeriod.clientWidth + oWorkImage.clientWidth) + ', ' + oWork.clientWidth + ', ' + oWorkPeriod.clientWidth + ', ' + oWorkImage.clientWidth);
		gDayWidth = Math.floor(oWorkPeriod.clientWidth / gShowDays);

		oTHDiv.style.position = 'relative';
		oTHDiv.style.top = '0px';
		oTHDiv.style.left = '0px';
		oTHDiv.style.height = '20px';
		oTHDiv.style.width = (gShowDays * gDayWidth) + 'px';
		oTHDiv.style.backgroundColor = 'beige';

		for (i = 0; i < gWork2.aWork2.length; ++i) {
			fnInsertWork2(gWork2.aWork2[i]);
		}

		return true;
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/12/27 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 웹 초기화과정을 수행한다.
 */

	var	gaVarName = [
//		'gShowId'
	];
	function fnSetCookie() {
		var	aVarVal = new Array(gaVarName.length);
		var	cookie = '';
		for (var i = 0; i < gaVarName.length; ++i) {
			aVarVal[i] = eval(gaVarName[i]);
		}
		document.cookie = 'Work2.Var=' + aVarVal;
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
			if (aNameVal[0] == "Work2.Var") {
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

		// .rowIndex를 설정한다.
		for (var i = 0; i < gaWork2.length; ++i) {
			for (var k = 0; k < gaWork2[i].aWork2.length; ++k) {
				gaWork2[i].aWork2[k].rowIndex = k;
			}
		}
	}

	function fnSetVar() {
		// gShowId: number
		gShowId = parseInt(gShowId);
		if (isNaN(gShowId)) {
			// (정책) 기본값은 첫 번째 작품이다.
			gShowId = gaWork2[0].showId;
		} else {
			// 주어진 SHOW_ID가 있는 지 확인한다.
			for (var i = 0; i < gaWork2.length; ++i) {
				if (gaWork2[i].showId == gShowId) {	// 있다.
					break;
				}
			}
			if (i == gaWork2.length) {	// 없다.
				// (정책) 기본값은 첫 번째 작품이다.
				gShowId = gaWork2[0].showId;
			}
		}
	}

	window.onload = fnOnLoad;
	window.onunload = fnOnUnload;
	function fnOnLoad() {
		// 쿠키로부터 전역변수를 얻는다.
		fnGetCookie();

		// 사용할 데이터를 설정한다.
		fnSetData();
		if (gaWork2.length == 0) {
			alert('작품이 하나도 없으니 작업에 관한 어떤 일도 진행할 수 없다');
			return;
		}

		// 작품에 관한 목록을 만든다.
		for (var i = 0; i < gaWork2.length; ++i) {
			var	oOp = document.createElement("OPTION");
			oOp.value = gaWork2[i].showId
			oOp.text = gaWork2[i].showName
			sltShow.options.add(oOp);
		}

		// 설정된 전역변수가 없다면 전역변수를 정책에 따른 기본값으로 설정하고,
		// 있다면 설정된 데이터를 바탕으로 다시 확인한다.
		fnSetVar();

		// 작품에 따라 필요한 정보들을 초기화한다.
		if (!fnInit(gShowId)) {
			alert('fnInit(): Failure');
		}
	}
	// SHOW_ID = showId인 작업을 보여준다.
	function fnInit(showId) {
		// 1. 작업에 사용되는 gWork2를 결정한다.
		for (var i = 0; i < gaWork2.length; ++i) {
			if (gaWork2[i].showId == showId) {
				gWork2 = gaWork2[i];
				gShowDays = 1 + (gWork2.showEndDate - gWork2.showBeginDate) / (24 * 60 * 60 * 1000);
				break;
			}
		}
		if (gaWork2.length == 0 || i == gaWork2.length) {
			alert('작품(SHOW_ID = ' + showId + ')에 해당하는 정보를 찾을 수 없다');
			return false;
		}
		// 작품목록에서 해당 작품을 선택하게 한다.
		for (var i = 0; i < sltShow.options.length; ++i) {
			var	oOp = sltShow.options[i];
			oOp.selected = showId == oOp.value;
		}

		// 작업테이블을 만든다.
		if (!fnMakeWorkScheduleTable()) {
			alert('fnMakeWorkScheduleTable(): Failure');
			return false;
		}
		return true;
	}
	function fnOnUnload() {
		fnSetCookie();
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/12/28 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 상수들을 설정한다.
 */

	// .workType
	var	cWORK_DESIGN		= <?PHP echo WORK_DESIGN ?>;
	var	cWORK_STORYBOARD	= <?PHP echo WORK_STORYBOARD ?>;
	// .designType
	var	cDESIGN_CONCEPT	= <?PHP echo DESIGN_CONCEPT ?>;		// 컨셉 디장인
	var	cDESIGN_DETAIL	= <?PHP echo DESIGN_DETAIL ?>;		// 구체적인 디자인
	// .targetType
	var	cTARGET_CHR	= <?PHP echo TARGET_CHR ?>;		// cWORK_DESIGN의 경우: 인물
	var	cTARGET_BG	= <?PHP echo TARGET_BG ?>;		// cWORK_DESIGN의 경우: 배경
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/12/26 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 오브젝트를 정의한다.
 */

	// 해당 작업 이미지를 누가 만들었고, 그 이미지의 이름이 무엇인지를 말해주는 오브젝트이다.
	function objWorkImage(nickname, aFilenameUuid) {
		this.nickname = nickname;
		this.aFilenameUuid = aFilenameUuid;
						// WORK_DESIGN: 작업 이미지가 있든 없든 배열의 크기는 1이다.
						// WORK_STORYBOARD: 배열의 크기는 해당 장면의 컷 수이다.
						// 값이 undefined이면 해당 이미지가 없음을 뜻한다.
	}
	// 작업에 대한 정보를 담은 오브젝트이다.
	function objWork2(workId, workType, workName, workBeginDate, workEndDate, aWorkImage) {
		this.workId = workId;		// WORK_ID
		this.workType = workType;	// WORK_[DESIGN|STORYBOARD]
		this.workName = workName;	// WORK_NAME
		this._workBeginDate = this.workBeginDate = workBeginDate;
		this._workEndDate = this.workEndDate = workEndDate;
						// _workBeginDate, _workEndDate는 날짜가 변경되었는 지를 판단할 때 사용된다.
		this.aWorkImage = aWorkImage;	// objWorkImage에 대한 배열
						//	WORK_DESIGN: 배열의 크기는,
						//	디자인 작업을 진행하는 작업자 중에서 작업 이미지가 있는 작업자의 수이다.
						//	WORK_STORYBOARD: 배열의 크기는 1이다.

		this.rowIndex = -1;		// tblWork2의 몇 번째 행에 표시되는 지를 말해준다.
						// 데이터를 모두 읽어들인 직후에 초기화한다.
	}
	// 작품과 그 작품에 속한 작업들에 대한 정보를 담는 오브젝트이다.
	function objaWork2(showId, showName, showBeginDate, showEndDate, aWork2) {
		this.showId = showId;		// SHOW_ID
		this.showName = showName;	// SHOW_NAME
		this.showBeginDate = showBeginDate;
		this.showEndDate = showEndDate;
		this.aWork2 = aWork2;		// objWork2로 이루어진 배열
	}
	var	gaWork2;	// objaWork2로 이루어진 배열
	var	gShowId;
	var	gWork2;		// gaWork중에서 .showId가 gShowId에 해당하는 objaWork2
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
	// Drag를 시작할 때 만드는 오브젝트이다.
	function objDrag(cursor, clientX, left, width) {
		this.cursor = cursor;

		// 마우스 왼쪽 버튼을 눌렀을 때, 마우스 포인터의
		this.clientX = clientX;	// X좌표

		// 대상 오브젝트의
		this.left = left;	// left좌표
		this.width = width;	// 넓이
	}
	function fnOnMouseDown() {
		var	o = event.srcElement;
		o.oDrag = new objDrag(o.style.cursor, event.clientX, parseInt(o.style.left), parseInt(o.style.width));
		o.onmousemove = fnOnMouseMove;
	}
	function fnGetDateString(givenDate) {
		var	aMonth = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'];
		return givenDate.getFullYear() + '/' + (1 + givenDate.getMonth()) + '/' + givenDate.getDate();
//		return aMonth[givenDate.getMonth()] + ' ' + givenDate.getDate() + ', ' + givenDate.getFullYear();
	}
	function fnOnMouseMove() {
//		alert(event.srcElement.tagName);
//		event.srcElement.innerHTML = '(' + event.clientX + ', ' + event.clientY + ')';
		var	oDiv = event.srcElement;
		var	oDrag = oDiv.oDrag;
		var	deltaX = event.clientX - oDrag.clientX;
		deltaX = Math.floor(deltaX / gDayWidth) * gDayWidth;
		var	left = parseInt(oDrag.left) + deltaX;
		if (oDrag.cursor == 'w-resize') {
			if (left >= 0) {
				oDiv.style.left = left + 'px';
				oDiv.style.width = (oDrag.width - deltaX) + 'px';
			}
		} else if (oDrag.cursor == 'e-resize') {
			var	newWidth = oDrag.width + deltaX;
			if (oDrag.left + newWidth <= gShowDays * gDayWidth) {
				oDiv.style.width = (oDrag.width + deltaX) + 'px';
			}
		} else {	// 'move'
			var	right = left + parseInt(oDiv.style.width);
			if (left >= 0 && right <= gShowDays * gDayWidth) {
				oDiv.style.left = left + 'px';
			}
		}

		var	year = gWork2.showBeginDate.getFullYear();
		var	month = gWork2.showBeginDate.getMonth();
		var	day = gWork2.showBeginDate.getDate();
		var	title = '';
		title += new Date(year, month, day + (parseInt(oDiv.style.left) / gDayWidth));
		title += ' - ';
		title += new Date(year, month, day + (((parseInt(oDiv.style.left)) + parseInt(oDiv.style.width) - 1) / gDayWidth));
		oDiv.title = title;
		var	newWorkBeginDate = fnGetDateString(new Date(year, month, day + (parseInt(oDiv.style.left) / gDayWidth)));
		var	newWorkEndDate = fnGetDateString(new Date(year, month, day + (((parseInt(oDiv.style.left)) + parseInt(oDiv.style.width) - 1) / gDayWidth)));
		oDiv.title = newWorkBeginDate + ' - ' + newWorkEndDate;
		var	oWork2 = gWork2.aWork2[oDiv.parentElement.parentElement.rowIndex - 1];
		oWork2.workBeginDate = new Date(newWorkBeginDate);
		oWork2.workEndDate = new Date(newWorkEndDate);
		if (fnGetDateString(oWork2._workBeginDate) == newWorkBeginDate && fnGetDateString(oWork2._workEndDate) == newWorkEndDate) {
			oDiv.style.backgroundColor = 'green';
		} else {
			oDiv.style.backgroundColor = '#0066cc';
		}
	}
	function fnOnMouseUp() {
//alert('fnOnMouseUp: ' + o.oDrag);
		var	o = event.srcElement;
		if (o.oDrag !== undefined) {
			o.oDrag = undefined;
		}
		o.onmousemove = new Function('return false');
	}
	function fnOnMouseOut() {
		var	o = event.srcElement;
//		delete o.oDrag;
//alert('fnOnMouseOut: ' + o.oDrag);
		o.onmousemove = new Function('return false');
	}
	function fnOnMouseOver() {
		var	o = event.srcElement;
//alert('fnOnMouseOver: ' + o.oDrag);
//		if (o.oDrag === undefined) {
			var	iWidth = parseInt(o.style.width);	// '100px' -> 100
			var	offsetX = event.offsetX;
			o.style.cursor = offsetX < iWidth * 0.1 ? 'w-resize' : offsetX > iWidth * 0.9 ? 'e-resize' : 'move';
//		} else {
//			o.style.cursor = o.oDrag.cursor;
//		}
	}

	window.onresize = fnResize;
	function fnResize() {
		fnMakeWorkScheduleTable();
	}

	var	gShowDays;
	var	gDayWidth;
	function fnInsertWork2(oWork2) {
		var	oTr = tblWork2.insertRow();
		if (!oTr) {
			alert('tblWork2.insertRow(): Failure');
			return false;
		}

		// 작업
		var	oTd = oTr.insertCell();
		if (!oTd) {
			alert('.insertCell: Failure');
			tblWork2.deleteRow(oTr.rowIndex);
			return false;
		}
		oTd.align = 'CENTER';
		oTd.innerHTML = oWork2.workName;

		// 작업기간
		var	oTd = oTr.insertCell();
		if (!oTd) {
			alert('.insertCell: Failure');
			tblWork2.deleteRow(oTr.rowIndex);
			return false;
		}

		var	oDiv = document.createElement('DIV');
		oDiv.style.position = 'relative';
		oDiv.style.top = '0px';
		oDiv.style.left = (((oWork2.workBeginDate - gWork2.showBeginDate) / (24 * 60 * 60 * 1000)) * gDayWidth) + 'px';
		oDiv.style.height = '20px';
		oDiv.style.width = ((1 + (oWork2.workEndDate - oWork2.workBeginDate) / (24 * 60 * 60 * 1000)) * gDayWidth) + 'px';
//		oDiv.style.backgroundColor = 'green';
		oDiv.style.backgroundColor = fnGetDateString(oWork2._workBeginDate) == fnGetDateString(oWork2.workBeginDate) && fnGetDateString(oWork2._workEndDate) == fnGetDateString(oWork2.workEndDate) ? 'green' : '#0066cc';
		oDiv.zIndex = 1;
		oDiv.onmouseover = fnOnMouseOver;
		oDiv.onmouseout = fnOnMouseOut;
		oDiv.onmouseup = fnOnMouseUp;
		oDiv.onmousedown = fnOnMouseDown;
		oTd.appendChild(oDiv);

		// 작업결과
		var	oTd = oTr.insertCell();
		if (!oTd) {
			alert('.insertCell: Failure');
			tblWork2.deleteRow(oTr.rowIndex);
			return false;
		}
		var	innerHTML = '';
		for (var i = 0; i < oWork2.aWorkImage.length; ++i) {
			var	img;
			var	imgThumbnail = oWork2.aWorkImage[i].aFilenameUuid[0];
			if (imgThumbnail === undefined) {
				img = '/images/Thumbnail/_void.jpg';
			} else {
				img = '/images/Thumbnail/Work/' + (oWork2.workType == cWORK_DESIGN ? 'Design' : 'Storyboard') + '/' + imgThumbnail + '.png';
			}
			var	alt = ' ALT="' + (oWork2.workType == cWORK_DESIGN ? oWork2.aWorkImage[i].nickname : 'Play') + '"';
			var	onClick = ' onClick="fnDisplay(this, ' + i + ');"';
			innerHTML += '<IMG SRC="' + img + '" HEIGHT="50px" WIDTH="50px"' + alt + onClick + '>';
		}
		oTd.innerHTML = innerHTML == '' ? '&nbsp;' : innerHTML;
	}
	var	bWindow = false;
	var	displayWindow;
	function fnbWindowOpened(oWindow) {
		try {
			var	title = oWindow.document.title;
//			throw('asdf');
		} catch(e) {
/*
			if (typeof(e) == 'object') {
				for (i in e) {
					alert('|' + i + '|: |' + e[i] + '|');
				}
			} else {
				alert('Exception Throw Message: |' + e + '|');
			}
 */
			return false;
		}
		return true;
	}
	function fnDisplay(oImg, iNickname) {
		if (fnbWindowOpened(displayWindow)) {
//			displayWindow.close();
		} else {
//			displayWindow = window.open('fmDisplay.PHP', 'windowForTarget', "height = 400, status = no, toolbar = no, width = 400", false);
			displayWindow = window.open('', 'windowForTarget', "height = 400, resizable = yes, status = no, toolbar = no, width = 400", false);
			var	HTML = '';
			HTML += '<HTML>' + '\n';
			HTML += '<HEAD>' + '\n';
			HTML += '<TITLE>DisPlay Window</TITLE>' + '\n';
			// 다음 COMMENT를 추가해야 스트립트 에러가 뜨지 않는다. 왜?
			HTML += '<!--[if gte IE 5]>' + '\n';
			HTML += '<SCRIPT SRC="play.js">' + '\n';
			HTML += '</SCRIPT>' + '\n';
			HTML += '<SCRIPT LANGUAGE="JavaScript1.3">' + '\n';
			HTML += '\t' + 'var' + '\t' + 'gaImg;' + '\n';
			HTML += '</SCRIPT>' + '\n';
			// 이것두...
			HTML += '<![endif]-->' + '\n';
			HTML += '</HEAD>' + '\n';
			HTML += '<BODY>' + '\n';
			HTML += '<TABLE BORDER="0" WIDTH="100%" CELLSPACING="0" CELLPADDING="0"><TBODY>' + '\n';
			HTML += '<TR><TD ID="tdImg" WIDTH="100%" ALIGN="CENTER"></TD></TR>' + '\n';
			HTML += '<TR ID="trPlay"><TD ALIGN="CENTER"><INPUT ID="btnPlay" TYPE="BUTTON" VALUE="Play" onClick="fnPlay();">&nbsp;&nbsp;<INPUT ID="btnStop" TYPE="BUTTON" VALUE="Stop" onClick="fnStop();">' + '\n';
			HTML += '</TBODY></TABLE>' + '\n';
			HTML += '</BODY>' + '\n';
			HTML += '</HTML>' + '\n';
			displayWindow.document.write(HTML);
		}

		var	oWork2 = gWork2.aWork2[oImg.parentElement.parentElement.rowIndex - 1];
		var	imgLow = oWork2.aWorkImage[iNickname].aFilenameUuid[0];
		if (imgLow === undefined) {
			img = '/images/Thumbnail/_void.jpg';
		} else {
			img = '/images/Low/Work/' + (oWork2.workType == cWORK_DESIGN ? 'Design' : 'Storyboard') + '/' + imgLow + '.png';
		}
		var	alt = ' ALT="' + img + '"';
		displayWindow.tdImg.innerHTML = '<IMG HEIGHT="300px" WIDTH="300px" SRC="' + img + '">';
		if (oWork2.workType == cWORK_DESIGN) {
			displayWindow.trPlay.style.display = 'none';
		} else {
			displayWindow.trPlay.style.display = 'block';
			displayWindow.gaImg = [];
			for (var i = 0; i < oWork2.aWorkImage[0].aFilenameUuid.length; ++i) {
				displayWindow.gaImg.push(oWork2.aWorkImage[0].aFilenameUuid[i]);
			}
			displayWindow.btnPlay.disabled = oWork2.aWorkImage[0].aFilenameUuid.length == 1;
		}
		displayWindow.btnStop.disabled = true;
	}
</SCRIPT>
</HEAD>
<BODY>
<INPUT TYPE="BUTTON" VALUE="반영하기" onClick="fnSubmit();">
<DIV>
<P STYLE="text-decoration: underline;">작업 일정표&nbsp;&nbsp;&nbsp;&nbsp;작품: <SELECT ID="sltShow" SIZE="1" STYLE="width: 400px;" onChange="fnShowSelectOnChange();"></SELECT></P>
<TABLE ID="tblWork2" WIDTH="100%" BORDER="1" CELLSPACING="0" CELLPADDING="0"><TBODY>
<TR>
	<TD ID="oWork" ALIGN="CENTER" WIDTH="20%">작업</TD>
	<TD ID="oWorkPeriod" ALIGN="LEFT" WIDTH="70%"><DIV ID="oTHDiv">작업기간</DIV></TD>
	<TD ID="oWorkImage" ALIGN="CENTER" WIDTH="10%">작업결과</TD>
</TR>
</TBODY></TABLE>
</DIV>
</BODY>
</HTML>
<?PHP
	}
	// Closing connection
	mysql_close($link);
?>
