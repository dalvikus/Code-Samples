<?PHP
/*
 * 2005/11/29 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 작품, 장면, 대상(인물/배경)에 대한 참고자료 등록에 필요한 REF_ID를 만든다.
 */

	define("CITE_SHOW", 1);		// 작품에서 참고한다.
	define("CITE_SCENE", 2);	// 장면에서 참고한다.
	define("CITE_TARGET", 3);	// 대상(인물/배경)에서 참고한다.
	// 2005/12/6 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
	// 디자인 작업과 스토리보드 작업을 분리한다.
	define("CITE_DESIGN", 4);	// WORK_ID(= DESIGN_ID)에 해당하는 (디자인)작업에서 참고한다.
	define("CITE_STORYBOARD", 5);	// WORK_ID(= STORYBOARD_ID)에 해당하는 (스토리보드)작업에서 참고한다.
?>
<?PHP
/*
 * 2005/11/13 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 작업 일정표에 각 작업에 상태에 따라 작업 칸에 색깔을 넣는다.
 *
 * 작품별로 작업에 관한 관리를 할 수 있다.
 */
	define("BGCOLOR_INSERT", "blue");	// 새로 추가한 작업
	define("BGCOLOR_SELECT", "green");	// DB에서 가져온 작업
	define("BGCOLOR_UPDATE", "red");	// DB에서 가져와 수정한 작업
?>
<?PHP
/*
 * 2005/11/12 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 작업을 수정하는 UI를 구현한다.
 *
 * 날짜표현을 다시 제대로 수정한다.
 */
?>
<?PHP
/*
 * 2005/11/11 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 중복되는 작업이 등록되지 않도록 TDESIGN과 TSTORYBOARD테이블을 수정한다.
 * 디자인 작업: (SHOW_ID, SCENE_ID, DESIGN_TYPE, TARGET_ID)의 조합은 UNIQUE하다.
 * 스토리보드 작업: (SCENE_ID는 UNIQUE하다.
 * SCENE_ID에 SHOW_ID가 포함되어 있으므로 SCENE_ID의 UNIQUENESS만 따져도 된다.
 * 하지만 디자인 작업의 경우에는 SCENE_ID = 0이 경우가 있어 SHOW_ID까지 포함했다.
 *
 * 삭제된 작업을 해당 테이블에서 삭제한다.
 *
 * 작업을 추가하는 경우, 작업의 종류가 지존에 지웠졌던 작업과 같은 같은 지 확인한다.
 * 같다면 지워졌든 작업을 되살려 데이테베이스에 해당 작업에 수정한다.
 * 지우고 새로 만드는 것보다 효율적이다.
 *
 * 수정된 작업의 경우에 해당 테이블을 수정한다.
 */
?>
<?PHP
	// ->workType
	define("WORK_DESIGN", 1);	// 디자인 작업
	define("WORK_STORYBOARD", 2);	// 스토리보드 작업
	// -> designType
	define("DESIGN_CONCEPT", 1);	// 컨셉 디자인
	define("DESIGN_DETAIL", 2);	// 구체적인 디자인
	// ->targetType
	define("TARGET_CHR", 1);	// 인물
	define("TARGET_BG", 2);		// 배경

	// ->flag
	define("FLAG_DB",	1 << 0);	// 웹에서 다루는 데이터가 DB로부터 왔는 지를 알려준다.
	define("FLAG_DELETED",	1 << 1);	// DB로부터 가져온 데이터가 지워졌는 지를 알려준다.
	define("FLAG_SCENE",	1 << 2);	// 장면이 바뀌었는 지를 알려준다.
	define("FLAG_WORKER",	1 << 3);	// 작업자가 바뀌었는 지를 알려준다.
	define("FLAG_BEGIN",	1 << 4);	// 작업을 시작하는 날짜가 바뀌었는 지를 알려준다.
	define("FLAG_END",	1 << 5);	// 작업을 끝내는 날짜가 바귀었는 지를 알려준다.
	define("FLAG_ORDER",	1 << 6);	// 작업의 지시사항이 바뀌었는 지를 알려준다.
	define("FLAG_STATUS",	1 << 7);	// 작업의 진행상황이 바뀌었는 지를 알려준다.
	define("FLAG_DESIGN",	1 << 8);	// 디자인 유형(DESIGN_CONCEPT/DESIGN_DETAIL)이 바뀌었는 지 알려준다.
	define("FLAG_TARGET",	1 << 9);	// 대상의 유형(인물/배경) 바뀌었는 지 말해준다.
	define("FLAG_TARGETID",	1 << 10);	// 대상(인물/배경)이 바뀌었는 지 말해준다.
?>
<?PHP
/*
 * 2005/11/10 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 작업에 필요한 정보(장면, 인물/배경)에 대한 정보를 저장한 클래스를 만든다.
 * fmShow.PHP에 사용한 클래스의 축소형이다.
 * 아래의 JavaScript에서 사용하는 오브젝트인 oBase, oScene, oChrBg, oShow를 참고하라.
 */
	class TargetBase {
		var	$id;		// 0: 새로 추가되는 경우, >= 1: 고치는 경우 (장면: SCENE_ID, 인물/배경: TARGET_ID)
		var	$name;		// 장면또는 인물/배경의 이름
	};

	class TargetScene extends TargetBase {
		// 현재까지 따로 추가할 것이 없다.

		function TargetScene($id, $name) {
			$this->id = $id;
			$this->name = $name;
		}
	};

	class TargetChrBg extends TargetBase {
		var	$aSceneId;	// 인물/배경이 등장하는 장면의 SCENE_ID이다.

		function TargetChrBg($id, $name, $aSceneId) {
			$this->id = $id;
			$this->name = $name;

			$this->aSceneId = $aSceneId;
		}
	};

	class Show {
		var	$aScn;		// 장면에 대한 정보를 담고 있고, TargetScene오브젝트들의 배열이다.
		var	$aChr;		// 인물에 대한 정보를 담고 있고, TargetChrBg오브젝트들의 배열이다.
		var	$aBg;		// 배경에 대한 정보를 담고 있고, TargetChrBg오브젝트들의 배열이다.

		function Show($aScn, $aChr, $aBg) {
			$this->aScn = $aScn;
			$this->aChr = $aChr;
			$this->aBg = $aBg;
		}
	};
?>
<?PHP
	class WorkBase {
		var	$workType;	// 1: 디자인, 2: 스토리보드
		var	$workId;	// WORK_ID
					// 0: 새로 추가되는 경우, >= 1: 고치는 경우 (디자인: DESIGN_ID, 스토리보드: STORYBOARD_ID)
		var	$flag;		// 고치는 경우(즉 $workId >= 1)에 아래의 어느 부분이 바뀌었는 지를 말해준다.
					// 새로 추가하는 경우($workId = 0)에는 이 값은 사용되지 않는다.

		var	$sceneId;	// 인물/배경이 등장하는 작면
		var	$aUserId;	// 작업자를 구성하는, USER_ID로 이루어진 배열
		var	$date_begin;	// 작업을 시작하는 날
		var	$date_end;	// 작업을 끝내는 날
		var	$order;		// 작업에 대한 지시사항
		var	$status;	// 작업의 진행상황
	};

	class WorkDesign extends WorkBase {
		var	$designType;	// enum(DESIGN_CONCEPT, DESIGN_DETAIL)
		var	$targetType;	// enum(TARGET_CHR, TARGET_BG)
		var	$targetId;	// TARGET_ID

		function WorkDesign($workId, $flag, $sceneId, $aUserId, $date_begin, $date_end, $order, $status, $designType, $targetType, $targetId) {
			$this->workType = WORK_DESIGN;

			$this->workId = $workId;
			$this->flag = $flag;

			$this->sceneId = $sceneId;
			$this->aUserId = $aUserId;
			$this->date_begin = $date_begin;
			$this->date_end = $date_end;
			$this->order = $order;
			$this->status = $status;

			$this->designType = $designType;
			$this->targetType = $targetType;
			$this->targetId = $targetId;
		}
	};

	class WorkStoryboard extends WorkBase {
		// 현재까지 따로 추가할 것이 없다.

		function WorkStoryboard($workId, $flag, $sceneId, $aUserId, $date_begin, $date_end, $order, $status) {
			$this->workType = WORK_STORYBOARD;

			$this->workId = $workId;
			$this->flag = $flag;

			$this->sceneId = $sceneId;
			$this->aUserId = $aUserId;
			$this->date_begin = $date_begin;
			$this->date_end = $date_end;
			$this->order = $order;
			$this->status = $status;
		}
	};
?>
<?PHP
	function displayWork($aaWork) {
		foreach ($aaWork as $showId => $aWork) {
			echo 'SHOW_ID: |' . $showId . '|' . "\n";
			echo '# of 작업: ' . count($aWork) . "\n";
			foreach ($aWork as $oWork) {
				$workId = $oWork->workId;

				echo "\t" . '--------------------------------' . "\n";
				echo "\t" . 'type: ' . ($oWork->workType == WORK_DESIGN ? '디자인' : '스토리보드') . "\n";
				echo "\t" . 'id: |' . $workId . '|' . "\n";
				echo "\t" . 'flag: |' . $oWork->flag . '|' . "\n";

				echo "\t" . 'SCENE_ID: |' . $oWork->sceneId . '|' . "\n";
				echo "\t" . 'USER_ID: ' . '(|' . join('|, |', $oWork->aUserId[0]) . '|) -> (|' . join('|, |', $oWork->aUserId[1]) . '|)' . "\n";
				echo "\t" . '작업 기간: |' . $oWork->date_begin . '| - |' . $oWork->date_end . '|' . "\n";
				echo "\t" . '지시사항: |' . $oWork->order . '|' . "\n";
				echo "\t" . '진행상황: |' . $oWork->status . '|' . "\n";
				if ($oWork->workType == WORK_DESIGN) {
					echo "\t" . '디자인 유형: ' . ($oWork->designType == DESIGN_CONCEPT ? '컨셉 디자인' : '구체적인 디자인') . "\n";
					echo "\t" . '대상: ' . ($oWork->targetType == TARGET_CHR ? '인물' : '배경') . "\n";
					echo "\t" . 'TARGET_ID: |' . $oWork->targetId . '|' . "\n";
				}
				echo "\t" . '--------------------------------' . "\n";
				if ($workId == 0) {	// 새로운 작업을 등록한다.
					echo "\t" . '새로운 작업을 추가한다.' . "\n";
					// 1. 새로운 작업을 등록하고 WORK_ID를 얻는다.
					$query = sprintf(
						"INSERT INTO TWORK (TYPE, SHOW_ID, SCENE_ID, DATE_BEGIN, DATE_END, YOUR_ORDER, STATUS) VALUES (%d, %d, %d, '%s', '%s', '%s', %d)"
						,
						$oWork->workType == WORK_DESIGN ? 1 : 2,
						$showId,
						$oWork->sceneId,
						mysql_real_escape_string($oWork->date_begin), mysql_real_escape_string($oWork->date_end),
						mysql_real_escape_string($oWork->order),
						$oWork->status
					);
					echo "\t" . 'query: |' . $query . '|' . "\n";
					if (!mysql_query($query)) {
						$message = '';
						$message .= 'errno: |' . mysql_errno() . '|' . "\n";
						$message .= 'error: |' . mysql_error() . '|' . "\n";
						$message .= 'query: |' . $query . '|' . "\n";
						die($message);
					}
					// 작업의 WORK_ID를 얻는다.
					$query = "SELECT LAST_INSERT_ID()";
					if (!($result = mysql_query($query))) {
						$message = '';
						$message .= 'errno: |' . mysql_errno() . '|' . "\n";
						$message .= 'error: |' . mysql_error() . '|' . "\n";
						$message .= 'query: |' . $query . '|' . "\n";
						die($message);
					}
					if (!($row = mysql_fetch_array($result, MYSQL_NUM))) {
						die('Cannot get SHOW_ID');
					}
					$workId = $row[0];
					mysql_free_result($result);

					// 작업에 대한 참고자료 등록에 필요한 REF_ID를 만든다.
					//	2005/11/29 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
					$query = 'INSERT INTO TREFERENCE (CITE_TYPE, CITE_ID) VALUES (' . ($oWork->workType == WORK_DESIGN ? CITE_DESIGN : CITE_STORYBOARD) . ', ' . $workId . ')';
					if (!mysql_query($query)) {
						$message = '';
						$message .= 'errno: |' . mysql_errno() . '|' . "\n";
						$message .= 'error: |' . mysql_error() . '|' . "\n";
						$message .= 'query: |' . $query . '|' . "\n";
						die($message);
					}

					// 2. 작업자를 등록한다.
					$query = 'INSERT INTO TWORKER (WORKER_ID, USER_ID) VALUES ' . '(' . $workId . ', ' . join('), (' . $workId . ', ', $oWork->aUserId[1]) . ')';
					echo "\t" . 'query: |' . $query . '|' . "\n";
					if (!mysql_query($query)) {
						$message = '';
						$message .= 'errno: |' . mysql_errno() . '|' . "\n";
						$message .= 'error: |' . mysql_error() . '|' . "\n";
						$message .= 'query: |' . $query . '|' . "\n";
						// 방금 전에 등록한 작업도 삭제한다.
						echo '방금 전에 등록한 작업(WORK_ID = ' . $workId . ')도 삭제한다.' . "\n";
						$query = 'DELETE FROM TWORK WHERE WORK_ID = ' . $workId;
						if (!mysql_query($query)) {
							$message .= '방금 전에 등록한 작업을 삭제할 수도 없었다.' . "\n";
							$message .= '손으로 직접 삭제해야 한다.' . "\n";
							$message .= 'errno: |' . mysql_errno() . '|' . "\n";
							$message .= 'error: |' . mysql_error() . '|' . "\n";
							$message .= 'query: |' . $query . '|' . "\n";
							die($message);
						}
						die($message);
					}
					// 3. 주어진 WORK_ID를 디자인/스토리보드 테이블에 등록한다.
					if ($oWork->workType == WORK_DESIGN) {	// 디자인 작업
						$query = 'INSERT INTO TDESIGN (DESIGN_ID, SHOW_ID, SCENE_ID, TARGET_ID, TYPE) VALUES (' . $workId . ', ' . $showId . ', ' . $oWork->sceneId . ', ' . $oWork->targetId . ', ' . $oWork->designType . ')';
					} else {				// 스토리보드 작업
						$query = 'INSERT INTO TSTORYBOARD (STORYBOARD_ID, SCENE_ID) VALUES (' . $workId . ', ' . $oWork->sceneId . ')';
					}
					echo "\t" . 'query: |' . $query . '|' . "\n";
					if (!mysql_query($query)) {
						$message = '';
						$message .= 'errno: |' . mysql_errno() . '|' . "\n";
						$message .= 'error: |' . mysql_error() . '|' . "\n";
						$message .= 'query: |' . $query . '|' . "\n";
						// 방금 전에 등록한 작업도 삭제한다.
						echo '방금 전에 등록한 작업(WORK_ID = ' . $workId . ')도 삭제한다.' . "\n";
						$query = 'DELETE FROM TWORK WHERE WORK_ID = ' . $workId;
						if (!mysql_query($query)) {
							$message .= '방금 전에 등록한 작업을 삭제할 수도 없었다.' . "\n";
							$message .= '손으로 직접 삭제해야 한다.' . "\n";
							$message .= 'errno: |' . mysql_errno() . '|' . "\n";
							$message .= 'error: |' . mysql_error() . '|' . "\n";
							$message .= 'query: |' . $query . '|' . "\n";
							die($message);
						}
						die($message);
					}

				if ($oWork->workType == WORK_STORYBOARD) {	// 스토리보드 작업의 경우
					// 적어도 하나의 컷을 만든다.
					//	2005/12/5 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
					$query = 'INSERT INTO TCUT (STORYBOARD_ID, SERIAL, IMAGE_ID) VALUES (' . $workId . ', 1, 0)';
					if (!mysql_query($query)) {
						$message = '';
						$message .= 'errno: |' . mysql_errno() . '|' . "\n";
						$message .= 'error: |' . mysql_error() . '|' . "\n";
						$message .= 'query: |' . $query . '|' . "\n";
						die($message);
					}
				}

					echo "\t" . '새로운 작업(WORK_ID = ' . $workId . ')을 성공적으로 추가했다.' . "\n";
				} else if ($oWork->flag & FLAG_DELETED) {	// 기존의 작업을 삭제한다.
					echo "\t" . '기존 작업(WORK_ID = ' . $workId . ')을 삭제한다.' . "\n";
					// 1. TWORKER테이블에서 작업자를 지운다. (WORKER_ID = WORK_ID)
					// 2. TDESIGN이나 TSTORYBOARD에서 해당 작업을 지운다. (DESIGN_ID = WORK_ID 또는 STORYBOARD_ID = WORK_ID)
					// 작업에 대한 결과가 있을 때 즉
					// 디자인(스토리보드) 작업의 경우 TDESIGN(TSTORYBOARD_CUT)테이블의 IMAGE_ID가 0이 아닌 경우에는
					// (정책) 해당 이미지는 그대로 둔다.
					// 3. TWORK테이블에서 WORK_ID에 해당하는 작업을 지운다.
					// TWORKER테이블의 WORKER_ID와
					// TDESIGN/TSTORYBOARD테이블의 DESIGN_ID/STORYBOARD_ID는
					// TWORK테이블의 WORK_ID에 해당하는 FOREIGN KEY로 설정되었으므로
					// TWORK테이블에서 WORK_ID에 해당하는 행을 지우면 나머지도 자동적으로 지워진다.
					$query = 'DELETE FROM TWORK WHERE WORK_ID = ' . $workId;
					if (!($result = mysql_query($query))) {
						$message = '';
						$message .= 'errno: |' . mysql_errno() . '|' . "\n";
						$message .= 'error: |' . mysql_error() . '|' . "\n";
						$message .= 'query: |' . $query . '|' . "\n";
						die($message);
					}
					echo "\t" . '기존 작업(WORK_ID = ' . $workId . ')을 성공적으로 삭제했다.' . "\n";
				} else {			// 기존의 작업을 수정한다.
					// $oWork->flag를 참고해서 수정할 것을 결정한다.
					$flag = $oWork->flag;
					if (!($flag & ~FLAG_DB)) {	// FLAG_DB비트를 제외한 어떤 비트도 설정되지 않았다면
						// 작업에 어떤 변화도 없다.
						echo "\t" . '기존 작업(WORK_ID = ' . $workId . ')의 내용은 변함이 없다.' . "\n";
						continue;
					}
					echo "\t" . '기존 작업(WORK_ID = ' . $workId . ')을 수정한다.' . "\n";

					// TWORK: SCENE_ID, DATE_BEGIN, DATE_END, YOUR_ORDER, STATUS
					$aCol = array();
					if ($flag & FLAG_SCENE)
						array_push($aCol, 'SCENE_ID = ' . $oWork->sceneId);
					if ($flag & FLAG_BEGIN)
						array_push($aCol, 'DATE_BEGIN = \'' . mysql_real_escape_string($oWork->date_begin) . '\'');
					if ($flag & FLAG_END)
						array_push($aCol, 'DATE_END = \'' . mysql_real_escape_string($oWork->date_end) . '\'');
					if ($flag & FLAG_ORDER)
						array_push($aCol, 'YOUR_ORDER = \'' . mysql_real_escape_string($oWork->order) . '\'');
					if ($flag & FLAG_STATUS)
						array_push($aCol, 'STATUS = ' . $oWork->status);
					if (count($aCol)) {
						echo "\t" . 'TWORK테이블을 수정한다.' . "\n";
						$query = 'UPDATE TWORK SET ' . join(', ', $aCol) . ' WHERE WORK_ID = ' . $workId;
						echo "\t" . 'query: |' . $query . '|' . "\n";
						if (!mysql_query($query)) {
							$message = '';
							$message .= 'errno: |' . mysql_errno() . '|' . "\n";
							$message .= 'error: |' . mysql_error() . '|' . "\n";
							$message .= 'query: |' . $query . '|' . "\n";
							die($message);
						}
					} else {
						echo "\t" . 'TWORK테이블에는 변화가 없다.' . "\n";
					}

					// TWORKER: WORKER_ID
					$aDeleted = array_diff($oWork->aUserId[0], $oWork->aUserId[1]);
					print_r($aDeleted);
					$aAdded = array_diff($oWork->aUserId[1], $oWork->aUserId[0]);
					print_r($aAdded);
					if (count($aAdded) || count($aDeleted)) {
						echo "\t" . 'TWORKER테이블을 수정한다.' . "\n";
					} else {
						echo "\t" . 'TWORKER테이블에는 변화가 없다.' . "\n";
					}
					if (count($aDeleted)) {
						$query = 'DELETE FROM TWORKER WHERE ' . '(WORKER_ID = ' . $workId . ' AND USER_ID = ' . join(') OR (WORKER_ID = ' . $workId . ' AND USER_ID = ', $aDeleted) . ')';
						echo "\t" . 'query: |' . $query . '|' . "\n";
						if (!mysql_query($query)) {
							$message = '';
							$message .= 'errno: |' . mysql_errno() . '|' . "\n";
							$message .= 'error: |' . mysql_error() . '|' . "\n";
							$message .= 'query: |' . $query . '|' . "\n";
							die($message);
						}
					}
					if (count($aAdded)) {
						$query = 'INSERT INTO TWORKER (WORKER_ID, USER_ID) VALUES ' . '(' . $workId . ', ' . join('), (' . $workId . ', ', $aAdded) . ')';
						echo "\t" . 'query: |' . $query . '|' . "\n";
						if (!mysql_query($query)) {
							$message = '';
							$message .= 'errno: |' . mysql_errno() . '|' . "\n";
							$message .= 'error: |' . mysql_error() . '|' . "\n";
							$message .= 'query: |' . $query . '|' . "\n";
							die($message);
						}
					} else {
						echo "\t" . 'TWORKER테이블에는 변화가 없다.' . "\n";
					}

					// TDESIGN: SCENE_ID, TARGET_ID, (DESIGN_)TYPE
					$aCol = array();
					if ($flag & FLAG_SCENE)
						array_push($aCol, 'SCENE_ID = ' . $oWork->sceneId);
					if ($flag & FLAG_TARGET)
						array_push($aCol, 'TARGET_ID = ' . $oWork->targetId);
					if ($flag & FLAG_DESIGN)
						array_push($aCol, 'TYPE = ' . $oWork->designType);
					if (count($aCol)) {
						echo "\t" . 'TDESIGN테이블을 수정한다.' . "\n";
						$query = 'UPDATE TDESIGN SET ' . join(', ', $aCol) . ' WHERE WORK_ID = ' . $workId;
						echo "\t" . 'query: |' . $query . '|' . "\n";
						if (!mysql_query($query)) {
							$message = '';
							$message .= 'errno: |' . mysql_errno() . '|' . "\n";
							$message .= 'error: |' . mysql_error() . '|' . "\n";
							$message .= 'query: |' . $query . '|' . "\n";
							die($message);
						}
					} else {
						echo "\t" . 'TDESIGN테이블에는 변화가 없다.' . "\n";
					}

					// TSTORYBOARD: SCENE_ID
					$aCol = array();
					if ($oWork->flag & FLAG_SCENE)
						array_push($aCol, 'SCENE_ID = ' . $oWork->sceneId);
					if (count($aCol)) {
						echo "\t" . 'TSTORYBOARD테이블을 수정한다.' . "\n";
						$query = 'UPDATE TSTORYBOARD SET ' . join(', ', $aCol) . ' WHERE WORK_ID = ' . $workId;
						echo "\t" . 'query: |' . $query . '|' . "\n";
						if (!mysql_query($query)) {
							$message = '';
							$message .= 'errno: |' . mysql_errno() . '|' . "\n";
							$message .= 'error: |' . mysql_error() . '|' . "\n";
							$message .= 'query: |' . $query . '|' . "\n";
							die($message);
						}
					} else {
						echo "\t" . 'TSTORYBOARD테이블에는 변화가 없다.' . "\n";
					}
					echo "\t" . '기존 작업(WORK_ID = ' . $workId . ')을 성공적으로 수정했다.' . "\n";
				}
			}
		}
	}
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
<TITLE>작품</TITLE>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; CHARSET=euc-kr">
</HEAD>
<BODY>
<PRE>
<?PHP
		$evalCode = $_POST["PHP_EVAL_CODE"];
