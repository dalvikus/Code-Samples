<?PHP
/*
 * 2005/12/9 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 디자인 작업에서는 참고자료를 구별할 때, gCiteType으로 충분했다.
 * 하지만 스토리보드 작업의 경우에는, 참고하는 디자인 작업이 여러 개인 경우
 * gCiteType(= cCITE_DESIGN)이 중복된다. 따라서 gCiteType대신에 gRefId를 사용한다.
 * 참고자료가 디자인 작업인 경우, 여기에다 지금처럼 gDesignUserId가 더 있어야 한다.
 */
?>
<?PHP
/*
 * 2005/12/8 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * "컷 수정"을 구현한다.
 */
?>
<?PHP
/*
 * 2005/12/7 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 1. 스토리보드 작업은 한 사람에 의해서 이루어진다.
 * 2. 참고자료와 컷 이미지 등록을 마무리한다.
 */
?>
<?PHP
/*
 * 2005/12/6 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 스토리보드 작업과 작업에서 참고하는 자료에 대한 정보를 얻는다.
 */
?>
<?PHP
/*
 * 2005/12/5 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 스토리보드 작업에 필요한 오브젝트를 정의한다.
 * 많은 경우 디자인 작업의 경우와 같다. (objImage, objRefSeriea, objRef)
 * 오브젝트에서 사용되는 배열은 모두 참조에 의해서 사용된다.
 * (디자인 작업의 경우에도 이렇게 바꾸었다.)
 */
?>
<?PHP
/*
 * 2005/12/5 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 오브젝트에서 사용되는 배열은 모두 참조에 의해서 사용되므로,
 * 더 이상 ByVal, ByRef등의 표시는 의미가 없다.
 */
?>
<?PHP
/*
 * 2005/12/1 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * DB에서 얻은 정보를 토대로 그림(Low Resolution Image, Thumbnail Image)이나 HTML을 표시한다.
 */
?>
<?PHP
/*
 * 2005/12/1 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 쿠키에 설정된 개인 정보로부터 USER_ID를 얻는다.
 */

	// LOGON한 페이지에서 개인정보를 쿠키에 저장한다.
//	phpinfo();

	// 쿠키로부터 USER_ID를 얻는다.
	function fnGetUserId() {
		if (!isset($_COOKIE["User"])) {
			die('개인 정보를 얻을 수 없다');
		}
		$aUser = split(':', $_COOKIE["User"]);
		return $aUser[0];
	}
	$gUserId = fnGetUserId();
?>
<?PHP
	// submitType: 어떤 정보를 서버로 보내는 지를 말해준다.
	define("SUBMIT_WORK", 1);	// 작업의 결과 이미지를 보낸다.
	define("SUBMIT_REF", 2);	// 참고자료를 보내다.
	define("SUBMIT_EDITCUT", 3);	// "컷 수정"정보를 보낸다.

	// citeType: 이 참고자료를 인용하는 유형
	define("CITE_SHOW", 1);		// 작품에서 참고한다.
	define("CITE_SCENE", 2);	// 장면에서 참고한다.
	define("CITE_TARGET", 3);	// 대상(인물/배경)에서 참고한다.
	// 2005/12/6 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
	// 디자인 작업과 스토리보드 작업을 분리한다.
	define("CITE_DESIGN", 4);	// WORK_ID(= DESIGN_ID)에 해당하는 (디자인)작업에서 참고한다.
	define("CITE_STORYBOARD", 5);	// WORK_ID(= STORYBOARD_ID)에 해당하는 (스토리보드)작업에서 참고한다.
	// refType: 참조자료의 유형
	define("REF_IMAGE", 1);		// 그림
	define("REF_HTML", 2);		// HTML문서
	define("REF_LINK", 3);		// HTML문서에 대한 URI

	// designType: 디자인 유형
	define("DESIGN_CONCEPT", 1);	// 컨셉 디자인
	define("DESIGN_DETAIL", 2);	// 구체적인 디자인
	// targetType: 디자인 대상
	define("TARGET_CHR", 1);	// 인물
	define("TARGET_BG", 2);		// 배경
?>
<?PHP
	function uniqueFilename() {
		return sha1(uniqid(rand(), true));
	}
