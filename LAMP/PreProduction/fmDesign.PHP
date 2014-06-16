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
		if ($_POST["SubmitType"] == SUBMIT_REF) {
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
if (!isset($_FILES["DesignImage"])) {
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
			$fileDesignImage = $_FILES["DesignImage"];
			if ($fileDesignImage['error'] != UPLOAD_ERR_OK) {
				die(print_r($fileDesignImage, true));
			}
			$uniqueFilename = '';	// UNIQUE
			$highResImage = '';	// 원본 이미지
			$lowResImage = '';	// 웹에 표시되는 이미지
			$thumbnailImage = '';	// "Thumbnails"버튼을 눌렀을 때 나타나는 이미지
			do {
				$uniqueFilename = uniqueFilename();
				$highResImage = '/var/spool/Images/High/Work/Design/' . $uniqueFilename;
			} while (is_file($highResImage));
			$lowResImage = '/var/spool/Images/Low/Work/Design/' . $uniqueFilename . '.png';
			$thumbnailImage = '/var/spool/Images/Thumbnail/Work/Design/' . $uniqueFilename . '.png';
			if (!move_uploaded_file($fileDesignImage['tmp_name'], $highResImage)) {
				die('move_uploaded_file: Failure' . "\n" . print_r($fileDesignImage, true));
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

				// TDESIGN테이블에서 DESIGN_ID가 참조하는 IMAGE_ID를 갱신한다.
				$query = sprintf(
					"UPDATE TDESIGN SET IMAGE_ID = %d WHERE DESIGN_ID = %d",
					$imageId, $_POST['DesignID']
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
			// $imageId, $_POST["Revision"], $fileDesignImage['name'], $uniqueFilename, $aUser[0]
			$query = sprintf(
				"INSERT INTO TWORK_IMAGE (IMAGE_ID, REVISION, FILENAME, FILENAME_UUID, USER_ID) VALUES (%d, %d, '%s', '%s', %d)"
				,
				$imageId, $_POST["Revision"], $fileDesignImage['name'], $uniqueFilename, $aUser[0]
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
<TITLE>디자인 작업</TITLE>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; CHARSET=euc-kr">
</HEAD>
<BODY>
<PRE>
<?PHP
?>
</PRE>
<SCRIPT LANGUAGE="JavaScript1.3">
	location.replace("fmDesign.PHP");
</SCRIPT>
</BODY>
</HTML>
<?PHP
	} else {
/*
 * 2005/11/30 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 데이터베이스에서 필요한 정보를 얻어 JavaScript가 eval해서 gaaDesign, gaRef을 설정하도록 한다.
 */
if (1) {
		global	$gUserId;	// 이 작업자에 할당된 작업을 찾는다.

		global	$JavaScriptEvalCode;
		$JavaScriptEvalCode = '';	// JavaScript가 해석할 문자열
						// 이 문자열은 gaaDesign, gaRef에 대한 정보를 담고 있다.

		// 1. gaaDesign배열을 만든다.
/*
		gaaDesign = [
			new objWorkDesign(
				1,
				"작품 #1",
				[
					new objDesign(
						cDESIGN_CONCEPT,
						1,		// DESIGN_ID
						0,		// SCENE_ID
						"",
						1,		// TARGET_ID
						cTARGET_CHR,
						"인물 A",
						"지시사항: 인물 A",
						1,		// IMAGE_ID
						[
							new objImage(1, "의견 #1.1", "파일이름 #1.1", "UUID(#1.1)"),
							new objImage(2, "의견 #1.2", "파일이름 #1.2", "UUID(#1.2)"),
							new objImage(3, "의견 #1.3", "파일이름 #1.3", "UUID(#1.3)")
						]
					),
					new objDesign(
						cDESIGN_DETAIL,
						2,
						1,
						"장면 #1",
						1,
						cTARGET_BG,
						"배경 X",
						"지시사항: 배경 X",
						2,
						[
							new objImage(1, "의견 #2.1", "파일이름 #2.1", "UUID(#2.1)"),
							new objImage(2, "의견 #2.2", "파일이름 #2.2", "UUID(#2.2)")
						]
					)
				]
			),
			new objWorkDesign(
				2,
				"작품 #2",
				[
					new objDesign(
						cDESIGN_DETAIL,
						3,
						1,
						"장면 #1",
						2,
						cTARGET_CHR,
						"인물 B",
						"지시사항: 인물 B",
						3,
						[
							new objImage(1, "의견 #3.1", "파일이름 #3.1", "UUID(#3.1)"),
							new objImage(2, "의견 #3.2", "파일이름 #3.2", "UUID(#3.2)"),
							new objImage(3, "의견 #3.3", "파일이름 #3.3", "UUID(#3.3)"),
							new objImage(4, "의견 #3.4", "파일이름 #3.4", "UUID(#3.4)")
						]
					)
				]
			)
		];
 */
		// USER_ID에 할당된 작업을 찾는다.
		// 데이터 중에서...
		//	SHOW_ID에 해당하는 작품의 이름을 얻는다.
		//	SCENE_ID에 해당하는 장면의 이름을 얻는다.
		//	TARGET_ID에 해당하는 대상의 유형(인물/배경)과 이름을 얻는다.
		// 다음과 같은 쿼리를 사용하면 한 번에 위의 정보를 얻는다.
		$strUserId = $gUserId ? sprintf("USER_ID = %d AND ", $gUserId) : "";
		$query = sprintf(
			"SELECT T1.SHOW_ID, T4.NAME AS SHOW_NAME, T1.TYPE AS DESIGN_TYPE, DESIGN_ID, T1.SCENE_ID, T5.NAME AS SCENE_NAME, T1.TARGET_ID, T6.TYPE AS TARGET_TYPE, T6.NAME AS TARGET_NAME, YOUR_ORDER, IMAGE_ID FROM TDESIGN AS T1, TWORK AS T2, TWORKER AS T3, TSHOW AS T4, TSCENE AS T5, TTARGET AS T6 WHERE USER_ID = %d AND T3.WORKER_ID = T2.WORK_ID AND T2.WORK_ID = T1.DESIGN_ID AND T1.SHOW_ID = T4.SHOW_ID AND T1.SCENE_ID = T5.SCENE_ID AND T1.TARGET_ID = T6.TARGET_ID ORDER BY T1.SHOW_ID",
			$gUserId
		);
//		echo $query . "\n";
		if (!($resultDesign = mysql_query($query))) {
			$message = '';
			$message .= 'errno: |' . mysql_errno() . '|' . "\n";
			$message .= 'error: |' . mysql_error() . '|' . "\n";
			$message .= 'query: |' . $query . '|' . "\n";
			die($message);
		}
		// 성공적으로 쿼리를 전달했다.
		// SHOW_ID에 따라 행을 정렬하였으므로,
		$lastShowId = 0;		// 가장 최근의 SHOW_ID와
		$lastShowName = "";		// 작품의 이름을 기억해서
		// 작업에 대한 정보를 작품별로 얻도록 한다.
		$aDesignByDesignId = array();	// 작업들에 대한 배열
		$aaDesignByShowId = array();	// $aDesignByDesignId로 이루어진 배열
		$aRef = array();	// 참고자료에 대한 배열
		// 참고자료를 얻기 위하여 (작품, 장면, 대상(인물/배경), (디자인)작업)에 대한 ID를 기억해서
		// 중복되지 않도록 한다.
		// 예를 들어, $aShowId[SHOW_ID]가 설정되어 있다면(isset) 해당 SHOW_ID에 대한
		// 참고자료는 이미 얻었음을 뜻한다.
		$aShowId = array();
		$aSceneId = array();
		$aTargetId = array();
		$aDesignId = array();
		while ($row = mysql_fetch_array($resultDesign, MYSQL_ASSOC)) {
			$showId = $row["SHOW_ID"];
			$sceneId = $row["SCENE_ID"];
			$targetId = $row["TARGET_ID"];
			$designId = $row["DESIGN_ID"];

			// 1. 참고자료에 대한 정보를 얻는다.
			$a = array();	// REF_ID를 얻기위한 쿼리에 사용된다.
			if (!isset($aShowId[$showId])) {	// 작품
				$aShowId[$showId] = 1;		// 이 작품에 대한 참고자료는 얻었다고 표시한다.
				array_push($a, '(CITE_TYPE = ' . CITE_SHOW . ' AND CITE_ID = ' . $showId . ')');
			}
			if ($sceneId && !isset($aSceneId[$sceneId])) {	// 장면
				// SCENE_ID = 0인 경우의 참고자료는 SHOW_ID의 참고자료에 해당하므로,
				// SCENE_ID != 0인 경우에만 장면에 대한 참고자료가 있다.
				$aSceneId[$sceneId] = 1;	// 이 장면에 대한 참고자료는 얻었다고 표시한다.
				array_push($a, '(CITE_TYPE = ' . CITE_SCENE . ' AND CITE_ID = ' . $sceneId . ')');
			}
			if (!isset($aTargetId[$targetId])) {	// 대상(인물/배경)
				$aTargetId[$targetId] = 1;	// 이 대상에 대한 참고자료는 얻었다고 표시한다.
				array_push($a, '(CITE_TYPE = ' . CITE_TARGET . ' AND CITE_ID = ' . $targetId . ')');
			}
			if (!isset($aDesignId[$designId])) {	// (디자인)작업
				$aDesignId[$designId] = 1;	// 이 (디자인)작업에 대한 참고자료는 얻었다고 표시한다.
				array_push($a, '(CITE_TYPE = ' . CITE_DESIGN . ' AND CITE_ID = ' . $designId . ')');
			}
			// 이 작업이 참고하는 자료에 대한 REF_ID를 얻는다.
//			print_r($a);
			$query = 'SELECT REF_ID, CITE_TYPE, CITE_ID FROM TREFERENCE WHERE ' . join(' OR ', $a);
//			echo $query . "\n";
			if (!($resultRefId = mysql_query($query))) {
				$message = '';
				$message .= 'errno: |' . mysql_errno() . '|' . "\n";
				$message .= 'error: |' . mysql_error() . '|' . "\n";
				$message .= 'query: |' . $query . '|' . "\n";
				die($message);
			}
			while ($rowRefId = mysql_fetch_array($resultRefId, MYSQL_ASSOC)) {
				$refId = $rowRefId["REF_ID"];
				$citeType = $rowRefId["CITE_TYPE"];
				$citeId = $rowRefId["CITE_ID"];

				// TREFERENCE_SERIES테이블에서 REF_ID에 해당하는 참고자료에 대한 정보를 얻는다.
				$query = 'SELECT SERIAL, TYPE, CONTENT FROM TREFERENCE_SERIES WHERE REF_ID = ' . $refId . ' AND USER_ID = ' . $gUserId . ' ORDER BY SERIAL';
//				echo $query . "\n";
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
							('"' . preg_replace('/\r\n/', '\\\\n', preg_replace('/([\\\"])/', "\\\\$1", $rowRefSeries["CONTENT"])) . '"') . ')'
					);
				}
				// 사용한 결과를 반환한다.
				mysql_free_result($resultRefSeries);

/*
	new objRef(
		1,		// REF_ID
		cCITE_SHOW,
		1,		// SHOW_ID
		[
			new objRefSeries(1, cREF_IMAGE, "x.tif"),
			new objRefSeries(2, cREF_HTML, "<HTML>\n<HEAD>\n<TITLE>X.HTML</TITLE>\n</HEAD>\n<BODY>\nhello, world\n</BODY>\n</HTML>\n")
		]
	),
 */
				array_push($aRef,
					"\n" .
					"\t" . 'new objRef(' . "\n" .
					"\t\t" . $refId . ',' . "\n" .
					"\t\t" . ($citeType == CITE_SHOW ? 'cCITE_SHOW' : ($citeType == CITE_SCENE ? 'cCITE_SCENE' : ($citeType == CITE_TARGET ? 'cCITE_TARGET' : 'cCITE_DESIGN'))) . ',' . "\n" .
					"\t\t" . $citeId . ',' . "\n" .
					"\t\t" . '[' . join(',', $aRefSeries) . "\n" .
					"\t\t" . ']' . "\n" .
					"\t" . ')'
				);
			}
//			print_r($aRef);
			// 사용한 결과를 반환한다.
			mysql_free_result($resultRefId);

			// 2. (디자인)작업에 대한 정보를 얻는다.
			if ($showId != $lastShowId) {	// SHOW_ID(= $showId)가 기억된 SHOW_ID(= $lastShowId)와 다르다면
				// 작업들에 대한 배열($aDesignByDesignId)을 마무리지어 $aaDesignByShowId에 추가한다.
				if ($lastShowId != 0) {	// 처음에 아닌 경우에 한해서...
					array_push($aaDesignByShowId,
						"\n" .
						"\t" . 'new objWorkDesign(' . "\n" .
						"\t\t" . $lastShowId . ',' . "\n" .
						"\t\t" . ('"' . $lastShowName . '"') . ',' . "\n" .
						"\t\t" . '[' . join($aDesignByDesignId, ',') . "\n" .
						"\t\t" . ']' . "\n" .
						"\t" . ')'
					);
				}
				$lastShowId = $showId;			// $lastShowId와
				$lastShowName = $row["SHOW_NAME"];	// $showName을 바꾸고,
				$aDesignByDesignId = array();		// 작업들에 대한 배열을 새로 시작한다.
			}

			// 작업에 대한 정보를 $aDesignByDesignId에 추가한다.
			// 먼저, 각 (디자인)작업에서 IMAGE_ID에 해당하는 (디자인)작업 결과물에 대한 정보를 얻는다.
			$imageId = $row["IMAGE_ID"];
			$aImage = array();
			if ($imageId) {		// IMAGE_ID != 0인 경우에 한해서...
				$query = 'SELECT REVISION, YOUR_COMMENT, FILENAME, FILENAME_UUID, USER_ID FROM TWORK_IMAGE WHERE IMAGE_ID = ' . $imageId . ' AND USER_ID = ' . $gUserId . ' ORDER BY REVISION';
//				echo $query . "\n";
				if (!($resultImage = mysql_query($query))) {
					$message = '';
					$message .= 'errno: |' . mysql_errno() . '|' . "\n";
					$message .= 'error: |' . mysql_error() . '|' . "\n";
					$message .= 'query: |' . $query . '|' . "\n";
					die($message);
				}
				while ($rowImage = mysql_fetch_array($resultImage, MYSQL_ASSOC)) {
/*
				[
					new objImage(1, "의견 #2.1", "파일이름 #2.1", "UUID(#2.1)"),
					new objImage(2, "의견 #2.2", "파일이름 #2.2", "UUID(#2.2)")
				]
 */
					array_push($aImage,
						"\n" .
						"\t\t\t\t\t" . 'new objImage(' .
							$rowImage["REVISION"] . ', ' .
							('"' . preg_replace('/\r\n/', '\\\\n', preg_replace('/([\\\"])/', "\\\\$1", $rowImage["YOUR_COMMENT"])) . '"') . ', ' .
							('"' . $rowImage["FILENAME"] . '"') . ', ' .
							('"' . $rowImage["FILENAME_UUID"] . '"') . ')'
					);
				}
				// 사용한 결과를 반환한다.
				mysql_free_result($resultImage);
			}
/*
			new objDesign(
				cDESIGN_DETAIL,
				2,
				1,
				"장면 #1",
				1,
				cTARGET_BG,
				"배경 X",
				"지시사항: 배경 X",
				2,
				[
					new objImage(1, "의견 #2.1", "파일이름 #2.1", "UUID(#2.1)"),
					new objImage(2, "의견 #2.2", "파일이름 #2.2", "UUID(#2.2)")
				]
			)
 */
			array_push($aDesignByDesignId,
				"\n" .
				"\t\t\t" . 'new objDesign(' . "\n" .
				"\t\t\t\t" . ($row["DESIGN_TYPE"] == DESIGN_CONCEPT ? 'cDESIGN_CONCEPT' : 'cDESIGN_DETAIL') . ',' . "\n" .
				"\t\t\t\t" . $designId . ',' . "\n" .
				"\t\t\t\t" . $sceneId . ',' . "\n" .
				"\t\t\t\t" . ('"' . $row["SCENE_NAME"] . '"') . ',' . "\n" .
				"\t\t\t\t" . $targetId . ',' . "\n" .
				"\t\t\t\t" . ($row["TARGET_TYPE"] == TARGET_CHR ? 'cTARGET_CHR' : 'cTARGET_BG') . ',' . "\n" .
				"\t\t\t\t" . ('"' . $row["TARGET_NAME"] . '"') . ',' . "\n" .
				"\t\t\t\t" . ('"' . preg_replace('/\r\n/', '\\\\n', preg_replace('/([\\\"])/', "\\\\$1", $row["YOUR_ORDER"])) . '"') . ',' . "\n" .
				"\t\t\t\t" . $imageId . ',' . "\n" .
				"\t\t\t\t" . '[' . join(',', $aImage) . "\n" .
				"\t\t\t\t" . ']' . "\n" .
				"\t\t\t" . ')'
			);
		}
		// 사용한 결과를 반환한다.
		mysql_free_result($resultDesign);
		// 지금까지 모은 $aDesignByDesignId배열도 추가한다.
		if (count($aDesignByDesignId)) {
			array_push($aaDesignByShowId,
				"\n" .
				"\t" . 'new objWorkDesign(' . "\n" .
				"\t\t" . $lastShowId . ',' . "\n" .
				"\t\t" . ('"' . $lastShowName . '"') . ',' . "\n" .
				"\t\t" . '[' . join($aDesignByDesignId, ',') . "\n" .
				"\t\t" . ']' . "\n" .
				"\t" . ')'
			);
		}
		$JavaScriptEvalCode .=
			'gaaDesign = [' . join(',', $aaDesignByShowId) . "\n"
			. '];' . "\n";

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
<TITLE>fmDesign.PHP</TITLE>
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
	var	cTAB_DESIGN	= 1 << 2;
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
			Design: "DESIGN"
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
		var	oRefSeries = gRef.aRefSeries[giSer];
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
		var	oRefSubmitForm = document.createElement('<FORM METHOD="POST" ACTION="fmDesign.PHP" ENCTYPE="MULTIPART/FORM-DATA">');
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
		var	oRefSubmitForm = document.createElement('<FORM METHOD="POST" ACTION="fmDesign.PHP" ENCTYPE="MULTIPART/FORM-DATA">');
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

	// 디자인 작업의 현재 리비젼을 삭제한다.
	function fnDeleteDesignImage() {
		// 다시 한 번 확인한다.
		var	msg = '';
		msg += '디자인 작업의 다음 리비젼을 정말로 삭제해도 됩니까?' + '\n';
		msg += '\n';
		msg += gDesign.aImage[giRev].filename;
		if (!confirm(msg)) {
			return;
		}

		// 데이터를 서버로 보내는 데 필요한 FORM을 만든다.
		var	oDesignSubmitForm = document.createElement('<FORM METHOD="POST" ACTION="fmDesign.PHP" ENCTYPE="MULTIPART/FORM-DATA">');
		document.appendChild(oDesignSubmitForm);

		// Submit Type:
		var	oSubmitType = document.createElement('<INPUT TYPE="TEXT" NAME="SubmitType">');
		oSubmitType.value = cSUBMIT_WORK;
		oDesignSubmitForm.appendChild(oSubmitType);
		// Image ID:
		var	oImageId = document.createElement('<INPUT TYPE="TEXT" NAME="ImageID">');
		oImageId.value = gDesign.imageId;
		oDesignSubmitForm.appendChild(oImageId);
		// Revision #:
		var	oRev = document.createElement('<INPUT TYPE="TEXT" NAME="Revision">');
		oRev.value = gDesign.aImage[giRev].rev;
		oDesignSubmitForm.appendChild(oRev);

		// 모은 데이터를 서버로 보낸다.
		oDesignSubmitForm.submit();
	}

	// 디자인 작업의 결과 이미지를 전송한다.
	function fnDesignSubmit() {
		if (idDesignImage.value == "") {
			alert('작업 그림 파일을 선택해야 합니다');
			return;
		}

		// 데이터를 서버로 보내는 데 필요한 FORM을 만든다.
		var	oDesignSubmitForm = document.createElement('<FORM METHOD="POST" ACTION="fmDesign.PHP" ENCTYPE="MULTIPART/FORM-DATA">');
		document.appendChild(oDesignSubmitForm);

		// Submit Type:
		var	oSubmitType = document.createElement('<INPUT TYPE="TEXT" NAME="SubmitType">');
		oSubmitType.value = cSUBMIT_WORK;
		oDesignSubmitForm.appendChild(oSubmitType);
		// Design ID:		// IMAGE_ID = 0일 때, IMAGE_ID를 새로 등록하고,
					// 이 DESIGN_ID의 IMAGE_ID를 갱신할 때 사용된다.
		var	oDesignId = document.createElement('<INPUT TYPE="TEXT" NAME="DesignID">');
		oDesignId.value = gDesign.designId;
		oDesignSubmitForm.appendChild(oDesignId);
		// Image ID:
		var	oImageId = document.createElement('<INPUT TYPE="TEXT" NAME="ImageID">');
		oImageId.value = gDesign.imageId;
		oDesignSubmitForm.appendChild(oImageId);
		// Revision #:
		var	oRev = document.createElement('<INPUT TYPE="TEXT" NAME="Revision">');
		var	nRev = gDesign.aImage.length;
		oRev.value = nRev ? (gDesign.aImage[nRev - 1].rev + 1) : 1;
		oDesignSubmitForm.appendChild(oRev);
		// Comment(관리자) Or Image Filename(작업자):
		oDesignSubmitForm.appendChild(idDesignImage);

		// 모은 데이터를 서버로 보낸다.
		oDesignSubmitForm.submit();
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/28 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 참고자료(gRef)와 디자인 작업(gDesign)에 대한 정보를 표시하는 공통으로 사용되는 상수와 변수들이다.
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
	var	giRev;		// gDesign중에서 몇 번째 그림을 보여줄 지를 결정한다.
	var	gDesignFlag;	// "ADD"나 "THUMBNAIL"창이 활성될 지를 결정한다.
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
 * 디자인 작업(gDesign) 정보를 표시하는 코드를 구현한다.
 */

	// gDesign에 해당하는 디자인 작업의 정보를 표시한다.
	function fnDisplayDesign() {
		var	oDesign = gDesign;
//		alert(fnGetWorkText(oDesign));

		// gDesign의 aImage중에서 giRev값에 해당하는 그림을 tdDesignImage에 표시한다.
		// gDesignFlag에 따라 필요한 것들을 tblDesignFreeBoard에 표시한다.
		if (giRev < 0 || gDesign.aImage.length <= giRev) {	// 유효하지 않은 giSer이다.
			// (정책) 제일 마지막을 가리키도록 한다.
			giRev = gDesign.aImage.length - 1;
		}

		if (giRev == -1) {	// 보여줄 그림이 없다.
			gbOrder = 1;
			sltbOrder.selectedIndex = 0;
			sltbOrder.disabled = true;

			btnDesignDelete.disabled = true;

			tdDesignImage.innerHTML = '&nbsp;';

			btnDesignBack.disabled = true;
			btnDesignForward.disabled = true;

			btnDesignThumbnail.disabled = true;
			gDesignFlag &= ~cBTN_THUMBNAIL;
			_fnDesignThumbnail(false);

			tdDesignImageDownload.innerHTML = "&nbsp;";
		} else {
			sltbOrder.disabled = false;

			btnDesignDelete.disabled = false;

			fnDisplayDesignImage();

			btnDesignThumbnail.disabled = false;
			if (gDesignFlag & cBTN_THUMBNAIL) {
				if (!_fnDesignThumbnail(true)) {
					alert('_fnDesignThumbnail(): Failure');
				}
			}
		}

		if (gDesignFlag & cBTN_ADD) {
			if (!_fnDesignAdd(true)) {
				alert('_fnDesignAdd(): Failure');
			}
		}
	}
	// gDesign의 aImage중에서 giRev값에 해당하는 그림을 tdDesignImage에 표시한다.
	function fnDisplayDesignImage() {
		var	oImage = gDesign.aImage[giRev];
//		alert(oImage.filenameUuid + ',' + oImage.filename);
		var	img = oImage.filenameUuid;
		var	alt = ' ALT="' + img + '.png' + '"';
		tdDesignImage.align = "LEFT";//"CENTER";
		tdDesignImage.vAlign = "MIDDLE";
		var	tblMainBorder = tblMain.border;
		tdDesignImage.innerHTML = '<IMG SRC="/images/Low/Work/Design/' + img + '.png" HEIGHT="' + cPANEL_HEIGHT + 'px" WIDTH="' + (tdDesignImage.clientWidth - 2 * tblMainBorder) + 'px"' + alt + '>';
		fnDisplayOrderOrComment();

		btnDesignBack.disabled = giRev == 0;
		btnDesignForward.disabled = giRev == gDesign.aImage.length - 1;

		tdDesignImageDownload.innerHTML = '<A HREF="/images/High/Work/Design/' + img + '" TITLE="' + oImage.filename + '">↓</A>';
	}
	// gbOrder에 따라 지시사항/요구사항을 표시한다.
	function fnDisplayOrderOrComment() {
		var	oImage = gDesign.aImage[giRev];
		if (gbOrder) {
			// "지시사항"을 보여준다.
			sltbOrder.selectedIndex = 0;
			trStatus.style.display = "block";

			var	innerHTML = '';
			innerHTML += '<DIV STYLE="overflow-y: scroll; height: ' + (cPANEL_HEIGHT - gcTrbOrderHeight - gcTrStatusHeight) + 'px;">';
			innerHTML += gDesign.order;
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

	function fnDesignDirBtnPressed(iDir) {
		var	iRev = giRev + iDir;
		if (gDesignFlag & cBTN_THUMBNAIL) {
			eval('tdDesignThumbnailRev' + giRev + '.style.border = "solid 1px #66cc00"');
			eval('tdDesignThumbnailRev' + iRev + '.style.border = "solid 1px #ff0000"');
		}
		giRev = iRev;
		fnDisplayDesignImage();
	}

	function _fnDesignThumbnail(bOpen) {
		// 전에 열렸던 행이 있다면 먼저 닫는다.
		// 일반적으로 우리가 원하는 행이 아니다.
		var	rowIndex = fnFindRow(tblDesignFreeBoard, "trDesignThumbnail");
		if (rowIndex >= 0) {
			tblDesignFreeBoard.deleteRow(rowIndex);
		}

		if (!bOpen) {	// 닫아라.
			// 이미 닫았다.
			return rowIndex >= 0 ? true : false;
		}
		// bOpen = true: 열어라.

		var	oTr = tblDesignFreeBoard.insertRow();
		if (!oTr) {
			alert('tblDesignFreeBoard.insertRow(): Failure');
			return false;
		}
		oTr.title = "trDesignThumbnail";

		var	oTd = oTr.insertCell();
		if (!oTd) {
			alert('.insertCell: Failure');
			tblDesignFreeBoard.deleteRow(oTr.rowIndex);
			return false;
		}
		var	innerHTML = '';
		innerHTML += '<TABLE BORDER="0" CELLSPACING="1" CELLPADDING="0"><TBODY>';
		var	iRev = 0;
		var	nRev = gDesign.aImage.length;
		for (var iRow = 0; iRow < Math.ceil(nRev / 4); ++iRow) {
			innerHTML += '<TR>';
			for (var iCol = 0; iCol < 4; ++iCol, ++iRev) {
				var	oImage = gDesign.aImage[iRev];

				var	id;
				var	brdColor;
				var	img;
				var	alt;
				var	onClick;
				if (iRev < nRev) {
					id = ' ID="tdDesignThumbnailRev' + iRev + '"';
					img = '/images/Thumbnail/Work/Design/' + oImage.filenameUuid + '.png';
					alt = ' ALT="' + oImage.filename + '"';
					brdColor = iRev == giRev ? '#ff0000' : '#66cc00';
					onClick = ' onClick="fnDesignThumbnailOnClick(' + iRev + ');"';
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
	function fnToggleDesignThumbnail() {
		gDesignFlag ^= cBTN_THUMBNAIL;
		if (!_fnDesignThumbnail(gDesignFlag & cBTN_THUMBNAIL)) {
			alert('_fnDesignThumbnail(): Failure');
		}
	}
	function fnDesignThumbnailOnClick(iRev) {
		eval('tdDesignThumbnailRev' + giRev + '.style.border = "solid 1px #66cc00"');
		eval('tdDesignThumbnailRev' + iRev + '.style.border = "solid 1px #ff0000"');
		giRev = iRev;
		fnDisplayDesignImage();
	}

	function _fnDesignAdd(bOpen) {
		var	rowIndex = fnFindRow(tblDesignFreeBoard, "trDesignAdd");

		if (!bOpen) {	// 닫아라.
			if (rowIndex >= 0) {
				tblDesignFreeBoard.deleteRow(rowIndex);
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

		var	oTr = tblDesignFreeBoard.insertRow();
		if (!oTr) {
			alert('tblDesignFreeBoard.insertRow(): Failure');
			return false;
		}
		oTr.title = "trDesignAdd";

		var	oTd = oTr.insertCell();
		if (!oTd) {
			alert('.insertCell: Failure');
			tblLtFreeBoard.deleteRow(oTr.rowIndex);
			return false;
		}
		var	innerHTML = '';
		innerHTML += '<TABLE BORDER="1" FRAME="BOX" RULES="NONE" CELLSPACING="0" CELLPADDING="0"><TBODY>';
		innerHTML += '<TR><TD><INPUT ID="idDesignImage" TYPE="FILE" NAME="DesignImage"></TD></TR>';
		innerHTML += '<TR><TD><INPUT TYPE="BUTTON" VALUE="더하기" onClick="fnDesignSubmit();"></TD></TR>';
		innerHTML += '</TD></TR></TBODY></TABLE>';
		oTd.innerHTML = innerHTML;
		return true;
	}
	function fnToggleDesignAdd() {
		gDesignFlag ^= cBTN_ADD;
		if (!_fnDesignAdd(gDesignFlag & cBTN_ADD)) {
			alert('_fnDesignAdd(): Failure');
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
		if (giSer < 0 || gRef.aRefSeries.length <= giSer) {	// 유효하지 않은 giSer이다.
			// (정책) 제일 마지막을 가리키도록 한다.
			giSer = gRef.aRefSeries.length - 1;
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

		if (gRef.citeType == cCITE_DESIGN) {
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
			btnRefAdd.title = '디자인 작업에 관련된 참고자료(파란색)만 등록할 수 있습니다';
			if (gRefFlag & cBTN_ADD) {
				if (!_fnRefAdd(false)) {
					alert('_fnRefAdd(): Failure');
				}
				gRefFlag &= ~cBTN_ADD;
			}

			btnRefDelete.disabled = true;
			btnRefDelete.title = '디자인 작업에 관련된 참고자료(파란색)만 삭제할 수 있습니다';
		}
	}
	// gRef의 aRefSeries중에서 giSer값에 해당하는 참고자료를 tdRefContent에 표시한다.
	function fnDisplayRefContent() {
		var	oRefSeries = gRef.aRefSeries[giSer];
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
			tdRefContent.innerHTML = '<IMG SRC="/images/Low/Ref/' + img + '.png" HEIGHT="' + cPANEL_HEIGHT + 'px" WIDTH="' + (tdRefContent.clientWidth - 2 * tblMain.border) + 'px"' + alt + '>';

			tdRefImageDownload.innerHTML = '<A HREF="/images/High/Ref/' + img + '" TITLE="' + aFilename[1] + '">↓</A>';
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
		var	nSer = gRef.aRefSeries.length;
		var	iSer = 0;
		for (var iRow = 0; iRow < Math.ceil(nSer / 4); ++iRow) {
			innerHTML += '<TR>';
			for (var iCol = 0; iCol < 4; ++iCol, ++iSer) {
				var	oRefSeries = gRef.aRefSeries[iSer];

				var	img;
				if (iSer < nSer) {
					if (oRefSeries.type == cREF_IMAGE) {
						var	aFilename = oRefSeries.content.split(':');
						img = '/images/Thumbnail/Ref/' + aFilename[0] + '.png';
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
		'gDesignId',
		'gCiteType',

		'giSer',
		'gRefFlag',

		'giRev',
		'gDesignFlag',
		'gbOrder',

		'gTabFlag'		// 탭(작품, 참고자료, 디자인 작업)이 열려있는 지를 말해준다.
	);
	function fnSetCookie() {
		var	aVarVal = new Array(gaVarName.length);
		var	cookie = '';
		for (var i = 0; i < gaVarName.length; ++i) {
			aVarVal[i] = eval(gaVarName[i]);
		}
		document.cookie = 'Design.Var=' + aVarVal;
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
			if (aNameVal[0] == "Design.Var") {
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
 * slt[Show|Design|Ref]에 대한 이벤트핸들러를 구현한다.
 */

	// 작품을 선택할 때 실행되는 이벤트핸들러이다.
	function fnSltShowOnChange() {
		gShowId = gaaDesign[sltShow.selectedIndex].showId;
		fnInitShow();
	}

	// 디자인 작업을 선택할 때 실행되는 이벤트핸들러이다.
	function fnSltDesignOnChange() {
		gDesignId = gaDesign[sltDesign.selectedIndex].designId;
		fnInitDesign();
	}

	// 참고자료를 선택할 때 실행되는 이벤트핸들러이다.
	function fnSltRefOnChange() {
		gCiteType = gaRef[gahiRefId[sltRef.selectedIndex].index].citeType;
		fnInitRef();
	}

	// gaaDesign을 이용하여 작품목록을 구성한다.
	function fnAddOptionToSltShow() {
		sltShow.options.length = 0;	// 초기화한다.
		for (var i = 0; i < gaaDesign.length; ++i) {
			var	oOp = document.createElement("OPTION");
			oOp.value = i;
			oOp.text = gaaDesign[i].showName + ' (작업 갯수 = ' + gaaDesign[i].aDesign.length + ')';
			sltShow.options.add(oOp);
		}
	}

	// oDesign에 해당하는 작업에 대한 "작업문장"을 얻는다.
	function fnGetWorkText(oDesign) {
		var	workText = '';
//		workText += '작품("' + gaaDesign[sltShow.selectedIndex].showName + '")';
		if (oDesign.sceneId == 0) {
			workText += '작품 전반에 걸쳐 ' + (oDesign.targetType == cTARGET_CHR ? '등장하는 ' : '사용되는 ');
		} else {
			workText += '특정 장면("' + oDesign.sceneName + '")에 ' + (oDesign.targetType == cTARGET_CHR ? '등장하는 ' : '사용되는 ');
		}
		workText += (oDesign.targetType == cTARGET_CHR ? "인물" : "배경") + '("' + oDesign.targetName + '")에 대한 ';
		workText += (oDesign.designType == cDESIGN_CONCEPT ? "컨셉" : "구체적인") + " 디자인";

		return workText;
	}
	// gaDesign을 이용해서 작업 목록을 구성한다.
	function fnAddOptionsToSltDesign() {
		sltDesign.options.length = 0;	// 초기화한다.
		for (var i = 0; i < gaDesign.length; ++i) {
			var	oDesign = gaDesign[i];
			var	oOp = document.createElement("OPTION");
			oOp.value = i;
			oOp.text = fnGetWorkText(oDesign) + ' (리비젼 갯수 = ' + oDesign.aImage.length + ')';
			sltDesign.options.add(oOp);
		}
	}

	// gahiRefId를 이용해서 참고자료 목록을 구성한다.
	function fnAddOptionsToSltRef() {
		sltRef.options.length = 0;	// 초기화한다.
		for (var i = 0; i < gahiRefId.length; ++i) {
			var	oOp = document.createElement("OPTION");
			oOp.value = i;
			var	oRef = gaRef[gahiRefId[i].index];
			var	citeType = oRef.citeType;
			var	oDesign = gaDesign[sltDesign.selectedIndex];
			var	text;
			switch (citeType) {
			case cCITE_SHOW:
				text = '작품: ' + gaaDesign[sltShow.selectedIndex].showName;
				break;
			case cCITE_SCENE:
				text = '장면: ' + oDesign.sceneName;
				break;
			case cCITE_TARGET:
				text = (oDesign.targetType == cTARGET_CHR ? '인물' : '배경') + ': ' + oDesign.targetName;
				break;
			default:	// cCITE_DESIGN
				text = '작업: ' + fnGetWorkText(oDesign);
				break;
			}
			oOp.text = text + ' (참고자료 갯수 = ' + oRef.aRefSeries.length + ')';
			oOp.style.color = citeType == cCITE_DESIGN ? "blue" : "red";
			sltRef.options.add(oOp);
		}
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/26 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 1. gaaDesign중에서 gShowId에 해당하는 gaDesign를 결정하고,
 * 이를 이용해서 (다자인)작업 목록을 만든다.
 * gaDesign중에서 gDesignId에 해당하는 gDesign을 결정하고,
 * 그 정보를 화면에 표시한다.
 * 2. gaRef중에서 gDesign에 해당하는 gahiRefId를 결정하고,
 * 이를 이용해서 참고자료 목록을 만든다.
 * 이 목록 중에서 gCiteType에 해당하는 gRef를 결정하고,
 * 그 정보를 화면에 표시한다.
 */

	// gDesignId에 해당하는 작업(gDesign)에서 참고하는 자료들에 대한,
	// gaRef에서의 첨자들을 얻어 gahiRefId배열을 결정한다.
	function fnSetahiRefId() {
		gahiRefId = [];
		for (var i = 0; i < gaRef.length; ++i) {
			var	oRef = gaRef[i];
			if (
				(oRef.citeType == cCITE_SHOW && oRef.citeId == gShowId) ||
				(oRef.citeType == cCITE_SCENE && gDesign.sceneId != 0 && oRef.citeId == gDesign.sceneId) ||
				(oRef.citeType == cCITE_TARGET && oRef.citeId == gDesign.targetId) ||
				(oRef.citeType == cCITE_DESIGN && oRef.citeId == gDesign.designId)
			) {
				gahiRefId.push(new objHashIndex(oRef.refId, i));
			}
		}
		return;
		var	aIndex = [];
		for (var i = 0; i < gahiRefId.length; ++i)
			aIndex.push(gahiRefId[i].index);
		alert(aIndex);
	}
	// gCiteType에 해당하는 gRef를 찾는다.
	// [가정] gaRef중에는 적어도 주어진 DESIGN_ID에 대한 참고가 있다.
	// 비록 참조자료가 하나도 없다 할 지라도...
	function fnSetRef() {
		gRef = undefined;
		for (var i = 0; i < gahiRefId.length; ++i) {
			var	oRef = gaRef[gahiRefId[i].index];
			if (gCiteType == oRef.citeType) {
				gRef = oRef;
				break;
			}
		}
		if (gRef == undefined) {
//			alert('gCiteType = ' + gCiteType + '에 해당하는 참고가 없다');
			// (정책) cCITE_DESIGN를 기본으로 한다.
			gCiteType = cCITE_DESIGN;
			for (var i = 0; i < gahiRefId.length; ++i) {
				var	oRef = gaRef[gahiRefId[i].index];
				if (gCiteType == oRef.citeType) {
					gRef = oRef;
					break;
				}
			}
			if (gRef == undefined) {
				alert('gaRef중에서 DESIGN_ID에 대한 참고는 반드시 있어야 한다');
				return;
			}
//			alert('DESIGN_ID에 대한 참조를 기본으로 한다');
		}
	}

	// gaaDesign중에서 gShowId에 해당하는 gaDesign을 결정한다.
	// [가정] 적어도 하나의 작업이 있다. (gaaDesign.length >= 1)
	function fnSetaDesign() {
		gaDesign = undefined;	// 초기화한다.
		for (var i = 0; i < gaaDesign.length; ++i) {
			if (gShowId == gaaDesign[i].showId) {
				// gShowId에 해당하는 objWorkDesisn을 찾았다.
				gaDesign = gaaDesign[i].aDesign;
			}
		}
		if (gaDesign == undefined) {
//			alert('SHOW_ID = ' + gShowId + '에 해당하는 작업이 없다');
			// [가정]으로부터 적어도 하나의 작업은 있다.
			gShowId = gaaDesign[0].showId;
			gaDesign = gaaDesign[0].aDesign;
//			alert('첫 번째 작품(SHOW_ID = ' + gShowId + ')을 기본으로 한다');
		}
	}
	// gaDesign중에서 gDesignId에 해당하는 gDesign을 결정한다.
	// [가정] DB에서 gaaDesign배열을 만들 때,
	// 작품에 작업이 없다면 그 작품은 gaaDesign에 나타날 수 없으므로,
	// gaaDesign배열에 작품이 있다는 것은 적어도 하나의 작업이 있다는 것을 뜻한다.
	// (gaaDesign[i].aDesign.length >= 1)
	function fnSetDesign() {
		// gDesignId에 해당하는 작업을 찾는다.
		gDesign = undefined;	// 초기화한다.
		for (var i = 0; i < gaDesign.length; ++i) {
			if (gaDesign[i].designId == gDesignId) {
				// gDesignId에 해당하는 작업을 찾았다.
				gDesign = gaDesign[i];
				break;
			}
		}
		if (gDesign == undefined) {
//			alert('DESIGN_ID = ' + gDesignId + '에 해당하는 작업이 없다');
			gDesign = gaDesign[0];
			gDesignId = gDesign.designId;
//			alert('첫 번째 작업(DESIGN_ID = ' + gDesignId + ')을 기본으로 한다');
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

	// citeType
	var	cCITE_SHOW	= <?PHP echo CITE_SHOW ?>;
	var	cCITE_SCENE	= <?PHP echo CITE_SCENE ?>;
	var	cCITE_TARGET	= <?PHP echo CITE_TARGET ?>;
	var	cCITE_DESIGN	= <?PHP echo CITE_DESIGN ?>;
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
	// 디자인 작업에 대한 정보를 담는다.
	function objDesign(designType, designId, sceneId, sceneName, targetId, targetType, targetName, order, imageId, aImage) {
		this.designType = designType;	// (DESIGN_)TYPE
						//	cDESIGN_DETAIL: 구체적인 디자인, cDESIGN_CONCEPT: 컨셉 디자인
		this.designId = designId;	// DESIGN_ID
		this.sceneId = sceneId;		// SCENE_ID; SCENE_ID = 0인 경우에는 작품 전체에 쓰이는 디자인
		this.sceneName = sceneName;	// 표시를 위해서 사용된다.
		this.targetId = targetId;	// TARGET_ID
		// TARGET_ID를 통해서 얻어지는 다음 두 property는 표시를 위해서 사용된다.
		this.targetType = targetType;	// (TARGET_)TYPE
						//	cTARGET_CHR: 인물, cTARGET_BG: 배경
		this.targetName = targetName;

		this.order = order;		// 지시사항

		this.imageId = imageId;
		this.aImage = aImage;
	}
	function objWorkDesign(showId, showName, aDesign) {
		this.showId = showId;
		this.showName = showName;	// 표시를 위해서 사용된다.
		this.aDesign = aDesign;
	}
	var	gaaDesign;	// objWorkDesign으로 이루어진 배열
	var	gShowId;	//
	var	gaDesign;	// gaaDesign중에서 gShowId에 해당하는 objWorkDesign오브젝트의 aDesign
	var	gDesignId;	//
	var	gDesign;	// gaDesign중에서 gDesignId에 해당하는 objDesign

	// 참고자료에 대한 일련의 구체적인 정보를 담는다.
	function objRefSeries(serial, type, content) {
		this.serial = serial;
		this.type = type;		// (cREF_IMAGE, cREF_HTML)
		this.content = content;		// cREF_IMAGE: 그림파일의 FILENAME_UUID, cREF_HTML: HTML문서의 내용
	}
	// 참고자료에 대한 전반적인 정보를 담는다.
	function objRef(refId, citeType, citeId, aRefSeries) {
		this.refId = refId;		// REF_ID
		this.citeType = citeType;	// (cCITE_SHOW, cCITE_SCENE, cCITE_TARGET, cCITE_DESIGN)
		this.citeId = citeId;		// cCITE_SHOW: SHOW_ID, cCITE_SCENE: SCENE_ID, cCITE_TARGET: TARGET_ID, cCITE_DESIGN: DESIGN_ID
		this.aRefSeries = aRefSeries;
	}
	var	gaRef;		// objRef로 이루어진 배열
	var	gahiRefId;	// gaRef중에서 gRef로 사용되는 objRef들을 결정하는 objHashIndex로 이루어진 배열
				// gDesign이 결정되어야 gahiRefId가 결정된다.
				// 자세한 것은 fnSetahiRefId함수를 참고하라.
	var	gCiteType;	// cCITE_SHOW, cCITE_SCENE, cCITE_TARGET, cCITE_DESIGN
				// 작업의 SCENE_ID = 0이면 cCITE_SCENE대신에 cCITE_SHOW가 사용된다.
	var	gRef;		// gahiRefId이 결정하는 objRef중에서 gCiteType에 해당하는 objRef 

	// 작품을 선택했을 때 행하는 초기화 과정이다.
	function fnInitShow() {
		// gaaDesign중에서 gShowId에 해당하는 gaDesign을 결정한다.
		fnSetaDesign();
		// 작품 목록에서 gShowId에 해당하는 작품을 선택한다.
		for (var i = 0; i < sltShow.options.length; ++i) {
			sltShow.options[i].selected = gShowId == gaaDesign[i].showId;
		}

		// gaDesign의 정보를 이용해서 디자인 작업 목록을 만든다.
		fnAddOptionsToSltDesign();

		// gDesignId에 해당하는 디자인 작업을 선택했을 때 필요한 초기화를 실시한다.
		fnInitDesign();
	}
	// gDesignId에 해당하는 디자인 작업을 선택했을 때 행하는 초기화 과정이다.
	function fnInitDesign() {
		// gaDesign중에서 gDesignId에 해당하는 gDesign을 결정한다.
		fnSetDesign();
		// 디자인 작업 목록에서 gDesignId에 해당하는 작업을 선택한다.
		for (var i = 0; i < sltDesign.options.length; ++i) {
			sltDesign.options[i].selected = gDesignId == gaDesign[i].designId;
		}

		// gDesign정보를 화면에 표시한다.
		fnDisplayDesign();

		// gDesign을 바탕으로 gahiRefId배열을 결정한다.
		fnSetahiRefId();
		// gahiRefId의 정보를 이용해서 참고자료 목록을 만든다.
		fnAddOptionsToSltRef();

		// gCiteType에 해당하는 참고자료를 선택했을 때 필요한 초기화를 실시한다.
		fnInitRef();
	}
	// gCiteType에 해당하는 참고자료를 선택했을 때 행하는 초기화 과정이다.
	function fnInitRef() {
		// gCiteType에 해당하는 gRef를 찾는다.
		fnSetRef();
		// 참고자료 목록에서 gCiteType에 해당하는 참고자료를 선택한다.
		for (var i = 0; i < sltRef.options.length; ++i) {
			var	oOp = sltRef.options[i];
			oOp.selected = gCiteType == gaRef[gahiRefId[i].index].citeType;
		}

		// gRef정보를 화면에 표시한다.
		fnDisplayRef();
	}

	function fnSetData() {
if (1) {
		var	JavaScriptEvalCode = "<?PHP echo $JavaScriptEvalCode ?>";
//		alert(JavaScriptEvalCode);
		eval(JavaScriptEvalCode);
} else {
		gaaDesign = [
			new objWorkDesign(
				1,
				"작품 #1",
				[
					new objDesign(
						cDESIGN_CONCEPT,
						1,		// DESIGN_ID
						0,		// SCENE_ID
						"",
						1,		// TARGET_ID
						cTARGET_CHR,
						"인물 A",
						"지시사항: 인물 A",
						1,		// IMAGE_ID
						[
							new objImage(1, "의견 #1.1", "파일이름 #1.1", "UUID(#1.1)"),
							new objImage(2, "의견 #1.2", "파일이름 #1.2", "UUID(#1.2)"),
							new objImage(3, "의견 #1.3", "파일이름 #1.3", "UUID(#1.3)")
						]
					),
					new objDesign(
						cDESIGN_DETAIL,
						2,
						1,
						"장면 #1",
						1,
						cTARGET_BG,
						"배경 X",
						"지시사항: 배경 X",
						2,
						[
							new objImage(1, "의견 #2.1", "파일이름 #2.1", "UUID(#2.1)"),
							new objImage(2, "의견 #2.2", "파일이름 #2.2", "UUID(#2.2)")
						]
					)
				]
			),
			new objWorkDesign(
				2,
				"작품 #2",
				[
					new objDesign(
						cDESIGN_DETAIL,
						3,
						1,
						"장면 #1",
						2,
						cTARGET_CHR,
						"인물 B",
						"지시사항: 인물 B",
						3,
						[
							new objImage(1, "의견 #3.1", "파일이름 #3.1", "UUID(#3.1)"),
							new objImage(2, "의견 #3.2", "파일이름 #3.2", "UUID(#3.2)"),
							new objImage(3, "의견 #3.3", "파일이름 #3.3", "UUID(#3.3)"),
							new objImage(4, "의견 #3.4", "파일이름 #3.4", "UUID(#3.4)")
						]
					)
				]
			)
		];

		gaRef = [
			// SHOW_ID에 해당하는 작품에서 참고하는 자료
			new objRef(
				1,		// REF_ID
				cCITE_SHOW,
				1,		// SHOW_ID
				[
					new objRefSeries(1, cREF_IMAGE, "x.tif"),
					new objRefSeries(2, cREF_HTML, "<HTML>\n<HEAD>\n<TITLE>X.HTML</TITLE>\n</HEAD>\n<BODY>\nhello, world\n</BODY>\n</HTML>\n")
				]
			),

			// SCENE_ID에 해당하는 장면에서 참고하는 자료
			new objRef(
				2,
				cCITE_SCENE,
				1,		// SCENE_ID
				[
				]
			),

			// TARGET_ID에 해당하는 대상(인물/배경)에서 참고하는 자료
			new objRef(
				3,
				cCITE_TARGET,
				1,		// TARGET_ID
				[
				]
			),

			// DESIGN_ID에 해당하는 작업에서 참고하는 자료
			// DESIGN_ID마다 있어야 한다.
			new objRef(
				3,
				cCITE_DESIGN,
				1,		// DESIGN_ID
				[
				]
			),
			new objRef(
				3,
				cCITE_DESIGN,
				2,
				[
				]
			),
			new objRef(
				3,
				cCITE_DESIGN,
				3,
				[
				]
			)
		];
}
	}

	function fnSetVar() {
		// gShowId: number
		gShowId = parseInt(gShowId);
		if (isNaN(gShowId)) {		// 정의되지 않았거나 숫자가 아닌 경우
			// (정책) 기본값은 0으로 아무런 작품도 선택하지 않은 상태이다.
			gShowId = 0;
		}
		// gShowId의 유효성 검사는 fnSetaDesign함수에서 이루어진다.

		// gDesignId: number
		gDesignId = parseInt(gDesignId);
		if (isNaN(gDesignId)) {	// 정의되지 않았거나 숫자가 아닌 경우
			// (정책) 기본값은 0으로 아무런 작업도 선택하지 않은 상태이다.
			gDesignId = 0;
		}
		// gDesignId의 유효성 검사는 fnSetDesign함수에서 이루어진다.

		// gCiteType: number ∈ (cCITE_SHOW, cCITE_SCENE, cCITE_TARGET, cCITE_DESIGN)
		gCiteType = parseInt(gCiteType);
		if (
			isNaN(gCiteType) ||	// 정의되지 않았거나 숫자가 아닌 경우
			!(gCiteType == cCITE_SHOW || gCiteType == cCITE_SCENE || gCiteType == cCITE_TARGET || gCiteType == cCITE_DESIGN)
						// 유효하지 않은 경우
		) {
			// (정책) 디자인 작업에 관한 참고자료가 선택된 것으로 한다.
			gCiteType = cCITE_DESIGN;
		}

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
		// giRev의 유효성 검사는 fnDisplayDesign함수에서 이루어진다.

		// gDesignFlag: number
		if (
			isNaN(parseInt(gDesignFlag))	// 숫자가 아닌 경우
		) {
			// (정책) 아무 플래그도 설정되지 않았다.
			gDesignFlag = 0;
		}
		// cBTN_ADD와 cBTN_THUMBNAIL만 그대로 둔다.
		gDesignFlag &= cBTN_ADD | cBTN_THUMBNAIL;

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
		// (cTAB_SHOW, cTAB_REF, cTAB_DESIGN)만이 가능하다.
		gTabFlag &= cTAB_SHOW | cTAB_REF | cTAB_DESIGN;
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

		if (gaaDesign.length == 0) {
			alert('작업이 하나도 없으므로 더 이상 진행할 수 없다');
			return;
		}
		// 작품 목록을 구성한다.
		fnAddOptionToSltShow();

		gcTrbOrderHeight = trbOrder.clientHeight;
		// gShowId에 해당하는 작품을 선택했을 때의 초기화과정을 수행한다.
		tdRefContent.style.height = (cPANEL_HEIGHT + 2 * tblMain.border) + 'px';
		// "trStatus"가 차지하는 높이를 얻는다.
		gcTrStatusHeight = trStatus.clientHeight;
		tdDesignImage.style.height = (cPANEL_HEIGHT + 2 * tblMain.border) + 'px';

		fnInitShow();

		// gTabFlag에 따라 탭의 상태를 나타낸다.
		// 각 SELECT의 목록이 선택되어 있어야 하므로 fnInitShow다음에 호출한다.
		_fnSetTab();
	}

	function fnOnUnload() {
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
	<TR><TD ID="tdDesign" WIDTH="100%" ALIGN="CENTER" STYLE="background: #fffbf0; cursor: hand;" onClick="fnTdOnClick(this);">디자인 작업</TD></TR>
	<TR><TD><SELECT ID="sltDesign" SIZE="4" STYLE="width: 800px;" onChange="fnSltDesignOnChange();"></SELECT></TD></TR>
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
	<TD WIDTH="35%" ID="tdDesignImage" TITLE="tdDesignImage" VALIGN="TOP">&nbsp;</TD>
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
	<TD ALIGN="CENTER"><TABLE WIDTH="100%" BORDER="0" CELLSPACING="0" CELLPADDING="0"><TBODY><TR ID="trDesignButton" ALIGN="CENTER">
		<TD ID="tdDesignImageDownload" WIDTH="20px">&nbsp;</TD>
		<TD><INPUT ID="btnDesignAdd" TYPE="BUTTON" VALUE="Add" onClick="fnToggleDesignAdd();"></TD>
		<TD><INPUT ID="btnDesignDelete" TYPE="BUTTON" VALUE="Del" onClick="fnDeleteDesignImage();"></TD>
		<TD><INPUT ID="btnDesignBack" TYPE="BUTTON" VALUE="←" onClick="fnDesignDirBtnPressed(-1);"></TD>
		<TD><INPUT ID="btnDesignForward" TYPE="BUTTON" VALUE="→" onClick="fnDesignDirBtnPressed(+1);"></TD>
		<TD><INPUT ID="btnDesignThumbnail" TYPE="BUTTON" VALUE="Thumbnails" onClick="fnToggleDesignThumbnail();"></TD>
	</TR></TBODY></TABLE></TD>
	<TD>&nbsp;</TD>
</TR>
<TR>
	<TD VALIGN="TOP" COLSPAN="2"><TABLE ID="tblRefFreeBoard" TITLE="tblRefFreeBoard" WIDTH="100%" STYLE="border: solid 2px #cc0066;" CELLSPACING="0" CELLPADDING="0"><TBODY>
	</TBODY></TABLE></TD>
	<TD VALIGN="TOP" COLSPAN="2"><TABLE ID="tblDesignFreeBoard" TITLE="tblDesignFreeBoard" WIDTH="100%" STYLE="border: solid 2px #66cc00;" CELLSPACING="0" CELLPADDING="0"><TBODY>
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