//		echo "PHP_EVAL_CODE = |" . $evalCode . "|\n";	// 웹 클라이언트가 웹 서버로 보낼 때 변환된 PHP코드
		// 변환한다: (\\->\, \"->", \'->')
		$evalCode = preg_replace('/\\\\([\\\"\'])/', '$1', $evalCode);	// PHP코드
//		echo "PHP_EVAL_CODE = |" . $evalCode . "|\n";	// 

		// PHP코드를 해석해서 필요한 변수(예를 들어, oShow: Show클래스에 대한 오브젝트)를 설정한다.
		eval($evalCode);
////		eval(preg_replace('/\\\\([\\\"\'])/', '$1', $_POST["PHP_EVAL_CODE"]));	// 한 줄로 쓰면...

		displayWork($aaWork);
?>
</PRE>
<SCRIPT LANGUAGE="JavaScript1.3">
	location.replace("fmWork.PHP");
</SCRIPT>
</BODY>
</HTML>
<?PHP
	} else {
		// 모든 작업과 작업에 필요한 정보(작업자, 장면, 인물/배경)를 얻어
		// 다음과 같이 JavaScript가 fnSetData에서 eval할 코드를 만든다.
		global	$JavaScriptEvalCode;
if (1) {
/*
		gUser = [
			new oUser(1, "갑"),
			new oUser(2, "을"),
			new oUser(3, "병"),
			new oUser(4, "정")
		];

		gaShow = [
			new oShow(
				1,
				"장면 #1",
				[
					new oScene(1, "장면 #1"),
					new oScene(2, "장면 #2: 인물(자)"),
					new oScene(3, "장면 #3: 인물(자), 배경(수성)"),
					new oScene(4, "장면 #4: 인물(축)"),
					new oScene(5, "장면 #5: 배경(지구)")
				],
				[
					new oChrBg(1, "자", [1, 2]),
					new oChrBg(2, "축", [3]),
					new oChrBg(3, "인", [-1])
				],
				[
					new oChrBg(4, "수성", [2]),
					new oChrBg(5, "금성", [-1]),
					new oChrBg(6, "지구", [4])
				]
			)
		];

		gaWork = [
			new _oWork(
				1,		// SHOW_ID = 1인 작품에 설정된
				[	// 작업들에 대한 배열
					// 2개의 디자인 작업
					new oDesign(
						1,
						0,
						[1, 2, 3],
						new Date('2005/6/12'),
						new Date('2005/12/10'),
						"지시사항: SHOW_ID = 1, 디자인 작업 = #1: ...",
						1,
						cDESIGN_CONCEPT,
						cTARGET_CHR,
						1,
						0		// 첫 번째 행에 표시된다.
					),
					new oDesign(
						2,
						0,
						[2, 3],
						new Date('2005/10/7'),
						new Date('2005/11/30'),
						"지시사항: SHOW_ID = 1, 디자인 작업 = #2: ...",
						1,
						cDESIGN_DETAIL,
						cTARGET_BG,
						4,
						1		// 두 번째 행에 표시된다.
					),
					// 1개의 스토리보드 작업
					new oStoryboard(
						3,
						1,
						[1, 3],
						new Date('2005/9/7'),
						new Date('2005/12/31'),
						"지시사항: SHOW_ID = 1, 스토리보드 작업 = #2: ...",
						1,
						2		// 세 번째 행에 표시된다.
					)
				]
			)
		];
 */
		$JavaScriptEvalCode = '';

		// 작업자를 구성할 USER들에 대한 목록을 얻는다.
/*
		gUser = [
			new oUser(1, "갑"),
			new oUser(2, "을"),
			new oUser(3, "병"),
			new oUser(4, "정")
		];
 */
		$JavaScriptEvalCode .= 'gUser = [';
{
		$query = 'SELECT USER_ID, NICKNAME FROM TUSER WHERE USER_ID > 0';
		if (!($result = mysql_query($query))) {
			$message = '';
			$message .= 'errno: |' . mysql_errno() . '|' . "\n";
			$message .= 'error: |' . mysql_error() . '|' . "\n";
			$message .= 'query: |' . $query . '|' . "\n";
			die($message);
		}
		$a = array();
		while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
			array_pusH($a,
				"\n" .
				"\t" . 'new oUser(' . $row["USER_ID"] . ', "' . $row["NICKNAME"] . '")'
			);
		}
		// 사용한 결과를 반환한다.
		mysql_free_result($result);
		$JavaScriptEvalCode .= join(',', $a) . "\n";
}
		$JavaScriptEvalCode .= '];' . "\n";

		// 각 작품별로, 작업할 대상(인물/배경)과 각 대상이 등장하는 장면에 대한 정보를 얻는다.
		$aShow = array();	// (SHOW_ID => oShow)로 이루어진 배열
		// 1. 각 작품에 있는 장면에 목록을 얻는다.
		// 2005/11/13 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
		// 작품에 따라 작업을 분류할 것이므로, 작품의 이름도 얻는다.
		$aShowName = array();
		// 작품은 정했는 데, 장면은 아직 정하지 않은 경우가 있을 수 있으므로,
		// 작품에 대한 정보를 얻은 후 해당 작품에 있는 장면에 대한 정보를 얻는다.
		// 1.1. 작품에 대한 정보를 얻는다.
		$query = 'SELECT SHOW_ID, NAME FROM TSHOW WHERE SHOW_ID > 0';
		if (!($result = mysql_query($query))) {
			$message = '';
			$message .= 'errno: |' . mysql_errno() . '|' . "\n";
			$message .= 'error: |' . mysql_error() . '|' . "\n";
			$message .= 'query: |' . $query . '|' . "\n";
			die($message);
		}
		while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
			$showId = $row["SHOW_ID"];

			$aShow[$showId] = new Show(array(), array(), array());
			$aShowName[$showId] = $row["NAME"];
		}
		// 사용한 결과를 반환한다.
		mysql_free_result($result);
		// 1.2. 작품별로 장면에 대한 정보를 얻는다.
		$query = 'SELECT T1.SHOW_ID, T2.NAME AS SCENE_NAME, SCENE_ID FROM TSHOW AS T1, TSCENE AS T2 WHERE T1.SHOW_ID = T2.SHOW_ID AND T1.SHOW_ID > 0';
		if (!($result = mysql_query($query))) {
			$message = '';
			$message .= 'errno: |' . mysql_errno() . '|' . "\n";
			$message .= 'error: |' . mysql_error() . '|' . "\n";
			$message .= 'query: |' . $query . '|' . "\n";
			die($message);
		}
		while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
			array_push($aShow[$row["SHOW_ID"]]->aScn, new TargetScene($row["SCENE_ID"], $row["SCENE_NAME"]));
		}
		// 사용한 결과를 반환한다.
		mysql_free_result($result);

		// 2. 대상(인물/배경)과 그 대상이 참조하는 장면에 대한 정보를 얻는다.
		$query = 'SELECT TYPE, T1.TARGET_ID, SHOW_ID, SCENE_ID, NAME FROM TTARGET AS T1, TTARGET_SCENE AS T2 WHERE T1.TARGET_ID = T2.TARGET_ID ORDER BY SHOW_ID';
		if (!($result = mysql_query($query))) {
			$message = '';
			$message .= 'errno: |' . mysql_errno() . '|' . "\n";
			$message .= 'error: |' . mysql_error() . '|' . "\n";
			$message .= 'query: |' . $query . '|' . "\n";
			die($message);
		}
		while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
			$showId = $row["SHOW_ID"];
			$targetId = $row["TARGET_ID"];

			$aChrBg = $row["TYPE"] == TARGET_CHR ? $aShow[$showId]->aChr : $aShow[$showId]->aBg;
			// 주어진 TARGET_ID를 가진 TargetChrBg가 있는 지 알아본다.
			for ($i = 0; $i < count($aChrBg); ++$i) {
				if ($targetId == $aChrBg[$i]->id) {
					// 찾았다.
					break;
				}
			}
			if ($i == count($aChrBg)) {
				// 찾지 못했다.
				$aChrBg[$i] = new TargetChrBg($targetId, $row["NAME"], array());
			}
			array_push($aChrBg[$i]->aSceneId, $row["SCENE_ID"]);
			if ($row["TYPE"] == TARGET_CHR)
				$aShow[$showId]->aChr = $aChrBg;
			else
				$aShow[$showId]->aBg = $aChrBg;
		}
		// 사용한 결과를 반환한다.
		mysql_free_result($result);
//		print_r($aShow);
/*
		gaShow = [
			new oShow(
				1,
				"장면 #1",
				[
					new oScene(1, "장면 #1"),
					new oScene(2, "장면 #2: 인물(자)"),
					new oScene(3, "장면 #3: 인물(자), 배경(수성)"),
					new oScene(4, "장면 #4: 인물(축)"),
					new oScene(5, "장면 #5: 배경(지구)")
				],
				[
					new oChrBg(1, "자", [1, 2]),
					new oChrBg(2, "축", [3]),
					new oChrBg(3, "인", [-1])
				],
				[
					new oChrBg(4, "수성", [2]),
					new oChrBg(5, "금성", [-1]),
					new oChrBg(6, "지구", [4])
				]
			)
		];
 */
		$JavaScriptEvalCode .= "\n";
		$JavaScriptEvalCode .= 'gaShow = [';
		$a = array();
		foreach ($aShow as $showId => $oShow) {
			$aScn = array();
			if (isset($oShow->aScn)) {	// 장면에 대한 정보가 있는 경우에 한해서...
				foreach ($oShow->aScn as $oScn) {
					array_push($aScn,
						"\n" .
						"\t\t\t" . 'new oScene(' . $oScn->id . ', "' . $oScn->name . '")'
					);
				}
			}
			$aChr = array();
			foreach ($oShow->aChr as $oChr) {
				array_push($aChr,
					"\n" .
					"\t\t\t" . 'new oChrBg(' . $oChr->id . ', "' . $oChr->name . '", [' . join(', ', $oChr->aSceneId) . '])'
					);
			}
			$aBg = array();
			foreach ($oShow->aBg as $oBg) {
				array_push($aBg,
					"\n" .
					"\t\t\t" . 'new oChrBg(' . $oBg->id . ', "' . $oBg->name . '", [' . join(', ', $oBg->aSceneId) . '])'
					);
			}
			array_push($a,
				"\n" .
				"\t" . 'new oShow(' . "\n" .
				"\t\t" . $showId . ',' . "\n" .
				"\t\t" . '"' . $aShowName[$showId] . '"' . ',' . "\n" .
				"\t\t" . '[' .
				join(',', $aScn) . "\n" .
				"\t\t" . '],' . "\n" .
				"\t\t" . '[' .
				join(',', $aChr) . "\n" .
				"\t\t" . '],' . "\n" .
				"\t\t" . '[' .
				join(',', $aBg) . "\n" .
				"\t\t" . ']' . "\n" .
				"\t" . ')'
			);
		}
//		print_r(join(',', $a) . "\n");
		$JavaScriptEvalCode .= join(',', $a) . "\n";
		$JavaScriptEvalCode .= '];' . "\n";

		// 작업에 대한 정보를 얻는다.
		// 1. 작업자에 대한 정보를 얻는다.
		$aWorker = array();	// (WORKER_ID => (USER_ID))
		$query = 'SELECT WORKER_ID, USER_ID FROM TWORKER';
		if (!($result = mysql_query($query))) {
			$message = '';
			$message .= 'errno: |' . mysql_errno() . '|' . "\n";
			$message .= 'error: |' . mysql_error() . '|' . "\n";
			$message .= 'query: |' . $query . '|' . "\n";
			die($message);
		}
		while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
			$workerId = $row["WORKER_ID"];	// = WORK_ID
			if (!isset($aWorker[$workerId])) {
				$aWorker[$workerId] = array();
			}
			array_push($aWorker[$workerId], $row["USER_ID"]);
		}
		// 사용한 결과를 반환한다.
		mysql_free_result($result);
//		print_r($aWorker);
		// 2. 디자인 작업의 경우 추가 정보(TYPE, 인물/배경, TARGET_ID)를 얻는다.
		$aDesign = array();	// (DESIGN_ID => (DESIGN_TYPE, TARGET_TYPE, TARGET_ID))로 이루어진 배열
		$query = 'SELECT DESIGN_ID, T1.TYPE, T2.TYPE, T1.TARGET_ID FROM TDESIGN AS T1, TTARGET AS T2 WHERE T1.TARGET_ID = T2.TARGET_ID';
		if (!($result = mysql_query($query))) {
			$message = '';
			$message .= 'errno: |' . mysql_errno() . '|' . "\n";
			$message .= 'error: |' . mysql_error() . '|' . "\n";
			$message .= 'query: |' . $query . '|' . "\n";
			die($message);
		}
		while ($row = mysql_fetch_array($result, MYSQL_NUM)) {
			$designId = $row[0];	// WORK_ID
			$designType = $row[1];
			$targetType = $row[2];
			$targetId = $row[3];
			$aDesign[$designId] = array($designType, $targetType, $targetId);
		}
		// 사용한 결과를 반환한다.
		mysql_free_result($result);