?>
<?PHP
/*
 * 2005/12/1 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 웹을 통해 얻은 정보를 해당 테이블에 저장한다.
 */
	// Connecting, selecting database
	$link = mysql_connect('localhost', 'root', 'MySQL-4.1.14') or die('Could not connect: ' . mysql_error());
	mysql_select_db('PREPRODUCTION') or die('Could not select database');
	if (isset($_POST["SubmitType"])) {
//		phpinfo();
		if ($_POST["SubmitType"] == SUBMIT_EDITCUT) {
			$storyboardId = $_POST["StoryboardID"];

			$aInsertSerial = array();	// INSERT INTO TCUT (STORYBOARD_ID, SERIAL) VALUES (#, #), (#, #), ...
			$aDeleteCutId = array();	// DELETE FROM TCUT WHERE CUT_ID = # OR CUT_ID = # OR ...
			$aUpdateQuery = array();	// ('UPDATE TCUT SET SERIAL = # WHERE CUT_ID = #', ...)
			$aCutIdSerial = split(',', $_POST["CutIdSerial"]);
			foreach ($aCutIdSerial as $cutIdSerial) {
				$a = split(':', $cutIdSerial);
				if ($a[0] == 0) {	// 추가
					// SERIAL: $a[1]
					array_push($aInsertSerial, '(' . $storyboardId . ', ' . $a[1] . ')');
				} else if ($a[1] == 0) {	// 삭제
					// CUT_ID: $a[0]
					array_push($aDeleteCutId, 'CUT_ID = ' . $a[0]);
				} else {			// 변경
					// CUT_ID: $a[0], SERIAL: $a[1];
					array_push($aUpdateQuery, 'UPDATE TCUT SET SERIAL = ' . $a[1] . ' WHERE CUT_ID = ' . $a[0]);
				}
			}
			// 일련번호의 충돌을 막기위하여 (삭제 -> 변경 -> 추가)순으로 진행해야한다.
			if (count($aDeleteCutId)) {
				$query = 'DELETE FROM TCUT WHERE ' . join(' OR ', $aDeleteCutId);
				echo 'DELETE: |' . $query . '|' . "\n";
				if (!mysql_query($query)) {
					$message = '';
					$message .= 'errno: |' . mysql_errno() . '|' . "\n";
					$message .= 'error: |' . mysql_error() . '|' . "\n";
					$message .= 'query: |' . $query . '|' . "\n";
					die($message);
				}
			}
			foreach ($aUpdateQuery as $query) {
				echo 'UPDATE: |' . $query . '|' . "\n";
				if (!mysql_query($query)) {
					$message = '';
					$message .= 'errno: |' . mysql_errno() . '|' . "\n";
					$message .= 'error: |' . mysql_error() . '|' . "\n";
					$message .= 'query: |' . $query . '|' . "\n";
					die($message);
				}
			}
			if (count($aInsertSerial)) {
				$query = 'INSERT INTO TCUT (STORYBOARD_ID, SERIAL) VALUES ' . join(', ', $aInsertSerial);
				echo 'INSERT: |' . $query . '|' . "\n";
				if (!mysql_query($query)) {
					$message = '';
					$message .= 'errno: |' . mysql_errno() . '|' . "\n";
					$message .= 'error: |' . mysql_error() . '|' . "\n";
					$message .= 'query: |' . $query . '|' . "\n";
					die($message);
				}
			}
		} else if ($_POST["SubmitType"] == SUBMIT_REF) {
if (!isset($_FILES["RefImage"]) && !isset($_POST["RefHtml"])) {
	// TREFERENCE_SERIES테이블에서,
	// ($_POST["RefID"], $_POST["Serial"])에 해당하는 참고자료를 삭제한다.
	$query = sprintf("DELETE FROM TREFERENCE_SERIES WHERE REF_ID = %d AND SERIAL = %d", $_POST["RefID"], $_POST["Serial"]);
	if (!mysql_query($query)) {
		$message = '';
		$message .= 'errno: |' . mysql_errno() . '|' . "\n";
		$message .= 'error: |' . mysql_error() . '|' . "\n";
		$message .= 'query: |' . $query . '|' . "\n";
		die($message);
	}
} else {
			$refType = isset($_FILES["RefImage"]) ? REF_IMAGE : REF_HTML;
			$content = $refType == REF_HTML ? mysql_real_escape_string($_POST["RefHtml"]) : '';
			// REF_IMAGE인 경우에는 아래에서 설정한다.
			if ($refType == REF_IMAGE) {
				$fileRefImage = $_FILES["RefImage"];
				if ($fileRefImage['error'] != UPLOAD_ERR_OK) {
					die(print_r($fileRefImage, true));
				}
				$uniqueFilename = '';	// UNIQUE
				$highResImage = '';	// 원본 이미지
				$lowResImage = '';	// 웹에 표시되는 이미지
				$thumbnailImage = '';	// "Thumbnails"버튼을 눌렀을 때 나타나는 이미지
				do {
					$uniqueFilename = uniqueFilename();
					$highResImage = '/var/spool/Images/High/Ref/' . $uniqueFilename;
				} while (is_file($highResImage));
				$lowResImage = '/var/spool/Images/Low/Ref/' . $uniqueFilename . '.png';
				$thumbnailImage = '/var/spool/Images/Thumbnail/Ref/' . $uniqueFilename . '.png';
				if (!move_uploaded_file($fileRefImage['tmp_name'], $highResImage)) {
					die('move_uploaded_file: Failure' . "\n" . print_r($fileRefImage, true));
				}
				system("/usr/local/bin/convert -thumbnail 200 $highResImage $lowResImage 2>&1");
				if (!is_file($lowResImage)) {
					die('system("convert -thumbnail 200 ' . $highResImage . ' ' . $lowResImage . '"): Failure');
				}
				system("/usr/local/bin/convert -thumbnail 50 $highResImage $thumbnailImage 2>&1");
				if (!is_file($lowResImage)) {
					die('system("convert -thumbnail 50 ' . $highResImage . ' ' . $thumbnailImage . '"): Failure');
				}

				$content = $uniqueFilename . ':' . $fileRefImage['name'];
			}
			// TREFERENCE_SERIES테이블에 등록한다.
			$aUser = split(',', $_COOKIE["User"]);
			$query = sprintf(
				"INSERT INTO TREFERENCE_SERIES (REF_ID, SERIAL, TYPE, CONTENT, USER_ID) VALUES (%d, %d, %d, '%s', %d)",
				$_POST["RefID"], $_POST["Serial"], $refType, $content, $aUser[0]
			);
			if (!mysql_query($query)) {
				$message = '';
				$message .= 'errno: |' . mysql_errno() . '|' . "\n";
				$message .= 'error: |' . mysql_error() . '|' . "\n";
				$message .= 'query: |' . $query . '|' . "\n";
				die($message);
			}
}
		} else {	// cSUBMIT_WORK
if (!isset($_FILES["CutImage"])) {
	// TWORK_IMAGE테이블에서,
	// ($_POST["ImageID"], $_POST["Revision"])에 해당하는 리비젼을 삭제한다.
	$query = sprintf("DELETE FROM TWORK_IMAGE WHERE IMAGE_ID = %d AND REVISION = %d", $_POST["ImageID"], $_POST["Revision"]);
	if (!mysql_query($query)) {
		$message = '';
		$message .= 'errno: |' . mysql_errno() . '|' . "\n";
		$message .= 'error: |' . mysql_error() . '|' . "\n";
		$message .= 'query: |' . $query . '|' . "\n";
		die($message);
	}
} else {
			$fileCutImage = $_FILES["CutImage"];
			if ($fileCutImage['error'] != UPLOAD_ERR_OK) {
				die(print_r($fileCutImage, true));
			}
			$uniqueFilename = '';	// UNIQUE
			$highResImage = '';	// 원본 이미지
			$lowResImage = '';	// 웹에 표시되는 이미지
			$thumbnailImage = '';	// "Thumbnails"버튼을 눌렀을 때 나타나는 이미지
			do {
				$uniqueFilename = uniqueFilename();
				$highResImage = '/var/spool/Images/High/Work/Storyboard/' . $uniqueFilename;
			} while (is_file($highResImage));
			$lowResImage = '/var/spool/Images/Low/Work/Storyboard/' . $uniqueFilename . '.png';
			$thumbnailImage = '/var/spool/Images/Thumbnail/Work/Storyboard/' . $uniqueFilename . '.png';
			if (!move_uploaded_file($fileCutImage['tmp_name'], $highResImage)) {
				die('move_uploaded_file: Failure' . "\n" . print_r($fileCutImage, true));
			}
			system("/usr/local/bin/convert -thumbnail 200 $highResImage $lowResImage 2>&1");
			if (!is_file($lowResImage)) {
				die('system("convert -thumbnail 200 ' . $highResImage . ' ' . $lowResImage . '"): Failure');
			}
			system("/usr/local/bin/convert -thumbnail 50 $highResImage $thumbnailImage 2>&1");
			if (!is_file($lowResImage)) {
				die('system("convert -thumbnail 50 ' . $highResImage . ' ' . $thumbnailImage . '"): Failure');
			}

			$imageId = $_POST["ImageID"];
			if ($imageId == 0) {
				// 먼저 IMAGE_ID부터 얻는다.
				// 새로운 작품을 등록한다.
				$query = 'INSERT INTO TIMAGE () VALUES ()';
				if (!mysql_query($query)) {
					$message = '';
					$message .= 'errno: |' . mysql_errno() . '|' . "\n";
					$message .= 'error: |' . mysql_error() . '|' . "\n";
					$message .= 'query: |' . $query . '|' . "\n";
					die($message);
				}
				// IMAGE_ID를 얻는다.
				$query = "SELECT LAST_INSERT_ID()";
				if (!($result = mysql_query($query))) {
					$message = '';
					$message .= 'errno: |' . mysql_errno() . '|' . "\n";
					$message .= 'error: |' . mysql_error() . '|' . "\n";
					$message .= 'query: |' . $query . '|' . "\n";
					die($message);
				}
				if (!($row = mysql_fetch_array($result, MYSQL_NUM))) {
					die('Cannot get IMAGE_ID');
				}
				$imageId = $row[0];
				mysql_free_result($result);

				// TCUT테이블에서 CUT_ID가 참조하는 IMAGE_ID를 갱신한다.
				$query = sprintf(
					"UPDATE TCUT SET IMAGE_ID = %d WHERE CUT_ID = %d",
					$imageId, $_POST['CutID']
				);
				if (!($result = mysql_query($query))) {
					$message = '';
					$message .= 'errno: |' . mysql_errno() . '|' . "\n";
					$message .= 'error: |' . mysql_error() . '|' . "\n";
					$message .= 'query: |' . $query . '|' . "\n";
					die($message);
				}
			}
			$aUser = split(',', $_COOKIE["User"]);
			// $imageId, $_POST["Revision"], $fileCutImage['name'], $uniqueFilename, $aUser[0]
			$query = sprintf(
				"INSERT INTO TWORK_IMAGE (IMAGE_ID, REVISION, FILENAME, FILENAME_UUID, USER_ID) VALUES (%d, %d, '%s', '%s', %d)"
				,
				$imageId, $_POST["Revision"], $fileCutImage['name'], $uniqueFilename, $aUser[0]
			);
			if (!mysql_query($query)) {
				$message = '';
				$message .= 'errno: |' . mysql_errno() . '|' . "\n";
				$message .= 'error: |' . mysql_error() . '|' . "\n";
				$message .= 'query: |' . $query . '|' . "\n";
				die($message);
			}
}
		}
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML>
<HEAD>
<TITLE>스토리보드 작업</TITLE>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; CHARSET=euc-kr">
</HEAD>
<BODY>
<PRE>
<?PHP
?>
</PRE>
<SCRIPT LANGUAGE="JavaScript1.3">
	location.replace("fmStoryboard.PHP");
</SCRIPT>
</BODY>
</HTML>
<?PHP
	} else {
/*
 * 2005/11/30 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 데이터베이스에서 필요한 정보를 얻어 JavaScript가 eval해서 gaaStoryboard, gaRef을 설정하도록 한다.
 */
if (1) {
		global	$gUserId;	// 이 작업자에 할당된 작업을 찾는다.

		global	$JavaScriptEvalCode;
		$JavaScriptEvalCode = '';	// JavaScript가 해석할 문자열
						// 이 문자열은 gaaStoryboard, gaRef에 대한 정보를 담고 있다.

		// 1. gaaStoryboard배열을 만든다.
		// 1.1. 모든 스토리보드 작업을 얻는다.
		//	1) 이는 주어진 작품에 대한 스토리보드 작업 결과들을 연속적으로 볼 때 필요하다.
		//	2) 이 때, USER_ID에 대한 정보로부터
		//	작업자에 할당된 스토리보트 작업을 얻을 수도 있다.
		//	3) 표시를 위해서 장면의 이름도 얻는다.
		//	4) 이 작업이 참고하는 자료에 대한 REF_ID도 얻는다.
		$query = 'SELECT T1.SHOW_ID, T7.NAME AS SHOW_NAME, T1.WORK_ID, T1.SCENE_ID, T5.NAME AS SCENE_NAME, T2.USER_ID, T1.YOUR_ORDER, T1.STATUS, T4.CUT_ID, T4.SERIAL, T4.IMAGE_ID FROM TWORK AS T1, TWORKER AS T2, TSTORYBOARD AS T3, TCUT AS T4, TSCENE AS T5, TSHOW AS T7 WHERE T1.WORK_ID = T2.WORKER_ID AND T1.WORK_ID = T3.STORYBOARD_ID AND T3.STORYBOARD_ID = T4.STORYBOARD_ID AND T1.SCENE_ID = T5.SCENE_ID AND T1.SHOW_ID = T7.SHOW_ID ORDER BY T1.SHOW_ID, T1.WORK_ID, T4.SERIAL';
//		echo $query . "\n";
		if (!($resultStoryboard = mysql_query($query))) {
			$message = '';
			$message .= 'errno: |' . mysql_errno() . '|' . "\n";
			$message .= 'error: |' . mysql_error() . '|' . "\n";
			$message .= 'query: |' . $query . '|' . "\n";
			die($message);
		}
		// 성공적으로 쿼리를 전달했다.

		// 작업에 대한 정보를 작품별로 얻도록 한다.
		$aStoryboardByStoryboardId = array();	// 작업들에 대한 배열
		$aaStoryboardByShowId = array();	// $aStoryboardByStoryboardId로 이루어진 배열
		// (SHOW_ID, WORK_ID, SERIAL)에 따라 행을 정렬했다.
		$lastShowId = 0; $lastShowName = ""; $bUserAtWorkInShow = 0;
		// 주어진 WORK_ID에 대하여 REF_ID, SCENE_ID, SCENE_NAME, USER_ID, YOUR_ORDER, STATUS는 UNIQUE하다.
		$lastWorkId = 0; $lastRefId = 0; $lastSceneId = 0; $lastSceneName = ""; $lastUserId = 0; $lastOrder = ""; $lastStatus = 0;
		$aCut = array();
/*
+---------+-------------+---------+----------+------------------------+---------+--------+--------+----------+
| SHOW_ID | SHOW_NAME   | WORK_ID | SCENE_ID | SCENE_NAME             | USER_ID | CUT_ID | SERIAL | IMAGE_ID |
+---------+-------------+---------+----------+------------------------+---------+--------+--------+----------+
|       1 | Camel In OH |       3 |        1 | 새로운 시작            |       1 |      1 |      1 |        0 |
|       1 | Camel In OH |       3 |        1 | 새로운 시작            |       1 |      4 |      2 |        0 |
|       1 | Camel In OH |       5 |        3 | 아쉬운 이별            |       7 |      2 |      1 |        0 |
|       1 | Camel In OH |       6 |        2 | 작지만 포근한 보금자리 |       1 |      3 |      1 |        0 |
|       1 | Camel In OH |       6 |        2 | 작지만 포근한 보금자리 |       1 |      5 |      2 |        0 |
+---------+-------------+---------+----------+------------------------+---------+--------+--------+----------+
 */
		// 참고자료에 대한 정보를 모으기 위하여...
		$aRefShowId = array();
		$aRefSceneId = array();
		$aRefWorkId = array();
		while ($row = mysql_fetch_array($resultStoryboard, MYSQL_ASSOC)) {
			$showId = $row["SHOW_ID"];
			$showName = $row["SHOW_NAME"];
			$workId = $row["WORK_ID"];

			if ($showId != $lastShowId) {
				if ($lastWorkId) {
					// 지금까지 모은 작업에 대한 정보를 쓴다.
					array_push($aStoryboardByStoryboardId,
						"\n" .
						"\t\t\t" . 'new objStoryboard(' . "\n" .
						"\t\t\t\t" . $lastWorkId . ',' . "\n" .
						"\t\t\t\t" . $lastSceneId . ',' . "\n" .
						"\t\t\t\t" . ('"' . $lastSceneName . '"') . ',' . "\n" .
						"\t\t\t\t" . $lastUserId . ',' . "\n" .
						"\t\t\t\t" . ('"' . preg_replace('/\r\n/', '\\\\n', preg_replace('/([\\\"])/', "\\\\$1", $lastOrder)) . '"') . ',' . "\n" .
						"\t\t\t\t" . $lastStatus . ',' . "\n" .
						"\t\t\t\t" . '[' . join(',', $aCut) . "\n" .
						"\t\t\t\t" . ']' . "\n" .
						"\t\t\t" . ')'
					);
					if ($lastUserId = $gUserId) {
						$bUserAtWorkInShow = 1;
						// $gUserId에 해당하는 USER가 참여하는 스토리보드 작업의 경우,
						// 장면과 작업에 대한 참고자료를 얻기 위하여,
						// REF_ID와 SCENE_ID를 기억한다.
						array_push($aRefSceneId, $lastSceneId);
						array_push($aRefWorkId, $lastWorkId);
					}
				}
				if ($lastShowId) {
					// 지금까지 모은, 작품에서의 작업에 대한 정보를 쓴다.
					array_push($aaStoryboardByShowId,
						"\n" .
						"\t" . 'new objWorkStoryboard(' . "\n" .
						"\t\t" . $lastShowId . ',' . "\n" .
						"\t\t" . ('"' . $lastShowName . '"') . ',' . "\n" .
						"\t\t" . '[' . join($aStoryboardByStoryboardId, ',') . "\n" .
						"\t\t" . ']' . "\n" .
						"\t" . ')'
					);
					if ($bUserAtWorkInShow) {
						// USER가 참여하는 작업이 있는 작품이라면,
						// 작품에 대한 참고자료를 얻기 위하여,
						// SHOW_ID를 기억한다.
						array_push($aRefShowId, $lastShowId);
					}
				}
				// 새로운 작품에 대해서 반복한다.
				$lastShowId = $showId; $lastShowName = $showName; $bUserAtWorkInShow = 0;
				$lastWorkId = 0; $lastRefId = 0; $lastSceneId = 0; $lastSceneName = ""; $lastUserId = 0; $lastOrder = ""; $lastStatus = 0;
				$aCut = array();
			}
			if ($workId != $lastWorkId) {
				if ($lastWorkId != 0) {
					// 지금까지 모은 작업에 대한 정보를 쓴다.
					array_push($aStoryboardByStoryboardId,
						"\n" .
						"\t\t\t" . 'new objStoryboard(' . "\n" .
						"\t\t\t\t" . $lastWorkId . ',' . "\n" .
						"\t\t\t\t" . $lastSceneId . ',' . "\n" .
						"\t\t\t\t" . ('"' . $lastSceneName . '"') . ',' . "\n" .
						"\t\t\t\t" . $lastUserId . ',' . "\n" .
						"\t\t\t\t" . ('"' . preg_replace('/\r\n/', '\\\\n', preg_replace('/([\\\"])/', "\\\\$1", $lastOrder)) . '"') . ',' . "\n" .
						"\t\t\t\t" . $lastStatus . ',' . "\n" .
						"\t\t\t\t" . '[' . join(',', $aCut) . "\n" .
						"\t\t\t\t" . ']' . "\n" .
						"\t\t\t" . ')'
					);
					if ($lastUserId == $gUserId) {
						$bUserAtWorkInShow = 1;
						// $gUserId에 해당하는 USER가 참여하는 스토리보드 작업의 경우,
						// 장면과 작업에 대한 참고자료를 얻기 위하여,
						// REF_ID와 SCENE_ID를 기억한다.
						array_push($aRefSceneId, $lastSceneId);
						array_push($aRefWorkId, $lastWorkId);
					}
				}
				// 새로운 작업에 대해서 반복한다.
				$lastWorkId = $workId; $lastRefId = $row["REF_ID"]; $lastSceneId = $row["SCENE_ID"]; $lastSceneName = $row["SCENE_NAME"]; $lastUserId = $row["USER_ID"]; $lastOrder = $row["YOUR_ORDER"]; $lastStatus = $row["STATUS"];
				$aCut = array();
			}
			// $workId에 해당하는 스토리보드 작업의 컷에 대한 정보를 얻는다.
			$imageId = $row["IMAGE_ID"];
			$aImage = array();
			if ($imageId) {		// IMAGE_ID != 0인 경우에 한해서...
				$query = 'SELECT REVISION, YOUR_COMMENT, FILENAME, FILENAME_UUID, USER_ID FROM TWORK_IMAGE WHERE IMAGE_ID = ' . $imageId . ' ORDER BY REVISION';
//				echo $query . "\n";
				if (!($resultImage = mysql_query($query))) {
					$message = '';
					$message .= 'errno: |' . mysql_errno() . '|' . "\n";
					$message .= 'error: |' . mysql_error() . '|' . "\n";
					$message .= 'query: |' . $query . '|' . "\n";
					die($message);
				}
				while ($rowImage = mysql_fetch_array($resultImage, MYSQL_ASSOC)) {
					array_push($aImage,
						"\n" .
						"\t\t\t\t\t\t\t" . 'new objImage(' .
							$rowImage["REVISION"] . ', ' .
							('"' . preg_replace('/\r\n/', '\\\\n', preg_replace('/([\\\"])/', "\\\\$1", $rowImage["YOUR_COMMENT"])) . '"') . ', ' .
							('"' . $rowImage["FILENAME"] . '"') . ', ' .
							('"' . $rowImage["FILENAME_UUID"] . '"') .
						')'
					);
				}
				// 사용한 결과를 반환한다.
				mysql_free_result($resultImage);
			}
			array_push($aCut,
				"\n" .
				"\t\t\t\t\t" . 'new objCut(' . "\n" .
				"\t\t\t\t\t\t" . $row["CUT_ID"] . ',' . "\n" .
				"\t\t\t\t\t\t" . $row["SERIAL"] . ',' . "\n" .
				"\t\t\t\t\t\t" . $imageId . ',' . "\n" .
				"\t\t\t\t\t\t" . '[' . join(',', $aImage) . "\n" .
				"\t\t\t\t\t\t" . ']' . "\n" .
				"\t\t\t\t\t" . ')'
			);
		}
		// 사용한 결과를 반환한다.
		mysql_free_result($resultStoryboard);

		if ($lastWorkId) {
			// 지금까지 모은 작업에 대한 정보를 쓴다.
			array_push($aStoryboardByStoryboardId,
				"\n" .
				"\t\t\t" . 'new objStoryboard(' . "\n" .
				"\t\t\t\t" . $lastWorkId . ',' . "\n" .
				"\t\t\t\t" . $lastSceneId . ',' . "\n" .
				"\t\t\t\t" . ('"' . $lastSceneName . '"') . ',' . "\n" .
				"\t\t\t\t" . $lastUserId . ',' . "\n" .
				"\t\t\t\t" . ('"' . preg_replace('/\r\n/', '\\\\n', preg_replace('/([\\\"])/', "\\\\$1", $lastOrder)) . '"') . ',' . "\n" .
				"\t\t\t\t" . $lastStatus . ',' . "\n" .
				"\t\t\t\t" . '[' . join(',', $aCut) . "\n" .
				"\t\t\t\t" . ']' . "\n" .
				"\t\t\t" . ')'
			);
			if ($lastUserId = $gUserId) {
				$bUserAtWorkInShow = 1;
				// $gUserId에 해당하는 USER가 참여하는 스토리보드 작업의 경우,
				// 장면과 작업에 대한 참고자료를 얻기 위하여,
				// REF_ID와 SCENE_ID를 기억한다.
				array_push($aRefSceneId, $lastSceneId);
				array_push($aRefWorkId, $lastWorkId);
			}
		}
		if ($lastShowId) {
			array_push($aaStoryboardByShowId,
				"\n" .
				"\t" . 'new objWorkStoryboard(' . "\n" .
				"\t\t" . $lastShowId . ',' . "\n" .
				"\t\t" . ('"' . $lastShowName . '"') . ',' . "\n" .
				"\t\t" . '[' . join($aStoryboardByStoryboardId, ',') . "\n" .
				"\t\t" . ']' . "\n" .
				"\t" . ')'
			);
			if ($bUserAtWorkInShow) {
				// USER가 참여하는 작업이 있는 작품이라면,
				// 작품에 대한 참고자료를 얻기 위하여,
				// SHOW_ID를 기억한다.
				array_push($aRefShowId, $lastShowId);
			}
		}
		$JavaScriptEvalCode .=
			'gaaStoryboard = [' . join(',', $aaStoryboardByShowId) . "\n" .
			'];' . "\n";
//		echo $JavaScriptEvalCode;

		// 2. 참고자료에 대한 정보를 얻는다.
		$aRef = array();
//		print_r($aRefShowId);
//		print_r($aRefSceneId);
//		print_r($aRefWorkId);
		$query =	// 읽기 쉬운 형태
			'SELECT' . "\n" .
			"\t" . 'REF_ID, CITE_TYPE, CITE_ID, -1 AS SCENE_ID, NAME' . "\n" .
			'FROM' . "\n" .
			"\t" . 'TREFERENCE, TSHOW' . "\n" .
			'WHERE' . "\n" .
			"\t" . '(CITE_TYPE = ' . CITE_SHOW  . ' AND (CITE_ID = ' . join(' OR CITE_ID = ', $aRefShowId) . ')' . ')' . ' AND' . "\n" .
			"\t" . 'CITE_ID = SHOW_ID' . "\n" .
			"\n" .
			'UNION' . "\n" .
			"\n" .
			'SELECT' . "\n" .
			"\t" . 'REF_ID, CITE_TYPE, CITE_ID, -1 AS SCENE_ID, NAME' . "\n" .
			'FROM' . "\n" .
			"\t" . 'TREFERENCE, TSCENE' . "\n" .
			'WHERE' . "\n" .
			"\t" . '(CITE_TYPE = ' . CITE_SCENE  . ' AND (CITE_ID = ' . join(' OR CITE_ID = ', $aRefSceneId) . ')' . ')' . ' AND' . "\n" .
			"\t" . 'CITE_ID = SCENE_ID' . "\n" .
			"\n" .
			'UNION' . "\n" .
			"\n" .
			'SELECT' . "\n" .
			"\t" . 'REF_ID, CITE_TYPE, CITE_ID, -1 AS SCENE_ID, "NOT USED" NAME' . "\n" .
			'FROM' . "\n" .
			"\t" . 'TREFERENCE' . "\n" .
			'WHERE' . "\n" .
			"\t" . '(CITE_TYPE = ' . CITE_STORYBOARD  . ' AND (CITE_ID = ' . join(' OR CITE_ID = ', $aRefWorkId) . ')' . ')' . "\n" .
			"\n" .
			'UNION' . "\n" .
			"\n" .
			// 디자인 작업 중에서 SCENE_ID가 0이거나 $aRefSceneId에 있는 디자인 작업에 대한 정보를 얻는다.
			// 즉 스토리보드 작업에 참고하는 디자인 작업에 대한 그림들을 얻는다.
			'SELECT' . "\n" .
			"\t" . 'REF_ID, CITE_TYPE, CITE_ID, T1.SCENE_ID, CONCAT(' . "\n" .
			"\t\t" . 'CASE T1.SCENE_ID WHEN "" THEN' . "\n" .
			"\t\t\t" . 'CONCAT(' . "\n" .
			"\t\t\t\t" . '"작품 전반에 ",' . "\n" .
			"\t\t\t\t" . 'CASE T3.TYPE WHEN ' . TARGET_CHR . "\n" .
			"\t\t\t\t" . 'THEN' . "\n" .
			"\t\t\t\t\t" . '"등장하는 "' . "\n" .
			"\t\t\t\t" . 'ELSE' . "\n" .
			"\t\t\t\t\t" . '"사용되는 "' . "\n" .
			"\t\t\t\t" . 'END' . "\n" .
			"\t\t\t" . ')' . "\n" .
			"\t\t" . 'ELSE' . "\n" .
			"\t\t\t" . 'CONCAT(' . "\n" .
			"\t\t\t\t" . '"특정장면(",' . "\n" .
			"\t\t\t\t" . 'T2.NAME,' . "\n" .
			"\t\t\t\t" . '")에 ",' . "\n" .
			"\t\t\t\t" . 'CASE T3.TYPE WHEN ' . TARGET_CHR . "\n" .
			"\t\t\t\t" . 'THEN' . "\n" .
			"\t\t\t\t\t" . '"등장하는 "' . "\n" .
			"\t\t\t\t" . 'ELSE' . "\n" .
			"\t\t\t\t\t" . '"사용되는 "' . "\n" .
			"\t\t\t\t" . 'END' . "\n" .
			"\t\t\t" . ')' . "\n" .
			"\t\t" . 'END,' . "\n" .
			"\t\t" . 'CASE T3.TYPE WHEN ' . TARGET_CHR . "\n" .
			"\t\t" . 'THEN' . "\n" .
			"\t\t\t" . '"인물"' . "\n" .
			"\t\t" . 'ELSE' . "\n" .
			"\t\t\t" . '"배경"' . "\n" .
			"\t\t" . 'END,' . "\n" .
			"\t\t" . '"(",' . "\n" .
			"\t\t" . 'T3.NAME,' . "\n" .
			"\t\t" . '")에 대한 디자인"' . "\n" .
			"\t" . ')' . "\n" .
			'FROM' . "\n" .
			"\t" . 'TREFERENCE, TDESIGN AS T1, TSCENE AS T2, TTARGET AS T3' . "\n" .
			'WHERE' . "\n" .
			"\t" . '(T1.SCENE_ID = 0 OR (' . ('T1.SCENE_ID = ' . join(' OR T1.SCENE_ID = ', $aRefSceneId)) . ')) AND' . "\n" .
			"\t" . '(CITE_TYPE = ' . CITE_DESIGN . ' AND CITE_ID = DESIGN_ID) AND' . "\n" .
			"\t" . 'T1.SCENE_ID = T2.SCENE_ID AND' . "\n" .
			"\t" . 'T1.TARGET_ID = T3.TARGET_ID'
		;
//		echo $query . "\n";
		$query =	// 한 줄에
			'SELECT REF_ID, CITE_TYPE, CITE_ID, -1 AS SCENE_ID, NAME FROM TREFERENCE, TSHOW WHERE ' .
			'(CITE_TYPE = ' . CITE_SHOW  . ' AND (CITE_ID = ' . join(' OR CITE_ID = ', $aRefShowId) . ')' . ')' . ' AND CITE_ID = SHOW_ID' .
			' UNION ' .
			'SELECT REF_ID, CITE_TYPE, CITE_ID, -1 AS SCENE_ID, NAME FROM TREFERENCE, TSCENE WHERE ' .
			'(CITE_TYPE = ' . CITE_SCENE  . ' AND (CITE_ID = ' . join(' OR CITE_ID = ', $aRefSceneId) . ')' . ')' . ' AND CITE_ID = SCENE_ID' .
			' UNION ' .
			'SELECT REF_ID, CITE_TYPE, CITE_ID, -1 AS SCENE_ID, "NOT USED" NAME FROM TREFERENCE WHERE ' .
			'(CITE_TYPE = ' . CITE_STORYBOARD  . ' AND (CITE_ID = ' . join(' OR CITE_ID = ', $aRefWorkId) . ')' . ')' .
			' UNION ' .
			// 디자인 작업 중에서 SCENE_ID가 0이거나 $aRefSceneId에 있는 디자인 작업에 대한 정보를 얻는다.
			// 즉 스토리보드 작업에 참고하는 디자인 작업에 대한 그림들을 얻는다.
			'SELECT REF_ID, CITE_TYPE, CITE_ID, T1.SCENE_ID, CONCAT(CASE T1.SCENE_ID WHEN "" THEN CONCAT("작품 전반에 ", CASE T3.TYPE WHEN ' . TARGET_CHR . ' THEN "등장하는 " ELSE "사용되는 " END) ELSE CONCAT("특정장면(", T2.NAME, ")에 ", CASE T3.TYPE WHEN ' . TARGET_CHR . ' THEN "등장하는 " ELSE "사용되는 " END) END, CASE T3.TYPE WHEN ' . TARGET_CHR . ' THEN "인물" ELSE "배경" END, "(", T3.NAME, ")에 대한 디자인") FROM TREFERENCE, TDESIGN AS T1, TSCENE AS T2, TTARGET AS T3 WHERE (T1.SCENE_ID = 0 OR (' . ('T1.SCENE_ID = ' . join(' OR T1.SCENE_ID = ', $aRefSceneId)) . ')) AND (CITE_TYPE = ' . CITE_DESIGN . ' AND CITE_ID = DESIGN_ID) AND T1.SCENE_ID = T2.SCENE_ID AND T1.TARGET_ID = T3.TARGET_ID';
//		echo $query . "\n";
		if (!($resultRef = mysql_query($query))) {
			$message = '';
			$message .= 'errno: |' . mysql_errno() . '|' . "\n";
			$message .= 'error: |' . mysql_error() . '|' . "\n";
			$message .= 'query: |' . $query . '|' . "\n";
			die($message);
		}
		while ($rowRefId = mysql_fetch_array($resultRef, MYSQL_ASSOC)) {
			$refId = $rowRefId["REF_ID"];
			$citeType = $rowRefId["CITE_TYPE"];
			$citeId = $rowRefId["CITE_ID"];

			// TREFERENCE_SERIES테이블에서 REF_ID에 해당하는 참고자료에 대한 정보를 얻는다.
			if ($citeType == CITE_DESIGN) {
				$query = 'SELECT REVISION AS SERIAL, ' . REF_IMAGE . ' AS TYPE, CONCAT(FILENAME_UUID, ":", FILENAME) AS CONTENT, T2.USER_ID, T3.NICKNAME FROM TDESIGN AS T1, TWORK_IMAGE AS T2, TUSER AS T3 WHERE T1.DESIGN_ID = ' . $citeId . ' AND T1.IMAGE_ID = T2.IMAGE_ID AND T2.USER_ID = T3.USER_ID ORDER BY T2.USER_ID, REVISION';
				$citeId = $rowRefId["SCENE_ID"];
			} else {
				$query = 'SELECT SERIAL, TYPE, CONTENT, USER_ID, "NOT USED" AS NICKNAME FROM TREFERENCE_SERIES WHERE REF_ID = ' . $refId . ' AND USER_ID = ' . $gUserId . ' ORDER BY SERIAL';
			}
//			echo $query . "\n";
			if (!($resultRefSeries = mysql_query($query))) {
				$message = '';
				$message .= 'errno: |' . mysql_errno() . '|' . "\n";
				$message .= 'error: |' . mysql_error() . '|' . "\n";
				$message .= 'query: |' . $query . '|' . "\n";
				die($message);
			}
			$aRefSeries = array();
			while ($rowRefSeries = mysql_fetch_array($resultRefSeries, MYSQL_ASSOC)) {
//			new objRefSeries(1, cREF_IMAGE, "x.tif"),
				array_push($aRefSeries,
					"\n" .
					"\t\t\t" . 'new objRefSeries(' .
						$rowRefSeries["SERIAL"] . ', ' .
						($rowRefSeries["TYPE"] == REF_IMAGE ? 'cREF_IMAGE' : 'cREF_HTML') . ', ' .
						('"' . preg_replace('/\r\n/', '\\\\n', preg_replace('/([\\\"])/', "\\\\$1", $rowRefSeries["CONTENT"])) . '"') . ', ' .
						$rowRefSeries["USER_ID"] . ', "' . $rowRefSeries["NICKNAME"] . '"' .
					')'
				);
			}
			// 사용한 결과를 반환한다.
			mysql_free_result($resultRefSeries);

			// 디자인 작업이 참고자료인 경우,
			// 디자인 작업의 결과가 있을 때에만 참고자료로 사용된다.
			if ($citeType == CITE_DESIGN && count($aRefSeries) == 0) {
				continue;
			}
			array_push($aRef,
				"\n" .
				"\t" . 'new objRef(' . "\n" .
				"\t\t" . $refId . ',' . "\n" .
				"\t\t" . ($citeType == CITE_SHOW ? 'cCITE_SHOW' : ($citeType == CITE_SCENE ? 'cCITE_SCENE' : ($citeType == CITE_TARGET ? 'cCITE_TARGET' : ($citeType == CITE_DESIGN ? 'cCITE_DESIGN' : 'cCITE_STORYBOARD')))) . ',' . "\n" .
				"\t\t" . $citeId . ',' . "\n" .
				"\t\t" . ('"' . $rowRefId["NAME"] . '"') . ',' . "\n" .
				"\t\t" . '[' . join(',', $aRefSeries) . "\n" .
				"\t\t" . ']' . "\n" .
				"\t" . ')'
			);
		}
		// 사용한 결과를 반환한다.
		mysql_free_result($resultRef);
//		print_r($aRef);
		// 2. $aRef를 이용해서 gaRef배열을 만든다.
		$JavaScriptEvalCode .= "\n" .
			'gaRef = [' . join(',', $aRef) . "\n" .
			'];' . "\n";

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
<TITLE>fmStoryboard.PHP</TITLE>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; CHARSET=euc-kr">
<SCRIPT LANGUAGE="JavaScript1.3">
/*
	try {
//		alert(fdajsf.dsfkaj);
		throw "asdf";
	} catch(e) {
		if (typeof(e) == "object") {
			for (i in e) {
				alert('|' + i + '|: |' + e[i] + '|');
			}
		} else {
			alert('Exception Throw Message: |' + e + '|');
		}
	}
 */
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/12/8 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 이벤트핸들러를 통해 SELECT의 목록이 보이거나 보이지않게 한다.
 */

	var	gTabFlag = 0;
	// 플래그가 설정되어 있으면 해당 탭이 닫혀있음을 뜻한다.
	var	cTAB_SHOW	= 1 << 0;
	var	cTAB_REF	= 1 << 1;
	var	cTAB_STORYBOARD	= 1 << 2;
	var	cTAB_CUT	= 1 << 3;
	function fnTdOnClick(oTd) {
		var	tdId = oTd.id;
		var	nameUpperCase = tdId.slice(2).toUpperCase();
		eval('gTabFlag ^= cTAB_' + nameUpperCase);		// Toggle한다.
		_fnSetTab();
		return;
	}
	function _fnSetTab() {
		var	aTabName = {
			Show: "SHOW",
			Ref: "REF",
			Storyboard: "STORYBOARD",
			Cut: "CUT"
		};	// 오브젝트
		for (tabName in aTabName) {
			eval('var bTabFlag = gTabFlag & cTAB_' + aTabName[tabName]);
			eval('slt' + tabName + '.style.display = bTabFlag ? "none" : "block"');
			eval('td' + tabName + '.style.backgroundColor = bTabFlag ? "#95cf81" : "#fffbf0"');
			eval('td' + tabName + '.title = bTabFlag ? slt' + tabName + '.options[slt' + tabName + '.selectedIndex].text : ""');
		}
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/12/8 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * "컷 수정"을 구현한다.
 */

	// "컷 수정"에서 각 행에 대한 정보를 담고 있다.
	function objCutInEditCut(cutId, serial, rowIndex) {
		this.cutId = cutId;		// TCUT DB테이블의 CUT_ID이다. 새로 만든 컷이라면 0이다.
		this.serial = serial;		// TCUT DB테이블의 SERIAL값이다. 새로 만든 컷이라면 0이다.
		this.rowIndex = rowIndex;	// tblEditCut테이블에서 몇 번째 행에 위치하는 지를 말해준다.
		this.bDeleted = false;		// serial이 0이 아닐 때에만 의미가 있다.
	}
	var	gaCutInEditCut;	// "컷 수정"에서 사용되는, objCutInEditCut으로 이루어진 배열
				// "컷 수정"에서의 모든 조작은 이 배열에서 이루어진다.
	var	gbEditCut;	// "컷 수정"을 열었는 지 닫았는 지를 알 수 있다.
	var	gRowIndexInEditCut;	// "컷 수정"에서 몇 번째 컷이 CHECKED로 되어있는 지를 말해준다.
	function fnEditCutSubmit() {
		// 데이터를 서버로 보내는 데 필요한 FORM을 만든다.
		var	oEditCutSubmitForm = document.createElement('<FORM METHOD="POST" ACTION="fmStoryboard.PHP">');
		document.appendChild(oEditCutSubmitForm);

		// Submit Type:
		var	oSubmitType = document.createElement('<INPUT TYPE="TEXT" NAME="SubmitType">');
		oSubmitType.value = cSUBMIT_EDITCUT;
		oEditCutSubmitForm.appendChild(oSubmitType);
		// Storyboard Id:
		var	oStoryboardId = document.createElement('<INPUT TYPE="TEXT" NAME="StoryboardID">');
		oStoryboardId.value = gStoryboard.storyboardId;
		oEditCutSubmitForm.appendChild(oStoryboardId);
		// Cut Id, Serial:
		var	oCutIdSerial = document.createElement('<INPUT TYPE="TEXT" NAME="CutIdSerial">');
		var	aCutIdSerial = [];	// 추가: "0:SERIAL", 삭제: "CUT_ID:0", 변경: "CUT_ID:SERIAL"
		// TCUT DB테이블에 반영할 때에는 (삭제 -> 변경 -> 추가)의 순서로 해야한다.
		// 왜냐하면 이렇게 해야 일련번호가 중복되지 않기 때문이다.
		// 문제는 변경할 때이다.
		// 일련번호가 서로 엉켜있을 경우에는 SERIAL이 중복되므로 곧 바로 변경할 수 없다.
		var	aSerial = [];		//
		for (var i = 0; i < gaCutInEditCut.length; ++i) {
			var	oCut = gaCutInEditCut[i];
			if (oCut.serial && !oCut.bDeleted) {
				aSerial[oCut.serial] = 1;
			}
		}
/*
		var	a = [];
		for (var serial in aSerial) {
			a.push(serial);
		}
		alert(a);
 */
		// 중복없이 안전하게 사용할 수 있는 SERIAL번호이다.
		// 현재 할당된 가장 큰 일련번호보다 1만큼 크다.
		nextSerial = gStoryboard.aCut[gStoryboard.aCut.length - 1].serial + 1;
		var	aCutIdTmpTo = [];	// aCutidTmpTo[CUT_ID] = [TmpSerial, ToSerial]
		for (var i = 0; i < gaCutInEditCut.length; ++i) {
/*
			// gaCutInEditCut은 fnEditCut함수에서 rowIndex에 따라 정렬이 되어있다.
			// 정력규칙은 fnCompareFunctionByRowIndex함수를 보라.
 */
			var	oCut = gaCutInEditCut[i];
			var	toSerial = 1 + oCut.rowIndex;	// 바꾸고자 하는 일련번호이다.
			if (oCut.serial == 0) {		// 새로운 컷이다.
				aCutIdSerial.push("0:" + toSerial);
							// 의미: 새로운 컷(0)의 일련번호(SERIAL)는 toSerial이다.
			} else {
				var	cutId = oCut.cutId;
				var	fromSerial = oCut.serial;	// DB에서 이 CUT_ID에 할당된 일련번호이다.
				if (oCut.bDeleted) {
					aCutIdSerial.push(cutId + ':0');
							// 의미: 기존 컷(cutId)을 삭제(0)한다.
				} else if (fromSerial != toSerial) {	// 일련번호를 변경해야한다.
					// fromSerial에 해당하는 일련번호는 바꿀 것이므로 사용할 수 있다.
					delete aSerial[fromSerial];
					if (aSerial[toSerial] === undefined) {	// 중복되지 않는다.
						aCutIdSerial.push(oCut.cutId + ':' + toSerial);
								// 의미: 기존 컷(cutId)의 일련번호를 toSerial로 바꾼다.
						// toSerial에 해당하는 일련번호는 사용 중이다.
						aSerial[toSerial] = 1;
					} else {	// toSerial에 해당하는 일련번호가 중복된다.
						// 우선 임시로 사용하지 않는 일련번호로 바꾼다.
						aCutIdSerial.push(cutId + ':' + nextSerial);
						// nextSerial에 해당하는 일련번호는 사용 중이다.
						aSerial[nextSerial] = 1;
						// 나중에 원래대로 원하는 일련번호로 바뀌기 이 상황을 기억한다.
						aCutIdTmpTo[cutId] = [nextSerial, toSerial];
						++nextSerial;
					}
				}
			}
		}
/*
		var	a = [];
		for (var serial in aSerial) {
			a.push(serial);
		}
		alert(a);
 */
		for (var cutId in aCutIdTmpTo) {
			var	tmpSerial = aCutIdTmpTo[cutId][0];
			var	toSerial = aCutIdTmpTo[cutId][1];
//			alert('CUT_ID = ' + cutId + ': ' + aCutIdTmpTo[cutId][0] + ' -> ' + aCutIdTmpTo[cutId][1]);
			if (aSerial[toSerial] === undefined) {
				// 이제는 원하는 일련번호로 바꿀 수 있다.
				aCutIdSerial.push(cutId + ':' + toSerial);
				// 임시 일련번호는 사용할 수 있다.
				delete aSerial[tmpSerial];
			} else {
				alert('알고리즘에 문제가 있습니다: 프로그래머를 찾으세요');
			}
		}
		oCutIdSerial.value = aCutIdSerial.join(',');
		oEditCutSubmitForm.appendChild(oCutIdSerial);

		// 모은 데이터를 서버로 보낸다.
		oEditCutSubmitForm.submit();
	}
	// btnEditCut버튼을 눌렀을 때(onClick) 호출되는 이벤트핸들러이다.
	function fnEditCut() {
		// btnEditCut버튼이 tdCut안에 있고,
		// tdCut은 onClick이벤트에 반응하도록 되어있으므로,
		// 이것은 btnEditCut버튼을 눌렀을 때 원하는 것이 아니다.
		// 따라서 더 이상의 onClick가 전달되지 않도록한다.
		window.event.cancelBubble = true;
		if (gbEditCut) {
/*
			// gaCutInEditCut배열을 rowIndex에 따라 정렬한다.
			gaCutInEditCut.sort(fnCompareFunctionByRowIndex);
 */
			var	bEditCutSubmit = false;	// "컷 수정"에서 어떤 변화가 있었는 지를 말해준다.
			for (var i = 0; i < gaCutInEditCut.length; ++i) {
				var	oCut = gaCutInEditCut[i];
if (1) {
				if (
					oCut.serial == 0 ||
					oCut.bDeleted ||
					oCut.serial != (1 + oCut.rowIndex)
				) {
					bEditCutSubmit = true;
				}
} else {
				var	msg;
				if (oCut.serial == 0) {	// 새로 추가한 컷이다.
					bEditCutSubmit = 1;
					msg = 'New';
				} else {
					if (oCut.bDeleted) {	// 삭제한 컷이다.
						bEditCutSubmit = 1;
						msg = 'From DB: SERIAL = ' + oCut.serial + ': Deleted';
					} else if (oCut.serial == (1 + oCut.rowIndex)) {	// 변화가 없는 컷이다.
						msg = 'From DB: SERIAL = ' + oCut.serial + ': Unchanged';
					} else {	// 일련번호에 변화가 있다.
						bEditCutSubmit = 1;
						msg = 'From DB: SERIAL = ' + oCut.serial + ' -> ' + (1 + oCut.rowIndex) + ': Changed';
					}
				}
				alert('gaCutInEditCut[' + i + ']: rowIndex = ' + oCut.rowIndex + ' (' + msg + ')');
}
			}

			tblCut.deleteRow(0);

			gbEditCut = 0;
			btnEditCut.value = '"컷 수정" 열기';

			if (bEditCutSubmit) {	// "컷 수정"에서 어떤 변화가 있었다.
				var	msg;
				do {
					msg = '"컷 수정"에서 어떤 변화가 있었습니다.' + '\n';
					msg += 'DB에도 반영하겠습니까?';
					if (confirm(msg)) {		// DB에 반영한다.
						fnEditCutSubmit();
						break;
					}
					msg = '"컷 수정"에서 어떤 변화가 있었는데도 "정말로" DB에 반영하지 않겠습니까?';
				} while (!confirm(msg));
			}

			gaCutInEditCut = undefined;
			return;
		}

		gbEditCut = 1;
		var	oTr = tblCut.insertRow(0);
		if (!oTr) {
			alert('tblCut.insertRow(): Failure');
			return false;
		}

		var	oTd = oTr.insertCell();
		if (!oTd) {
			alert('.insertCell: Failure');
			tblCut.deleteRow(oTr.rowIndex);
			return false;
		}
		oTd.align = "CENTER";
		oTd.width = "100%";

		gRowIndexInEditCut = giCut;

		var	innerHTML = '';
		innerHTML = '<TABLE ID="tblEditCut" WIDTH="80%" BORDER="1" CELLSPACING="0" CELLPADDING="0"><TBODY>';
		for (var i = 0; i < gStoryboard.aCut.length; ++i) {
			innerHTML += '<TR><TD WIDTH="100%">&nbsp;&nbsp;<INPUT TYPE="RADIO" NAME="rdoiEditCut" onClick="gRowIndexInEditCut = this.parentElement.parentElement.rowIndex; fnSetEditCutButtonsStatus();">&nbsp;&nbsp;&nbsp;컷 #' + (1 + i) + ' (리비젼 갯수 = ' + gStoryboard.aCut[i].aImage.length + ')</TD></TR>';
		}
		innerHTML += '<TR><TD ALIGN="CENTER">';
		innerHTML += '<INPUT ID="btnInsertCut" TYPE="BUTTON" VALUE="추가" onClick="fnInsertCut();">&nbsp;&nbsp;';
		innerHTML += '<INPUT ID="btnDeleteCut" TYPE="BUTTON" VALUE="삭제" onClick="fnDeleteCut();">&nbsp;&nbsp;';
		innerHTML += '<INPUT ID="btnMoveUpCut" TYPE="BUTTON" VALUE="↑" onClick="fnMoveCut(-1);">&nbsp;&nbsp;';
		innerHTML += '<INPUT ID="btnMoveDownCut" TYPE="BUTTON" VALUE="↓" onClick="fnMoveCut(+1);">';
		innerHTML += '</TD></TR>';
		innerHTML += '</TBODY></TABLE>';
		oTd.innerHTML = innerHTML;

		// gaCutInEditCut배열을 설정한다.
		gaCutInEditCut = [];
		for (var i = 0; i < gStoryboard.aCut.length; ++i) {
			gaCutInEditCut[i] = new objCutInEditCut(gStoryboard.aCut[i].cutId, gStoryboard.aCut[i].serial, i);
		}

		fnSetEditCutButtonsStatus();
		btnEditCut.value = '"컷 수정" 닫기';
	}
	function fnSetEditCutButtonsStatus() {
		// gRowIndexInEditCut에 해당하는 행에서 RADIO버튼을 CHECKED상태로 만든다.
		tblEditCut.rows[gRowIndexInEditCut].children[0].children[0].checked = true;

		if (tblEditCut.rows.length - 1 == 1) {		// "추가", "삭제", ...행은 제외한다.
			btnDeleteCut.title = "적어도 하나의 컷은 있어야 한다";
			btnDeleteCut.disabled = true;
			btnMoveUpCut.disabled = true;
			btnMoveDownCut.disabled = true;
			return;
		}
		btnDeleteCut.title = "";
		btnDeleteCut.disabled = false;

		btnMoveUpCut.disabled = gRowIndexInEditCut == 0;
		btnMoveDownCut.disabled = (1 + gRowIndexInEditCut) == tblEditCut.rows.length - 1;	// "추가", "삭제", ...행은 제외한다.
	}
	function fnInsertCut() {
		// gaCutInEditCut배열의 해당하는 위치에,
		// 새로운 컷에 대한 정보를 담은 objCutInEditCut오브젝트를 넣는다.
		var	index = fnGetiCutInEditCutBy(gRowIndexInEditCut);
		// 먼저, gaCutInEditCut배열에서 rowIndex값이 gRowIndexInEditCut보가 크거나 같은 모든 성분에서
		// rowIndex값을 1씩 증가시킨다.
		for (var i = 0; i < gaCutInEditCut.length; ++i) {
			var	oCut = gaCutInEditCut[i];
			if (oCut.bDeleted == false && oCut.rowIndex >= gRowIndexInEditCut) {
				++oCut.rowIndex;
			}
		}
		gaCutInEditCut.splice(index, 0, new objCutInEditCut(0, 0, gRowIndexInEditCut));

		// tblEditCut테이블의 해당하는 위치에 새로운 컷에 대한 행을 만든다.
		var	oTr = tblEditCut.insertRow(gRowIndexInEditCut);
		if (!oTr) {
			alert('tblEditCut.insertRow(): Failure');
			return false;
		}

		var	oTd = oTr.insertCell();
		if (!oTd) {
			alert('.insertCell: Failure');
			tblCut.deleteRow(oTr.rowIndex);
			return false;
		}
		oTd.align = "LEFT";
		oTd.innerHTML = '&nbsp;&nbsp;<INPUT TYPE="RADIO" NAME="rdoiEditCut" onClick="gRowIndexInEditCut = this.parentElement.parentElement.rowIndex; fnSetEditCutButtonsStatus();">&nbsp;&nbsp;&nbsp;새로운 컷';

		fnSetEditCutButtonsStatus();
	}
	function fnDeleteCut() {
		// gaCutInEditCut배열에서 해당하는 성분을 삭제하거나(새로 만든 컷인 경우),
		// 지웠다는 표시를 한다.(DB에서 가져온 컷인 경우)
		var	index = fnGetiCutInEditCutBy(gRowIndexInEditCut);
		if (gaCutInEditCut[index].serial == 0) {	// 새로 만든 컷인 경우
			gaCutInEditCut.splice(index, 1);
		} else {
			gaCutInEditCut[index].bDeleted = true;
			gaCutInEditCut[index].rowIndex = undefined;
		}

		// gaCutInEditCut배열에서 rowIndex값이 gRowIndexInEditCut보가 큰 모든 성분에서
		// rowIndex값을 1씩 감소시킨다.
		for (var i = 0; i < gaCutInEditCut.length; ++i) {
			var	oCut = gaCutInEditCut[i];
			if (oCut.bDeleted == false && oCut.rowIndex > gRowIndexInEditCut) {
				--oCut.rowIndex;
			}
		}

		// tblEditCut테이블에서 해당하는 행을 지운다.
		tblEditCut.deleteRow(gRowIndexInEditCut);
		if (gRowIndexInEditCut == tblEditCut.rows.length - 1) {		// "추가", "삭제", ...행은 제외한다.
			// 제일 마지막 행을 가리키고 있었다면,
			// gRowIndexInEditCut은 다시 제일 마지막 행을 가리키도록 한다.
			--gRowIndexInEditCut;
		}

		fnSetEditCutButtonsStatus();
	}
	function fnMoveCut(by) {	// by = +1: MoveDown, -1: MoveUp
		var	newRowIndex = gRowIndexInEditCut + by;

		// gaCutInEditCut배열에서 해당하는 성분의 rowIndex값을 바꾼다.
		var	index = fnGetiCutInEditCutBy(gRowIndexInEditCut);
		var	newIndex = fnGetiCutInEditCutBy(newRowIndex);
		gaCutInEditCut[index].rowIndex = newRowIndex;
		gaCutInEditCut[newIndex].rowIndex = gRowIndexInEditCut;

		// tblEditCut테이블에서 해당하는 행을 바꾼다.
		tblEditCut.moveRow(gRowIndexInEditCut, newRowIndex);

		// gRowIndexInEditCut값을 새로운 값으로 바꾼다.
		gRowIndexInEditCut = newRowIndex;

		fnSetEditCutButtonsStatus();
	}
	function fnGetiCutInEditCutBy(rowIndex) {
		for (var i = 0; i < gaCutInEditCut.length; ++i) {
			var	oCut = gaCutInEditCut[i];
			if (oCut.bDeleted == false && oCut.rowIndex == rowIndex) {
				return i;
			}
		}
		return -1;
	}
	function fnCompareFunctionByRowIndex(aCut, bCut) {
		var	aRowIndex = aCut.rowIndex;
		var	bRowIndex = bCut.rowIndex;

		if (aRowIndex === undefined && bRowIndex === undefined) {
			// 두 컷 모두 DB에서 가져왔으니 "컷 수정"에서 모두 삭제되었다.
			// 순서에는 변화가 없다.
			return 0;
		}
		if (aCut.rowIndex === undefined) {
			// 첫 번째 컷만 삭제되었다.
			// 첫 번째 컷을 뒤로 보낸다.
			return 1;
		}
		if (bCut.rowIndex === undefined) {
			// 두 번째 컷만 삭제되었다.
			// 두 번째 컷을 뒤로 보낸다.
			return -1;
		}

		return aCut.rowIndex - bCut.rowIndex;
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/12/2 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * USER_ID를 기억한다.
 */

	var	gUserId;
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/29 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 참고자료나 디자인 작업의 결과를 서버롤 보내는 코드를 구현한다.
 */

	// 지금 보고 있는 참고자료를 삭제한다.
	function fnDeleteRef() {
		// 다시 한 번 확인한다.
		var	aRefSeries = gRef.citeType == cCITE_DESIGN ? gRef.aRefSeriesByUserId[gDesignUserId] : gRef.aRefSeries;
		var	oRefSeries = aRefSeries[giSer];
		var	type = oRefSeries.type;
		var	msg = '';
		msg += '다음 참고자료(' + (type == cREF_IMAGE ? '그림' : '문서') + ')를 정말로 삭제해도 됩니까?' + '\n';
		msg += '\n';
		var	content = oRefSeries.content;
		if (oRefSeries.type == cREF_IMAGE) {
			var	a = content.split(':');
			msg += a[1];
		} else {
			msg += content;
		}
		if (!confirm(msg)) {
			return;
		}

		// 데이터를 서버로 보내는 데 필요한 FORM을 만든다.
		var	oRefSubmitForm = document.createElement('<FORM METHOD="POST" ACTION="fmStoryboard.PHP" ENCTYPE="MULTIPART/FORM-DATA">');
		document.appendChild(oRefSubmitForm);

		// Submit Type:
		var	oSubmitType = document.createElement('<INPUT TYPE="TEXT" NAME="SubmitType">');
		oSubmitType.value = cSUBMIT_REF;
		oRefSubmitForm.appendChild(oSubmitType);
		// Ref ID:
		var	oRefId = document.createElement('<INPUT TYPE="TEXT" NAME="RefID">');
		oRefId.value = gRef.refId;
		oRefSubmitForm.appendChild(oRefId);
		// Serial #:
		var	nSerial = gRef.aRefSeries.length;
		var	oSerial = document.createElement('<INPUT TYPE="TEXT" NAME="Serial">');
		oSerial.value = gRef.aRefSeries[giSer].serial;
		oRefSubmitForm.appendChild(oSerial);

		// 모은 데이터를 서버로 보낸다.
		oRefSubmitForm.submit();
	}

	// 제일 마지막에 참고자료를 추가한다.
	function fnRefSubmit() {
		var	refType = idRefTypeImage.checked ? cREF_IMAGE : cREF_HTML;
		var	oRefImageOrHtml = refType == cREF_IMAGE ? idRefImage : idRefHtml;
		if (oRefImageOrHtml.value == "") {
			alert(refType == cREF_IMAGE ? '참고할 그림 파일을 선택해야 합니다' : '참고할 내용을 적어야 합니다');
			return;
		}
		// 데이터를 서버로 보내는 데 필요한 FORM을 만든다.
		var	oRefSubmitForm = document.createElement('<FORM METHOD="POST" ACTION="fmStoryboard.PHP" ENCTYPE="MULTIPART/FORM-DATA">');
		document.appendChild(oRefSubmitForm);

		// Submit Type:
		var	oSubmitType = document.createElement('<INPUT TYPE="TEXT" NAME="SubmitType">');
		oSubmitType.value = cSUBMIT_REF;
		oRefSubmitForm.appendChild(oSubmitType);
		// Ref ID:
		var	oRefId = document.createElement('<INPUT TYPE="TEXT" NAME="RefID">');
		oRefId.value = gRef.refId;
		oRefSubmitForm.appendChild(oRefId);
		// Serial #:
		var	nSerial = gRef.aRefSeries.length;
		var	oSerial = document.createElement('<INPUT TYPE="TEXT" NAME="Serial">');
		oSerial.value = nSerial ? (gRef.aRefSeries[nSerial - 1].serial + 1) : 1;
		oRefSubmitForm.appendChild(oSerial);
		// Image Or HTML:
		oRefSubmitForm.appendChild(refType == cREF_IMAGE ? idRefImage : idRefHtml);

		// 모은 데이터를 서버로 보낸다.
		oRefSubmitForm.submit();
	}

	// 컷 작업의 현재 리비젼을 삭제한다.
	function fnDeleteCutImage() {
		// 다시 한 번 확인한다.
		var	msg = '';
		msg += '컷의 다음 리비젼을 정말로 삭제해도 됩니까?' + '\n';
		msg += '\n';
		msg += gCut.aImage[giRev].filename;
		if (!confirm(msg)) {
			return;
		}

		// 데이터를 서버로 보내는 데 필요한 FORM을 만든다.
		var	oCutSubmitForm = document.createElement('<FORM METHOD="POST" ACTION="fmStoryboard.PHP" ENCTYPE="MULTIPART/FORM-DATA">');
		document.appendChild(oCutSubmitForm);

		// Submit Type:
		var	oSubmitType = document.createElement('<INPUT TYPE="TEXT" NAME="SubmitType">');
		oSubmitType.value = cSUBMIT_WORK;
		oCutSubmitForm.appendChild(oSubmitType);
		// Image ID:
		var	oImageId = document.createElement('<INPUT TYPE="TEXT" NAME="ImageID">');
		oImageId.value = gCut.imageId;
		oCutSubmitForm.appendChild(oImageId);
		// Revision #:
		var	oRev = document.createElement('<INPUT TYPE="TEXT" NAME="Revision">');
		oRev.value = gCut.aImage[giRev].rev;
		oCutSubmitForm.appendChild(oRev);

		// 모은 데이터를 서버로 보낸다.
		oCutSubmitForm.submit();
	}

	// 컷 작업의 결과 이미지를 전송한다.
	function fnCutSubmit() {
		if (idCutImage.value == "") {
			alert('작업 그림 파일을 선택해야 합니다');
			return;
		}

		// 데이터를 서버로 보내는 데 필요한 FORM을 만든다.
		var	oCutSubmitForm = document.createElement('<FORM METHOD="POST" ACTION="fmStoryboard.PHP" ENCTYPE="MULTIPART/FORM-DATA">');
		document.appendChild(oCutSubmitForm);

		// Submit Type:
		var	oSubmitType = document.createElement('<INPUT TYPE="TEXT" NAME="SubmitType">');
		oSubmitType.value = cSUBMIT_WORK;
		oCutSubmitForm.appendChild(oSubmitType);
		// Cut ID:		// IMAGE_ID = 0일 때, IMAGE_ID를 새로 등록하고,
					// 이 CUT_ID의 IMAGE_ID를 갱신할 때 사용된다.
		var	oCutId = document.createElement('<INPUT TYPE="TEXT" NAME="CutID">');
		oCutId.value = gCut.cutId;
		oCutSubmitForm.appendChild(oCutId);
		// Image ID:
		var	oImageId = document.createElement('<INPUT TYPE="TEXT" NAME="ImageID">');
		oImageId.value = gCut.imageId;
		oCutSubmitForm.appendChild(oImageId);
		// Revision #:
		var	oRev = document.createElement('<INPUT TYPE="TEXT" NAME="Revision">');
		var	nRev = gCut.aImage.length;
		oRev.value = nRev ? (gCut.aImage[nRev - 1].rev + 1) : 1;
		oCutSubmitForm.appendChild(oRev);
		// Comment(관리자) Or Image Filename(작업자):
		oCutSubmitForm.appendChild(idCutImage);

		// 모은 데이터를 서버로 보낸다.
		oCutSubmitForm.submit();
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/28 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 참고자료(gRef)와 스코리보드 작업의 컷(gCut)에 대한 정보를 표시하는 공통으로 사용되는 상수와 변수들이다.
 */

	var	cPANEL_HEIGHT	= 300;	// 300픽셀
	var	cIMG_BG = "_bg";
	var	cIMG_DESC = "_desc";
	var	cIMG_HTML = "_html";
	var	cIMG_VOID = "_void";

	var	cBTN_ADD	= 1 << 0;
	var	cBTN_THUMBNAIL	= 1 << 1;

	var	giSer;		// gRef중에서 몇 번째의 참고자료를 보여줄 지를 결정한다.
	var	gRefFlag;	// "ADD"나 "THUMBNAIL"창이 활성될 지를 결정한다.

	var	gcTrbOrder;	// "지시사항/요구사항"행이 차지하는 높이
	var	gcTrStatusHeight;	// "진행상황"행이 차지하는 높이
	var	giRev;		// gCut중에서 몇 번째 그림을 보여줄 지를 결정한다.
	var	gCutFlag;	// "ADD"나 "THUMBNAIL"창이 활성될 지를 결정한다.
	var	gbOrder;	// "지시사항"(= 1)을 표시할 것 인지, "요구사항"(= 0)을 표시할 것 인지를 결정한다.

	// 주어진 TABLE오브젝트(oTbl)에서 ID가 id인 행의 인덱스를 구한다.
	// 리턴값:
	//	> 0: 성공, -1: 실패
	function fnFindRow(oTbl, title) {
		for (var i = 0; i < oTbl.rows.length; ++i) {
			if (oTbl.rows(i).title == title) {
				return i;
			}
		}
		return -1;
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/28 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 스토리보드 작업의 컷(gCut) 정보를 표시하는 코드를 구현한다.
 */

	// gCut에 해당하는 스토리보드 작업의 컷 정보를 표시한다.
	function fnDisplayCut() {
		// gCut의 aImage중에서 giRev값에 해당하는 그림을 tdCutImage에 표시한다.
		// gCutFlag에 따라 필요한 것들을 tblCutFreeBoard에 표시한다.
		if (giRev < 0 || gCut.aImage.length <= giRev) {	// 유효하지 않은 giSer이다.
			// (정책) 제일 마지막을 가리키도록 한다.
			giRev = gCut.aImage.length - 1;
		}

		if (giRev == -1) {	// 보여줄 그림이 없다.
			gbOrder = 1;
			sltbOrder.selectedIndex = 0;
			sltbOrder.disabled = true;

			btnCutDelete.disabled = true;

			tdCutImage.innerHTML = '&nbsp;';

			btnCutBack.disabled = true;
			btnCutForward.disabled = true;

			btnCutThumbnail.disabled = true;
			gCutFlag &= ~cBTN_THUMBNAIL;
			_fnCutThumbnail(false);

			tdCutImageDownload.innerHTML = "&nbsp;";
		} else {
			sltbOrder.disabled = false;

			btnCutDelete.disabled = false;

			fnDisplayCutImage();

			btnCutThumbnail.disabled = false;
			if (gCutFlag & cBTN_THUMBNAIL) {
				if (!_fnCutThumbnail(true)) {
					alert('_fnCutThumbnail(): Failure');
				}
			}
		}

		if (gCutFlag & cBTN_ADD) {
			if (!_fnCutAdd(true)) {
				alert('_fnCutAdd(): Failure');
			}
		}
	}
	// gCut의 aImage중에서 giRev값에 해당하는 그림을 tdCutImage에 표시한다.
	function fnDisplayCutImage() {
		var	oImage = gCut.aImage[giRev];
//		alert(oImage.filenameUuid + ',' + oImage.filename);
		var	img = oImage.filenameUuid;
		var	alt = ' ALT="' + img + '.png' + '"';
		tdCutImage.align = "LEFT";//"CENTER";
		tdCutImage.vAlign = "MIDDLE";
		var	tblMainBorder = tblMain.border;
		tdCutImage.innerHTML = '<IMG SRC="/images/Low/Work/Storyboard/' + img + '.png" HEIGHT="' + cPANEL_HEIGHT + 'px" WIDTH="' + (tdCutImage.clientWidth - 2 * tblMainBorder) + 'px"' + alt + '>';
		fnDisplayOrderOrComment();

		btnCutBack.disabled = giRev == 0;
		btnCutForward.disabled = giRev == gCut.aImage.length - 1;

		tdCutImageDownload.innerHTML = '<A HREF="/images/High/Work/Storyboard/' + img + '" TITLE="' + oImage.filename + '">↓</A>';
	}
	// gbOrder에 따라 지시사항/요구사항을 표시한다.
	function fnDisplayOrderOrComment() {
		var	oImage = gCut.aImage[giRev];
		if (gbOrder) {
			// "지시사항"을 보여준다.
			sltbOrder.selectedIndex = 0;
			trStatus.style.display = "block";

			var	innerHTML = '';
			innerHTML += '<DIV STYLE="overflow-y: scroll; height: ' + (cPANEL_HEIGHT - gcTrbOrderHeight - gcTrStatusHeight) + 'px;">';
			innerHTML += gStoryboard.order;
			innerHTML += '</DIV>';
			tdOrderOrComment.innerHTML = innerHTML;
		} else {
			// "요구사항"을 보여준다.
			sltbOrder.selectedIndex = 1;
			trStatus.style.display = "none";

			var	innerHTML = '';
			innerHTML += '<DIV STYLE="overflow-y: scroll; height: ' + (cPANEL_HEIGHT - gcTrbOrderHeight) + ';">';
			innerHTML += oImage.comment;
			innerHTML += '</DIV>';
			tdOrderOrComment.innerHTML = innerHTML;
		}
	}

	function fnSltbOrderOnChange() {
		gbOrder = sltbOrder.selectedIndex ? 0 : 1;
		fnDisplayOrderOrComment();
	}

	function fnCutDirBtnPressed(iDir) {
		var	iRev = giRev + iDir;
		if (gCutFlag & cBTN_THUMBNAIL) {
			eval('tdCutThumbnailRev' + giRev + '.style.border = "solid 1px #66cc00"');
			eval('tdCutThumbnailRev' + iRev + '.style.border = "solid 1px #ff0000"');
		}
		giRev = iRev;
		fnDisplayCutImage();
	}

	function _fnCutThumbnail(bOpen) {
		// 전에 열렸던 행이 있다면 먼저 닫는다.
		// 일반적으로 우리가 원하는 행이 아니다.
		var	rowIndex = fnFindRow(tblCutFreeBoard, "trCutThumbnail");
		if (rowIndex >= 0) {
			tblCutFreeBoard.deleteRow(rowIndex);
		}

		if (!bOpen) {	// 닫아라.
			// 이미 닫았다.
			return rowIndex >= 0 ? true : false;
		}
		// bOpen = true: 열어라.

		var	oTr = tblCutFreeBoard.insertRow();
		if (!oTr) {
			alert('tblCutFreeBoard.insertRow(): Failure');
			return false;
		}
		oTr.title = "trCutThumbnail";

		var	oTd = oTr.insertCell();
		if (!oTd) {
			alert('.insertCell: Failure');
			tblCutFreeBoard.deleteRow(oTr.rowIndex);
			return false;
		}
		var	innerHTML = '';
		innerHTML += '<TABLE BORDER="0" CELLSPACING="1" CELLPADDING="0"><TBODY>';
		var	iRev = 0;
		var	nRev = gCut.aImage.length;
		for (var iRow = 0; iRow < Math.ceil(nRev / 4); ++iRow) {
			innerHTML += '<TR>';
			for (var iCol = 0; iCol < 4; ++iCol, ++iRev) {
				var	oImage = gCut.aImage[iRev];

				var	id;
				var	brdColor;
				var	img;
				var	alt;
				var	onClick;
				if (iRev < nRev) {
					id = ' ID="tdCutThumbnailRev' + iRev + '"';
					img = '/images/Thumbnail/Work/Storyboard/' + oImage.filenameUuid + '.png';
					alt = ' ALT="' + oImage.filename + '"';
					brdColor = iRev == giRev ? '#ff0000' : '#66cc00';
					onClick = ' onClick="fnCutThumbnailOnClick(' + iRev + ');"';
				} else {
					id = '';
					brdColor = '#ffffff';
					img = '/images/Thumbnail/' + cIMG_BG + '.jpg';
					alt = '';
					onClick = '';
				}
				innerHTML += '<TD' + id + ' STYLE="border: solid 1px ' + brdColor + ';">';
				innerHTML += '<IMG SRC="' + img + '" HEIGHT="50px" WIDTH="50px"' + alt + onClick + '>';
				innerHTML += '</TD>';
			}
			innerHTML += '</TR>';
		}
		innerHTML += '</TBODY></TABLE>';
		oTd.innerHTML = innerHTML;
		return true;
	}
	function fnToggleCutThumbnail() {
		gCutFlag ^= cBTN_THUMBNAIL;
		if (!_fnCutThumbnail(gCutFlag & cBTN_THUMBNAIL)) {
			alert('_fnCutThumbnail(): Failure');
		}
	}
	function fnCutThumbnailOnClick(iRev) {
		eval('tdCutThumbnailRev' + giRev + '.style.border = "solid 1px #66cc00"');
		eval('tdCutThumbnailRev' + iRev + '.style.border = "solid 1px #ff0000"');
		giRev = iRev;
		fnDisplayCutImage();
	}

	function _fnCutAdd(bOpen) {
		var	rowIndex = fnFindRow(tblCutFreeBoard, "trCutAdd");

		if (!bOpen) {	// 닫아라.
			if (rowIndex >= 0) {
				tblCutFreeBoard.deleteRow(rowIndex);
				return true;
			}
			// 해당하는 행을 찾을 수 없다.
			return false;
		}
		// bOpen = true: 열어라.
		if (rowIndex >= 0) {
			// 이미 열려있다.
			return true;
		}

		var	oTr = tblCutFreeBoard.insertRow();
		if (!oTr) {
			alert('tblCutFreeBoard.insertRow(): Failure');
			return false;
		}
		oTr.title = "trCutAdd";

		var	oTd = oTr.insertCell();
		if (!oTd) {
			alert('.insertCell: Failure');
			tblLtFreeBoard.deleteRow(oTr.rowIndex);
			return false;
		}
		var	innerHTML = '';
		innerHTML += '<TABLE BORDER="1" FRAME="BOX" RULES="NONE" CELLSPACING="0" CELLPADDING="0"><TBODY>';
		innerHTML += '<TR><TD><INPUT ID="idCutImage" TYPE="FILE" NAME="CutImage"></TD></TR>';
		innerHTML += '<TR><TD><INPUT TYPE="BUTTON" VALUE="더하기" onClick="fnCutSubmit();"></TD></TR>';
		innerHTML += '</TD></TR></TBODY></TABLE>';
		oTd.innerHTML = innerHTML;
		return true;
	}
	function fnToggleCutAdd() {
		gCutFlag ^= cBTN_ADD;
		if (!_fnCutAdd(gCutFlag & cBTN_ADD)) {
			alert('_fnCutAdd(): Failure');
		}
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/28 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 참고자료(gRef) 정보를 표시하는 코드를 구현한다.
 */

	// gRef에 해당하는 참고자료의 정보를 표시한다.
	function fnDisplayRef() {
		// gRef의 aRefSeries중에서 giSer값에 해당하는 참고자료를 tdRefContent에 표시한다.
		// gRefFlag에 따라 필요한 것들을 tblRefFreeBoard에 표시한다.
		var	aRefSeries = gRef.citeType == cCITE_DESIGN ? gRef.aRefSeriesByUserId[gDesignUserId] : gRef.aRefSeries;
		if (giSer < 0 || aRefSeries.length <= giSer) {	// 유효하지 않은 giSer이다.
			// (정책) 제일 마지막을 가리키도록 한다.
			giSer = aRefSeries.length - 1;
		}

		if (giSer == -1) {	// 보여줄 참고자료가 없다.
			btnRefDelete.disabled = true;

			tdRefContent.innerHTML = '&nbsp;';

			btnRefBack.disabled = true;
			btnRefForward.disabled = true;

			btnRefThumbnail.disabled = true;
			if (gRefFlag & cBTN_THUMBNAIL) {
				if (!_fnRefThumbnail(false)) {
					alert('_fnRefThumbnail(): Failure');
				}
				gRefFlag &= ~cBTN_THUMBNAIL;
			}

			tdRefImageDownload.innerHTML = "&nbsp;"
		} else {
			btnRefDelete.disabled = false;

			fnDisplayRefContent();

			btnRefThumbnail.disabled = false;
			if (gRefFlag & cBTN_THUMBNAIL) {
				if (!_fnRefThumbnail(true)) {
					alert('_fnRefThumbnail(): Failure');
				}
			}
		}

		if (gRef.citeType == cCITE_STORYBOARD) {
			btnRefAdd.disabled = false;
			btnRefAdd.title = '';
			if (gRefFlag & cBTN_ADD) {
				if (!_fnRefAdd(true)) {
					alert('_fnRefAdd(): Failure');
				}
			}

			if (giSer != -1)
				btnRefDelete.disabled = false;
			btnRefDelete.title = '';
		} else {
			btnRefAdd.disabled = true;
			btnRefAdd.title = '스토리보드 작업에 관련된 참고자료(파란색)만 등록할 수 있습니다';
			if (gRefFlag & cBTN_ADD) {
				if (!_fnRefAdd(false)) {
					alert('_fnRefAdd(): Failure');
				}
				gRefFlag &= ~cBTN_ADD;
			}

			btnRefDelete.disabled = true;
			btnRefDelete.title = '스토리보드 작업에 관련된 참고자료(파란색)만 삭제할 수 있습니다';
		}
	}
	// gRef의 aRefSeries(gRef.citeType != cCITE_DESIGN)또는 aRefSeriesByUserId[gDesignUserId] 중에서 giSer값에 해당하는 참고자료를 tdRefContent에 표시한다.
	function fnDisplayRefContent() {
		var	aRefSeries = gRef.citeType == cCITE_DESIGN ? gRef.aRefSeriesByUserId[gDesignUserId] : gRef.aRefSeries;
		var	oRefSeries = aRefSeries[giSer];
//		alert('TYPE: ' + (oRefSeries.type == cREF_IMAGE ? '그림' : oRefSeries.type == cREF_HTML ? 'HTML' : 'LINK') + ', CONTENT = "' + oRefSeries.content + '"');
		if (oRefSeries.type == cREF_HTML) {
			var	innerHTML = '';
			innerHTML += '<DIV STYLE="overflow-y: scroll; height=' + cPANEL_HEIGHT + 'px;">';
			//
			innerHTML += oRefSeries.content;
			//
			innerHTML += '</DIV>';
			tdRefContent.align = "LEFT";
			tdRefContent.vAlign = "TOP";
			tdRefContent.innerHTML = innerHTML;

			tdRefImageDownload.innerHTML = "&nbsp;"
		} else {
			var	aFilename = oRefSeries.content.split(':');
			var	img = aFilename[0];
			var	alt = ' ALT="' + img + '.png' + '"';
			tdRefContent.align = "LEFT";//"CENTER";
			tdRefContent.vAlign = "MIDDLE";
			tdRefContent.innerHTML = '<IMG SRC="/images/Low/' + (gRef.citeType == cCITE_DESIGN ? 'Work/Design' : 'Ref') + '/' + img + '.png" HEIGHT="' + cPANEL_HEIGHT + 'px" WIDTH="' + (tdRefContent.clientWidth - 2 * tblMain.border) + 'px"' + alt + '>';

			tdRefImageDownload.innerHTML = '<A HREF="/images/High/' + (gRef.citeType == cCITE_DESIGN ? 'Work/Design' : 'Ref') + '/' + img + '" TITLE="' + aFilename[1] + '">↓</A>';
		}

		btnRefBack.disabled = giSer == 0;
		btnRefForward.disabled = giSer == gRef.aRefSeries.length - 1;
	}

	function fnRefDirBtnPressed(iDir) {
		var	iSer = giSer + iDir;
		if (gRefFlag & cBTN_THUMBNAIL) {
			eval('tdRefThumbnailRev' + giSer + '.style.border = "solid 1px #66cc00"');
			eval('tdRefThumbnailRev' + iSer + '.style.border = "solid 1px #ff0000"');
		}
		giSer = iSer;
		fnDisplayRefContent();
	}

	function _fnRefAdd(bOpen) {
		var	rowIndex = fnFindRow(tblRefFreeBoard, "trRefAdd");

		if (!bOpen) {	// 닫아라.
			if (rowIndex >= 0) {
				tblRefFreeBoard.deleteRow(rowIndex);
				return true;
			}
			// 해당하는 행을 찾을 수 없다.
			return false;
		}
		// bOpen = true: 열어라.
		if (rowIndex >= 0) {
			// 이미 열려있다.
			return true;
		}

		var	oTr = tblRefFreeBoard.insertRow();
		if (!oTr) {
			alert('tblRefFreeBoard.insertRow(): Failure');
			return false;
		}
		oTr.title = "trRefAdd";

		var	oTd = oTr.insertCell();
		if (!oTd) {
			alert('.insertCell: Failure');
			tblRefFreeBoard.deleteRow(oTr.rowIndex);
			return false;
		}
		var	innerHTML = '';
		innerHTML += '<TABLE BORDER="1" FRAME="BOX" RULES="NONE" CELLSPACING="0" CELLPADDING="0"><TBODY>';
		innerHTML += '<TR><TD><INPUT ID="idRefTypeImage" TYPE="RADIO" NAME="AddType" VALUE="IMAGE" CHECKED onClick="refHtml.style.display = \'none\'; refImage.style.display = \'block\'; return true;">&nbsp;그림</TD></TR>';
		innerHTML += '<TR ID="refImage" TITLE="refImage"><TD>&nbsp;&nbsp;&nbsp;&nbsp;<INPUT ID="idRefImage" TYPE="FILE" NAME="RefImage"></TD></TR>';
		innerHTML += '<TR><TD><INPUT ID="idRefTypeHtml" TYPE="RADIO" NAME="AddType" VALUE="HTML" onClick="refImage.style.display = \'none\'; refHtml.style.display = \'block\';">&nbsp;HTML</TD></TR>';
		innerHTML += '<TR ID="refHtml" TITLE="refHtml" STYLE="display: none;"><TD>&nbsp;&nbsp;&nbsp;&nbsp;<TEXTAREA ID="idRefHtml" NAME="RefHtml" ROWS="10" STYLE="width: 300px;"></TEXTAREA></TD></TR>';
		innerHTML += '<TR><TD><INPUT TYPE="BUTTON" VALUE="더하기" onClick="fnRefSubmit();"></TD></TR>';
		innerHTML += '</TD></TR></TBODY></TABLE>';
		oTd.innerHTML = innerHTML;
		return true;
	}
	function fnToggleRefAdd() {
		gRefFlag ^= cBTN_ADD;
		if (!_fnRefAdd(gRefFlag & cBTN_ADD)) {
			alert('_fnRefAdd(): Failure');
		}
	}
	function _fnRefThumbnail(bOpen) {
		// 전에 열렸던 행이 있다면 먼저 닫는다.
		// 일반적으로 우리가 원하는 행이 아니다.
		var	rowIndex = fnFindRow(tblRefFreeBoard, "trRefThumbnail");
		if (rowIndex >= 0) {
			tblRefFreeBoard.deleteRow(rowIndex);
		}

		if (!bOpen) {	// 닫아라.
			// 이미 닫았다.
			return rowIndex >= 0 ? true : false;
		}
		// bOpen = true: 열어라.

		var	oTr = tblRefFreeBoard.insertRow();
		if (!oTr) {
			alert('tblRefFreeBoard.insertRow(): Failure');
			return false;
		}
		oTr.title = "trRefThumbnail";

		var	oTd = oTr.insertCell();
		if (!oTd) {
			alert('.insertCell: Failure');
			tblRefFreeBoard.deleteRow(oTr.rowIndex);
			return false;
		}
		var	innerHTML = '';
		innerHTML += '<TABLE BORDER="0" CELLSPACING="1" CELLPADDING="0"><TBODY>';
		var	aRefSeries = gRef.citeType == cCITE_DESIGN ? gRef.aRefSeriesByUserId[gDesignUserId] : gRef.aRefSeries;
		var	nSer = aRefSeries.length;
		var	iSer = 0;
		for (var iRow = 0; iRow < Math.ceil(nSer / 4); ++iRow) {
			innerHTML += '<TR>';
			for (var iCol = 0; iCol < 4; ++iCol, ++iSer) {
				var	oRefSeries = aRefSeries[iSer];

				var	img;
				if (iSer < nSer) {
					if (oRefSeries.type == cREF_IMAGE) {
						var	aFilename = oRefSeries.content.split(':');
						img = '/images/Thumbnail/' + (gRef.citeType == cCITE_DESIGN ? 'Work/Design' : 'Ref') + '/' + aFilename[0] + '.png';
						alt = ' ALT="' + aFilename[1] + '"';
					} else {
						img = '/images/Thumbnail/' + cIMG_HTML + '.jpg';
						alt = ' ALT="HTML"';
					}
				} else {
					img = '/images/Thumbnail/' + cIMG_BG + '.jpg';
				}
				var	id;
				var	brdColor;
				var	alt;
				var	onClick;
				if (iSer < nSer) {
					id = ' ID="tdRefThumbnailRev' + iSer + '"';
					brdColor = iSer == giSer ? '#ff0000' : '#66cc00';
					onClick = ' onClick="fnRefThumbnailOnClick(' + iSer + ');"';
				} else {
					id = '';
					brdColor = '#ffffff';
					alt = '';
					onClick = '';
				}
				innerHTML += '<TD' + id + ' STYLE="border: solid 1px ' + brdColor + ';">';
				innerHTML += '<IMG SRC="' + img + '" HEIGHT="50px" WIDTH="50px"' + alt + onClick + '>';
				innerHTML += '</TD>';
			}
			innerHTML += '</TR>';
		}
		innerHTML += '</TBODY></TABLE>';
		oTd.innerHTML = innerHTML;
		return true;
	}
	function fnToggleRefThumbnail() {
		gRefFlag ^= cBTN_THUMBNAIL;
		if (!_fnRefThumbnail(gRefFlag & cBTN_THUMBNAIL)) {
			alert('_fnRefThumbnail(): Failure');
		}
	}
	function fnRefThumbnailOnClick(iSer) {
		eval('tdRefThumbnailRev' + giSer + '.style.border = "solid 1px #66cc00"');
		eval('tdRefThumbnailRev' + iSer + '.style.border = "solid 1px #ff0000"');
		giSer = iSer;
		fnDisplayRefContent();
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/28 (EST)
 *		By Gwang-Ho Kim (iccentric@gmail.com)
 *
 * 저장할 필요가 있는 전역변수를 쿠키로 저장하고, 쿠키로부터 전역변수를 얻는다.
 */

	var	gaVarName = new Array(
		'gShowId',
		'gStoryboardId',
		'giCut',
		'gRefId',
		'gDesignUserId',

		'giSer',
		'gRefFlag',

		'giRev',
		'gCutFlag',
		'gbOrder',

		'gTabFlag'		// 탭(작품, 참고자료, 스토리보드 작업, 컷)이 열려있는 지를 말해준다.
	);
	function fnSetCookie() {
		var	aVarVal = new Array(gaVarName.length);
		var	cookie = '';
		for (var i = 0; i < gaVarName.length; ++i) {
			aVarVal[i] = eval(gaVarName[i]);
		}
		document.cookie = 'Storyboard.Var=' + aVarVal;
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
			if (aNameVal[0] == "Storyboard.Var") {
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
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/26 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 * 2005/11/27 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * slt[Show|Storyboard|Cut|Ref]에 대한 이벤트핸들러를 구현한다.
 */

	// 작품을 선택할 때 실행되는 이벤트핸들러이다.
	function fnSltShowOnChange() {
		gShowId = gahiShowId[sltShow.selectedIndex].id;
		fnInitShow();
	}

	// 스토리보드 작업을 선택할 때 실행되는 이벤트핸들러이다.
	function fnSltStoryboardOnChange() {
		gStoryboardId = gahiStoryboardId[sltStoryboard.selectedIndex].id;
		fnInitStoryboard();
	}

	// 컷을 선택했을 때 실행되는 이벤트핸들러이다.
	function fnSltCutOnChange() {
		giCut = sltCut.selectedIndex;
		fnInitCut();
	}

	// 참고자료를 선택할 때 실행되는 이벤트핸들러이다.
	function fnSltRefOnChange() {
		var	a = sltRef.options[sltRef.selectedIndex].value.split(":");
		// cCITE_DESIGN: "indexTogahiRefId:userId"
		// otherwise: "indexTogahiRefId"
		gRefId = gahiRefId[a[0]].id;
		if (a.length == 2) {	// cCITE_DESIGN
			gDesignUserId = a[1];
		} else {
			gDesignUserId = undefined;
		}
		fnInitRef();
	}

	// gahiShowId를 이용하여 작품목록을 구성한다.
	function fnAddOptionToSltShow() {
		sltShow.options.length = 0;	// 초기화한다.
		for (var i = 0; i < gahiShowId.length; ++i) {
			var	oOp = document.createElement("OPTION");
			oOp.value = i;
			var	aaStoryboard = gaaStoryboard[gahiShowId[i].index];
			var	aStoryboard = aaStoryboard.aStoryboard;
			var	n = 0;
			for (var k = 0; k < aStoryboard.length; ++k) {
				if (gUserId == aStoryboard[k].userId) {
					++n;
				}
			}
			oOp.text = aaStoryboard.showName + ' (총 작업 갯수 = ' + aStoryboard.length + ', 내가 진행하는 작업 갯수 = ' + n + ')';
			sltShow.options.add(oOp);
		}
	}

	// oStoryboard에 해당하는 작업에 대한 "작업문장"을 얻는다.
	function fnGetWorkText(oStoryboard) {
		var	workText = '';
//		workText += '작품("' + gaaStoryboard[gahiShowId[sltShow.selectedIndex].index].showName + '")의 ';
		workText += '장면("' + oStoryboard.sceneName + '")에 대한 스토리보드 작업';
		return workText;
	}
	// gahiStoryboardId를 이용해서 작업 목록을 구성한다.
	function fnAddOptionsToSltStoryboard() {
		sltStoryboard.options.length = 0;	// 초기화한다.
		for (var i = 0; i < gahiStoryboardId.length; ++i) {
			var	oStoryboard = gaStoryboard[gahiStoryboardId[i].index];
			var	oOp = document.createElement("OPTION");
			oOp.value = i;
			oOp.text = fnGetWorkText(oStoryboard) + ' (컷 갯수 = ' + oStoryboard.aCut.length + ')';
			sltStoryboard.options.add(oOp);
		}
	}
	// gStoryboad.aCut을 이용하여 컷 목록을 구성한다.
	function fnAddOptionsToSltCut() {
		sltCut.options.length = 0;	// 초기화한다.
		for (var i = 0; i < gStoryboard.aCut.length; ++i) {
			var	oOp = document.createElement("OPTION");
			oOp.value = i;
			oOp.text = '컷 #' + (1 + i) + ' (리비젼 갯수 = ' + gStoryboard.aCut[i].aImage.length + ')';
			sltCut.options.add(oOp);
		}
	}
	// gahiRefId를 이용해서 참고자료 목록을 구성한다.
	function fnAddOptionsToSltRef() {
		sltRef.options.length = 0;	// 초기화한다.
		for (var i = 0; i < gahiRefId.length; ++i) {
			var	oRef = gaRef[gahiRefId[i].index];
			var	citeType = oRef.citeType;
			if (citeType == cCITE_DESIGN) {
				for (var userId in oRef.aRefSeriesByUserId) {
					var	oOp = document.createElement("OPTION");
					oOp.value = i + ':' + userId;
					oOp.text = '디자인 작업: ' + oRef.citeName + '(작업자: ' + oRef.aRefSeriesByUserId[userId][0].nickname + ', 리비젼 갯수 = ' + oRef.aRefSeriesByUserId[userId].length + ')';
					oOp.style.color = "green";
					sltRef.options.add(oOp);
				}
				continue;
			}

			// citeType != cCITE_DESIGN
			var	oOp = document.createElement("OPTION");
			oOp.value = i;
			var	text;
			switch (oRef.citeType) {
			case cCITE_SHOW:
				text = '작품: ' + oRef.citeName;
				break;
			case cCITE_SCENE:
				text = '장면: ' + oRef.citeName;
				break;
			default:	// cCITE_STORYBOARD
				text = '작업: ' + fnGetWorkText(gaStoryboard[gahiStoryboardId[sltStoryboard.selectedIndex].index]);
				break;
			}
			oOp.text = text + ' (참고자료 갯수 = ' + oRef.aRefSeries.length + ')';
			oOp.style.color = citeType == cCITE_STORYBOARD ? "blue" : "red";
			sltRef.options.add(oOp);
		}
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/26 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 1. gaaStoryboard중에서 gShowId에 해당하는 gaStoryboard를 결정하고,
 * 이를 이용해서 (다자인)작업 목록을 만든다.
 * gaStoryboard중에서 gStoryboardId에 해당하는 gStoryboard을 결정하고,
 * 그 정보를 화면에 표시한다.
 * 2. gaRef중에서 gStoryboard에 해당하는 gahiRefId를 결정하고,
 * 이를 이용해서 참고자료 목록을 만든다.
 * 이 목록 중에서 (gRefId, gDesignUserId)에 해당하는 gRef를 결정하고,
 * 그 정보를 화면에 표시한다.
 */

	// gStoryboardId에 해당하는 작업(gStoryboard)에서 참고하는 자료들에 대한,
	// gaRef에서의 첨자들을 얻어 gahiRefId배열을 결정한다.
	function fnSetahiRefId() {
		gahiRefId = [];
		for (var i = 0; i < gaRef.length; ++i) {
			var	oRef = gaRef[i];
			if (
				(oRef.citeType == cCITE_SHOW && oRef.citeId == gShowId) ||
				(oRef.citeType == cCITE_SCENE && oRef.citeId == gStoryboard.sceneId) ||
				(oRef.citeType == cCITE_STORYBOARD && oRef.citeId == gStoryboard.storyboardId) ||
				(oRef.citeType == cCITE_DESIGN && (oRef.citeId == 0 || oRef.citeId == gStoryboard.sceneId))
			) {
				gahiRefId.push(new objHashIndex(oRef.refId, i));
			}
		}
/*
		var	aIndex = [];
		for (var i = 0; i < gahiRefId.length; ++i)
			aIndex.push(gahiRefId[i].index);
		alert(aIndex);
 */
		return;
	}
	// giCut에 해당하는 gCut을 찾는다.
	// [가정] 적어도 하나의 컷이 있다.
	function fnSetCut() {
		gCut = undefined;
		if (giCut < 0 || gStoryboard.aCut.length <= giCut) {
			// (정책) 기본값은 첫 번째 컷을 선택하는 것이다.
			// [가정]에 의해서 반드시 첫 번째 컷은 존재한다.
			giCut = 0;
		}
		gCut = gStoryboard.aCut[giCut];
	}
	// (gRefId, gDesignUserId)에 해당하는 gRef를 찾는다.
	// [가정] gaRef중에는 적어도 주어진 STORYBOARD_ID에 대한 참고가 있다.
	// 비록 참조자료가 하나도 없다 할 지라도...
	function fnSetRef() {
		gRef = undefined;
		for (var i = 0; i < gahiRefId.length; ++i) {
			var	refId = gahiRefId[i].id;
			var	oRef = gaRef[gahiRefId[i].index];
			var	citeType
			if (oRef.citeType == cCITE_DESIGN) {
				if (gRefId == refId && oRef.aRefSeriesByUserId[gDesignUserId] !== undefined) {
					gRef = oRef;
					break;
				}
			} else {
				if (gRefId == refId) {
					gRef = oRef;
					break;
				}
			}
		}
		if (gRef == undefined) {
//			alert('(gRefId, gDesignUserId) = (' + gRefId + ', ' + gDesignUserId + ') + '에 해당하는 참고가 없다');
			// (정책) cCITE_STORYBOARD를 기본으로 한다.
			for (var i = 0; i < gahiRefId.length; ++i) {
				var	oRef = gaRef[gahiRefId[i].index];
				if (oRef.citeType == cCITE_STORYBOARD) {
					gRefId = gahiRefId[i].id;
					gDesignUserId = undefined;
					gRef = oRef;
					break;
				}
			}
			if (gRef == undefined) {
				alert('gaRef중에서 STORYBOARD_ID에 대한 참고는 반드시 있어야 한다');
				return;
			}
//			alert('STORYBOARD_ID에 대한 참조를 기본으로 한다');
		}
	}

	// gaaStoryboard중에서 gShowId에 해당하는 gaStoryboard을 결정한다.
	// [가정] 적어도 하나의 작업이 있다. (gahiShowId.length >= 1)
	function fnSetaStoryboard() {
		gaStoryboard = undefined;	// 초기화한다.
		for (var i = 0; i < gahiShowId.length; ++i) {
			if (gShowId == gahiShowId[i].id) {
				// gShowId에 해당하는 objWorkStoryboard을 찾았다.
				gaStoryboard = gaaStoryboard[gahiShowId[i].index].aStoryboard;
			}
		}
		if (gaStoryboard == undefined) {
//			alert('SHOW_ID = ' + gShowId + '에 해당하는 작업이 없다');
			// [가정]으로부터 적어도 하나의 작업은 있다.
			gShowId = gahiShowId[0].id;
			gaStoryboard = gaaStoryboard[gahiShowId[0].index].aStoryboard;
//			alert('첫 번째 작품(SHOW_ID = ' + gShowId + ')을 기본으로 한다');
		}

		// gUserId가 참여하는 작업을 쉽게 접근하기 위한 gahiStoryboardId배열을 만든다.
		gahiStoryboardId = [];
		for (i = 0; i < gaStoryboard.length; ++i) {
			if (gUserId == gaStoryboard[i].userId) {
				gahiStoryboardId.push(new objHashIndex(gaStoryboard[i].storyboardId, i));
			}
		}
	}
	// gaStoryboard중에서 gStoryboardId에 해당하는 gStoryboard을 결정한다.
	// [가정] DB에서 gaaStoryboard배열을 만들 때,
	// 작품에 작업이 없다면 그 작품은 gaaStoryboard에 나타날 수 없으므로,
	// gaaStoryboard배열에 작품이 있다는 것은 적어도 하나의 작업이 있다는 것을 뜻한다.
	// (gaaStoryboard[gahiShowId[i].index].aStoryboard.length >= 1)
	function fnSetStoryboard() {
		// gStoryboardId에 해당하는 작업을 찾는다.
		gStoryboard = undefined;	// 초기화한다.
		for (var i = 0; i < gahiStoryboardId.length; ++i) {
			if (gStoryboardId == gahiStoryboardId[i].id) {
				// gStoryboardId에 해당하는 작업을 찾았다.
				gStoryboard = gaStoryboard[gahiStoryboardId[i].index];
				break;
			}
		}
		if (gStoryboard == undefined) {
//			alert('STORYBOARD_ID = ' + gStoryboardId + '에 해당하는 작업이 없다');
			gStoryboard = gaStoryboard[gahiStoryboardId[0].index];
			gStoryboardId = gahiStoryboardId[0].id;
//			alert('첫 번째 작업(DESIGN_ID = ' + gStoryboardId + ')을 기본으로 한다');
		}
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/22 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 */

	// submitType
	var	cSUBMIT_WORK	= <?PHP echo SUBMIT_WORK ?>;
	var	cSUBMIT_REF	= <?PHP echo SUBMIT_REF ?>;
	var	cSUBMIT_EDITCUT	= <?PHP echo SUBMIT_EDITCUT ?>;

	// citeType
	var	cCITE_SHOW	= <?PHP echo CITE_SHOW ?>;
	var	cCITE_SCENE	= <?PHP echo CITE_SCENE ?>;
	var	cCITE_TARGET	= <?PHP echo CITE_TARGET ?>;
	var	cCITE_DESIGN	= <?PHP echo CITE_DESIGN ?>;
	var	cCITE_STORYBOARD	= <?PHP echo CITE_STORYBOARD ?>;
	// refType
	var	cREF_IMAGE	= <?PHP echo REF_IMAGE ?>;
	var	cREF_HTML	= <?PHP echo REF_HTML ?>;

	// designType
	var	cDESIGN_CONCEPT	= <?PHP echo DESIGN_CONCEPT ?>;		// 컨셉 디장인
	var	cDESIGN_DETAIL	= <?PHP echo DESIGN_DETAIL ?>;		// 구체적인 디자인
	// targetType
	var	cTARGET_CHR	= <?PHP echo TARGET_CHR ?>;		// 인물
	var	cTARGET_BG	= <?PHP echo TARGET_BG ?>;		// 배경
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/22 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
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

	// 그림에 대한 정보를 담는다.
	function objImage(rev, comment, filename, filenameUuid) {
		this.rev = rev;
		this.comment = comment;		// 요구사항
		this.filename = filename;
		this.filenameUuid = filenameUuid;
	}
	// 컷에 대한 정보를 담는다.
	function objCut(cutId, serial, imageId, aImage) {
		// DB에서 가져온 컷이라면 id, serial 모두 0보다 크다. (1, 2, 3, ...)
		// 새로 만든 컷이라면 id, serial 모두 0이다.
		this.cutId = cutId;			// CUT_ID
		this.serial = serial;			// SERIAL: 일련번호

		// 컷 이미지에 대한 리비젼 정보이다.
		this.imageId = imageId;			// IMAGE_ID
		this.aImage = aImage;
	}
	// 스토리보드 작업에 대한 정보를 담는다.
	function objStoryboard(storyboardId, sceneId, sceneName, userId, order, status, aCut) {
		this.storyboardId = storyboardId;	// STORYBOARD_ID(= WORK_ID)
		this.sceneId = sceneId;			// SCENE_ID
		this.sceneName = sceneName;		// 표시를 위해서 사용된다.
		this.userId = userId;			// USER_ID
		this.order = order;			// YOUR_ORDER
		this.status = status;			// STATUS
		this.aCut = aCut;			// objCut으로 이루어진 배열이다.
	}
	// 각 작품별로 스토리보드 작업을 분류하는 데 사용되는 오브젝트이다.
	function objWorkStoryboard(showId, showName, aStoryboard) {
		this.showId = showId;
		this.showName = showName;	// 표시를 위해서 사용된다.
		this.aStoryboard = aStoryboard;
	}
	var	gaaStoryboard;	// objWorkStoryboard으로 이루어진 배열
	var	gahiShowId;	// gaaStoryboard중에서,
				// gUserId가 참여하는 작업이 속한 작품을 접근하기 쉽게 하기 위한,
				// objHashIndex로 이루어진 배열
	var	gShowId;	//
	var	gaStoryboard;	// gaaStoryboard중에서 gShowId에 해당하는 objWorkStoryboard오브젝트의 aStoryboard
	var	gahiStoryboardId;	// gaStoryboard중에서,
					// gUserId가 참여하는 작업을 접근하기 쉽게 하기 위한,
					// objHashIndex로 이루어진 배열
	var	gStoryboardId;	//
	var	gStoryboard;	// gaStoryboard중에서 gStoryboardId에 해당하는 objStoryboard
	var	giCut;
	var	gCut;		// gStoryboard의 aCut중에서 giCut번째 objCut을 가리킨다.

	// 2005/12/5 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
	//	참고자료에 대한 구조는 디자인 작업의 경우와 같다.
	//	단, citeType = cCITE_DESIGN의 경우, content에는
	//	디자인 작업의 경우에는 작업에 필요한 참고자료가 들어있지만,
	//	스토리보드 작업의 경우에는 디자인 작업의 결과(objImage)가 된다.
	// 참고자료에 대한 일련의 구체적인 정보를 담는다.
	function objRefSeries(serial, type, content, userId, nickname) {
		this.serial = serial;
		this.type = type;		// (cREF_IMAGE, cREF_HTML)
		this.content = content;		// cREF_IMAGE: 그림파일의 "FILENAME_UUID:FILENAME", cREF_HTML: HTML문서의 내용
		this.userId = userId;
		this.nickname = nickname;	// cCITE_DESIGN의 경우에,
						// 디자인 작업을 진행한 USER의 이름을 표시하기 위하여...
	}
	// 참고자료에 대한 전반적인 정보를 담는다.
	// 2005/12/5 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
	//	표시를 위해서 citeName을 추가한다.
	function objRef(refId, citeType, citeId, citeName, aRefSeries) {
		this.refId = refId;		// REF_ID
		this.citeType = citeType;	// (cCITE_SHOW, cCITE_SCENE, cCITE_DESIGN, cCITE_STORYBOARD)
		this.citeId = citeId;		// cCITE_SHOW(cCITE_SCENE): 참고하는 작품(장면)의 SHOW_ID(SCENE_ID)
						// cCITE_DESIGN: gUserId가 참여하는 스토리보드 작업의 장면에
						//	등장하는(사용되는) 인물(배경)에 대한 디자인 작업의 SCENE_ID이다.
						//	SCENE_ID = 0: 작품 전반에 걸쳐 등장하는(사용되는) 인물(배경)이므로
						//	모든 스토리보드 작업의 참고자료가 된다.
						//	SCENE_ID != 0: 스토리보드 작업의 장면이 SCENE_ID와 같은
						//	스토리보드 작업의 참고자료가 된다.
						// cCITE_STORYBOARD: gUserId가 참여하는 스토리보드 작업의 STORYBOARD_ID
		this.citeName = citeName;	// 표시를 위해서 사용된다.
						// cCITE_SHOW(cCITE_SCENE): 작품(장면)의 이름
						// cCITE_DESING: 장면, 대상을 명시한 디자인 작업에 대한 표시 이름
		this.aRefSeries = aRefSeries;	// cCITE_SHOW, cCITE_SCENE, cCITE_STORYBOARD: 참고자료
						// cCITE_DESIGN: 디자인 작업 결과 그림
		if (citeType == cCITE_DESIGN) {
			// USER_ID에 따라 다르게 사용한다.
			this.aRefSeriesByUserId = [];
			for (var i = 0; i < aRefSeries.length; ++i) {
				var	userId = aRefSeries[i].userId;
				if (this.aRefSeriesByUserId[userId] == undefined) {
					this.aRefSeriesByUserId[userId] = [];
				}
				this.aRefSeriesByUserId[userId].push(aRefSeries[i]);
			}
if (0) {
			var	nUserId = 0;
			for (var userId in this.aRefSeriesByUserId) {
				++nUserId;
				alert('USER_ID: ' + userId + ', # of ... = ' + this.aRefSeriesByUserId[userId].length);
			}
			alert('# of UserId = ' + nUserId);
}
		}
	}
	var	gaRef;		// objRef로 이루어진 배열
	var	gahiRefId;	// gaRef중에서 gRef로 사용되는 objRef들을 결정하는 objHashIndex로 이루어진 배열
				// gStoryboard이 결정되어야 gahiRefId가 결정된다.
				// 자세한 것은 fnSetahiRefId함수를 참고하라.
	var	gRefId;		//
	var	gDesignUserId;	// gRefId에 해당하는 objRef에서 citeType이 cCITE_DESIGN의 경우,
				// 디자인 작업을 진행한 USER의 USER_ID
	var	gRef;		// gahiRefId이 결정하는 objRef중에서 (gRefId, gDesignUserId)에 해당하는 objRef 

	// 작품을 선택했을 때 행하는 초기화 과정이다.
	function fnInitShow() {
		// gaaStoryboard중에서 gShowId에 해당하는 gaStoryboard을 결정한다.
		fnSetaStoryboard();
		// 작품 목록에서 gShowId에 해당하는 작품을 선택한다.
		for (var i = 0; i < sltShow.options.length; ++i) {
			sltShow.options[i].selected = gShowId == gahiShowId[i].id;
		}

		// gaStoryboard의 정보를 이용해서 스토리보드 작업 목록을 만든다.
		fnAddOptionsToSltStoryboard();

		// gStoryboardId에 해당하는 스토리보드 작업을 선택했을 때 필요한 초기화를 실시한다.
		fnInitStoryboard();
	}
	// gStoryboardId에 해당하는 스토리보드 작업을 선택했을 때 행하는 초기화 과정이다.
	function fnInitStoryboard() {
		// gaStoryboard중에서 gStoryboardId에 해당하는 gStoryboard을 결정한다.
		fnSetStoryboard();
		// 스토리보드 작업 목록에서 gStoryboardId에 해당하는 작업을 선택한다.
		for (var i = 0; i < sltStoryboard.options.length; ++i) {
			sltStoryboard.options[i].selected = gStoryboardId == gahiStoryboardId[i].id;
		}

		// 컷 목록을 만든다.
		fnAddOptionsToSltCut();
		// giCut에 해당하는 컷을 선택했을 때 필요한 초기화를 실시한다.
		fnInitCut();

		// gStorybaord을 바탕으로 gahiRefId배열을 결정한다.
		fnSetahiRefId();
		// gahiRefId의 정보를 이용해서 참고자료 목록을 만든다.
		fnAddOptionsToSltRef();
		// (gRefId, gDesignUserId)에 해당하는 참고자료를 선택했을 때 필요한 초기화를 실시한다.
		fnInitRef();
	}
	// giCut에 해당하는 참고자료를 선택했을 때 행하는 초기화 과정이다.
	function fnInitCut() {
		if (gbEditCut) {
			fnEditCut();
		}
		// giCut에 해당하는 gCut을 결정한다.
		fnSetCut();
		// 컷 목록에서 giCut에 해당하는 컷을 선택한다.
		for (var i = 0; i < sltCut.options.length; ++i) {
			sltCut.options[i].selected = i == giCut;
		}

		// gCut에 해당하는 정보를 표시한다.
		fnDisplayCut();
	}
	// (gRefId, gDesignUserId)에 해당하는 참고자료를 선택했을 때 행하는 초기화 과정이다.
	function fnInitRef() {
		// (gRefId, gDesignUserId)에 해당하는 gRef를 찾는다.
		fnSetRef();
		// 참고자료 목록에서 (gRefId, gDesignUserId)에 해당하는 참고자료를 선택한다.
		var	iOp = 0;
		for (var i = 0; i < gahiRefId.length; ++i) {
			var	refId = gahiRefId[i].id;
			var	oRef = gaRef[gahiRefId[i].index];
			if (oRef.citeType == cCITE_DESIGN) {
				for (var userId in oRef.aRefSeriesByUserId) {
					var	oOp = sltRef.options[iOp++];
					oOp.selected = gRefId == refId && gDesignUserId == userId;
				}
			} else {
				var	oOp = sltRef.options[iOp++];
				oOp.selected = gRefId == refId;
			}
		}

		// gRef정보를 화면에 표시한다.
		fnDisplayRef();
	}

	function fnSetData() {
		var	JavaScriptEvalCode = "<?PHP echo $JavaScriptEvalCode ?>";
//		alert(JavaScriptEvalCode);
		eval(JavaScriptEvalCode);

		gahiShowId = [];
		for (var i = 0; i < gaaStoryboard.length; ++i) {
			// 주어진 작품에서 내가 참여하는 작업의 갯수를 알아본다.
			var	aStoryboard = gaaStoryboard[i].aStoryboard;
			var	nWork = 0;
			for (var k = 0; k < aStoryboard.length; ++k) {
				if (gUserId == aStoryboard[k].userId) {
					++nWork;
				}
			}
			if (nWork) {	// 내가 참여하는 작업에 이 작품에 있다.
				gahiShowId.push(new objHashIndex(gaaStoryboard[i].showId, i));
			}
		}
	}

	function fnSetVar() {
		// gShowId: number
		gShowId = parseInt(gShowId);
		if (isNaN(gShowId)) {		// 정의되지 않았거나 숫자가 아닌 경우
			// (정책) 기본값은 0으로 아무런 작품도 선택하지 않은 상태이다.
			gShowId = 0;
		}
		// gShowId의 유효성 검사는 fnSetaStoryboard함수에서 이루어진다.

		// gStoryboardId: number
		gStoryboardId = parseInt(gStoryboardId);
		if (isNaN(gStoryboardId)) {	// 정의되지 않았거나 숫자가 아닌 경우
			// (정책) 기본값은 0으로 아무런 작업도 선택하지 않은 상태이다.
			gStoryboardId = 0;
		}
		// gStoryboardId의 유효성 검사는 fnSetStoryboard함수에서 이루어진다.
		// giCut: number
		giCut = parseInt(giCut);
		if (isNaN(giCut)) {	// 정의되지 않았거나 숫자가 아닌 경우
			// (정책) 기본값은 0으로 첫 번째 컷을 선택한 것으로 한다.
			giCut = 0;
		}
		// giCut의 유효성 검사는 fnSetCut함수에서 이루어진다.

		// gRefId: number
		gRefId = parseInt(gRefId);
		if (isNaN(gRefId)) {	// 정의되지 않았거나 숫자가 아닌 경우
			// (정책) 기본값은 0으로 아무런 작업도 선택하지 않은 상태이다.
			gRefId = 0;
		}
		// gRefId의 유효성 검사는 fnSetRef함수에서 이루어진다.
		// gDesignUserId: number
		gDesignUserId = parseInt(gDesignUserId);
		if (isNaN(gDesignUserId)) {	// 정의되지 않았거나 숫자가 아닌 경우
			// (정책) 기본값은 0으로 어떤 디자인 작업자도 선택하지 않은 상태이다.
			gDesignUserId = 0;
		}
		// gDesignUserId의 유효성 검사는 fnSetRef함수에서 이루어진다.

		// giSer: number
		if (
			giSer == undefined ||	// 정의되지 않았거가
			isNaN(parseInt(giSer))	// 숫자가 아닌 경우
		) {
			// (정책) 기본값은 -1으로 아무런 참고자료도 선택하지 않은 상태이다.
			giSer = -1;
		}
		// giSer의 유효성 검사는 fnDisplayRef함수에서 이루어진다.

		// gRefFlag: number
		if (
			isNaN(parseInt(gRefFlag))	// 숫자가 아닌 경우
		) {
			// (정책) 아무 플래그도 설정되지 않았다.
			gRefFlag = 0;
		}
		// cBTN_ADD와 cBTN_THUMBNAIL만 그대로 둔다.
		gRefFlag &= cBTN_ADD | cBTN_THUMBNAIL;

		// giRev: number
		if (
			giRev == undefined ||	// 정의되지 않았거가
			isNaN(parseInt(giRev))	// 숫자가 아닌 경우
		) {
			// (정책) 기본값은 -1으로 아무런 그림도 선택하지 않은 상태이다.
			giRev = -1;
		}
		// giRev의 유효성 검사는 fnDisplayCut함수에서 이루어진다.

		// gCutFlag: number
		if (
			isNaN(parseInt(gCutFlag))	// 숫자가 아닌 경우
		) {
			// (정책) 아무 플래그도 설정되지 않았다.
			gCutFlag = 0;
		}
		// cBTN_ADD와 cBTN_THUMBNAIL만 그대로 둔다.
		gCutFlag &= cBTN_ADD | cBTN_THUMBNAIL;

		// gbOrder: number
		if (
			isNaN(parseInt(gbOrder))	// 숫자가 아닌 경우
		) {
			// (정책) "지시사항"을 표시한다.
			gbOrder = 1;
		}
		// 0이나 1만 가질 수 있다.
		gbOrder &= 1;

		// gTabFlag: number
		if (
			isNaN(parseInt(gTabFlag))	// 숫자가 아닌 경우
		) {
			// (정책) 모든 탭이 열려있다.
			gTabFlag = 0;
		}
		// (cTAB_SHOW, cTAB_REF, cTAB_STORYBOARD, cTAB_CUT)만이 가능하다.
		gTabFlag &= cTAB_SHOW | cTAB_REF | cTAB_STORYBOARD | cTAB_CUT;
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/22 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * CVS를 통해서 버젼관리를 한다.
 *
 * UI는 fmStoryboard.PHP를 바탕으로 하고,
 * 오브젝트관리는 fmShow.PHP나 fmWork.PHP의 경험을 토대로 한다.
 */
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
	function fnOnLoad() {
		fnGetCookie();
//		alert('USER_ID = ' + gUserId);

		fnSetData();

		// 설정된 전역변수가 없다면 전역변수를 정책에 따른 기본값으로 설정하고,
		// 있다면 설정된 데이터를 바탕으로 다시 확인한다.
		fnSetVar();

		if (!gahiShowId.length) {
			alert('내가 참여하는 작업이 하나도 없으므로 더 이상 진행할 수 없다');
			return;
		}
		// 작품 목록을 구성한다.
		fnAddOptionToSltShow();

		gcTrbOrderHeight = trbOrder.clientHeight;
		// gShowId에 해당하는 작품을 선택했을 때의 초기화과정을 수행한다.
		tdRefContent.style.height = (cPANEL_HEIGHT + 2 * tblMain.border) + 'px';
		// "trStatus"가 차지하는 높이를 얻는다.
		gcTrStatusHeight = trStatus.clientHeight;
		tdCutImage.style.height = (cPANEL_HEIGHT + 2 * tblMain.border) + 'px';

		fnInitShow();

		// gTabFlag에 따라 탭의 상태를 나타낸다.
		// 각 SELECT의 목록이 선택되어 있어야 하므로 fnInitShow다음에 호출한다.
		_fnSetTab();
	}

	function fnOnUnload() {
		if (gbEditCut) {
			fnEditCut();
		}
		fnSetCookie();
	}
	window.onload = fnOnLoad;
	window.onunload = fnOnUnload;
</SCRIPT>
</HEAD>
<BODY>
<DIV ALIGN="CENTER">
<TABLE ID="tblMain" BORDER="1" WIDTH="80%" CELLSPACING="0" CELLPADDING="0">
<TR><TD COLSPAN="4"><TABLE WIDTH="100%" BORDER="0" FRAME="RHS" CELLSPACING="0" CELLPADDING="0"><TBODY>
	<TR><TD ID="tdShow" WIDTH="100%" ALIGN="CENTER" STYLE="background: #fffbf0; cursor: hand;" onClick="fnTdOnClick(this);">작품</TD></TR>
	<TR><TD><SELECT ID="sltShow" SIZE="2" STYLE="width: 400px;" onChange="fnSltShowOnChange();"></SELECT></TD></TR>
</TBODY></TABLE></TD></TR>
<TR><TD COLSPAN="4"><TABLE WIDTH="100%" BORDER="0" FRAME="RHS" CELLSPACING="0" CELLPADDING="0"><TBODY>
	<TR><TD ID="tdStoryboard" WIDTH="100%" ALIGN="CENTER" STYLE="background: #fffbf0; cursor: hand;" onClick="fnTdOnClick(this);">스토리보드 작업</TD></TR>
	<TR><TD><SELECT ID="sltStoryboard" SIZE="4" STYLE="width: 800px;" onChange="fnSltStoryboardOnChange();"></SELECT></TD></TR>
</TBODY></TABLE></TD></TR>
<TR><TD COLSPAN="4"><TABLE ID="tblCut" WIDTH="100%" BORDER="0" FRAME="RHS" CELLSPACING="0" CELLPADDING="0"><TBODY>
	<TR><TD ID="tdCut" WIDTH="100%" ALIGN="CENTER" STYLE="background: #fffbf0; cursor: hand;" onClick="fnTdOnClick(this);">컷&nbsp;&nbsp;&nbsp;&nbsp;<INPUT ID="btnEditCut" TYPE="BUTTON" VALUE='"컷 수정" 열기' onClick="fnEditCut();"></TD></TR>
	<TR><TD><SELECT ID="sltCut" SIZE="4" STYLE="width: 200px;" onChange="fnSltCutOnChange();"></SELECT></TD></TR>
</TBODY></TABLE></TD></TR>
<TR><TD COLSPAN="4"><TABLE WIDTH="100%" BORDER="0" FRAME="RHS" CELLSPACING="0" CELLPADDING="0"><TBODY>
	<TR><TD ID="tdRef" WIDTH="100%" ALIGN="CENTER" STYLE="background: #fffbf0; cursor: hand;" onClick="fnTdOnClick(this);">참고자료</TD></TR>
	<TR><TD><SELECT ID="sltRef" SIZE="4" STYLE="width: 800px;" onChange="fnSltRefOnChange();"></SELECT></TD></TR>
</TBODY></TABLE></TD></TR>
<!--
<TR>
	<TD WIDTH="35%" VALIGN="TOP"><TABLE BORDER="1" CELLSPACING="0" CELLPADDING="0"><TBODY>
	<TR><TD WIDTH="100%" ALIGN="CENTER">참고자료</TD></TR>
	<TR><TD><SELECT ID="XsltRef" SIZE="4" STYLE="width: 200px;" onChange="fnSltRefOnChange();"></SELECT></TD></TR>
	</TBODY></TABLE></TD>
	<TD WIDTH="10%">&nbsp;</TD>
	<TD WIDTH="35%" VALIGN="TOP"><TABLE BORDER="1" CELLSPACING="0" CELLPADDING="0"><TBODY>
	<TR><TD WIDTH="100%" ALIGN="CENTER">작업</TD>
	<TR><TD><SELECT ID="XsltDesign" SIZE="4" STYLE="width: 200px;" onChange="fnSltDesignOnChange();"></SELECT></TD></TR>
	</TBODY></TABLE></TD>
	<TD WIDTH="20%" VALIGN="BOTTOM"><TABLE BORDER="1" CELLSPACING="0" CELLPADDING="0"><TBODY>
	<TR ID="trbOrder">
		<TD><SELECT ID="sltbOrder" SIZE="1" STYLE="width; 120px;" onChange="fnSltbOrderOnChange();">
		<OPTION>지시사항</OPTION>
		<OPTION>요구사항</OPTION>
		</SELECT></TD>
	</TR>
	</TBODY></TABLE></TD>
</TR>
-->
<TR ID="MAIN_IMAGE">
	<TD WIDTH="35%" ID="tdRefContent" TITLE="참고자료" VALIGN="TOP">&nbsp;</TD>
	<TD WIDTH="10%">&nbsp;</TD>
	<TD WIDTH="35%" ID="tdCutImage" TITLE="tdCutImage" VALIGN="TOP">&nbsp;</TD>
	<TD WIDTH="20%"><TABLE HEIGHT="100%" WIDTH="100%" BORDER="0" CELLSPACING="0" CELLPADDING="0"><TBODY>
	<TR ID="trbOrder">
		<TD><SELECT ID="sltbOrder" SIZE="1" STYLE="width; 120px;" onChange="fnSltbOrderOnChange();">
		<OPTION>지시사항</OPTION>
		<OPTION>요구사항</OPTION>
		</SELECT></TD>
	</TR>
	<TR ID="trOrderOrCommentBoard" TITLE="trOrderOrCommentBoard" HEIGHT="100%" VALIGN="TOP">
		<TD ID="tdOrderOrComment" TITLE="tdOrderOrComment" WIDTH="100%">&nbsp;</TD>
	</TR>
	<TR ID="trStatus" TITLE="trStatus">
		<TD ALIGN="CENTER"><TABLE WIDTH="100%" BORDER="1" CELLSPACING="0" CELLPADDING="0"><TBODY><TR ALIGN="CENTER">
			<TD>1</TD>
			<TD>2</TD>
			<TD>3</TD>
		</TR></TBODY></TABLE></TD>
	</TR>
	</TBODY></TABLE></TD>
</TR>
<TR>
	<TD ALIGN="CENTER"><TABLE WIDTH="100%" BORDER="0" CELLSPACING="0" CELLPADDING="0"><TBODY><TR ID="trRefButton" ALIGN="CENTER">
		<TD ID="tdRefImageDownload" WIDTH="20px">&nbsp;</TD>
		<TD><INPUT ID="btnRefAdd" TYPE="BUTTON" VALUE="Add" onClick="fnToggleRefAdd();"></TD>
		<TD><INPUT ID="btnRefDelete" TYPE="BUTTON" VALUE="Del" onClick="fnDeleteRef();"></TD>
		<TD><INPUT ID="btnRefBack" TYPE="BUTTON" VALUE="←" onClick="fnRefDirBtnPressed(-1);"></TD>
		<TD><INPUT ID="btnRefForward" TYPE="BUTTON" VALUE="→" onClick="fnRefDirBtnPressed(+1);"></TD>
		<TD><INPUT ID="btnRefThumbnail" TYPE="BUTTON" VALUE="Thumbnails" onClick="fnToggleRefThumbnail();"></TD>
	</TR></TBODY></TABLE></TD>
	<TD>&nbsp;</TD>
	<TD ALIGN="CENTER"><TABLE WIDTH="100%" BORDER="0" CELLSPACING="0" CELLPADDING="0"><TBODY><TR ID="trCutButton" ALIGN="CENTER">
		<TD ID="tdCutImageDownload" WIDTH="20px">&nbsp;</TD>
		<TD><INPUT ID="btnCutAdd" TYPE="BUTTON" VALUE="Add" onClick="fnToggleCutAdd();"></TD>
		<TD><INPUT ID="btnCutDelete" TYPE="BUTTON" VALUE="Del" onClick="fnDeleteCutImage();"></TD>
		<TD><INPUT ID="btnCutBack" TYPE="BUTTON" VALUE="←" onClick="fnCutDirBtnPressed(-1);"></TD>
		<TD><INPUT ID="btnCutForward" TYPE="BUTTON" VALUE="→" onClick="fnCutDirBtnPressed(+1);"></TD>
		<TD><INPUT ID="btnCutThumbnail" TYPE="BUTTON" VALUE="Thumbnails" onClick="fnToggleCutThumbnail();"></TD>
	</TR></TBODY></TABLE></TD>
	<TD>&nbsp;</TD>
</TR>
<TR>
	<TD VALIGN="TOP" COLSPAN="2"><TABLE ID="tblRefFreeBoard" TITLE="tblRefFreeBoard" WIDTH="100%" STYLE="border: solid 2px #cc0066;" CELLSPACING="0" CELLPADDING="0"><TBODY>
	</TBODY></TABLE></TD>
	<TD VALIGN="TOP" COLSPAN="2"><TABLE ID="tblCutFreeBoard" TITLE="tblCutFreeBoard" WIDTH="100%" STYLE="border: solid 2px #66cc00;" CELLSPACING="0" CELLPADDING="0"><TBODY>
	</TBODY></TABLE></TD>
</TR>
</TABLE>
</DIV>
</BODY>
</HTML>
<?PHP
	}
	// Closing connection
	mysql_close($link);
?>