//		print_r($aDesign);
		// $aWorker, $aDesign을 이용해서 작업에 대한 정보를 모은다.
		$aaWork = array();	// (SHOW_ID => $aWork)로 이루어진 배열
					// 여기서 $aWork는 WorkDesign이나 WorkStoryboard로 이루어진 배열
		$query = 'SELECT TYPE, WORK_ID, SHOW_ID, SCENE_ID, DATE_BEGIN, DATE_END, YOUR_ORDER, STATUS FROM TWORK';
		if (!($result = mysql_query($query))) {
			$message = '';
			$message .= 'errno: |' . mysql_errno() . '|' . "\n";
			$message .= 'error: |' . mysql_error() . '|' . "\n";
			$message .= 'query: |' . $query . '|' . "\n";
			die($message);
		}
		while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
			$showId = $row["SHOW_ID"];
			$type = $row["TYPE"];
			$workId = $row["WORK_ID"];
			if (!isset($aaWork[$showId])) {
				$aaWork[$showId] = array();
			}
//			print_r($aWorker[$workId]);
			if (!isset($aWorker[$workId])) {
				// 다른 경로를 통해서 WORKER_ID = WORK_ID에 해당하는 데이터가 지워졌다.
				// 아래의 TDESIGN테이블의 경우와 마찬가지이다.
//				echo 'WORKER_ID = WORK_ID(= ' . $workId . ')에 해당하는 데이터가 없다.' . "\n";
//				echo '이 작업은 고려하지 않는다.' . "\n";
				continue;
			}

			// WORK_ID를 이용해서 작업자에 대한 정보를 얻는다.
			if ($type == WORK_DESIGN) {
//				print_r($aDesign[$workId]);
				if (!isset($aDesign[$workId])) {
					// 다른 경로를 통해 DESIGN_ID = WORK_ID에 해당하는 데이터가 없다.
					// 예를 들어, TARGET_ID가 TTARGET테이블에 대한 FOREIGN KEY로 사용되고 있는 데,
					// 다른 경로를 통해 TTARGET테이블에서 해당하는 TARGET_ID가 지워질 때
					// TDESIGN테이블의 해당하는 행도 지워진다.
// TODO: TWORK에서도 지워지게 해야한다.
//					echo 'DESIGN_ID = WORK_ID(= ' . $workId . ')에 해당하는 데이터가 없다.' . "\n";
//					echo '이 작업은 고려하지 않는다.' . "\n";
					continue;
				}
				array_push(
					$aaWork[$showId],
					new WorkDesign(
						$workId,
						0,
						$row["SCENE_ID"],
						$aWorker[$workId],
						$row["DATE_BEGIN"],
						$row["DATE_END"],
						$row["YOUR_ORDER"],
						$row["STATUS"],
						$aDesign[$workId][0],
						$aDesign[$workId][1],
						$aDesign[$workId][2]
					)
				);
			} else {
				array_push(
					$aaWork[$showId],
					new WorkStoryboard(
						$workId,
						0,
						$row["SCENE_ID"],
						$aWorker[$workId],
						$row["DATE_BEGIN"],
						$row["DATE_END"],
						$row["YOUR_ORDER"],
						$row["STATUS"]
					)
				);
			}
		}
		// 사용한 결과를 반환한다.
		mysql_free_result($result);
//		print_r($aaWork);
/*
		gaWork = [
			new _oWork(
				1,	// SHOW_ID = 1인 작품에 설정된
				[	// 작업들에 대한 배열
					// 2개의 디자인 작업
					new oDesign(
						1,
						0,
						[1, 2, 3],
						new Date('2005/6/12'),
						new Date('2005/12/10'),
						"지시사항: SHOW_ID = 1, 디자인 작업 = #1: ...",
						1,
						cDESIGN_CONCEPT,
						cTARGET_CHR,
						1,
						0		// 첫 번째 행에 표시된다.
					),
					new oDesign(
						2,
						0,
						[2, 3],
						new Date('2005/10/7'),
						new Date('2005/11/30'),
						"지시사항: SHOW_ID = 1, 디자인 작업 = #2: ...",
						1,
						cDESIGN_DETAIL,
						cTARGET_BG,
						4,
						1		// 두 번째 행에 표시된다.
					),
					// 1개의 스토리보드 작업
					new oStoryboard(
						3,
						1,
						[1, 3],
						new Date('2005/9/7'),
						new Date('2005/12/31'),
						"지시사항: SHOW_ID = 1, 스토리보드 작업 = #2: ...",
						1,
						2		// 세 번째 행에 표시된다.
					)
				]
			)
		];
 */
		$JavaScriptEvalCode .= "\n";
		$JavaScriptEvalCode .= 'gaWork = [';
		$aa = array();
		foreach ($aaWork as $showId => $aWork) {
			$a = array();
			foreach ($aWork as $i => $oWork) {
				array_push($a,
					"\n" .
					"\t\t\t" . 'new o' . ($oWork->workType == WORK_DESIGN ? 'Design' : 'Storyboard') . '(' . "\n" .
					"\t\t\t\t" . $oWork->workId . ',' . "\n" .
					"\t\t\t\t" . $oWork->sceneId . ',' . "\n" .
					"\t\t\t\t" . '[' . join(', ', $oWork->aUserId) . '],' . "\n" .
					"\t\t\t\t" . 'new Date("' . join('/', split('-', $oWork->date_begin)) . '"),' . "\n" .
					"\t\t\t\t" . 'new Date("' . join('/', split('-', $oWork->date_end)) . '"),' . "\n" .
					"\t\t\t\t" . '"' . preg_replace('/\r\n/', '\\\\n', preg_replace('/([\\\"])/', "\\\\$1", $oWork->order)) . '",' . "\n" .
					"\t\t\t\t" . $oWork->status . ',' . "\n" .
					(
						$oWork->workType == WORK_DESIGN ?
						(
							"\t\t\t\t" . ($oWork->designType == DESIGN_CONCEPT ? 'cDESIGN_CONCEPT' : 'cDESIGN_DETAIL') . ',' . "\n" .
							"\t\t\t\t" . ($oWork->targetType == TARGET_CHR ? 'cTARGET_CHR' : 'cTARGET_BG') . ',' . "\n" .
							"\t\t\t\t" . $oWork->targetId . ',' . "\n"
						) :
						''
					) .
					"\t\t\t\t" . $i . "\n" .
					"\t\t\t" . ')'
				);
			}
			array_push($aa,
				"\n" .
				"\t" . 'new _oWork(' . "\n" .
				"\t\t" . $showId . ',' . "\n" .
				"\t\t" . '[' .
				join(',', $a) . "\n" .
				"\t\t" . ']' . "\n" .
				"\t" . ')'
			);
		}
//		print_r(join(',' . "\n", $aa) . "\n");
		$JavaScriptEvalCode .= join(',' . "\n", $aa) . "\n";
		$JavaScriptEvalCode .= '];' . "\n";
}

		// JavaScript에서 문자열로 받아들여야 하므로 다음과 같은 변환을 한다.
		// (\) -> (\\), (") -> (\")
//		echo '|' . $JavaScriptEvalCode . '|' . "\n";
		$JavaScriptEvalCode = preg_replace('/([\\\"])/', "\\\\$1", $JavaScriptEvalCode);
		// 줄바꿈도 변환한다.
		$JavaScriptEvalCode = preg_replace('/\n/', '\\\\n', $JavaScriptEvalCode);
//		echo '|' . $JavaScriptEvalCode . '|' . "\n";
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML>
<HEAD>
<TITLE>fmWork.PHP</TITLE>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; CHARSET=euc-kr">

<STYLE TYPE="TEXT/CSS">
	TABLE.TBAND {
		margin: 1px 0px;
	}
</STYLE>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/13 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 작업 일정표에 각 작업에 상태에 따라 작업 칸에 색깔을 넣는다.
 *
 * 작품별로 작업에 관한 관리를 할 수 있다.
 */

	var	cBGCOLOR_INSERT = "<?PHP echo BGCOLOR_INSERT; ?>";	// 새로 추가할 작업
	var	cBGCOLOR_SELECT = "<?PHP echo BGCOLOR_SELECT; ?>";	// DB에서 가져온 작업
	var	cBGCOLOR_UPDATE = "<?PHP echo BGCOLOR_UPDATE; ?>";	// DB에서 가져와 수정한 작업

	function fnShowSelectOnChange() {
		gShowId = sltShow.options[sltShow.selectedIndex].value;
		if (!fnInit(gShowId)) {
			alert('fnInit(): Failure');
		}
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/10 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 작업자에 대한 정의를 명확히 한다.
 * 각 작업을 맡아서 작업을 진행하는 개인또는 집단을 작업자라 부른다.
 * 여러 명이 동시에 한 작업을 진행할 수 있으므로 개인 사용자와는 구별되어야 한다.
 * 작업자에는 WORKER_ID를, 개인 사용자에는 USER_ID를 부여해서 구분한다.
 * 따라서 클래스나 오브젝트의 aWorkerId대신에 aUserId를 사용한다.
 *
 * JavaScript코드에서 사용하는 oWorkBase오브젝트에서 workId멤버의 이름을 _workId으로 바꾸었다.
 * 다른 멤버와 달리 내부적으로만 쓰이기 때문이다.
 * 이는 DB에서 얻어온 데이터를 나중에 비교하기 위해서 저장하는 변수의 이름 앞에 '_'가 붙은 이유와 같다.
 */
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/9 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 각 열(작업, 작업자, 장면, 작업기간)에 따라 정렬한다.
 */

	var	gSort = 0;	// 아래의 해당 비트가 설정되어있으면 정렬 순서는 역순이다.
	var	cSORT_WORK		= 1 << 0;
	var	cSORT_WORKER		= 1 << 1;
	var	cSORT_SCENE		= 1 << 2;
	var	cSORT_WORK_PERIOD	= 1 << 3;
	function fnSort(cSORT_XXX) {
		// gWork를 정렬한 후,
		var	byWhat = cSORT_XXX == cSORT_WORK ? "ByWork" :
			cSORT_XXX == cSORT_WORKER ? "ByWorker" :
			cSORT_XXX == cSORT_SCENE ? "ByScene" :
			"ByWorkPeriod";
		eval('gWork.sort(fnCompareFunction' + byWhat + ')');

		// .rowIndex를 다시 쓴다.
		var	rowIndex = 0;
		for (var iWork = 0; iWork < gWork.length; ++iWork) {
			if (!(gWork[iWork].flag & cOFLAG_DELETED)) {
				gWork[iWork].rowIndex = rowIndex++;
			}
		}

		// 해당 비트를 toggle한다.
		gSort ^= cSORT_XXX;

		// 작업테이블의 본문행을
		// 모든 없애고
		while (tblWorkSchedule.rows.length != 2) {
			tblWorkSchedule.deleteRow(1);
		}
		// 다시 만든다.
		if (!fnMakeBodyRows(gCurDate)) {
			alert('fnMakeBodyRows(): Failure');
			return false;
		}
	}
	function fnCompareFunctionByWork(aWork, bWork) {
		if (aWork.workType == bWork.workType) {	// 같은 유형(디자인/스토리보드)의 작업이다.
			if (
				aWork.workType == cWORK_STORYBOARD ||	// 스토리보드 작업이거나
				aWork.designType == bWork.designType	// 디자인 작업의 경우 같은 유형(CONCEPT/DETAIL)의 디자인 작업이라면
			) {
				// 순서는 없다.
				return 0;
			}
			// 디자인 작업의 경우 컨셉디자인이 우선이다.
			return (gSort & cSORT_WORK ? -1 : 1) * (aWork.designType == cDESIGN_CONCEPT ? -1 : 1);			
		}
		// 서로 다른 유형이다.
		// (aWork: 디자인, bWork: 스토리보드)이거나
		// (aWork: 스토리보드, bWork: 디자인)이다.
		return (gSort & cSORT_WORK ? -1 : 1) * (aWork.workType == cWORK_DESIGN ? -1 : 1);
	}
	function fnCompareFunctionByScene(aWork, bWork) {
		return (gSort & cSORT_SCENE ? -1 : 1) * (aWork.sceneId - bWork.sceneId);
	}
	function fnCompareFunctionByWorker(aWork, bWork) {
		// (정책) 어떤 식으로 정렬할 것인가?
		// 첫번째 사람의 이름으로...
		var	aaUserName = [];
		for (var iUser = 0; iUser < aWork.aUserId.length; ++iUser) {
			for (var i = 0; i < gUser.length; ++i) {
				if (gUser[i].id == aWork.aUserId[iUser]) {
					aaUserName.push(gUser[i].name);
					break;
				}
			}
		}
		aaUserName.sort();
		var	abUserName = [];
		for (var iUser = 0; iUser < bWork.aUserId.length; ++iUser) {
			for (var i = 0; i < gUser.length; ++i) {
				if (gUser[i].id == bWork.aUserId[iUser]) {
					abUserName.push(gUser[i].name);
					break;
				}
			}
		}
		abUserName.sort();
		if (aaUserName[0] == abUserName[0])
			return 0;
		var	a = [aaUserName[0], abUserName[0]];
		a.sort();
		return (gSort & cSORT_WORKER ? -1 : 1) * (a[0] == aaUserName[0] ? -1 : 1);
	}
	function fnCompareFunctionByWorkPeriod(aWork, bWork) {
		// (정책) 시작날짜를 기준으로? 아니면 끝나는 날짜를 기준으로?
		// 끝나는 날짜를 기준으로...
		return (gSort & cSORT_WORK_PERIOD ? -1 : 1) * (aWork.date_end.valueOf() - bWork.date_end.valueOf());
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/8 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 데이터들을 모아 이를 처리하는 페이지를 호출한다.
 */

	function fnSubmit() {
		// 데이터를 서버로 보내는 데 필요한 FORM을 만든다.
		var	oFORM = document.createElement('<FORM METHOD="POST" ACTION="fmWork.PHP" ENCTYPE="APPLICATION/X-WWW-FORM-URLENCODED">');
		document.appendChild(oFORM);

		// .PHP에서 받아서 eval할 PHP코드를 포함하는 데이터를 만드는 데 필요한 TEXTAREA를 만든다.
		// 일반적으로 PHP코드는 여러 줄에 걸쳐있으므로 TEXTAREAD를 사용한다.
		var	oINPUT = document.createElement('<TEXTAREA NAME="PHP_EVAL_CODE">');
		var	oVal = fnMakePHPEvalCode();
//		alert(oVal);
		oINPUT.value = oVal;
		oFORM.appendChild(oINPUT);

		// 웹 클라이언트에서 데이터를 서버로 보낼 때,
		// (\ -> \\, ' -> \', " -> \")와 같은 변환을 한다.
		// 따라서 서버에서는 역으로 변화해야 한다.
		// (\\ -> \, \' -> ', \" -> ")
		// 데이터를 서버로 보낸다.
		oFORM.submit();
	}
/*
	$aaWork = array(
		new Work(
			1,
			array(
				new WorkDesign(
					WORK_DESIGN,
					$workId,
					$flag,
					$sceneId,
					$aUserId,
					$date_begin,
					$date_end,
					$order,
					$status,
					$designType,
					$targetType,
					$targetId
				),
				new WorkStoryboard(
					WORK_STORYBOARD,
					$workId,
					$flag,
					$sceneId,
					$aUserId,
					$date_begin,
					$date_end,
					$order,
					$status
				)
			)
		)
	);
 */
	// 위과 같은, gaWork정보를 PHP가 eval할 코드를 만든다.
	function fnMakePHPEvalCode() {
		var	PHPEvalCode = '';
		PHPEvalCode += '$aaWork = array(' + '\n';
		for (var iShow = 0; iShow < gaWork.length; ++iShow) {
			PHPEvalCode += '\t' + gaWork[iShow].showId + ' => array(' + '\n';
			var	_oWork = gaWork[iShow];
			for (var iWork = 0; iWork < _oWork.aWork.length; ++iWork) {
				var	oWork = _oWork.aWork[iWork];
				var	flag = oWork.getFlag();
				if (oWork.workType == cWORK_DESIGN) {
					flag |= (oWork.designType == oWork._designType ? 0 : cOFLAG_DESIGN) |
						(oWork.targetType == oWork._targetType ? 0 : cOFLAG_TARGET) |
						(oWork.targetId == oWork._targetId ? 0 : cOFLAG_TARGETID);
				}
				PHPEvalCode += '\t\t' + 'new Work' + (oWork.workType == cWORK_DESIGN ? 'Design' : 'Storyboard') + '(' + '\n';

				PHPEvalCode += '\t\t\t' + (oWork._workId) + ',' + '\n';		// WORK_ID
				PHPEvalCode += '\t\t\t' + (flag) + ',' + '\n';			// Flag...
				PHPEvalCode += '\t\t\t' + (oWork.sceneId) + ',' + '\n';		// SCENE_ID
				PHPEvalCode += '\t\t\t' + ('array(' + (
					!(flag & cOFLAG_DB) ||		// 새로 만들거나
					!(flag & cOFLAG_DELETED) &&	// 삭제되지 않고,
					oWork._aUserId.sort().join(', ') != oWork.aUserId.sort().join(', ') ?
									// 작업자를 구성하는 USER_ID의 목록에 변동이 있는 경우에만
					('array(' + oWork._aUserId.sort().join(', ') + '), array(' + oWork.aUserId.sort().join(', ') + ')') :
									// 작업자를 구성하는 USER_ID의 목록의 전(前), 후(後)를 알려주고,
					'array(), array()'		// 그렇지 않은 경우에는 의미없는 정보를 보낸다.
				) + ')') + ',' + '\n';
				PHPEvalCode += '\t\t\t' + ('"' + (
					!(flag & cOFLAG_DB) ||		// 새로 만들거나
					!(flag & cOFLAG_DELETED) &&	// 삭제되지 않고,
					flag & cOFLAG_BEGIN ?		// 작업을 시작하는 날짜가 변경된 경우에만
					fnMakeDateString(oWork.date_begin) :	// 변경된 날짜를 알려주고,
					''				// 그렇지 않은 경우에는 아무것도 보내지 않는다
				) + '"') + ',' + '\n';
				PHPEvalCode += '\t\t\t' + ('"' + (
					!(flag & cOFLAG_DB) ||		// 새로 만들거나
					!(flag & cOFLAG_DELETED) &&	// 삭제되지 않고,
					flag & cOFLAG_END ?		// 작업을 끝내는 날짜가 변경된 경우에만
					fnMakeDateString(oWork.date_end) :	// 변경된 날짜를 알려주고,
					''				// 그렇지 않은 경우에는 아무것도 보내지 않는다
				) + '"') + ',' + '\n';
				PHPEvalCode += '\t\t\t' + ('"' + (
					!(flag & cOFLAG_DB) ||		// 새로 만들거나
					!(flag & cOFLAG_DELETED) &&	// 삭제되지 않고,
					flag & cOFLAG_ORDER ?		// 지시사항이 변경된 경우에만
					oWork.order.replace(/([\\"])/g, "\\$1") :	// 변경된 지시사항을 알려주고,
					''				// 그렇지 않은 경우에는 아무것도 보내지 않는다
				) + '"') + ',' + '\n';
				PHPEvalCode += '\t\t\t' + (oWork.status) + (oWork.workType == cWORK_DESIGN ? ',' : '') + '\n';
				if (oWork.workType == cWORK_DESIGN) {
					// designType, targetType, targetId
					PHPEvalCode += '\t\t\t' + (oWork.designType == cDESIGN_CONCEPT ? 'DESIGN_CONCEPT' : 'DESIGN_DETAIL') + ',' + '\n';
					PHPEvalCode += '\t\t\t' + (oWork.targetType == cTARGET_CHR ? 'TARGET_CHR' : 'TARGET_BG') + ',' + '\n';
					PHPEvalCode += '\t\t\t' + (oWork.targetId) + '\n';
				}

				PHPEvalCode += '\t\t' + ')' + (iWork == _oWork.aWork.length - 1 ? '' : ',') + '\n';
			}
			PHPEvalCode += '\t' + ')' + (iShow  == gaWork.length - 1 ? '' : ',') + '\n';
		}
		PHPEvalCode += ');' + '\n';
		return PHPEvalCode;
	}
</SCRIPT>
<SCRIPT LANGUAGE="JAVASCRIPT1.3">
/*
 * 2005/11/8 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 오브젝트를 써서 작업에 필요한 정보(장면, 인물/배경)를 저장한다.
 * fmShow.PHP에 사용한 오브젝트의 축소형을 사용한다.
 */
	function oBase(id, name) {
		this.id = id;
		this.name = name;
	}
	// 작업자를 구성하는 USER에 대한 정보를 가지는 오브젝트로서 여기서는 oBase와 같다.
	function oUser(id, name) {
		this.base = oBase;
		this.base(id, name);
	}
	// 장면에 대한 정보를 가지는 오브젝트로서 여기서는 oBase와 똑같다.
	function oScene(id, name) {
		this.base = oBase;
		this.base(id, name);
	}
	// 인물/배경에 대한 정보를 가지는 오브젝트
	function oChrBg(id, name, aSceneId) {
		this.base = oBase;
		this.base(id, name);
		// 인물/배경이 등장하는 장면의 SCENE_ID이다.
		//	0: 작품 전체에 등장한다.
		//	>= 1: SCENE_ID에 해당하는 장면에만 등장한다.
		// aSceneId배열은 적어도 하나의 성분을 가진다.
		// 성분은 0이나 자연수들의 집합이다. 동시에 0과 자연수를 포함할 수 없다.
		this.aSceneId = aSceneId;
	}
	// 작품에 대한 몇 가지 정보를 담고 있다.
	// 어떤 장면이 있고, 등장하는 인물/배경에는 무엇이 있는 지,
	// 그리고 특정 인물/배경은 특정 장면에만 등장하는 지 등등.
	function oShow(showId, showName, aScn, aChr, aBg) {
		this.showId = showId;
		this.showName = showName;
		this.aScn = aScn;
		this.aChr = aChr;
		this.aBg = aBg;
	}
	var	gUser;		// (DB) 작업자를 구성하는 USER들의 목록이다. (oUser오브젝트로 이루어진 배열)
	var	gaShow = [];	// (DB) oShow로 이루어진 배열
	var	gShow;		// gaShow중에서 gShowId(= SHOW_ID)에 해당하는 oShow오브젝트
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/7 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 인물/배경을 선택했을 때...
 */

	// 선택된 인물/배경에 따라 가능한 장면의 목록을 만든다.
	// (정책) 기본값으로 제일 첫번째 장면이 선택한다.
	// 다른 곳에서 다른 장면을 선택할 수 있다.
	function fnSelectOnChange(ChrBg) {
		var	targetType = ChrBg == "Chr" ? cTARGET_CHR : cTARGET_BG;
		eval('var sltChrBg = slt' + ChrBg);
		eval('var aChrBg = gShow.a' + ChrBg);

		// 선택된 인물/배경(TARGET_ID = targetId)이 참조하는 장면들의 목록을 얻는다.
		var	op = sltChrBg.options[sltChrBg.selectedIndex];
		var	targetId = op.value;
		var	aSceneId;
		for (var i = 0; i < aChrBg.length; ++i) {
			if (targetId == aChrBg[i].id) {
				aSceneId = aChrBg[i].aSceneId;
				break;
			}
		}
		if (aSceneId == undefined) {
			alert('TARGET_ID = ' + targetId + '에 해당하는 ' + (ChrBg == "Chr" ? '인물' : '배경') + '을 찾을 수 없다');
			// 이럴 수가 없다.
		}
		var	aScn = [];
		if (aSceneId.length == 1 && aSceneId[0] == 0) {		// 작품 전체에서 등장한다.
			aScn.push(new oScene(0, "작품 전체에서 등장한다"));
		} else {
			for (var i = 0; i < aSceneId.length; ++i) {
				for (var iScene = 0; iScene < gShow.aScn.length; ++iScene) {
					if (aSceneId[i] == gShow.aScn[iScene].id) {
						break;	
					}
				}
				if (iScene == gShow.aScn.length) {
					alert('SCENE_ID = ' + aSceneId[i] + '에 해당하는 장면을 찾을 수 없다');
					// 이럴 수는 없다.
				}
				aScn.push(gShow.aScn[iScene]);
			}
		}
//		alert(aScn);
		fnAddOptionsToScnSelect(aScn);
	}
	function fnAddOptionsToScnSelect(aScn) {
		// 디자인 작업은 여러 명의 작업자가 동시에 진행할 수 있지만,
		// 스토리보드 작업은 오직 한 면의 작업자만이 진행한다.
		// 스토리보드 작업에서 해당 장면의 컷을 여러 명이 나누는 것은 생각하기 어려운 상황이다.
		sltWorker.multiple = !(gWrkEdtFlag & cWEFLAG_STRYBRD);

		// "장면"에 대한 목록
		sltScn.options.length = 0;	// 초기화한다.
		for (var i = 0; i < aScn.length; ++i) {
			oOp = document.createElement("OPTION");
			oOp.value = aScn[i].id;
			oOp.text = aScn[i].name;
			sltScn.options.add(oOp);
		}
		if (aScn.length > 0) {
			sltScn.selectedIndex = 0;	// (정책) 첫번째를 자동으로 선택한다.
		}
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/7 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 자주 쓰는 함수들이다.
 */

	// 날짜를 yyyy-mm-dd형태로 문자열로 만든다.
	function fnMakeDateString(date) {
		return date.getYear() + '-' + (1 + date.getMonth()) + '-' + date.getDate();
	}
</SCRIPT>
<SCRIPT LANGUAGE="JAVASCRIPT1.3">
/*
 * 2005/11/7 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 오브젝트를 써서 작업에 대한 정보를 저장한다.
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
	// .flag
	var	cOFLAG_DB	= <?PHP echo FLAG_DB ?>;	// 1: DB에서 가져온 작업, 0: 새로 등록한 작업
	// 아래의 모든 cOFLALG_XXX는 DB에서 가져온 작업의 경우에 의미가 있다.
	var	cOFLAG_DELETED	= <?PHP echo FLAG_DELETED ?>;	// 1: DB에서 가져온 작업인데, 삭제한 경우이다.
								// 새로 시작한 경우에는 의미가 없다.
	var	cOFLAG_SCENE	= <?PHP echo FLAG_SCENE ?>;	// 1: DB에서 가져온 작업에서 장면에 대한 정보가 바뀌었다.
	var	cOFLAG_WORKER	= <?PHP echo FLAG_WORKER ?>;	// 1: 작업자에 대한 정보가 바뀌었다.
	var	cOFLAG_BEGIN	= <?PHP echo FLAG_BEGIN ?>;	// 1: 작업을 시작하는 날짜가 바뀌었다.
	var	cOFLAG_END	= <?PHP echo FLAG_END ?>;	// 1: 작업을 끝내는 날짜가 바뀌었다.
	var	cOFLAG_ORDER	= <?PHP echo FLAG_ORDER ?>;	// 1: 작업에 대한 지시사항이 바뀌었다.
	var	cOFLAG_STATUS	= <?PHP echo FLAG_STATUS ?>;	// 1: 작업의 진행상황이 바뀌었다.
	var	cOFLAG_DESIGN	= <?PHP echo FLAG_DESIGN ?>;	// 1: 디자인의 유형(cDESIGN_CONCEPT/cDESIGN_DETAIL)이 바뀌었다.
	var	cOFLAG_TARGET	= <?PHP echo FLAG_TARGET ?>;	// 1: 대상의 유형(인물/배경)이 바뀌었다.
	var	cOFLAG_TARGETID	= <?PHP echo FLAG_TARGETID ?>;	// 1: 대상(인물/배경)이 바뀌었다.
	function oWorkBase(workType, _workId, sceneId, aUserId, date_begin, date_end, order, status, rowIndex) {
		this.workType = workType;
		this._workId = _workId;		// >= 1: DB로 부터 가져온 작업, 0: 새로 시작한 작업

		this.sceneId = sceneId;
		this.aUserId = [];
		for (var i = 0; i < aUserId.length; ++i)
			this.aUserId[i] = aUserId[i];
		this.date_begin = new Date(date_begin);
		this.date_end = new Date(date_end);
		this.order = order;
		this.status = status;

		this.rowIndex = rowIndex;	// 이 작업이 테이블에서 몇 번째 행에 표시되는 지를 말해준다.
						// cOFLAG_DELETED가 설정된 경우에는 이 property는 참고하지 않는다.

		this.flag = 0;
		this._aUserId = [];	// 새로 만든 작업의 경우에는 아무것도 없다.
		if (_workId > 0) {	// >= 1: DB로 부터 가져온 작업, 0: 새로 시작한 작업
			this.flag = cOFLAG_DB;

			// 작업에 대한 정보가 변경되었는 지 알아보기 위하여 원본을 저장한다.
			this._sceneId = sceneId;
//			this._aUserId = [];
			for (var i = 0; i < aUserId.length; ++i)
				this._aUserId[i] = aUserId[i];
			this._date_begin = new Date(date_begin);
			this._date_end = new Date(date_end);
			this._order = order;
			this._status = status;
		}

		this.getFlag = function fnGetFlag() {
			if (!(this.flag & cOFLAG_DB)) {		// 새로 만든 작업이라면
				// 어떤 비트도 설정하지 않는다.
				return 0;
			}
			return (this.flag & (cOFLAG_DB | cOFLAG_DELETED)) |
				(this.sceneId == this._sceneId ? 0 : cOFLAG_SCENE) |
				(this.aUserId.sort().join() == this._aUserId.sort().join() ? 0 : cOFLAG_WORKER) |
				(fnMakeDateString(this.date_begin) == fnMakeDateString(this._date_begin) ? 0 : cOFLAG_BEGIN) |
				(fnMakeDateString(this.date_end) == fnMakeDateString(this._date_end) ? 0 : cOFLAG_END) |
				(this.order == this._order ? 0 : cOFLAG_ORDER) |
				(this.status == this._status ? 0 : cOFLAG_STATUS);
		}
	}
	function oDesign(_workId, sceneId, aUserId, date_begin, date_end, order, status, designType, targetType, targetId, rowIndex) {
		this.base = oWorkBase;
		this.base(cWORK_DESIGN, _workId, sceneId, aUserId, date_begin, date_end, order, status, rowIndex);

		this.designType = designType;
		this.targetType = targetType;
		this.targetId = targetId;

		if (_workId > 0) {	// >= 1: DB로 부터 가져온 작업, 0: 새로 시작한 작업
			this._designType = designType;
			this._targetType = targetType;
			this._targetId = targetId;
		}
	}
	function oStoryboard(_workId, sceneId, aUserId, date_begin, date_end, order, status, rowIndex) {
		this.base = oWorkBase;
		this.base(cWORK_STORYBOARD, _workId, sceneId, aUserId, date_begin, date_end, order, status, rowIndex);
	}
	function _oWork(showId, aWork) {
		this.showId = showId;	// >= 1; SHOW_ID가 showId인 작품에 있는,
		this.aWork = aWork;	// 작업(oDesign/oStoryboard)들에 대한 배열이다.
	};	// 오브젝트

	var	gaWork = [];	// (DB) _oWork들로 이루어진 배열
	var	gShowId;	// SHOW_ID >= 1
	var	gWork;		// gaWork중에서 gShowId(= SHOW_ID)에 해당하는 _oWork오브젝트

	function oPartition(segment, color, fraction) {
		this.segment = segment;
		this.color = color;
		this.fraction = fraction;
	}
</SCRIPT>
<SCRIPT LANGUAGE="JAVASCRIPT1.3">
/*
 * 2005/10/20 (EST)
 *		By 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 작업일정표의 머리행과 꼬리행을 만든다.
 */

	// 작업일정표(tblWorkSchedule)를 만든다.
	function fnMakeWorkScheduleTable(givenDate) {
		// 모든 행을 없앤다.
		while (tblWorkSchedule.rows.length) {
			tblWorkSchedule.deleteRow();
		}

		if (!fnMakeHeadRow()) {
			alert('fnMakeHeadRow(): Failure');
			return false;
		}

		if (!fnMakeBodyRows(givenDate)) {
			alert('fnMakeBodyRows(): Failure');
			return false;
		}

		if (!fnMakeFootRow(givenDate)) {
			alert('fnMakeFootRow(): Failure');
			return false;
		}

		// 기준 날짜를 변경한다.
		gCurDate = new Date(givenDate);
		return true;
	}
	// 작업일정표(tblWorkSchedule)의 머리행을 만든다.
	function fnMakeHeadRow() {
/*
<TR>
	<TD ALIGN="CENTER" WIDTH="20%">...</TD>
	<TD WIDTH="80%">...</TD>
</TR>
 */
		var	oTr = tblWorkSchedule.insertRow();
		if (!oTr) {
			alert('tblWorkSchedule.insertRow(): Failure');
			return false;
		}

/*
	<TD ALIGN="CENTER" WIDTH="20%">작업</TD>
 */
		var	oTd1 = oTr.insertCell();
		if (!oTd1) {
			alert('oTr.insertCell(): Failure');
			tblWorkSchedule.deleteRow(oTr.rowIndex);
			return false;
		}
		oTd1.align = "CENTER";
		oTd1.width = cWORK_WIDTH + "%";
		oTd1.innerHTML = '<INPUT TYPE="BUTTON" VALUE="작업" onClick="fnSort(cSORT_WORK);">';

		// 해당 열(들)을 만든다.
		// 1. 해당 열(들)이 차지하는 공간의 크기(WIDTH)를 결정할 때 필요한 정보를 얻는다.
		var	ratioTotal = 0;		// 해당 열(들)이 차지할 비율을 결정하는 데 필요하다.
		for (var iCol = 0; iCol < gaColumn.length; ++iCol) {
			var	oCol = gaColumn[iCol];
			if (!fnIsSetChkFlag(oCol.chkId)) {	// 표시되지 않는 열이라면
				// 그냥 건너뛴다.
				continue;
			}
			ratioTotal += oCol.ratio;
		}
		// 2. 해당 열(들)을 적절한 비율로 만든다.
		for (var iCol = 0; iCol < gaColumn.length; ++iCol) {
			var	oCol = gaColumn[iCol];
			if (!fnIsSetChkFlag(oCol.chkId)) {	// 표시되지 않는 열이라면
				// 그냥 건너뛴다.
				continue;
			}

/*
	<TD WIDTH="80%"><TABLE BORDER="1" FRAME="VSIDES" RULES="NONE" WIDTH="100%" CELLSPACING="0" CELLPADDING="0"><TBODY><TR ALIGN="CENTER">
		<TD WIDTH="100%">작업기간(월별)</TD>
	</TR></TBODY></TABLE></TD>
 */
			var	oTd2 = oTr.insertCell();
			if (!oTd2) {
				alert('oTr.insertCell(): Failure');
				tblWorkSchedule.deleteRow(oTr.rowIndex);
				return false;
			}
			oTd2.align = "CENTER";
			// 최대 (100 - cWORK_WIDTH)%에서...
			oTd2.width = (oCol.ratio / ratioTotal * (100 - cWORK_WIDTH)) + "%";
			var	innerHTML;
			if (oCol.id == 'COLUMN_ID__WORK_PERIOD') {
				innerHTML = '';
				innerHTML += '<TABLE BORDER="1" FRAME="VSIDES" RULES="NONE" WIDTH="100%" CELLSPACING="0" CELLPADDING="0"><TBODY><TR ALIGN="CENTER">';
				innerHTML += '	<TD WIDTH="100%"><INPUT TYPE="BUTTON" VALUE="작업기간(월별)" onClick="fnSort(cSORT_WORK_PERIOD);"></TD>';
				innerHTML += '</TR></TBODY></TABLE>';
			} else {	// 'COLUMN_ID__SCENE', 'COLUMN_ID__WORKER'
				innerHTML = '<INPUT TYPE="BUTTON" VALUE="' + oCol.name + '" onClick="fnSort(' + (oCol.id == 'COLUMN_ID__SCENE' ? cSORT_SCENE : cSORT_WORKER) + ');">';
			}
			oTd2.innerHTML = innerHTML;
		}

		return true;
	}

	// 작업일정표(tblWorkSchedule)의 본행(들)을 만든다.
	function fnMakeBodyRows(givenDate) {
		var	iRowIndex = 0;
		for (var iWork = 0; iWork < gWork.length; ++iWork) {
			if (gWork[iWork].flag & cOFLAG_DELETED) {	// DB에서 가져왔으나 웹에서 삭제된 행은
				// 건너뛴다.
				continue;
			}

			if (!fnInsertWorkRow(gWork[iWork], 1 + iRowIndex++, givenDate)) {	// 머리행 다음부터 넣는다.
				alert('fnInsertWorkRow(): Failure');
				return false;
			}
		}
		// giWork에 해당하는 작업행의 RADIO버튼을 선택한다.
		if (!fnSetWorkIndexChecked(giWork)) {
			alert('fnSetWorkIndexChecked(): Failure');
			return false;
		}
		return true;
	}

	// 작업일정표(tblWorkSchedule)의 꼬리행을 만든다.
	// givenDate를 기준으로 작업기간(월별)에 표시되는 " <= ", " => "버튼을 만든다.
	function fnMakeFootRow(givenDate) {
/*
<TR>
	<TD ALIGN="CENTER" WIDTH="20%">...</TD>
	<TD WIDTH="80%">...</TD>
</TR>
 */
		var	oTr = tblWorkSchedule.insertRow();
		if (!oTr) {
			alert('tblWorkSchedule.insertRow(): Failure');
			return false;
		}

/*
	<TD ALIGN="CENTER" WIDTH="20%">&nbsp;</TD>
 */
		var	oTd1 = oTr.insertCell();
		if (!oTd1) {
			alert('oTr.insertCell(): Failure');
			tblWorkSchedule.deleteRow(oTr.rowIndex);
			return false;
		}
		oTd1.align = "CENTER";
		var	innerHTML = '';
		innerHTML += '<INPUT TYPE="BUTTON" VALUE="작업관리" onClick="fnToggleWorkManager();">';
		var	length = 0;	// 테이블에 표시할 작업의 갯수
		for (var iWork = 0; iWork < gWork.length; ++iWork) {
			if (!(gWork[iWork].flag & cOFLAG_DELETED)) {
				// DB에서 가져왔으나 삭제된 작업은 무시한다.
				++length;
			}
		}
		if (length) {
			innerHTML += '&nbsp;<INPUT TYPE="BUTTON" VALUE="삭제" onClick="fnDeleteWork();">';
		}
		oTd1.innerHTML = innerHTML;

		// 해당 열(들)을 만든다.
		for (var iCol = 0; iCol < gaColumn.length; ++iCol) {
			var	oCol = gaColumn[iCol];
			if (!fnIsSetChkFlag(oCol.chkId)) {	// 표시되지 않는 열이라면
				// 그냥 건너뛴다.
				continue;
			}

			var	oTd2 = oTr.insertCell();
			if (!oTd2) {
				alert('oTr.insertCell(): Failure');
				tblWorkSchedule.deleteRow(oTr.rowIndex);
				return false;
			}
			oTd2.align = "CENTER";

			var	columnID = oCol.id;
			if (columnID == 'COLUMN_ID__SCENE' || columnID == 'COLUMN_ID__WORKER') {
				oTd2.innerHTML = "&nbsp;";
				continue;
			} else if (columnID != 'COLUMN_ID__WORK_PERIOD') {
				alert('Unknown column ID = |' + columnID + '|');
				return false;
			}
			// column_ID = 'COLUMN_ID__WORK_PERIOD'

/*
	<TD WIDTH="80%"><TABLE BORDER="1" FRAME="VSIDES" RULES="NONE" WIDTH="100%" CELLSPACING="0" CELLPADDING="0"><TBODY><TR>
		<TD ALIGN="CENTER" WIDTH="20%">&nbsp;</TD>
		<TD ALIGN="CENTER" WIDTH="10%"><INPUT TYPE="BUTTON" VALUE="   <=   " onClick="fnDisplayWorkSchedule(new Date('...')); return false;"></TD>
		<TD ALIGN="CENTER" WIDTH="40%">2005/10</TD>
		<TD ALIGN="CENTER" WIDTH="10%"><INPUT TYPE="BUTTON" VALUE="   =>   " onClick="fnDisplayWorkSchedule(new Date('...')); return false;"></TD>
		<TD ALIGN="CENTER" WIDTH="20%">&nbsp;</TD>
	</TR></TBODY></TABLE></TD>
 */
			var	innerHTML = '';
			innerHTML += '<TABLE BORDER="1" FRAME="VSIDES" RULES="NONE" WIDTH="100%" CELLSPACING="0" CELLPADDING="0"><TBODY><TR>';
			innerHTML += '	<TD ALIGN="CENTER" WIDTH="20%">&nbsp;</TD>';
			var	previousMonth = new Date();
			previousMonth.setDate(1);
			previousMonth.setYear(givenDate.getYear());		// 연도를 먼저 맞추어야
			previousMonth.setMonth(givenDate.getMonth() - 1);	// 달이 -1이 되더라도, 연도를 1만큼 낮추고 달을 11로 만든다.
			innerHTML += '	<TD ALIGN="CENTER" WIDTH="10%"><INPUT ID="btnPreviousMonth" TYPE="BUTTON" VALUE="   <=   " onClick="fnMakeWorkScheduleTable(new Date(\'' + previousMonth + '\')); return false;"></TD>';
			innerHTML += '	<TD ALIGN="CENTER" WIDTH="40%">' + givenDate.getYear() + '년 ' + (1 + givenDate.getMonth()) + '월</TD>';
			var	nextMonth = new Date();
			nextMonth.setDate(1);
			nextMonth.setYear(givenDate.getYear());			// 연도를 먼저 맞추어야
			nextMonth.setMonth(givenDate.getMonth() + 1);		// 달이 12가 되더라도, 연도를 1만큼 올리고 달을 0으로 만든다.
			innerHTML += '	<TD ALIGN="CENTER" WIDTH="10%"><INPUT ID="btnNextMonth" TYPE="BUTTON" VALUE="   =>   " onClick="fnMakeWorkScheduleTable(new Date(\'' + nextMonth + '\')); return false;"></TD>';
			innerHTML += '	<TD ALIGN="CENTER" WIDTH="20%">&nbsp;</TD>';
			innerHTML += '</TR></TBODY></TABLE>';
			oTd2.innerHTML = innerHTML;

			// 표시가능한 달인지 알아본다.
			// (정책) 제일 나중 날짜로 잡는다.
			// 그래야 작업행이 없을 때, 표시 달[月]에 상관없이 " <= "버튼이 비활성화된다.
			// (아래부분을 참고하라.)
			var	leastBeginDate = new Date('2080/12/31');
			// (정책) 제일 처음 날짜로 잡는다.
			// 그래야 작업행이 없을 때, 표시 달[月]에 상관없이 " => "버튼이 비활성화된다.
			// (아래부분을 참고하라.)
			var	greatestEndDate = new Date('1/1/1');
			for (var i = 0; i < gWork.length; ++i) {
				if (gWork[i].flag & cOFLAG_DELETED) {	// DB에서 가져왔으니 삭제된 작업은
					// 건너뛴다.
					continue;
				}

				var	beginDate = new Date(gWork[i].date_begin);
				var	endDate = new Date(gWork[i].date_end);
				if (beginDate.valueOf() < leastBeginDate.valueOf()) {
					leastBeginDate.setDate(beginDate.getDate());	// 날[日]을 먼저 맞추어야
					leastBeginDate.setMonth(beginDate.getMonth());	// 달[月]을 맞출 때 제대로 된다.
											// 만약 달을 먼저 맞추면 현재 가지고 있는 날[日]이
											// 맞추려는 달의 날수보다 크면 "자동적"으로 달이 증가한다.
											// 예를 들어 날이 31인 상태에서 맞추려는 달이 6월이라고 하자.
											// 6월달은 30일까지 있으므로, 이 상태에서 달을 맞추면
											// 달은 7월이 되고 날은 1이된다.
					leastBeginDate.setYear(beginDate.getYear());
				}
				if (greatestEndDate.valueOf() < endDate.valueOf()) {
					greatestEndDate.setDate(endDate.getDate());	// 앞에와 마찬가지 이유로
					greatestEndDate.setMonth(endDate.getMonth());
					greatestEndDate.setYear(endDate.getYear());
				}
			}
			// previousMonth의 제일 마지막 날이 제일 빠른 시작날보다 작으면 안된다.
			previousMonth.setDate(fnGetDaysOfMonthInYear(previousMonth));
			if (previousMonth.valueOf() < leastBeginDate.valueOf()) {
				// previousMonth는 표시될 수 없다.
				// 버튼을 비활성화한다.
				document.getElementById("btnPreviousMonth").disabled = true;
			} else {
				document.getElementById("btnPreviousMonth").disabled = false;
			}
			// nextMonth의 첫째 날이 제일 늦은 끝나는 날보다 크면 안된다.
			if (greatestEndDate.valueOf() < nextMonth.valueOf()) {
				// nextMonth는 표시될 수 없다.
				// 버튼을 비활성화한다.
				document.getElementById("btnNextMonth").disabled = true;
			} else {
				document.getElementById("btnNextMonth").disabled = false;
			}
		}

		return true;
	}
</SCRIPT>
<SCRIPT LANGUAGE="JAVASCRIPT1.3">
/*
 * 2005/10/19 (EST)
 *		By 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 작업일정표를 행단위로 만든다.
 */

	// 작업에 대한 정보를 가진 oWork를 이용해서
	// 작업일정표(tblWorkSchedule)의 iRow번째 행을 만든다.
	// 작업기간(월별)에는 givenDate에 해당하는 년월을 표시한다.
	function fnInsertWorkRow(oWork, iRow, givenDate) {
		// givenDate를 기준으로, 작업기간 정보를 이용해서 파티션을 만든다.
		var	aPartition = [];
		if (fnSetPartition(givenDate, new Date(oWork.date_begin), new Date(oWork.date_end), aPartition)) {
			// 작업기간이 givenDate에 걸쳐있다.
		} else {
			// 작업기간이 givenDate에 걸쳐있지 않다.
		}

		// 행을 만든다.
		var	oTr = tblWorkSchedule.insertRow(iRow);
		if (!oTr) {
			alert('tblWorkSchedule.insertRow(): Failure');
			return false;
		}

		// 해당 열(들)을 만든다.
		// "작업"열은 항상 표시한다.
		var	oTd1 = oTr.insertCell();
		if (!oTd1) {
			alert('oTr.insertCell(): Failure');
			tblWorkSchedule.deleteRow(oTr.rowIndex);
			return false;
		}
		var	work;
		if (oWork.workType == cWORK_DESIGN) {
			var	aTarget = oWork.targetType == cTARGET_CHR ? gShow.aChr : gShow.aBg;
			var	targetId = oWork.targetId;
			var	targetName;
			for (var i = 0; i < aTarget.length; ++i) {
				if (targetId == aTarget[i].id) {
					targetName = aTarget[i].name;
					break;
				}
			}
			if (targetName == undefined) {
				alert('TARGET_ID = ' + targetId + '에 해당하는 ' + (oWork.targetType == cTARGET_CHR ? '인물' : '배경') + '을 찾을 수 없습니다');
				targetName = '未詳';
			}
			work = (oWork.targetType == cTARGET_CHR ? '인물' : '배경') + '(' + targetName + ') ' + (oWork.designType == cDESIGN_CONCEPT ? '컨셉 ' : '') + '디자인';
		} else {
			work = '스토리보드';
		}
//		oTd1.innerHTML = '&nbsp;<INPUT TYPE="RADIO" NAME="rdoiWork" VALUE="" onClick="fnrdoiWorkOnClick(this);">&nbsp;&nbsp;' + work;
		var	bgColor = !(oWork.flag & cOFLAG_DB) ? cBGCOLOR_INSERT : (oWork.getFlag() & ~cOFLAG_DB) == 0 ? cBGCOLOR_SELECT : cBGCOLOR_UPDATE;
//		oTd1.style.background = bgColor;
		oTd1.innerHTML = '&nbsp;<INPUT TYPE="RADIO" NAME="rdoiWork" VALUE="" STYLE="background: ' + bgColor + ';" onClick="fnrdoiWorkOnClick(this);">&nbsp;&nbsp;' + work;
		for (var iCol = 0; iCol < gaColumn.length; ++iCol) {
			var	oCol = gaColumn[iCol];
			if (!fnIsSetChkFlag(oCol.chkId)) {	// 표시되지 않는 열이라면
				// 그냥 건너뛴다.
				continue;
			}

			var	oTd2 = oTr.insertCell();
			if (!oTd2) {
				alert('oTr.insertCell(): Failure');
				tblWorkSchedule.deleteRow(oTr.rowIndex);
				return false;
			}
			oTd2.align = "CENTER";

			var	innerHTML;
			var	colID = oCol.id;
			if (colID == 'COLUMN_ID__WORK_PERIOD') {
				innerHTML = '';
				innerHTML += '<TABLE BORDER="1" FRAME="VSIDES" RULES="NONE" WIDTH="100%" CELLSPACING="0" CELLPADDING="0" CLASS="TBAND"><TBODY><TR ALIGN="CENTER" TITLE="' + oWork.date_begin + ' - ' + oWork.date_end + '">';
				for (var j = 0; j < aPartition.length; ++j) {
					innerHTML += '	<TD WIDTH="' + aPartition[j].fraction + '%" STYLE="background-color: ' + aPartition[j].color + '">' + aPartition[j].segment + '</TD>';
				}
				innerHTML += '</TR></TBODY></TABLE>';
			} else if (colID == 'COLUMN_ID__SCENE') {
				var	scnName;
				if (oWork.sceneId == 0) {
					scnName = '작품 전체';
				} else {
					for (var i = 0; i < gShow.aScn.length; ++i) {
						if (gShow.aScn[i].id == oWork.sceneId) {
							scnName = gShow.aScn[i].name;
							break;
						}
					}
				}
				if (scnName == undefined) {
					alert('장면(SCENE_ID = ' + oWork.sceneId + ')을 찾을 수 없다');
				}
				innerHTML = scnName;
			} else if (colID == 'COLUMN_ID__WORKER') {
				var	aUser = [];
				for (var iUser = 0; iUser < oWork.aUserId.length; ++iUser) {
					for (var i = 0; i < gUser.length; ++i) {
						if (gUser[i].id == oWork.aUserId[iUser]) {
							aUser.push(gUser[i].name);
							break;
						}
					}
					if (gUser.length == 0 || i == gUser.length) {
						alert('USER_ID = ' + oWork.aUserId[iUser] + '에 해당하는 USER를 찾을 수 없다');
						aUser.push('Unknown User(ID = ' + oWork.aUserId[iUser] + ')');
					}
				}
				innerHTML = aUser.sort().join(', ');
			} else {
				alert('Unknown column ID = |' + colID + '|');
				return false;
			}
			oTd2.innerHTML = innerHTML;
		}

		return true;
	}
</SCRIPT>
<SCRIPT LANGUAGE="JAVASCRIPT1.3">
/*
 * 2005/10/19 (EST)
 *		By Gwang-Ho Kim (iccentric@gmail.com)
 *
 * 작업을 관리하는 부분을 완성한다.
 *
 */

	// 2005/11/12 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
	// 작업테이블에서 선택된 작업의 내용을 작업관리창에 나타나게 한다.
	// 원하면 아무 때나 수정할 수 있도록 한다.
	function fnSetWork() {
		if (!gbWorkManager) {
			// 작업관리창이 열려있지 않으면 할 일이 없다.
			return;
		}

		if (giWork == -1) {
			// 작업이 하나도 없다.
			// 기본값을 가지고 선택하도록 한다.

			// (정책) 작업 유형의 기본값은 인물에 대한 컨셉 디자인이다.
			gWrkEdtFlag = cWEFLAG_CONCPT;
			if (gShow.aChr.length == 0) {	// 선택할 인물이 없다.
				// (정책) 선택할 인물이 없을 경우에는 대상이 배경으로 바뀐다.
				gWrkEdtFlag |= cWEFLAG_BG;
				if (gShow.aBg.length == 0) {	// 선택할 배경도 없다.
					// 디자인할 대상이 없으므로 디자인 작업은 불가능하다.
					// (정책) 스토리보드 작업으로 바뀐다.
					gWrkEdtFlag = cWEFLAG_STRYBRD;
					if (gShow.aScn.length == 0) {	// 선택할 장면도 없다.
						// 어떤 작업도 불가능하다.
						alert('대상(인물/배경)이 없으므로 디자인 작업도 불가능하고, 장면도 없으니 스토리보드 작업도 불가능하다');
						return;
					}
					// 장면의 목록과 선택은 아래에서 이루어진다.
				} else {
					// (정책) 첫 번째 배경을 기본적으로 선택하게 한다.
					sltBg.options[0].selected = true;
				}
			} else {
				// (정책) 첫 번째 인물을 기본적으로 선택하게 한다.
				sltChr.options[0].selected = true;
			}
			// 해당 버튼이 선택되게 한다.
			fnSetRadioChecked();

			// 작업의 유형에 따라 장면의 목록을 만든 후,
			if (gWrkEdtFlag & cWEFLAG_STRYBRD)
				fnAddOptionsToScnSelect(gShow.aScn);
			else
				fnSelectOnChange(gWrkEdtFlag & cWEFLAG_BG ? "Bg" : "Chr");
			// (정책) 첫 번재 장면을 선택한다.
			sltScn.options[0].selected = true;

			return;
		}

		// giWork >= 0: 작업이 적어도 하나가 있다.
		// 1. 작업의 유형을 설정한다.
		var	iWork = fniWorkByRow(giWork);
		if (iWork == -1) {
			// 찾을 수 없다!
			// 이럴 수가 없는데...
			alert('작업테이블의 ' + (1 + giWork) + '번째 행에 해당하는 작업을 찾을 수 없다');
			return;
		}
		var	oWork = gWork[iWork];	// 선택된 작업에 대한 정보를 담고 있다
		// 작업의 종류를 결정해서 gWrkEdtFlag의 해당 비트를 0이나 1로 한다.
		if (oWork.workType == cWORK_DESIGN) {
			gWrkEdtFlag &= ~cWEFLAG_STRYBRD;
			if (oWork.designType == cDESIGN_CONCEPT) {
				gWrkEdtFlag |= cWEFLAG_CONCPT;
			} else {
				gWrkEdtFlag &= ~cWEFLAG_CONCPT;
			}
			if (oWork.targetType == cTARGET_CHR) {
				gWrkEdtFlag &= ~cWEFLAG_BG;
			} else {
				gWrkEdtFlag |= cWEFLAG_BG;
			}
		} else {
			gWrkEdtFlag |= cWEFLAG_STRYBRD;
		}
		// 해당 버튼이 선택되게 한다.
		fnSetRadioChecked();

		// 2. 선택하는 부분들(작업자, 인물/배경, 장면)에서 작업에 맞는 부분을 선택한다.
		// 작업자 목록에서...
		for (var i = 0; i < sltWorker.options.length; ++i) {
			var	oOp = sltWorker.options[i];
			var	userId = oOp.value;
			for (var iUser = 0; iUser < oWork.aUserId.length; ++iUser) {
				if (userId == oWork.aUserId[iUser])
					break;
			}
			oOp.selected = !(oWork.aUserId.length == 0 || iUser == oWork.aUserId.length);
		}
		// 디자인 작업이라면 인물/배경 목록에서...
		if (!(gWrkEdtFlag & cWEFLAG_STRYBRD)) {
			// 인물/배경 목록에서...
			eval("var sltChrBg = slt" + (gWrkEdtFlag & cWEFLAG_BG ? "Bg" : "Chr"));
			for (var i = 0; i < sltChrBg.options.length; ++i) {
				var	oOp = sltChrBg.options[i];
				oOp.selected = oOp.value == oWork.targetId;
			}
		}
		// 작업의 유형에 따라 장면의 목록을 만든 후,
		if (gWrkEdtFlag & cWEFLAG_STRYBRD)
			fnAddOptionsToScnSelect(gShow.aScn);
		else
			fnSelectOnChange(gWrkEdtFlag & cWEFLAG_BG ? "Bg" : "Chr");
		// 작업에 쓰이는 장면을 선택한다.
		for (var i = 0; i < sltScn.options.length; ++i) {
			var	oOp = sltScn.options[i];
			oOp.selected = oOp.value == oWork.sceneId;
		}

		// 3. 나머지(작업기간, 지시사항, ...)도 설정한다.
		// 작업기간
		idWorkBegin.value = fnMakeDateString(oWork.date_begin);
		idWorkEnd.value = fnMakeDateString(oWork.date_end);
		// 지시사항
		idOrder.value = oWork.order;
	}
	function fnToggleWorkManager() {
		gbWorkManager = !gbWorkManager;
		idWorkManager.style.display = gbWorkManager ? "block" : "none";
		if (gbWorkManager) {
			// giWork가 가리키는 작업에 따라
			// 작업관리창의 해당 부분을 설정한다.
			fnSetWork();
		}
	}

	function fnrdoiWorkOnClick(o) {
		//        INPUT            TD            TR
		var	oTr = o.parentElement.parentElement;
//		alert(oTr.nodeName);
		if (oTr.nodeName != "TR") {
			alert('oTr.nodeName(' + oTr.nodeName + ', o.nodeName = ' + o.nodeName + ') != "TR"');
			// (정책) 제일 마지막 행을 가리키도록 한다.
			giWork = gWork.length - 1;
		} else {
			giWork = oTr.rowIndex - 1;	// 머리행은 제외한다.
		}

		// 작업관리창이 열려있다면 giWork가 가리키는 작업에 따라
		// 작업관리창의 해당 부분을 설정한다.
		fnSetWork();
	}

	var	cACTION_INSERT		= 0;	// 선택된 행 앞에 추가
	var	cACTION_UPDATE		= 1;	// 선택된 행을 변경
	// 작업테이블에서 rowIndex행에 해당하는 작업의 gWork배열에 대한 첨자를 얻는다.
	function fniWorkByRow(rowIndex) {
		for (var iWork = 0; iWork < gWork.length; ++iWork) {
			var	oWork = gWork[iWork];
			if (!(oWork.flag & cOFLAG_DELETED) && oWork.rowIndex == rowIndex) {
				return iWork;
			}
		}
		return -1;
	}
	// 기존의 작업 중에서 주어진 작업과 유형이 같은 작업이 있는 지 알아본다.
	// iIgnore가 정의되어 있다면 그 작업은 건너뛴다.
	function fniExistWork(oWork, iIgnore) {
		for (var iWork = 0; iWork < gWork.length; ++iWork) {
			var	oWork_ = gWork[iWork];
			if (
				(iIgnore == undefined || iWork != iIgnore) &&
				oWork_.workType == oWork.workType &&
				// 스토리보드 작업: SCENE_ID가 같으면 같은 작업
				// 디자인 작업: (DESIGN_TYPE, TARGET_ID, SCENE_ID)가 같으면 같은 작업
				(
					oWork_.workType == cWORK_STORYBOARD ?
					oWork_.sceneId == oWork.sceneId :
					(oWork_.designType == oWork.designType && oWork_.targetId == oWork.targetId && oWork_.sceneId == oWork.sceneId)
				)
			) {
				return iWork;
			}
		}
		return -1;
	}
	function fnInsertWork() {
////////////////////////////////////////////////////////////////////////
		// 작업을 결정한다.
		var	workType;
		var	designType;
		var	targetType;
		var	targetId;
		if (!(gWrkEdtFlag & cWEFLAG_STRYBRD)) {	// 디자인 작업의 경우에는...
			workType = cWORK_DESIGN;
			if (gWrkEdtFlag & cWEFLAG_BG) {	// 대상으로 배경이 선택되었다.
				targetType = cTARGET_BG;
				targetId = sltBg.options[sltBg.selectedIndex].value;
			} else {			// 대상으로 인물이 선택되었다.
				targetType = cTARGET_CHR;
				targetId = sltChr.options[sltChr.selectedIndex].value;
			}
			designType = gWrkEdtFlag & cWEFLAG_CONCPT ? cDESIGN_CONCEPT : cDESIGN_DETAIL;
		} else {
			workType = cWORK_STORYBOARD;
		}
//		work = workType == cWORK_DESIGN ? ((targetType == cTARGET_CHR ? '인물' : '배경') + ' ' + (designType == cDESIGN_CONCEPT ? '컨셉 ' : '') + '디자인') : '스토리보드';
//		alert(work);

		// 장면을 결정한다.
		var	sceneId = sltScn.options[sltScn.selectedIndex].value;
//		alert(sceneId);

		// 작업자를 결정한다.
		if (sltWorker.selectedIndex == -1) {
			alert('적어도 한명의 작업자를 지정해야 합니다.');
			return false;
		}
		var	aUserId = [];
		var	options = sltWorker.options;
		for (var i = 0; i < options.length; ++i) {
			if (options[i].selected) {
				aUserId.push(options[i].value);
			}
		}
//		alert(aUserId);

		// 작업기간을 결정한다.
		var	workBegin = idWorkBegin.value.replace(/-/g, '/');
		var	dtWorkBegin = new Date(workBegin);
		if (isNaN(dtWorkBegin.valueOf())) {
			var	msg = '';
			msg += '내용: 작업을 시작하는 날짜를 입력하는 곳에 유효한 날짜가 입력되지 않았습니다.' + '\n';
			msg += '입력값: "' + workBegin + '"';
			alert(msg);
			return false;
		}
//		alert(dtWorkBegin.getYear() + '년 ' + (1 + dtWorkBegin.getMonth()) + '월 ' + dtWorkBegin.getDate() + '일');
		var	workEnd = idWorkEnd.value.replace(/-/g, '/');
		var	dtWorkEnd = new Date(workEnd);
		if (isNaN(dtWorkEnd.valueOf())) {
			var	msg = '';
			msg += '내용: 작업을 끝내는 날짜를 입력하는 곳에 유효한 날짜가 입력되지 않았습니다.' + '\n';
			msg += '입력값: "' + workEnd + '"';
			alert(msg);
			return false;
		}
//		alert(dtWorkEnd.getYear() + '년 ' + (1 + dtWorkEnd.getMonth()) + '월 ' + dtWorkEnd.getDate() + '일');
		if (dtWorkBegin > dtWorkEnd) {
			var	msg = '';
			msg += '시작하는 날짜(';
			msg += dtWorkBegin.getYear() + '년 ' + (1 + dtWorkBegin.getMonth()) + '월 ' + dtWorkBegin.getDate() + '일';
			msg += ')가 끝내는 날짜(';
			msg += dtWorkEnd.getYear() + '년 ' + (1 + dtWorkEnd.getMonth()) + '월 ' + dtWorkEnd.getDate() + '일';
			msg += ')보다 나중입니다.';
			alert(msg);
			return false;
		}
		// 지시사항
		var	order = idOrder.value;
		// 진행상황
		var	status = 1;	// 기본값으로...

		// 여기까지 작업을 정의하는 데 필요한 모든 정보를 모았다.
		// 작업을 만든다.
		//	작업 유형: workType
		//	WORK_ID: 0	// 새로 만드므로...
		//	장면: sceneId
		//	작업자를 구성하는 USER: aUserId
		//	시작하는 날짜: workBegin
		//	끝내는 날짜: workEnd
		//	지시사항: order
		//	진행상황: status(= 1: 시작)
		// 디자인 작업의 경우(workType = cWORK_DESIGN)
		//	디자인 유형: designType
		//	대상: targetType, targetId
		// 작업테이블의 giWork번째 행에 다음 작업을 표시한다.
		if (giWork == -1) {	// 작업이 하나도 없는 경우
			giWork = 0;
		}
		oWork = workType == cWORK_DESIGN ?
			new oDesign(0, sceneId, aUserId, workBegin, workEnd, order, status, designType, targetType, targetId, giWork) :
			new oStoryboard(0, sceneId, aUserId, workBegin, workEnd, order, status, giWork)
		;
////////////////////////////////////////////////////////////////////////

		// 현재 작업과 일치하는 작업이 있는 알아본다.
		var	iUpdate = fniExistWork(oWork);
		if (
			iUpdate != -1 &&				// 같은 종류의 작업이 존재하고,
			!( gWork[iUpdate].flag & cOFLAG_DELETED)	// 여전히 그 작업이 살아있다.
		) {
			alert((1 + gWork[iUpdate].rowIndex) + '번째 작업과 중복됩니다');
			return false;
		}
		// iUpdate = -1인 경우
		//	이 작업을 현재 작업행(giWork)에 해당하는 작업의 바로 앞에 추가한다.
		// iUpdate != -1인 경우
		//	iUpdate번째 행을 되살리고 고쳐서 다시 사용한다.
		if (iUpdate == -1) {
			// giWork값이 말해주는, 작업테이블에서 선택된 행을 근거로,
			// 새로운 작업이 gWork배열의 몇 번째 위치할 것인지를 결정한다.
			var	iWork;
			if (gWork.length) {
				iWork = fniWorkByRow(giWork);
				if (iWork == -1) {
					alert('작업테이블에서 ' + (1 + giWork) + '번째 행에 해당하는 작업을 찾을 수 없다');
					return false;
				}
			} else
				iWork = 0;
			// i번째에 작업을 추가한다.
			gWork.splice(iWork, 0, oWork);
		} else {
			// 삭제되었던 행을 
			gWork[iUpdate].rowIndex = giWork;		// giWork번째 행에
			gWork[iUpdate].flag &= ~cOFLAG_DELETED;	// 되살린다.

			// 작업의 내용을 고친다.
			gWork[iUpdate].sceneId = sceneId;
			gWork[iUpdate].aUserId = aUserId;
			gWork[iUpdate].date_begin = new Date(workBegin);
			gWork[iUpdate].date_end = new Date(workEnd);
			gWork[iUpdate].order = order
			gWork[iUpdate].status = status;
			if (workType == cWORK_DESIGN) {
				gWork[iUpdate].designType = designType;
				gWork[iUpdate].targetType = targetType;
				gWork[iUpdate].targetId = targetId;
			}
			// 아래에서 참고할 수 있도록 한다.
			oWork = gwork[iUpdate];
		}
		// 작업테이블의 giWork번째 행에 작업을 추가하므로
		// 살아있는(?) 작업 중에서 rowIndex가 giWork보다 크거나 같은 작업의 rowIndex값을 하나 증가시킨다.
		for (var i = 0; i < gWork.length; ++i) {
			var	o = gWork[i];
			if (!(o.flag & cOFLAG_DELETED) && o.rowIndex >= giWork) {
				++o.rowIndex;
			}
		}
		// 단, 방금 전에 추가한 작업은 제외한다.
		// 방금 추가한 작업도 여기에 해당하므로 추가한 작업의 .rowIndex을 하나 감소한다.
		--oWork.rowIndex;

////////////////////////////////////////////////////////////////////////
		// giWork번째 행에 oWork의 정보를 쓴다.
		// gCurDate를 기준으로 작업행을 만든다.
		if (!fnInsertWorkRow(oWork, 1 + giWork, gCurDate)) {	// 머리행은 건너뛴다.
			alert('fnInsertWorkRow(): Failure');
			return false;
		}

		// giWork에 해당하는 작업행의 RADIO버튼을 선택한다.
		if (!fnSetWorkIndexChecked(giWork)) {
			alert('fnSetWorkIndexChecked(): Failure');
			return false;
		}

		// gCurDate를 기준으로 작업일정표(tblWorkSchedule)의 꼬리행을 다시 만든다.
		tblWorkSchedule.deleteRow();
		if (!fnMakeFootRow(gCurDate)) {
			alert('fnMakeFootRow(): Failure');
			return false;
		}

		return true;
////////////////////////////////////////////////////////////////////////
	}
	function fnUpdateWork() {
////////////////////////////////////////////////////////////////////////
		// 작업을 결정한다.
		var	workType;
		var	designType;
		var	targetType;
		var	targetId;
		if (!(gWrkEdtFlag & cWEFLAG_STRYBRD)) {	// 디자인 작업의 경우에는...
			workType = cWORK_DESIGN;
			if (gWrkEdtFlag & cWEFLAG_BG) {	// 대상으로 배경이 선택되었다.
				targetType = cTARGET_BG;
				targetId = sltBg.options[sltBg.selectedIndex].value;
			} else {			// 대상으로 인물이 선택되었다.
				targetType = cTARGET_CHR;
				targetId = sltChr.options[sltChr.selectedIndex].value;
			}
			designType = gWrkEdtFlag & cWEFLAG_CONCPT ? cDESIGN_CONCEPT : cDESIGN_DETAIL;
		} else {
			workType = cWORK_STORYBOARD;
		}
//		work = workType == cWORK_DESIGN ? ((targetType == cTARGET_CHR ? '인물' : '배경') + ' ' + (designType == cDESIGN_CONCEPT ? '컨셉 ' : '') + '디자인') : '스토리보드';
//		alert(work);

		// 장면을 결정한다.
		var	sceneId = sltScn.options[sltScn.selectedIndex].value;
//		alert(sceneId);

		// 작업자를 결정한다.
		if (sltWorker.selectedIndex == -1) {
			alert('적어도 한명의 작업자를 지정해야 합니다.');
			return false;
		}
		var	aUserId = [];
		var	options = sltWorker.options;
		for (var i = 0; i < options.length; ++i) {
			if (options[i].selected) {
				aUserId.push(options[i].value);
			}
		}
//		alert(aUserId);

		// 작업기간을 결정한다.
		var	workBegin = idWorkBegin.value.replace(/-/g, '/');
		var	dtWorkBegin = new Date(workBegin);
		if (isNaN(dtWorkBegin.valueOf())) {
			var	msg = '';
			msg += '내용: 작업을 시작하는 날짜를 입력하는 곳에 유효한 날짜가 입력되지 않았습니다.' + '\n';
			msg += '입력값: "' + workBegin + '"';
			alert(msg);
			return false;
		}
//		alert(dtWorkBegin.getYear() + '년 ' + (1 + dtWorkBegin.getMonth()) + '월 ' + dtWorkBegin.getDate() + '일');
		var	workEnd = idWorkEnd.value.replace(/-/g, '/');
		var	dtWorkEnd = new Date(workEnd);
		if (isNaN(dtWorkEnd.valueOf())) {
			var	msg = '';
			msg += '내용: 작업을 끝내는 날짜를 입력하는 곳에 유효한 날짜가 입력되지 않았습니다.' + '\n';
			msg += '입력값: "' + workEnd + '"';
			alert(msg);
			return false;
		}
//		alert(dtWorkEnd.getYear() + '년 ' + (1 + dtWorkEnd.getMonth()) + '월 ' + dtWorkEnd.getDate() + '일');
		if (dtWorkBegin > dtWorkEnd) {
			var	msg = '';
			msg += '시작하는 날짜(';
			msg += dtWorkBegin.getYear() + '년 ' + (1 + dtWorkBegin.getMonth()) + '월 ' + dtWorkBegin.getDate() + '일';
			msg += ')가 끝내는 날짜(';
			msg += dtWorkEnd.getYear() + '년 ' + (1 + dtWorkEnd.getMonth()) + '월 ' + dtWorkEnd.getDate() + '일';
			msg += ')보다 나중입니다.';
			alert(msg);
			return false;
		}
		// 지시사항
		var	order = idOrder.value;
		// 진행상황
		var	status = 1;	// 기본값으로...

		// 여기까지 작업을 정의하는 데 필요한 모든 정보를 모았다.
		// 작업을 만든다.
		//	작업 유형: workType
		//	WORK_ID: 0	// 새로 만드므로...
		//	장면: sceneId
		//	작업자를 구성하는 USER: aUserId
		//	시작하는 날짜: workBegin
		//	끝내는 날짜: workEnd
		//	지시사항: order
		//	진행상황: status(= 1: 시작)
		// 디자인 작업의 경우(workType = cWORK_DESIGN)
		//	디자인 유형: designType
		//	대상: targetType, targetId
		// 작업테이블의 giWork번째 행에 다음 작업을 표시한다.
		if (giWork == -1) {	// 작업이 하나도 없는 경우
			giWork = 0;
		}
		oWork = workType == cWORK_DESIGN ?
			new oDesign(0, sceneId, aUserId, workBegin, workEnd, order, status, designType, targetType, targetId, giWork) :
			new oStoryboard(0, sceneId, aUserId, workBegin, workEnd, order, status, giWork)
		;
////////////////////////////////////////////////////////////////////////

		// giWork값이 말해주는, 작업테이블에서 선택된 행을 근거로,
		// 수정할 작업이 gWork배열의 몇 번째 위치할 것인지를 결정한다.
		var	iUpdate = fniWorkByRow(giWork);
		if (iUpdate == -1) {
			alert('작업테이블에서 ' + (1 + giWork) + '번째 행에 해당하는 작업을 찾을 수 없다');
			return false;
		}
		// 현재 작업을 제외하고 현재 작업과 일치하는 작업이 있는 알아본다.
		var	iExistWork = fniExistWork(oWork, iUpdate);
		if (
			iExistWork != -1 &&				// 같은 종류의 작업이 존재하고,
			!(gWork[iExistWork].flag & cOFLAG_DELETED)	// 여전히 그 작업이 살아있다.
		) {
			alert((1 + gWork[iExistWork].rowIndex) + '번째 작업과 중복됩니다');
			return false;
		}
		// 작업을 고친다.
		gWork[iUpdate].sceneId = sceneId;
		gWork[iUpdate].aUserId = aUserId;
		gWork[iUpdate].date_begin = new Date(workBegin);
		gWork[iUpdate].date_end = new Date(workEnd);
		gWork[iUpdate].order = order
		gWork[iUpdate].status = status;
		if (workType == cWORK_DESIGN) {
			gWork[iUpdate].designType = designType;
			gWork[iUpdate].targetType = targetType;
			gWork[iUpdate].targetId = targetId;
		}
		// giWork행을 지운다.
		tblWorkSchedule.deleteRow(1 + giWork);
		// 아래에서 참고할 수 있도록 한다.
		oWork = gWork[iUpdate];

////////////////////////////////////////////////////////////////////////
		// giWork번째 행에 oWork의 정보를 쓴다.
		// gCurDate를 기준으로 작업행을 만든다.
		if (!fnInsertWorkRow(oWork, 1 + giWork, gCurDate)) {	// 머리행은 건너뛴다.
			alert('fnInsertWorkRow(): Failure');
			return false;
		}

		// giWork에 해당하는 작업행의 RADIO버튼을 선택한다.
		if (!fnSetWorkIndexChecked(giWork)) {
			alert('fnSetWorkIndexChecked(): Failure');
			return false;
		}

		// gCurDate를 기준으로 작업일정표(tblWorkSchedule)의 꼬리행을 다시 만든다.
		tblWorkSchedule.deleteRow();
		if (!fnMakeFootRow(gCurDate)) {
			alert('fnMakeFootRow(): Failure');
			return false;
		}

		return true;
////////////////////////////////////////////////////////////////////////
	}
	// index에 해당하는 RADIO버튼이 CHECKED로 한다.
	function fnSetWorkIndexChecked(index) {
		var	length = 0;	// 테이블에 표시할 작업의 갯수
		for (var iWork = 0; iWork < gWork.length; ++iWork) {
			if (!(gWork[iWork].flag & cOFLAG_DELETED)) {
				// DB에서 가져왔으나 삭제된 작업은 무시한다.
				++length;
			}
		}
		if (length == 0) {	// 표시될 것이 없다.
			return true;
		}
		var	ordoiWork = document.getElementsByName("rdoiWork");
		if (ordoiWork == undefined) {
			alert('document.getElementsByName("rdoiWork"): Failure');
			return false;
		}
		for (var i = 0; i < ordoiWork.length; ++i) {
			if (i == index) {
				ordoiWork[i].checked = true;
				break;
			}
		}
		if (ordoiWork.length == 0 || i == ordoiWork.length) {
			alert('Cannot find index = ' + index);
			return false;
		}
		return true;
	}
	function fnDeleteWork() {
		if (
			giWork == undefined ||			// 정의되지 않았거나
			(giWork < 0 || giWork >= gWork.length)	// 유효하지 않다면
		) {
			alert('선택된 인덱스(' + giWork + ')가 정의되지 않았거나 유효하지 않습니다.');
			return false;
		}

		// 작업 테이블의 giWork번째 행에 해당하는 작업을 찾는다.
		var	iDelete = fniWorkByRow(giWork);

		// .rowIndex가 giWork보다 크거나 같은 작업에서 .rowIndex값을 하나 감소시킨다.
		for (var iWork = 0; iWork < gWork.length; ++iWork) {
			if (!(gWork[iWork].flag & cOFLAG_DELETED) && gWork[iWork].rowIndex >= giWork) {
				--gWork[iWork].rowIndex;
			}
		}

		// iDelete번째 작업을 삭제하거나 삭제했다고 표시한다.
		if (gWork[iDelete].flag & cOFLAG_DB) {	// DB에서 가져온 작업이라면
			// 지워졌다는 표시만 한다.
			gWork[iDelete].flag |= cOFLAG_DELETED;
		} else {				// 웹에서 새로 만든 작업이라면
			// 작업을 여기서 없앤다.
			gWork.splice(iDelete, 1);
		}

		// 테이블에서 해당하는 행을 지운다.
		tblWorkSchedule.deleteRow(1 + giWork);	// 머리행은 건너뛴다.
		if (giWork == tblWorkSchedule.rows.length - 2) {	// 제일 마지막을 가리키고 있었다면
			// 새로운 마지막을 가리키도록 한다.
			giWork = tblWorkSchedule.rows.length - 3;
		}

		// giWork에 해당하는 작업행의 RADIO버튼을 선택한다.
		if (!fnSetWorkIndexChecked(giWork)) {
			alert('fnSetWorkIndexChecked(): Failure');
			return false;
		}

		// gCurDate를 기준으로 작업일정표(tblWorkSchedule)의 꼬리행을 다시 만든다.
		tblWorkSchedule.deleteRow();
		if (!fnMakeFootRow(gCurDate)) {
			alert('fnMakeFootRow(): Failure');
			return false;
		}

		// 작업관리창이 열려있다면 giWork가 가리키는 작업에 따라
		// 작업관리창의 해당 부분을 설정한다.
		fnSetWork();

		return true;
	}
</SCRIPT>
<SCRIPT LANGUAGE="JAVASCRIPT1.3">
/*
 * 2005/10/18 (EST)
 *		By Gwang-Ho Kim (iccentric@gmail.com)
 *
 * "작업 관리"창에 필요한 변수를 설정한다.
 */

	var	gbWorkManager;		// true: "작업 관리"창을 연다.
					// false: "작업 관리"창을 닫는다.

	var	gWrkEdtFlag;		// 어느 형태의 작업이 설정되었는 지를 말한다.
	// "인물"과 "배경"은 "장면 #"뿐만 아니라 어떤 인물이나 배경에 대한 디자인인지
	// 그리고 그 디자인이 컨셉인지 아닌지를 지정해야 한다.
	// 컨셉디자인이라면 그것이 전체에 대한 것인지 아니면 특정 장면에 대한 것인지 말해야 한다.
	var	cWEFLAG_STRYBRD	= 1 << 0;	// 이 비트가 설정되면 작업은 스토리보드를 만드는 것이다.
						// 다른 모든 비트는 무시된다.
						// 설정되어 있지 않다면 작업은 디자인을 하는 것이다.
						// 나머지 비트를 참조하여,
						// 형태와 범위 그리고 대상(인물/배경)을 결정한다.
	var	cWEFLAG_BG	= 1 << 1;	// 이 비트가 설정되면 "배경"에 대한 디자인이고,
						// 설정되지 않았다면 "인물"에 대한 것이다.
	var	cWEFLAG_CONCPT	= 1 << 2;	// 이 비트가 설정되어 있다면 디자인의 형태는 컨셉디자인이다.
						// 이 비트가 설정되어 있지 않다면 디자인의 형태는 구체적인 것이다.
	// 가능한 조합은 다음과 같다. (X: 참조하지 않는 비트임을 뜻한다.)
	//	XX1: 특정 장면에 대한 스토리보드
	//	000: 작품 전체에서 쓰이는 인물에 대한 구체적인 디자인
	//	010: 작품 전체에서 쓰이는 배경에 대한 구체적인 디자인
	//	000: 특정 장면에 쓰이는 인물에 대한 구체적인 디자인
	//	010: 특정 장면에 쓰이는 배경에 대한 구체적인 디자인
	//	100: 인물에 대한 전체적인 컨셉디자인
	//	110: 배경에 대한 전체적인 컨셉디자인
	//	100: 특정 장면에 쓰이는 인물에 대한 컨셉디자인
	//	110: 특정 장면에 쓰이는 배경에 대한 컨셉디자인
</SCRIPT>
<SCRIPT LANGUAGE="JAVASCRIPT1.3">
/*
 * 2005/10/18 (EST)
 *		By Gwang-Ho Kim (iccentric@gmail.com)
 *
 * 페이지에 들어가거나 나올 때 사용되는 함수들이다.
 */

	function fnSetVar() {
		// gCurDate: (Date) object
		if (
			gCurDate == undefined ||		// gCurDate가 정의되지 않았거나
			isNaN((new Date(gCurDate)).valueOf())	// 유효한 날짜가 아니라면
		) {
			// (정책) 오늘은 기준으로 삼는다.
			gCurDate = new Date();
		} else {	// gCurDate는 문자열이다. 이를 이용해서 Date오브젝트로 만든다.
			gCurDate = new Date(gCurDate);
		}

		// gchkFlag: number
		gchkFlag = parseInt(gchkFlag);
		if (isNaN(gchkFlag)) {		// 숫자가 아니라면
			// (정책) 모든 플래그를 설정한다.
			gchkFlag = cCHKFLAG_WORKER | cCHKFLAG_SCENE | cCHKFLAG_WORK_PERIOD;
		} else {
			// 의미있는 플래그만 그대로 둔다.
			gchkFlag &= cCHKFLAG_WORKER | cCHKFLAG_SCENE | cCHKFLAG_WORK_PERIOD;
		}

		// gShowId: number
		gShowId = parseInt(gShowId);
		if (isNaN(gShowId)) {
			// (정책) 기본값은 첫 번째 작품이다.
			gShowId = gaShow[0].showId;
		} else {
			// 주어진 SHOW_ID가 있는 지 확인한다.
			for (var i = 0; i < gaShow.length; ++i) {
				if (gaShow[i].showId == gShowId) {	// 있다.
					break;
				}
			}
			if (i == gaShow.length) {	// 없다.
				// (정책) 기본값은 첫 번째 작품이다.
				gShowId = gaShow[0].showId;
			}
		}

		// gbWorkManager: boolean
		if (
			gbWorkManager == undefined ||		// 정의되지 않았거나
			(gbWorkManager != "true" && gbWorkManager != "false")	// 유효한 값이 아니라면
		) {
			// (정책) 기본값은 "작업 관리"창을 닫는다.
			gbWorkManager = false;
		} else {
			gbWorkManager = gbWorkManager == "true";
		}

		// gWrkEdtFlag: number
		gWrkEdtFlag = parseInt(gWrkEdtFlag);
		if (isNaN(gWrkEdtFlag)) {	// 숫자가 아니라면
			// (정책) 기본값은 특정장면에서 사용될 인물에 대한 구체적인 디자인이다.
			gWrkEdtFlag = 0;
		}
	}

	function fnOnLoad() {
		// 쿠키로부터 전역변수를 얻는다.
		fnGetCookie();

		// 사용할 데이터를 설정한다.
		fnSetData();

		// 작품에 관한 목록을 만든다.
		for (var i = 0; i < gaShow.length; ++i) {
			var	oOp = document.createElement("OPTION");
			oOp.value = gaShow[i].showId
			oOp.text = gaShow[i].showName
			sltShow.options.add(oOp);
		}
		// "작업자"를 구성하는 USER를 선택하는 SELECT의 OPTION들의 목록을 만든다.
		// 작품에 무관하기 때문에 여기서 만든다.
		if (gUser.length == 0) {
			alert('작업자가 하나도 없으니 작업에 관한 어떤 일도 진행할 수 없다');
			return;
		}
		for (var i = 0; i < gUser.length; ++i) {
			var	oOp = document.createElement("OPTION");
			oOp.value = gUser[i].id;
			oOp.text = gUser[i].name;
			sltWorker.options.add(oOp);
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
		// 1. 작업에 사용되는 gShow를 결정한다.
		for (var i = 0; i < gaShow.length; ++i) {
			if (gaShow[i].showId == showId) {
				gShow = gaShow[i];
				break;
			}
		}
		if (gaShow.length == 0 || i == gaShow.length) {
			alert('작품(SHOW_ID = ' + showId + ')에 해당하는 정보를 찾을 수 없다');
			return false;
		}
		// 작품목록에서 해당 작품을 선택하게 한다.
		for (var i = 0; i < sltShow.options.length; ++i) {
			var	oOp = sltShow.options[i];
			oOp.selected = showId == oOp.value;
		}
		// 작업을 만들 때 사용되는 인물/배경의 목록을 만든다.
		// 장면의 경우는 작업에 따라 달라진다.
		// 따라서 아래의 fnSetWork에서 만든다.
		var	oOp;
		// 인물에 대한 목록
		sltChr.options.length = 0;	// 초기화한다.
		for (var i = 0; i < gShow.aChr.length; ++i) {
			oOp = document.createElement("OPTION");
			oOp.value = gShow.aChr[i].id;
			oOp.text = gShow.aChr[i].name;
			sltChr.options.add(oOp);
		}
		// 배경에 대한 목록
		sltBg.options.length = 0;	// 초기화한다.
		for (var i = 0; i < gShow.aBg.length; ++i) {
			oOp = document.createElement("OPTION");
			oOp.value = gShow.aBg[i].id;
			oOp.text = gShow.aBg[i].name;
			sltBg.options.add(oOp);
		}

		// 2. 작업에 사용되는 gWork를 결정한다.
		var	iWork = fniWorkByShowId(showId);
		if (iWork == -1) {	// SHOW_ID = showId에 해당하는 작업이 하나도 없다.
			gaWork.push(new _oWork(showId, []));
			iWork = gaWork.length - 1;
		}
		gWork = gaWork[iWork].aWork;
		// 작업테이블에서 몇 번째 행이 선택되어야 하는 지 결정한다.
		// 최근에 선택된 행이 쿠키에 giWork라는 변수로 저장된다.
		// giWork: number
		giWork = parseInt(giWork);
		if (
			isNaN(giWork) ||			// 숫자가 아니거나
			(giWork < 0 || gWork.length <= giWork)	// 유효한 값이 아니라면
		) {
			// (정책) 기본값은 마지막 작업이다.
			// 따라서 "자연스럽게" 작업이 없는 경우는 -1이 giWork의 값이 된다.
			giWork = gWork.length - 1;
		}
		// 작업관리창이 열려있다면 giWork가 가리키는 작업에 따라
		// 작업관리창의 해당 부분을 설정한다.
		if (gbWorkManager) {
			idWorkManager.style.display = "block";
			fnSetWork();
		} else
			idWorkManager.style.display = "none";

		// 작업테이블을 만든다.
		if (!fnMakeWorkScheduleTable(new Date(gCurDate))) {
			alert('fnMakeWorkScheduleTable(): Failure');
			return false;
		}
		return true;
	}

	function fnOnUnload() {
		// 전역변수들을 쿠키로 저장한다.
		fnSetCookie();
	}

	window.onload = fnOnLoad;
	window.onunload = fnOnUnload;
</SCRIPT>
<SCRIPT LANGUAGE="JAVASCRIPT1.3">
/*
 * 2005/10/18 (EST)
 *		By Gwang-Ho Kim (iccentric@gmail.com)
 *
 * 모든 전역변수를 쿠키로 저장하고, 쿠키로부터 전역변수를 얻는다.
 */

	var	gaVarName = new Array(
					// typeof(...)
		'gCurDate',		// (Date) object
		'gchkFlag',		// number
		'gShowId',		// number
		'giWork',		// number

		'gbWorkManager',	// boolean
		'gWrkEdtFlag'		// number
	);
	function fnSetCookie() {
		// gCurDate: (Date) object
//		alert('gCurDate: ' + typeof(gCurDate) + ', ' + gCurDate);
		// gchkFlag: number
//		alert('gchkFlag: ' + typeof(gchkFlag) + ', ' + gchkFlag);
		// giWork: number
//		alert('giWork: ' + typeof(giWork) + ', ' + giWork);
		// gbWorkManager: boolean
//		alert('gbWorkManager: ' + typeof(gbWorkManager) + ', ' + gbWorkManager);
		// gWrkEdtFlag: number
//		alert('gWrkEdtFlag: ' + typeof(gWrkEdtFlag) + ', ' + gWrkEdtFlag);
		var	aVarVal = new Array(gaVarName.length);
		var	cookie = '';
		for (var i = 0; i < gaVarName.length; ++i) {
			aVarVal[i] = eval(gaVarName[i]);
		}
		document.cookie = 'WorkSchedule.Var=' + aVarVal;
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
			if (aNameVal[0] == "WorkSchedule.Var") {
				var	aVarVal = aNameVal[1].split(",");
				for (var k = 0; k < gaVarName.length; ++k) {
					eval(gaVarName[k] + ' = "' + aVarVal[k] + '"');
				}
			}
		}
	}
</SCRIPT>
<SCRIPT LANGUAGE="JAVASCRIPT1.3">
/*
 * 2005/10/18 (EST)
 *		By Gwang-Ho Kim (iccentric@gmail.com)
 *
 * 페이지에서 사용되는, 데이터베이스로부터 얻는 데이터를 설정한다.
 */

	// SHOW_ID가 showId인 작업에 대한, gaWork배열에서의 첨자를 얻는다.
	// 해당하는 작업이 없다면 -1을 리턴한다.
	function fniWorkByShowId(showId) {
		for (var i = 0; i < gaWork.length; ++i) {
			if (gaWork[i].showId == showId)
				return i;
		}
		return -1;
	}
	function fnSetData() {
if (1) {
		var	JavaScriptEvalCode = "<?PHP echo $JavaScriptEvalCode ?>";
//		alert(JavaScriptEvalCode);
		eval(JavaScriptEvalCode);
} else {
		gUser = [
			new oUser(1, "갑"),
			new oUser(2, "을"),
			new oUser(3, "병"),
			new oUser(4, "정")
		];

		gaShow = [
			new oShow(
				1,
				"장면 #1",
				[
					new oScene(1, "장면 #1"),
					new oScene(2, "장면 #2: 인물(자)"),
					new oScene(3, "장면 #3: 인물(자), 배경(수성)"),
					new oScene(4, "장면 #4: 인물(축)"),
					new oScene(5, "장면 #5: 배경(지구)")
				],
				[
					new oChrBg(1, "자", [2, 3]),
					new oChrBg(2, "축", [4]),
					new oChrBg(3, "인", [0])
				],
				[
					new oChrBg(4, "수성", [3]),
					new oChrBg(5, "금성", [0]),
					new oChrBg(6, "지구", [5])
				]
			)
		];

		gaWork = [
			new _oWork(
				1,	// SHOW_ID = 1인 작품에 설정된
				[	// 작업들에 대한 배열
					// 2개의 디자인 작업
					new oDesign(
						1,
						0,
						[1, 2, 3],
						new Date('2005/6/12'),
						new Date('2005/12/10'),
						"지시사항: SHOW_ID = 1, 디자인 작업 = #1: ...",
						1,
						cDESIGN_CONCEPT,
						cTARGET_CHR,
						1,
						0		// 첫 번째 행에 표시된다.
					),
					new oDesign(
						2,
						0,
						[2, 3],
						new Date('2005/10/7'),
						new Date('2005/11/30'),
						"지시사항: SHOW_ID = 1, 디자인 작업 = #2: ...",
						1,
						cDESIGN_DETAIL,
						cTARGET_BG,
						4,
						1		// 두 번째 행에 표시된다.
					),
					// 1개의 스토리보드 작업
					new oStoryboard(
						3,
						1,
						[1, 3],
						new Date('2005/9/7'),
						new Date('2005/12/31'),
						"지시사항: SHOW_ID = 1, 스토리보드 작업 = #2: ...",
						1,
						2		// 세 번째 행에 표시된다.
					)
				]
			)
		];
}

		for (var i = gaShow.length - 1; i >= 0; --i) {
			// 장면, 인물/배경 어느 것도 없는 작품은 작업을 진행할 수 없다.
			var	o = gaShow[i];
			if (o.aScn.length == 0 && o.aChr.length == 0 && o.aBg.length == 0)
				gaShow.splice(i, 1);
		}
		if (gaShow.length == 0) {
			alert('작품이 하나도 없으니 작업에 관한 어떤 일도 진행할 수 없다');
			return;
		}
	}
</SCRIPT>
<SCRIPT LANGUAGE="JAVASCRIPT1.3">
/*
 * 2005/10/18 (EST)
 *		By Gwang-Ho Kim (iccentric@gmail.com)
 *
 * RADIO, CHKECKBOX버튼에 대한 onClick이벤트 핸들러들이다.
 */

	function fnSetRadioChecked() {
		// 대부분은 장면을 선택할 수 있다.
		sltScn.disabled = false;

		if (gWrkEdtFlag & cWEFLAG_STRYBRD) {
			// 디자인 창을 닫는다.
			olDesign.style.display = "none";

			idWorkTypeDesign.checked = false;
			idWorkTypeStoryboard.checked = true;
	
			return;
		}
		// 디자인 창을 연다.
		olDesign.style.display = "block";

		idWorkTypeDesign.checked = true;
		idWorkTypeStoryboard.checked = false;

		// 1. Design Type
		if (gWrkEdtFlag & cWEFLAG_CONCPT) {
			idDesignTypeConcept.checked = true;
			idDesignTypeDetail.checked = false;
		} else {
			idDesignTypeConcept.checked = false;
			idDesignTypeDetail.checked = true;
		}
		// 2. Design Target
		if (gWrkEdtFlag & cWEFLAG_BG) {
			sltChr.disabled = true;
			sltBg.disabled = false;
			idTargetTypeChr.checked = false;
			idTargetTypeBg.checked = true;
		} else {
			sltChr.disabled = false;
			sltBg.disabled = true;
			idTargetTypeChr.checked = true;
			idTargetTypeBg.checked = false;
		}
	}

	function fnWorkTypeDesignOnClick() {
		gWrkEdtFlag &= ~cWEFLAG_STRYBRD;
		// 디자인 대상에 따라 장면을 다르게 표시해야 한다.
//		fnSetRadioChecked();
		fnTargetOnClick(gWrkEdtFlag & cWEFLAG_BG ? 'Bg' : 'Chr');
	}
	function fnDesignConceptOnClick() {
		gWrkEdtFlag |= cWEFLAG_CONCPT;
		fnSetRadioChecked();
	}
	function fnDesignDetailOnClick() {
		gWrkEdtFlag &= ~cWEFLAG_CONCPT;
		fnSetRadioChecked();
	}
	function fnTargetOnClick(ChrBg) {
		if (ChrBg == "Chr")
			gWrkEdtFlag &= ~cWEFLAG_BG;
		else
			gWrkEdtFlag |= cWEFLAG_BG;
		fnSetRadioChecked();
		fnSelectOnChange(ChrBg);
	}
	function fnWorkTypeStoryboardOnClick() {
		if (gShow.aScn.length == 0) {
			alert('장면이 하나도 없다');
			idWorkTypeDesign.checked = true;
			idWorkTypeStoryboard.checked = false;
			return false;
		}

		gWrkEdtFlag |= cWEFLAG_STRYBRD;
		fnSetRadioChecked();
		fnAddOptionsToScnSelect(gShow.aScn);
	}
</SCRIPT>
<SCRIPT LANGUAGE="JAVASCRIPT1.3">
/*
 * 2005/10/18 (EST)
 *		By Gwang-Ho Kim (iccentric@gmail.com)
 *
 * 변수들을 정리하고 주석을 단다.
 *
 * 1.0.0에서 시작한다.
 */
</SCRIPT>
<SCRIPT LANGUAGE="JAVASCRIPT1.3">
	var	cWORK_WIDTH	= 30;	// %, (작업기간, 장면, 작업자)가 나머지 (100 - cWORK_WIDTH)%를 사용한다.

	function oColumn(chkId, id, name, ratio) {
		this.chkId = chkId;	// 해당 COLUMN이 보여줄 지를 결정한 CHKECKBOX의 ID이다.
		this.id = id;		// 해당 COLUMN을 구별할 때 사용한다.
					// 다른 정보를 이용해서도 COLUMN을 구별 할 수도 있다. 굳이...
		this.name = name;	// 해당 COLUMN의 이름이다.
		this.ratio = ratio;	// 해당 COLUMN이 다른 COLUMN에 비해서 상대적으로 얼마나 큰 지를 말해준다.
					// 아래에서 기본값은 3:2:5이다.
	}

	var	gaColumn = [
		new oColumn('chkWorker', 'COLUMN_ID__WORKER', '작업자', 3),
		new oColumn('chkScene', 'COLUMN_ID__SCENE', '장면', 2),
		new oColumn('chkWorkPeriod', 'COLUMN_ID__WORK_PERIOD', '작업기간(월별)', 5)
	];
</SCRIPT>
<SCRIPT LANGUAGE="JAVASCRIPT1.3">
/*
 * 2005/10/18 (EST)
 *		By Gwang-Ho Kim (iccentric@gmail.com)
 *
 * 페이지가 변할 때 저장하는 변수들이다.
 *
 */

	var	gCurDate;	// 현재 달[月]에 대한 정보이다.

	var	gchkFlag;	// "작업자", "장면", 작업기간(월별)"중에서 어느 COLUMN을 보여줄지를 결정한다.
	// 다음 세 값의 OR결합이다.
	// 작업일정표에서 세 CHKECKBOX버튼이 해당 플래그를 설정/해제할 수 있다.
	var	cCHKFLAG_WORKER		= 1 << 0;	// 작업자
	var	cCHKFLAG_SCENE		= 1 << 1;	// 장면
	var	cCHKFLAG_WORK_PERIOD	= 1 << 2;	// 작업기간(월별)
	function fnCheckboxOnClick(o, cCHKFLAG_XXX) {
		gchkFlag ^= cCHKFLAG_XXX;
/*
		// gchkFlag를 직접 수정하지 않는다.
		// 왜냐하면 "해당 열(들)"을 표시할 때 gaColumn[].chkId를 통하기 때문이다.
		for (var iCol = 0; iCol < gaColumn.length; ++iCol) {
			var	oCol = gaColumn[iCol];
			var	chkId = oCol.chkId;
			var	oCheckbox = eval('document.getElementById("' + chkId + '")');
			if (oCheckbox.checked) {
				oCheckbox.checked = true;
				fnSetChkFlag(chkId);
			} else {
				oCheckbox.checked = false;
				fnClearChkFlag(chkId);
			}
		}
 */
		if (!fnMakeWorkScheduleTable(gCurDate)) {
			alert('fnMakeWorkScheduleTable(): Failure');
		}
	}
	// 다음 세 함수는 gchkFlag를 다루는(설정, 해제, 확인) 함수들이다.
	function fnSetChkFlag(chkId) {
		if (chkId == 'chkWorker')
			gchkFlag |= cCHKFLAG_WORKER;
		else if (chkId == "chkScene")
			gchkFlag |= cCHKFLAG_SCENE;
		else if (chkId == "chkWorkPeriod")
			gchkFlag |= cCHKFLAG_WORK_PERIOD;
		else {
			// 다른 경우는 무시한다.
			alert('Unknown chkId = ' + chkId);
		}
	}
	function fnClearChkFlag(chkId) {
		if (chkId == 'chkWorker')
			gchkFlag &= ~cCHKFLAG_WORKER;
		else if (chkId == "chkScene")
			gchkFlag &= ~cCHKFLAG_SCENE;
		else if (chkId == "chkWorkPeriod")
			gchkFlag &= ~cCHKFLAG_WORK_PERIOD;
		else {
			// 다른 경우는 무시한다.
			alert('Unknown chkId = ' + chkId);
		}
	}
	function fnIsSetChkFlag(chkId) {
		if (chkId == 'chkWorker')
			return gchkFlag & cCHKFLAG_WORKER;
		else if (chkId == "chkScene")
			return gchkFlag & cCHKFLAG_SCENE;
		else if (chkId == "chkWorkPeriod")
			return gchkFlag & cCHKFLAG_WORK_PERIOD;
		else {
			alert('Unknown chkId = ' + chkId);
		}
		return false;
	}

	var	giWork;		// 작업일정표에서 RADIO버튼이 몇 번째 행에서 활성화되었는 지를 말해준다.
</SCRIPT>
<SCRIPT LANGUAGE="JAVASCRIPT1.3">
	// year: 1-based
	// month: 1-based
	// "year"년 "month"월은 몇 일까지 있는 가?
	function fnGetDaysOfMonthInYear(givenDate) {
		var	days = new Array(31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);

		var	year = givenDate.getYear();	// 1-based
		var	month = givenDate.getMonth();	// 0-based

		if (month != 1)
			return days[month];
		// month = 1 (2월)
		// 윤년인지 판단한다.
		return (year % 4 == 0 && year % 100 != 0 || year % 400 == 0) ? 29 : 28;
	}

	function fnSetPartitionArray(partitionArray, nTotal, nSegment, color) {
		partitionArray.push(new oPartition(nSegment, color, nSegment / nTotal * 100));
	}

	// 작업기간에 따라 주어진 달[月]을 몇 부분으로 나눈다.
	// 적어도 하나, 많아야 셋이다.
	//	경우 1: |        |	주어진 달[月]에 작업이 없을 경우
	//	경우 2: |---     |      주어진 달의 첫 날[日]부터 작업이 있는 경우
	//	경우 3: |     ---|	주어진 달의 첫 날이 아닌 날에 작업을 시작한 경우
	//	경우 4: | ---    |	주어진 달의 첫 날이 아닌 날에 시작해서 끝 날이 아닌 날에 마친 경우
	// 리턴값
	//	1: 작업기간이 givenDate에 걸쳐있다.
	//	0: 작업기간이 givenDate에 걸쳐있지 않다.
	function fnSetPartition(givenDate, beginDate, endDate, partitionArray) {
		// 표시가능한 달인지 알아본다.
		// 1. 표시하는 달의 제일 마지막 날이 시작날보다 작으면 안된다.
		var	lastDate = new Date(givenDate);		// 주어진 날짜(givenDate)의 제일 마지막 날[日]
		var	daysOfMonth = fnGetDaysOfMonthInYear(givenDate);
		lastDate.setDate(daysOfMonth);
		if (lastDate.valueOf() < beginDate.valueOf()) {
//			alert('표시하는 달의 제일 마지막 날[日]이 시작하는 날[日]보다 작으면 안된다.');
			fnSetPartitionArray(partitionArray, daysOfMonth, daysOfMonth, '#ffffff');
			return 0;
		}
		// 2. 표시하는 달의 첫째 날이 끝나는 날보다 크면 안된다.
		var	firstDate = new Date(givenDate);	// 주어진 날짜(givenDate)의 첫째 날[日]
		firstDate.setDate(1);
		if (endDate.valueOf() < firstDate.valueOf()) {
//			alert('표시하는 달의 첫째 날[日]이 끝나는 날[日]보다 크면 안된다.');
			fnSetPartitionArray(partitionArray, daysOfMonth, daysOfMonth, '#ffffff');
			return 0;
		}

		// 이번 달[月]에 표시되는 시작일이 얼마인지 알아본다.
		//	시작하는 달이 이번 달이라면 표시되는 일은 시작일이 된다.
		//	그렇지 않다면 1일부터 표시된다.
		var	d1;
		if (beginDate.getYear() == givenDate.getYear() && beginDate.getMonth() == givenDate.getMonth()) {
			d1 = beginDate.getDate();
			if (d1 != 1) {
				fnSetPartitionArray(partitionArray, daysOfMonth, d1 - 1, '#ffffff');
			}
		} else {
			d1 = 1;
		}
		// 이번 달[月]에 표시되는 마지막일이 얼마인지 알아본다.
		//	끝나는 달이 이번 달이라면 표시되는 일은 끝나는 일이 된다.
		//	그렇지 않다면 이번 달의 제일 마지막 날(daysOfMonth)이된다.
		var	d2;
		if (givenDate.getYear() == endDate.getYear() && givenDate.getMonth() == endDate.getMonth()) {
			d2 = endDate.getDate();
		} else {
			d2 = daysOfMonth;
		}
		fnSetPartitionArray(partitionArray, daysOfMonth, 1 + d2 - d1, '#0066cc');
		if (d2 != daysOfMonth) {
			fnSetPartitionArray(partitionArray, daysOfMonth, daysOfMonth - d2, '#ffffff');
		}

		return 1;
	}
</SCRIPT>
</HEAD>
<BODY>
<INPUT TYPE="BUTTON" VALUE="반영하기" onClick="fnSubmit();">
<UL>
<LI><P STYLE="text-decoration: underline;">작업 일정표&nbsp;&nbsp;&nbsp;&nbsp;작품: <SELECT ID="sltShow" SIZE="1" STYLE="width: 400px;" onChange="fnShowSelectOnChange();"></SELECT></P></LI>
<P>
<DIV ALIGN="CENTER">
<INPUT ID="chkWorker" TYPE="CHECKBOX" CHECKED onClick="fnCheckboxOnClick(this, cCHKFLAG_WORKER);">작업자
<INPUT ID="chkScene" TYPE="CHECKBOX" CHECKED onClick="fnCheckboxOnClick(this, cCHKFLAG_SCENE);">장면
<INPUT ID="chkWorkPeriod" TYPE="CHECKBOX" CHECKED onClick="fnCheckboxOnClick(this, cCHKFLAG_WORK_PERIOD);">작업기간(월별)
<TABLE ID="tblWorkSchedule" WIDTH="85%" BORDER="1" CELLSPACING="0" CELLPADDING="0"><TBODY>
</TBODY></TABLE>
<TABLE WIDTH="85%" BORDER="0"><TBODY>
<TR><TD WIDTH="20px" STYLE="background: <?PHP echo BGCOLOR_INSERT ?>;">&nbsp;</TD><TD WIDTH="*">새로 만든 작업</TD></TR>
<TR><TD WIDTH="20px" STYLE="background: <?PHP echo BGCOLOR_SELECT ?>;">&nbsp;</TD><TD WIDTH="*">DB에서 가져온 작업</TD></TR>
<TR><TD WIDTH="20px" STYLE="background: <?PHP echo BGCOLOR_UPDATE ?>;">&nbsp;</TD><TD WIDTH="*">DB에서 가져와 수정한 작업</TD></TR>
</TBODY></TABLE>
</DIV>
</P>

<LI ID="idWorkManager" STYLE="display: none;"><P STYLE="text-decoration: underline;">작업 관리자</P></LI>
<P>
<DIV ALIGN="CENTER">
<TABLE ID="tblWorkScheduleEditor" WIDTH="85%" BORDER="1" CELLSPACING="0" CELLPADDING="5"><TBODY>
<TR VALIGN="TOP">
	<TD ALIGN="RIGHT" WIDTH="15%">작업:</TD>
	<TD WIDTH="85%">
	<TABLE BORDER="0" CELLSPACING="0" CELLPADDING="0"><TBODY>
	<TR><TD>&nbsp;<INPUT ID="idWorkTypeDesign" TYPE="RADIO" NAME="rdoType" onClick="fnWorkTypeDesignOnClick();">&nbsp;&nbsp;디자인</TD></TR>
	<TR ID="olDesign"><TD><TABLE BORDER="0" CELLSPACING="0" CELLPADDING="0"><TBODY>
		<TR><TD>&nbsp;&nbsp;&nbsp;&nbsp;1. 유형</TD></TR>
		<TR><TD>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<INPUT ID="idDesignTypeConcept" TYPE="RADIO" NAME="rdoDesignType" onClick="fnDesignConceptOnClick();">&nbsp;&nbsp;컨셉 디자인</TD></TR>
	<TR><TD>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<INPUT ID="idDesignTypeDetail" TYPE="RADIO" NAME="rdoDesignType" onClick="fnDesignDetailOnClick();">&nbsp;&nbsp;디자인</TD></TR>
		<TR><TD>&nbsp;&nbsp;&nbsp;&nbsp;2. 대상 (인물/배경에 따라 등장하는 장면이 다를 수 있습니다.)</TD></TR>
		<TR><TD>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<INPUT ID="idTargetTypeChr" TYPE="RADIO" NAME="rdoDesignTarget" onClick="fnTargetOnClick('Chr');">&nbsp;&nbsp;인물&nbsp;<SELECT ID="sltChr" SIZE="1" STYLE="width: 400px;" onChange="fnSelectOnChange('Chr');"></SELECT></TD></TR>
		<TR><TD>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<INPUT ID="idTargetTypeBg" TYPE="RADIO" NAME="rdoDesignTarget" onClick="fnTargetOnClick('Bg');">&nbsp;&nbsp;배경&nbsp;<SELECT ID="sltBg" SIZE="1" STYLE="width: 400px;" onChange="fnSelectOnChange('Bg');"></SELECT></TD></TR>
	</TBODY></TABLE></TD></TR>
	<TR><TD>&nbsp;<INPUT ID="idWorkTypeStoryboard" TYPE="RADIO" NAME="rdoType" onClick="fnWorkTypeStoryboardOnClick();">&nbsp;&nbsp;스토리보드</TD></TR>
	</TBOEY></TABLE>
	</TD>
</TR>
<TR VALIGN="TOP">
	<TD ALIGN="RIGHT">작업자:</TD>
	<TD><SELECT ID="sltWorker" SIZE="4" MULTIPLE STYLE="width: 400px;"></SELECT>&nbsp;&nbsp;디자인 작업에서 여러 명의 작업자를 선택하려면 CTRL키를 누른 후 선택하세요. 스토리보드 작업은 오직 한 명의 작업자만이 진행할 수 있습니다.</TD>
</TR>
<TR>
	<TD ALIGN="RIGHT">장면:</TD>
	<TD><SELECT ID="sltScn" SIZE="4" STYLE="width: 400px;"></SELECT></TD>
</TR>
<TR>
	<TD ALIGN="RIGHT">작업기간:</TD>
	<TD><INPUT ID="idWorkBegin" TYPE="TEXT" VALUE="2005-10-7">&nbsp;-&nbsp;<INPUT ID="idWorkEnd" TYPE="TEXT" VALUE="2005-12-10">&nbsp;&nbsp;&nbsp;&nbsp;예)2005-10-7&nbsp;-&nbsp2005-12-10</TD>
</TR>
<TR>
	<TD ALIGN="RIGHT">지시사항:</TD>
	<TD><TEXTAREA ID="idOrder" ROWS="5" COLS="72"></TEXTAREA></TD>
</TR>
<TR>
	<TD ALIGN="CENTER" COLSPAN="2"><INPUT TYPE="BUTTON" VALUE="작업 더하기" onClick="fnInsertWork();">&nbsp;&nbsp;<INPUT TYPE="BUTTON" VALUE="작업 바꾸기" onClick="fnUpdateWork();"></TD>
</TR>
</TBODY></TABLE>
</DIV>
</P>
</OL>
</BODY>
</HTML>
<?PHP
	}
	// Closing connection
	mysql_close($link);
?>
