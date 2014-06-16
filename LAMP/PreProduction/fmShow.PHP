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
 * 2005/11/18 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * 작품을 업데이트할 때, 바뀐 값들만 복사하고 원래 값은 그대로 두어야 한다.
 * (fnUpdateShow함수를 보라.)
 *
 * 상황에 따라 "고치기"버튼과 "반영하기"버튼을 활성화/비활성화한다.
 * "고치기"버튼은 적어도 하나의 작품이 작품목록에 있을 때 활성화된다.
 * 하나도 없을 때에는 비왈성화된다.
 * "반영하기"버튼은 새로 추가된 작품이 있거나, DB에서 가져온 작업을 삭제하거나 고쳤을 때 활성화 된다.
 * 이렇지 않는 경우는 비활성화된다.
 */
?>
<?PHP
/*
 * 2005/11/15 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * fmWorkSchedule.PHP처럼 작품들도 일정표를 통해서 관리하도록 한다.
 * 따라서 일정표를 관리하는 데에 필요한 많은 부분은 fmWorkSchedule.PHP에서 가져왔다.
 */

// 2. 작품 목록을 표시한다.
?>
<?PHP
/*
 * 2005/11/14 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 * 2005/11/15 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * fmWorkSchedule.PHP처럼 작품들도 일정표를 통해서 관리하도록 한다.
 * 따라서 일정표를 관리하는 데에 필요한 많은 부분은 fmWorkSchedule.PHP에서 가져왔다.
 */

// 1. 데이터를 가져오거나 보낼 때, gShow(= new objShow(...))단위로 이루어지지 않고
// gaShow(= new Array(new objShow(...), ...))단위로 이루어지도록 한다.
	define("BGCOLOR_INSERT", "blue");	// 새로 추가한 작품
	define("BGCOLOR_SELECT", "green");	// DB에서 가져온 작품
	define("BGCOLOR_UPDATE", "red");	// DB에서 가져와 수정한 작품
?>
<?PHP
/*
 * 2005/11/5 (EST)
 *		By 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * oChgBg에서 참조하는 장면에 대한 정보인 aScnSerial에서
 * 어떤 변화가 있었는 지를 알아보기 위해 처음에 주어지는
 * 정보를 그대로 보관하지 않고, 해당하는 장면의 id(SCENE_ID)를 보관한다.
 * 왜냐하면 장면을 조작하다보면 처음에 참조했던 장면에 대한 첨자는 바뀌기 때문이다.
 * 그러기 위해서는 처음에 oChgBg를 초기화할 때, 장면의 id를 알고 있어야 한다.
 * 서버로 인물/배경에서 참조하는 장면에 대한 정보를 보낼 때, 보관한 id에 정보와
 * 웹에서 조작한 참조하는 장면에 대한 정보를 동시에 보내
 * 서버에서 이 둘을 비교해서 어떤 장면에 대한 참조가 변화했는 지 알 수 있다.
 * 자세한 사항은 fnSetScenePseudoId함수를 보라.
 */
?>
<?PHP
	define("TARGET_CHR",	1);	// 인물
	define("TARGET_BG",	2);	// 배경

	define("FLAG_DB",	1 << 0);	// 웹에서 다루는 데이터가 DB로부터 왔는 지를 알려준다.
	define("FLAG_DELETED",	1 << 1);	// DB로부터 가져온 데이터가 지워졌는 지를 알려준다.
	define("FLAG_NAME",	1 << 2);	// 이름(NAME)이 바뀌었는 지를 알려준다.
	define("FLAG_DESC",	1 << 3);	// 설명(DESCRIPTION)이 바뀌었는 지를 알려준다.
	define("FLAG_BEGIN",	1 << 4);	// 작품을 시작하는 날짜가 바뀌었는 지를 알려준다.
	define("FLAG_END",	1 << 5);	// 작품을 끝내는 날짜가 바귀었는 지를 알려준다.
	define("FLAG_SERIAL",	1 << 6);	// 장면의 일련번호가 바뀌었는 지를 알려준다.
	define("FLAG_SCN_REF",	1 << 7);	// 인물/배경에서 장면에 대한 참조가 바꾸었는 지를 알려준다.
?>
<?PHP
	class TargetBase {
		var	$id;		// 0: 새로 추가되는 경우, >= 1: 고치는 경우 (장면: SCENE_ID, 인물/배경: TARGET_ID)
		var	$flag;		// 고치는 경우(즉 $id >= 1)에 두 부분($name, $desc)중에서
					// 어느 부분이 바뀌었는 지를 말해준다.
					// 새로 추가하는 경우($id = 0)에는 이 값은 사용되지 않는다.
					// 이 클래스의 오브젝트들을 성분으로 하는 배열을 가지는 Show클래스에서
					// $id값이 0인 경우 즉 작품을 새로 추가하는 경우에,
					// ($id, $flag)는 무시된다.
		var	$name;		// 장면또는 인물/배경의 이름
		var	$desc;		// 설명 (multiline)
	};

	class TargetScene extends TargetBase {
		// 현재까지 따로 추가할 것이 없다.

		function TargetScene($id, $flag, $name, $desc) {
			$this->id = $id;
			$this->flag = $flag;

			$this->name = $name;
			$this->desc = $desc;
		}
	};

	class TargetChrBg extends TargetBase {
		// 장면과 달리 인물/배경의 경우,
		// 인물/배경이 작품 전체에 등장하지 않고 특정 장면(들)에서만 등장할 수 있다.
		// 다음 변수는 등장하는 장면들에 대한 배열이다.
		//	-1: 작품 전체에 등장한다.
		//	>= 0: 등장하는 장면들에 대한 정보로서 Show클래스의 aScn배열에 대한 첨자이다.
		var	$scene;		//

		function TargetChrBg($id, $flag, $name, $desc, $scene) {
			$this->id = $id;
			$this->flag = $flag;

			$this->name = $name;
			$this->desc = $desc;

			// 참조하는 장면의 변화를 말해준다.
			// array(array("전"), array("후"))의 형태이다.
			// array("전"): 인물/배경이 처음에 참조했던 장면의 대한 정보를 가지는 값들로 이루어진 배열이다.
			// 다음과 같은 값들을 가질 수 있다.
			//	처음 만들어진 인물/배경이라면 아무것도 없다.
			//	기존 인물/배경이라면 참조했던 장면의 id즉 TTARGET_SCENE테이블의 SCENE_ID가 된다.
			// array("후"): 인물/배경이 나중에 참조했던 장면의 대한 정보를 가지는 값들로 이루어진 배열이다.
			// 다음과 같은 값들을 가질 수 있다.
			//	>= 0: TTARGET_SCENE테이블의 SCENE_ID
			//	< 0: 새로 만들어진 장면을 참조하고 있다. 절대값에서 1을 뺀 값을 그 장면에 대한 첨자로 사용한다.
			$this->scene = $scene;
		}
	};

	class Show {
		var	$id;		// 0: 새로 추가하는 경우, >= 1: 고치는 경우 (기존 SHOW_ID)
		var	$flag;		// 고치는 경우(즉 $id >= 1)에 네 부분($name, $date_begin, $date_end, $desc) 중에서
					// 어느 부분이 바뀌었는 지를 말해준다.
					// 새로 추가하는 경우($id = 0)에는 이 값은 사용되지 않는다.
					// 또한 아래의 장면, 인물/배경에 대한 오브젝트들의 배열에서
					// TargetBase클래스의 ($id, $flag)도 무시된다.

		var	$name;		// 작품명
		var	$date_begin;	// 작품을 시작는 날짜 (yyyy-mm-dd)
		var	$date_end;	// 작품을 마치는 날짜
		var	$desc;		// 시나리오 (multiline)

		var	$aScn;		// 장면에 대한 정보를 담고 있고, TargetScene오브젝트들의 배열이다.
		var	$aChr;		// 인물에 대한 정보를 담고 있고, TargetChrBg오브젝트들의 배열이다.
		var	$aBg;		// 배경에 대한 정보를 담고 있고, TargetChrBg오브젝트들의 배열이다.

		function Show($id, $flag, $name, $date_begin, $date_end, $desc, $aScn, $aChr, $aBg) {
			$this->id = $id;
			$this->flag = $flag;

			$this->name = $name;
			$this->date_begin = $date_begin;
			$this->date_end = $date_end;
			$this->desc = $desc;

			$this->aScn = $aScn;
			$this->aChr = $aChr;
			$this->aBg = $aBg;
		}
	};
?>
<?PHP
	function displayShow($aShow) {
	foreach ($aShow as $showId => $oShow) {
		echo '//// SHOW_ID: ' . $showId . "\n";
		echo 'id: |' . $oShow->id . '|' . "\n";
		echo 'flag: |' . $oShow->flag . '|' . "\n";
		echo '작품명: |' . $oShow->name . '|' . "\n";
		echo '제작기간: |' . $oShow->date_begin . '| - |' . $oShow->date_end . '|' . "\n";
		echo '대본: |' . $oShow->desc . '|' . "\n";
		$showId = $oShow->id;
		if ($showId == 0) {	// 새로운 작품을 등록하는 경우
			// 새로운 작품을 등록한다.
			$query = sprintf(
				"INSERT INTO TSHOW (NAME, DATE_BEGIN, DATE_END, DESCRIPTION) VALUES ('%s', '%s', '%s', '%s')"
				,
				mysql_real_escape_string($oShow->name),
				mysql_real_escape_string($oShow->date_begin), mysql_real_escape_string($oShow->date_end),
				mysql_real_escape_string($oShow->desc)
			);
			if (!mysql_query($query)) {
				$message = '';
				$message .= 'errno: |' . mysql_errno() . '|' . "\n";
				$message .= 'error: |' . mysql_error() . '|' . "\n";
				$message .= 'query: |' . $query . '|' . "\n";
				die($message);
			}
			// 작품의 SHOW_ID를 얻는다.
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
			$showId = $row[0];
			mysql_free_result($result);

			// 작품에 대한 참고자료 등록에 필요한 REF_ID를 만든다.
			//	2005/11/29 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
			$query = 'INSERT INTO TREFERENCE (CITE_TYPE, CITE_ID) VALUES (' . CITE_SHOW . ', ' . $showId . ')';
			if (!mysql_query($query)) {
				$message = '';
				$message .= 'errno: |' . mysql_errno() . '|' . "\n";
				$message .= 'error: |' . mysql_error() . '|' . "\n";
				$message .= 'query: |' . $query . '|' . "\n";
				die($message);
			}
		} else if ($oShow->flag & FLAG_DELETED) {
			echo '기존 작품(SHOW_ID = ' . $showId . ')을 삭제한다.' . "\n";
			$query = 'DELETE FROM TSHOW WHERE SHOW_ID = ' . $showId;
			echo 'query: |' . $query . '|' . "\n";
			if (!mysql_query($query)) {
				$message = '';
				$message .= 'errno: |' . mysql_errno() . '|' . "\n";
				$message .= 'error: |' . mysql_error() . '|' . "\n";
				$message .= 'query: |' . $query . '|' . "\n";
				die($message);
			}
			continue;
		} else {	// 기존 작품을 수정하는 경우
			$aCol = array();	// 수정이 필요한 열(COLUMN)을 모은다.
			if ($oShow->flag & FLAG_NAME)
				array_push($aCol, 'NAME = \'' . mysql_real_escape_string($oShow->name) . '\'');
			if ($oShow->flag & FLAG_BEGIN)
				array_push($aCol, 'DATE_BEGIN = \'' . mysql_real_escape_string($oShow->date_begin) . '\'');
			if ($oShow->flag & FLAG_END)
				array_push($aCol, 'DATE_END = \'' . mysql_real_escape_string($oShow->date_end) . '\'');
			if ($oShow->flag & FLAG_DESC)
				array_push($aCol, 'DESCRIPTION = \'' . mysql_real_escape_string($oShow->desc) . '\'');
			if (count($aCol)) {
				echo '기존 작품(SHOW_ID = ' . $showId . ')을 수정한다.' . "\n";
				$query = 'UPDATE TSHOW SET ' . join(', ', $aCol) . ' WHERE SHOW_ID = ' . $showId;
				echo 'query: |' . $query . '|' . "\n";
				if (!mysql_query($query)) {
					$message = '';
					$message .= 'errno: |' . mysql_errno() . '|' . "\n";
					$message .= 'error: |' . mysql_error() . '|' . "\n";
					$message .= 'query: |' . $query . '|' . "\n";
					die($message);
				}
			} else {
				echo '기존 작품(SHOW_ID = ' . $showId . ')의 전반적인 내용은 변함이 없다.' . "\n";
			}
		}
		echo 'SHOW_ID: ' . $showId . "\n";

		echo '장면:....' . "\n";
		echo '------------------------------------------------------------------------' . "\n";
		$scnIndexToId = array();	// 일련번호가 index인 장면의 Id(SCENE_ID)를 알 수 있다.
		foreach ($oShow->aScn as $i => $oScn) {
			echo '장면 #' . $i . "\n";
			echo "\t" . 'id: |' . $oScn->id . '|' . "\n";
			echo "\t" . 'flag: |' . $oScn->flag . '|' . "\n";
			echo "\t" . '이름: |' . $oScn->name . '|' . "\n";
			echo "\t" . '설명: |' . $oScn->desc . '|' . "\n";
			if ($oScn->id == 0) {	// 새로운 장면을 등록하는 경우
				// 새로운 장면을 등록한다.
				$query = sprintf(
					"INSERT INTO TSCENE (SHOW_ID, SERIAL, NAME, DESCRIPTION) VALUES (%d, %d, '%s', '%s')"
					,
					$showId, $i,
					mysql_real_escape_string($oScn->name),
					mysql_real_escape_string($oScn->desc)
				);
				if (!mysql_query($query)) {
					$message = '';
					$message .= 'errno: |' . mysql_errno() . '|' . "\n";
					$message .= 'error: |' . mysql_error() . '|' . "\n";
					$message .= 'query: |' . $query . '|' . "\n";
					die($message);
				}
				// 장면의 SCENE_ID를 얻는다.
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
				$scnIndexToId[$i] = $row[0];
				mysql_free_result($result);
				echo '새로운 장면(SCEND_ID = ' . $scnIndexToId[$i] . ')을 추가했다.' . "\n";

				// 장면에 대한 참고자료 등록에 필요한 REF_ID를 만든다.
				//	2005/11/29 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
				$query = 'INSERT INTO TREFERENCE (CITE_TYPE, CITE_ID) VALUES (' . CITE_SCENE . ', ' . $scnIndexToId[$i] . ')';
				if (!mysql_query($query)) {
					$message = '';
					$message .= 'errno: |' . mysql_errno() . '|' . "\n";
					$message .= 'error: |' . mysql_error() . '|' . "\n";
					$message .= 'query: |' . $query . '|' . "\n";
					die($message);
				}
			} else if ($oScn->flag & FLAG_DELETED) {	// 기존 장면을 삭제하는 경우
				echo '기존 장면(SCENE_ID = ' . $oScn->id . ')을 삭제한다.' . "\n";
				$query = 'DELETE FROM TSCENE WHERE SCENE_ID = ' . $oScn->id;
				echo 'query: |' . $query . '|' . "\n";
				if (!mysql_query($query)) {
					$message = '';
					$message .= 'errno: |' . mysql_errno() . '|' . "\n";
					$message .= 'error: |' . mysql_error() . '|' . "\n";
					$message .= 'query: |' . $query . '|' . "\n";
					die($message);
				}
			} else {	// 기존 장면을 수정하는 경우
				$scnIndexToId[$i] = $oScn->id;
				$aCol = array();	// 수정이 필요한 열(COLUMN)을 모은다.
				if ($oScn->flag & FLAG_NAME)
					array_push($aCol, 'NAME = \'' . mysql_real_escape_string($oScn->name) . '\'');
				if ($oScn->flag & FLAG_DESC)
					array_push($aCol, 'DESCRIPTION = \'' . mysql_real_escape_string($oScn->desc) . '\'');
				if ($oScn->flag & FLAG_SERIAL)
					array_push($aCol, 'SERIAL = ' . $i);
				if (count($aCol)) {
					echo '기존 장면(SCENE_ID = ' . $oScn->id . ')을 수정한다.' . "\n";
					$query = 'UPDATE TSCENE SET ' . join(', ', $aCol) . ' WHERE SCENE_ID = ' . $oScn->id;
					echo 'query: |' . $query . '|' . "\n";
					if (!mysql_query($query)) {
						$message = '';
						$message .= 'errno: |' . mysql_errno() . '|' . "\n";
						$message .= 'error: |' . mysql_error() . '|' . "\n";
						$message .= 'query: |' . $query . '|' . "\n";
						die($message);
					}
				} else {
					echo '기존 장면(SCENE_ID = ' . $oScn->id . ')의 전반적인 내용은 변함이 없다.' . "\n";
				}
			}
		}
		print_r($scnIndexToId);
		$aChrBg = array('Chr' => '인물', 'Bg' => '배경');
		foreach ($aChrBg as $ChrBg => $Target) {
			echo '------------------------------------------------------------------------' . "\n";
			echo $Target . ':....' . "\n";
			echo '------------------------------------------------------------------------' . "\n";
			eval('$aoChrBg = $oShow->a' . $ChrBg . ';');
			foreach ($aoChrBg as $i => $oChrBg) {
				echo $Target . ' #' . $i . "\n";
				echo "\t" . 'id: |' . $oChrBg->id . '|' . "\n";
				echo "\t" . 'flag: |' . $oChrBg->flag . '|' . "\n";
				$targetId = $oChrBg->id;

				// 인물/배경의 전반적인 내용(이름, 설명)과 관련된 처리를 한다.
				echo "\t" . '이름: |' . $oChrBg->name . '|' . "\n";
				echo "\t" . '설명: |' . $oChrBg->desc . '|' . "\n";
				if ($targetId == 0) {	// 새로운 인물/배경을 등록하는 경우
					// 새로운 인물/배경을 등록한다.
					$query = sprintf(
						"INSERT INTO TTARGET (SHOW_ID, TYPE, NAME, DESCRIPTION) VALUES (%d, %d, '%s', '%s')"
						,
						$showId,
						$Target == "인물" ? TARGET_CHR : TARGET_BG,
						mysql_real_escape_string($oChrBg->name),
						mysql_real_escape_string($oChrBg->desc)
					);
					if (!mysql_query($query)) {
						$message = '';
						$message .= 'errno: |' . mysql_errno() . '|' . "\n";
						$message .= 'error: |' . mysql_error() . '|' . "\n";
						$message .= 'query: |' . $query . '|' . "\n";
						die($message);
					}

					// 아래에서 인물/배경이 참조하는 장면을 저장한다.
					// 그 때 필요한 인물/배경의 TARGET_ID를 얻는다.
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
					$targetId = $row[0];
					mysql_free_result($result);

					// 대상(인물/배경)에 대한 참고자료 등록에 필요한 REF_ID를 만든다.
					//	2005/11/29 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
					$query = 'INSERT INTO TREFERENCE (CITE_TYPE, CITE_ID) VALUES (' . CITE_TARGET . ', ' . $targetId . ')';
					if (!mysql_query($query)) {
						$message = '';
						$message .= 'errno: |' . mysql_errno() . '|' . "\n";
						$message .= 'error: |' . mysql_error() . '|' . "\n";
						$message .= 'query: |' . $query . '|' . "\n";
						die($message);
					}
				} else if ($oChrBg->flag & FLAG_DELETED) {	// 기존 인물/배경을 삭제하는 경우
					echo '기존 ' . $Target . '(TARGET_ID = ' . $oChrBg->id . ')을 삭제한다.' . "\n";
					$query = 'DELETE FROM TTARGET WHERE TARGET_ID = ' . $targetId;
					echo 'query: |' . $query . '|' . "\n";
					if (!mysql_query($query)) {
						$message = '';
						$message .= 'errno: |' . mysql_errno() . '|' . "\n";
						$message .= 'error: |' . mysql_error() . '|' . "\n";
						$message .= 'query: |' . $query . '|' . "\n";
						die($message);
					}
					// 삭제하는 경우에는 아래의 인물/배경이 참조하는 장면과 관련된 처리에는 관여하지 않는다.
					continue;
				} else {	// 기존 인물/배경을 수정하는 경우
					$aCol = array();	// 수정이 필요한 열(COLUMN)을 모은다.
					if ($oChrBg->flag & FLAG_NAME)
						array_push($aCol, 'NAME = \'' . mysql_real_escape_string($oChrBg->name) . '\'');
					if ($oChrBg->flag & FLAG_DESC)
						array_push($aCol, 'DESCRIPTION = \'' . mysql_real_escape_string($oChrBg->desc) . '\'');
					if (count($aCol)) {
						echo '기존 ' . $Target . '(TARGET_ID = ' . $targetId . ')을 수정한다.' . "\n";
						$query = 'UPDATE TTARGET SET ' . join(', ', $aCol) . ' WHERE TARGET_ID = ' . $targetId;
						echo 'query: |' . $query . '|' . "\n";
						if (!mysql_query($query)) {
							$message = '';
							$message .= 'errno: |' . mysql_errno() . '|' . "\n";
							$message .= 'error: |' . mysql_error() . '|' . "\n";
							$message .= 'query: |' . $query . '|' . "\n";
							die($message);
						}
					} else {
						echo '기존 ' . $Target . '(TARGET_ID = ' . $targetId . ')의 전박적인 내용은 변함이 없다.' . "\n";
					}
				}

				// 인물/배경이 참조하는 장면과 관련된 처리를 한다.
				print_r($oChrBg->scene);
				echo "\t" . '등장 장면: ';
				$a = array();
				if (count($oChrBg->scene[0])) {
					if ($oChrBg->scene[0][0] == -1)
						$a[0] = 'SCENE_ID = 0';
					else {
						foreach ($oChrBg->scene[0] as $scnId)
							array_push($a, 'SCENE_ID = ' . $scnId);
					}
				} else {
					$a[0] = '無';
				}
				echo '(|' . join('|, |', $a) . '|)';
				echo ' -> ';
				$a = array();
				if (count($oChrBg->scene[0])) {
					if ($oChrBg->scene[1][0] == -1)
						$a[0] = 'SCENE_ID = 0';
					else {
						foreach ($oChrBg->scene[1] as $scnId)
							array_push($a, 'SCENE_ID = ' . $scnId);
					}
				} else {
					$a[0] = '無';
				}
				echo '(|' . join('|, |', $a) . '|)';
				echo "\n";
				// $aDeleted는 더 이상 참조하지 않은 장면의 id들로 이루어진 배열이다.
				$aDeleted = array_diff($oChrBg->scene[0], $oChrBg->scene[1]);
				print_r($aDeleted);
				if (count($aDeleted)) {
					// 더 이상 참고하지 않는 장면에 대한 참조를 지운다.
					$query = 'DELETE FROM TTARGET_SCENE WHERE ' . '(TARGET_ID = ' . $targetId . ' AND SCENE_ID = ' . join(') OR (TARGET_ID = ' . $targetId . ' AND SCENE_ID = ', $aDeleted) . ')';
					if (!mysql_query($query)) {
						$message = '';
						$message .= 'errno: |' . mysql_errno() . '|' . "\n";
						$message .= 'error: |' . mysql_error() . '|' . "\n";
						$message .= 'query: |' . $query . '|' . "\n";
						die($message);
					}
				}
				// $aAdded배열은 새로 참조하게 되는 장면에 대한 정보를 가지고 있다.
				//	1) 0보다 큰 경우: 기존 장면에 대한 id
				//	2) 0보다 작은 경우: 절대값을 취해서 1을 뺀 값이 새롭게 추가되는 장면을 가리키는 첨자: $scnIndexToId를 이용해서 참조하는 장면에 대한 id를 얻는다.
				//	3) 0인 경우: SCENE_ID = 0: 작품 전체에서 사용된다.
				$aAdded = array_diff($oChrBg->scene[1], $oChrBg->scene[0]);
				print_r($aAdded);
					foreach ($aAdded as $i => $scnId) {
						if ($scnId < 0)
							$aAdded[$i] = $scnIndexToId[(0 - $scnId) - 1];
					}
				print_r($aAdded);
				if (count($aAdded)) {
					// 참고하는 장면에 대한 참조를 등록한다.
					$query = 'INSERT INTO TTARGET_SCENE (TARGET_ID, SCENE_ID) VALUES ' . '(' . $targetId . ', ' . join('), (' . $targetId . ', ', $aAdded) . ')';
					if (!mysql_query($query)) {
						$message = '';
						$message .= 'errno: |' . mysql_errno() . '|' . "\n";
						$message .= 'error: |' . mysql_error() . '|' . "\n";
						$message .= 'query: |' . $query . '|' . "\n";
						die($message);
					}
				}
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
/* 웹페이지에서는 Show클래스에 대한 오브젝트를 만드는 데 필요한 정보를 설정해서 보내주어야 한다.
 *
		$oShow = new Show(
			0,			// 새로 추가한다.
			0,			// 새로 추가하므로 이 값은 사용되지 않는다.
			'작품 #0',
			'2005/10/12', '2005/12/10',
			'시나리오...',

			// 장면
			array(
				new TargetScene(
					0,		// 작품을 새로 추가하므로 이 값은 사용되지 않는다.
					0,		// 마찬가지로 사용되지 않는다.
					'장면 #0',
					'장면 #0: ...'
				),
				......
			),
			// 인물
			array(
				new TargetChrBg(
					0,		// 작품을 새로 추가하므로 이 값은 사용되지 않는다.
					0,		// 마찬가지로 사용되지 않는다.
					'인물 #0',
					'인물 #0: ...',
					array(array(), array(0))
				),
				......
			),
			// 배경
			array(
				new TargetChrBg(
					0,		// 작품을 새로 추가하므로 이 값은 사용되지 않는다.
					0,		// 마찬가지로 사용되지 않는다.
					'배경 #0',
					'배경 #0: ...',
					array(array(), array(0))
				),
				......
			)
		);
 */
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
		// PHP코드를 얻은 후
		$evalCode = $_POST["PHP_EVAL_CODE"];
		echo "PHP_EVAL_CODE = |" . $evalCode . "|\n";	// 웹 클라이언트가 웹 서버로 보낼 때 변환된 PHP코드
		// 변환한다: (\\->\, \"->", \'->')
		$evalCode = preg_replace('/\\\\([\\\"\'])/', '$1', $evalCode);	// PHP코드
//		echo "PHP_EVAL_CODE = |" . $evalCode . "|\n";	// 

		// PHP코드를 해석해서 필요한 변수(예를 들어, objShow: Show클래스에 대한 오브젝트)를 설정한다.
		eval($evalCode);
////		eval(preg_replace('/\\\\([\\\"\'])/', '$1', $_POST["PHP_EVAL_CODE"]));	// 한 줄로 쓰면...

		// 설정된 변수를 이용한다.
/* 연습용 */
/*
		echo "\$a[0] = |" . $a[0] . "|\n";
		echo "\$a[1] = |" . $a[1] . "|\n";
 */
		displayShow($aShow);
?>
</PRE>
<SCRIPT LANGUAGE="JavaScript1.3">
	location.replace("fmShow.PHP");
</SCRIPT>
</BODY>
</HTML>
<?PHP
	} else {
if (1 || $showId > 0) {
		global	$JavaScriptEvalCode;
		$JavaScriptEvalCode = '';	// JavaScript가 해석할 문자열
						// 이 문자열은 gaShow오브젝트에 대한 정보를 담고 있다.
		// 특정 작품이 아닌 모든 작품에 대한 정보를 담은 데이터를 보내기 위해
		// 먼저 작품과 관련된 모든 정보를 Show클래스의 오브젝트들로 이루어진 배열에 담는다.
		//	2005/11/14 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
		$aShow = array();	// 모든 작품에 대한 정보를 담고 있다.
					// 이를 이용해서 JavaScript가 해석할 문자열(JavaScriptEvalCode)를 만든다.
		// 1. 모든 작품에 대한 전반적인 정보(작품명, 제작기간, 시나리오)를 얻는다.
		$query = 'SELECT SHOW_ID, NAME, DATE_BEGIN, DATE_END, DESCRIPTION FROM TSHOW WHERE SHOW_ID > 0';
		if (!($result = mysql_query($query))) {
			$message = '';
			$message .= 'errno: |' . mysql_errno() . '|' . "\n";
			$message .= 'error: |' . mysql_error() . '|' . "\n";
			$message .= 'query: |' . $query . '|' . "\n";
			die($message);
		}
		// 성공적으로 쿼리를 전달했다.
		while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
			$showId = $row["SHOW_ID"];
			$aShow[$showId] = new Show(
				$showId,
				0,		// flag값은 사용되지 않는다.
				$row["NAME"],
				$row["DATE_BEGIN"], $row["DATE_END"],
				$row["DESCRIPTION"],
//				"버그로 인해 시나리오 내용이 보이지 않습니다.\\n하지만 입력하신 시나리오는 DB에 잘 보관되어 있습니다.\\n",
				// 장면에 대한 정보는 아래에서 얻는다.
				array(),
				// 대상(인물/배경)에 대한 정보도 아래에서 얻는다.
				array(), array()
			);
		}
		// 사용한 결과를 반환한다.
		mysql_free_result($result);

		// 2. 모든 장면에 대한 정보를 얻어, Show클래스의 aScn을 채운다.
		$query = 'SELECT SCENE_ID, SHOW_ID, SERIAL, NAME, DESCRIPTION FROM TSCENE WHERE SCENE_ID > 0 ORDER BY SERIAL';
		if (!($result = mysql_query($query))) {
			$message = '';
			$message .= 'errno: |' . mysql_errno() . '|' . "\n";
			$message .= 'error: |' . mysql_error() . '|' . "\n";
			$message .= 'query: |' . $query . '|' . "\n";
			die($message);
		}
		// 성공적으로 쿼리를 전달했다.
		// SCENE_ID가 해당하는 작품의 몇 번째 SERIAL에 할당되었는 지에 대한 정보를 얻는다.
		// 왜냐하면 대상(인물/배경)에서는 참조하는 장면에 대한 정보를 SCENE_ID가 아니라
		// SERIAL롤 저장하기 때문이다.
		$aSceneIdToSerial = array();	// (SCENE_ID => SERIAL)
						// SCENE_ID는 UNIQUE하기 때문에 SHOW_ID는 사용되지 않는다.
		while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
			$sceneId = $row["SCENE_ID"];

			$aSceneIdToSerial[$sceneId] = $row["SERIAL"];

			array_push(
				$aShow[$row["SHOW_ID"]]->aScn,
				new TargetScene(
					$sceneId,
					0,		// flag는 사용되지 않는다.
					$row["NAME"],
					$row["DESCRIPTION"]
				)
			);
		}
		// 사용한 결과를 반환한다.
		mysql_free_result($result);

		// 3. 모든 대상(인물/배경)이 참고하는 장면에 대한 정보를 얻는다.
		$query = 'SELECT TARGET_ID, SCENE_ID FROM TTARGET_SCENE';
		if (!($result = mysql_query($query))) {
			$message = '';
			$message .= 'errno: |' . mysql_errno() . '|' . "\n";
			$message .= 'error: |' . mysql_error() . '|' . "\n";
			$message .= 'query: |' . $query . '|' . "\n";
			die($message);
		}
		// 대상(인물/배경)이 어떤 장면을 참조하는 지를 기억한다.
		$aaTargetIdToaScnId = array();	// (TARGET_ID => (ScnSerial))
						// ScnSeral = aSceneIdToSerial[SCENE_ID]로 얻어진다.
		while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
			$targetId = $row["TARGET_ID"];
			if (!isset($aaTargetIdToaScnId[$targetId]))
				$aaTargetIdToaScnId[$targetId] = array();
			$sceneId = $row["SCENE_ID"];
			array_push($aaTargetIdToaScnId[$targetId], $sceneId == 0 ? -1 : $aSceneIdToSerial[$sceneId]);
		}
		// 사용한 결과를 반환한다.
		mysql_free_result($result);

		// 모든 대상(인물/배경)에 대한 정보를 얻는다.
		$query = 'SELECT TARGET_ID, SHOW_ID, TYPE, NAME, DESCRIPTION FROM TTARGET';
		if (!($result = mysql_query($query))) {
			$message = '';
			$message .= 'errno: |' . mysql_errno() . '|' . "\n";
			$message .= 'error: |' . mysql_error() . '|' . "\n";
			$message .= 'query: |' . $query . '|' . "\n";
			die($message);
		}
		while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
			$targetId = $row["TARGET_ID"];
			$showId = $row["SHOW_ID"];
if ($row["TYPE"] == TARGET_CHR)
			array_push(
				$aShow[$showId]->aChr,
				new TargetChrBg(
					$targetId,
					0,		// flag는 사용되지 않는다.
					$row["NAME"],
					$row["DESCRIPTION"],
					$aaTargetIdToaScnId[$targetId]
				)
			);
else
			array_push(
				$aShow[$showId]->aBg,
				new TargetChrBg(
					$targetId,
					0,		// flag는 사용되지 않는다.
					$row["NAME"],
					$row["DESCRIPTION"],
					$aaTargetIdToaScnId[$targetId]
				)
			);
		}
		// 사용한 결과를 반환한다.
		mysql_free_result($result);
//		print_r($aShow);

		$JavaScriptEvalCode .= 'gaShow = [';
		$a = array();
		foreach ($aShow as $showId => $oShow) {
			// 장면: oShow->aScn배열을 만든다.
			$aScn = array();
			foreach ($oShow->aScn as $iScn => $oScn) {
				array_push($aScn,
					"\n" .
					"\t\t\t" . 'new objScene(' . "\n" .
					"\t\t\t\t" . '"' . $oScn->name . '",' . "\n" .
					// 줄바꿈("\r\n")을 ('\n')로 바꾼다.
					// (\) -> (\\), (") -> (\")
					"\t\t\t\t" . '"' . preg_replace('/\r\n/', '\\\\n', preg_replace('/([\\\"])/', "\\\\$1", $oScn->desc)) . '",' . "\n" .
					"\t\t\t\t" . $iScn . ',' . "\n" .
					"\t\t\t\t" . $oScn->id . ',' . "\n" .
					"\t\t\t\t" . $iScn . "\n" .		// rowIndex
					"\t\t\t" . ')'
				);
			}

			// 대상(인물/배경): oShow->aChr/aBg배열을 만든다.
			$aChr = array();
			$aBg = array();
			foreach (array('Chr', 'Bg') as $ChrBg) {
				eval('$raChrBg = &$a' . $ChrBg . ';');
				eval('$aoChrBg = $oShow->a' . $ChrBg . ';');
				foreach ($aoChrBg as $i => $oChrBg) {
					array_push($raChrBg,
						"\n" .
						"\t\t\t" . 'new objChrBg(' . "\n" .
						"\t\t\t\t" . '"' . $oChrBg->name . '",' . "\n" .
						// (\) -> (\\), (") -> (\")
						// 줄바꿈("\r\n")을 ('\n')로 바꾼다.
						"\t\t\t\t" . '"' . preg_replace('/\r\n/', '\\\\n', preg_replace('/([\\\"])/', "\\\\$1", $oChrBg->desc)) . '",' . "\n" .
						"\t\t\t\t" . '[' . join(', ', $oChrBg->scene) . '],' . "\n" .
						"\t\t\t\t" . $oChrBg->id . ',' . "\n" .
						"\t\t\t\t" . $i . "\n" .
						"\t\t\t" . ')'
					);
				}
			}

			array_push($a,
				"\n" .
				"\t" . 'new objShow(' . "\n" .
				"\t\t" . $showId . ',' . "\n" .
				"\t\t" . '"' . $oShow->name . '"' . ',' . "\n" .
				"\t\t" . 'new Date("' . join('/', split('-', $oShow->date_begin)) . '"),' . "\n" .
				"\t\t" . 'new Date("' . join('/', split('-', $oShow->date_end)) . '"),' . "\n" .
				// (\) -> (\\), (") -> (\")
				// 줄바꿈("\r\n")을 ('\n')로 바꾼다.
				"\t\t" . '"' . preg_replace('/\r\n/', '\\\\n', preg_replace('/([\\\"])/', "\\\\$1", $oShow->desc)) . '",' . "\n" .
				"\n" .
				"\t\t" . '// 장면' . "\n" .
				"\t\t" . '[' . join(',', $aScn) . "\n" .
				"\t\t" . '],' . "\n" .
				"\n" .
				"\t\t" . '// 인물' . "\n" .
				"\t\t" . '[' . join(',', $aChr) . "\n" .
				"\t\t" . '],' . "\n" .
				"\t\t" . '// 배경' . "\n" .
				"\t\t" . '[' . join(',', $aBg) . "\n" .
				"\t\t" . ']' . "\n" .
				"\t" . ')'
			);
		}
		$JavaScriptEvalCode .= join(',', $a) . "\n";
		$JavaScriptEvalCode .= '];' . "\n";

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
<TITLE>fmShow.PHP</TITLE>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; CHARSET=euc-kr">

<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/17 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * "작품관리"에서 "더하기", "고치기"버튼을 눌렀을 때 호출하는 함수를 마무리한다.
 *
 * objShow의 생성에서 입력되는 aScn, aChr, aBg의 배열에 대한 참조가 아니라 값을 사용한다.
 * objChrBg에서 aScnSerial을 이용하는 것처럼...
 */
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/15 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * "작품관리"에서 "더하기", "고치기"버튼을 눌렀을 때 호출하는 함수를 구현한다.
 */

	// 살아있는 작품 중에서, 테이블의 (1 + giShow)번째 행에 해당하는 작품을 가리키는 첨자를 찾는다.
	function fniShow() {
		for (var iShow = 0; iShow < gaShow.length; ++iShow) {
			if (!(gaShow[iShow].flag & cOFLAG_DELETED) && gaShow[iShow].rowIndex == giShow) {
				// 찾았다.
				return iShow;
			}
		}
		// 찾지 못했다.
		return -1;
	}

	function fnbUniqueName() {
		for (i = 0; i < gShow.aScn.length; ++i) {
			var	oScn = gShow.aScn[i];
			if (oScn.flag & cOFLAG_DELETED) {	// 지워진 행은
				// 건너뛴다.
				continue;
			}

			if (oScn.name == '장면의 이름을 정하세요') {
				alert('장면의 이름을 정해야 합니다');
				tblScn.rows[oScn.rowIndex].children[1].children[0].children[0].children[0].children[1].children[0].select();
				return false;
			}
			for (k = i + 1; k < gShow.aScn.length; ++k) {
				var	oScn_ = gShow.aScn[k];
				if (oScn_.flag & cOFLAG_DELETED) {	// 지워진 행은
					// 건너뛴다.
					continue;
				}

				if (oScn_.name == oScn.name) {
					alert('장면의 이름은 달라야 합니다');
					tblScn.rows[oScn_.rowIndex].children[1].children[0].children[0].children[0].children[1].children[0].select();
					return false;
				}
			}
		}

		for (i = 0; i < gShow.aChr.length; ++i) {
			var	oChr = gShow.aChr[i];
			if (oChr.flag & cOFLAG_DELETED) {	// 지워진 행은
				// 건너뛴다.
				continue;
			}

			if (oChr.name == '인물의 이름을 정하세요') {
				alert('인물의 이름을 정해야 합니다');
				tblChr.rows[oChr.rowIndex].children[1].children[0].children[0].children[0].children[1].children[0].select();
				return false;
			}
			for (k = i + 1; k < gShow.aChr.length; ++k) {
				var	oChr_ = gShow.aChr[k];
				if (oChr_.flag & cOFLAG_DELETED) {	// 지워진 행은
					// 건너뛴다.
					continue;
				}

				if (oChr_.name == oChr.name) {
					alert('인물의 이름은 달라야 합니다');
					tblChr.rows[oChr_.rowIndex].children[1].children[0].children[0].children[0].children[1].children[0].select();
					return false;
				}
			}
		}

		for (i = 0; i < gShow.aBg.length; ++i) {
			var	oBg = gShow.aBg[i];
			if (oBg.flag & cOFLAG_DELETED) {	// 지워진 행은
				// 건너뛴다.
				continue;
			}

			if (oBg.name == '배경의 이름을 정하세요') {
				alert('배경의 이름을 정해야 합니다');
				tblBg.rows[oBg.rowIndex].children[1].children[0].children[0].children[0].children[1].children[0].select();
				return false;
			}
			for (k = i + 1; k < gShow.aBg.length; ++k) {
				var	oBg_ = gShow.aBg[k];
				if (oBg_.flag & cOFLAG_DELETED) {	// 지워진 행은
					// 건너뛴다.
					continue;
				}

				if (oBg_.name == oBg.name) {
					alert('배경의 이름은 달라야 합니다');
					tblBg.rows[oBg_.rowIndex].children[1].children[0].children[0].children[0].children[1].children[0].select();
					return false;
				}
			}
		}

		return true;
	}

	// giShow가 가리키는 작품 앞에 새로운 작품을 추가한다.
	// giShow = -1인 경우에는 첫 번째로 등록한다.
	function fnInsertShow() {
		// 장면, 인물/배경에서 같은 이름을 사용하는 지 확인한다.
		if (!fnbUniqueName()) {
			return;
		}

		// 테이블의 giShow번째 행에 해당하는 작업을 찾는다.
		var	iShow;
		if (giShow == -1) {
			giShow = 0;
			iShow = 0;
			gShow.rowIndex = 0;	// 첫 번째에 들어간다.
			// 꼬리행을 지우고, 다시 만든다.
			if (!fnMakeTailRow(true)) {	// "지운다"버튼을 만든다.
				alert('fnMakeTailRow(): Failure');
			}
		} else {
			iShow = fniShow();
			if (iShow == -1) {	// 찾을 수 없다.
				alert('giShow(= ' + giShow + ')에 해당하는 작품을 찾을 수 없다');
				return;
			}
			// 같은 제목을 가진 작품이 있는 지 알아본다.
			for (var i = 0; i < gaShow.length; ++i) {
				if (gaShow[i].name == gShow.name) {
					alert('같은 제목을 가진 작품이 ' + (1 + gaShow[i].rowIndex) + '번째 행에 있다');
					return;
				}
			}
		}
		// "반영하기"버튼은 무조건 활성화된다.
		btnSubmit.disabled = false;

		// gShow의 정보를 테이블에 표시한다.
		if (!fnInsertRow(gShow)) {
			alert('fnInsertRow(): Failure');
			return;
		}
		// giShow번째 버튼의 상태를  CHECKED로 한다.
		tblShowList.rows[1 + giShow].children[0].children[0].checked = true;
		// "고치기"버튼을 활성화한다.
		btnUpdateShow.disabled = false;

		// giShow번째에 행이 추가되므로,
		// giShow보다 크거나 같은 rowIndex를 가진 작품에서
		// rowIndex값을 하나 증가시킨다.
		for (var i = 0; i < gaShow.length; ++i) {
			if (gaShow[i].rowIndex >= giShow)
				++gaShow[i].rowIndex;
		}

		// gShow에 있는 모든 데이터는 새로 만든 것들이어야 한다.
		// 특히 장면, 인물/배경과 괄련된 데이터는 DB와는 상관없어야 한다.
		// 즉 id = 0이어야 한다.
		// cOFLAG_DELETED가 설정된 장면, 인물/배경은 없애면 된다.
		// 인물/배경에서 장면에 대한 참조가 바뀌므로,
		// 먼저 이 정보부터 변경해야 한다.
		var	iScnSerial = 0;		// 장면에 대한 새로운 참조 번호이다.
		for (var i = 0; i < gShow.aScn.length; ++i) {
			if (!(gShow.aScn[i].flag & cOFLAG_DELETED)) {	// 지워지지 않은 장면의 경우에 대해서
				// i번째 장면이 iScnSerial번째 장면이 된다.
				fnChangeSceneReference(i, iScnSerial);
				++iScnSerial;
			}
		}
		// 이제는 cOFLAG_DELETED가 설정된 모든 장면, 인물/배경을 없앤다.
		for (var i = gShow.aScn.length - 1; i >= 0; --i) {
			if (gShow.aScn[i].flag & cOFLAG_DELETED) {
				gShow.aScn.splice(i, 1);
			} else {
				// 새로운 장면이다.
				gShow.aScn[i].id = 0;	// 새로운 장면이다.
				gShow.aScn[i].flag = 0;
				gShow.aScn[i]._serial = undefined;
			}
		}
		for (var i = gShow.aChr.length - 1; i >= 0; --i) {
			if (gShow.aChr[i].flag & cOFLAG_DELETED) {
				gShow.aChr.splice(i, 1);
			} else {
				// 새로운 인물이다.
				gShow.aChr[i].id = 0;
				gShow.aChr[i].flag = 0;
				gShow.aChr[i]._aScnId = [];
			}
		}
		for (var i = gShow.aBg.length - 1; i >= 0; --i) {
			if (gShow.aBg[i].flag & cOFLAG_DELETED) {
				gShow.aBg.splice(i, 1);
			} else {
				// 새로운 배경이다.
				gShow.aBg[i].id = 0;	// 새로운 배경이다.
				gShow.aBg[i].flag = 0;
				gShow.aBg[i]._aScnId = [];
			}
		}
		// gaShow배열의 iShow번째에 gShow의 정보를 담은 objShow오브젝트를 추가한다.
		var	aScn = [];
		gaShow.splice(iShow, 0, new objShow(0, gShow.name, gShow.date_begin, gShow.date_end, gShow.desc, gShow.aScn, gShow.aChr, gShow.aBg));
		gaShow[iShow].rowIndex = gShow.rowIndex;
	}

	// giShow가 가리키는 작품을 수정한다.
	// (가정) giShow ∈ [0, gaShow.length)
	function fnDeleteShow() {
		// 테이블의 giShow번째 행에 해당하는 작업을 찾는다.
		var	iShow = fniShow();
		if (iShow == -1) {	// 찾을 수 없다.
			alert('giShow(= ' + giShow + ')에 해당하는 작품을 찾을 수 없다');
			return;
		}

		// iShow번째 작품이 DB에서 가져온 작업이라면 지웠다는 표시만 하고,
		// 웹에서 만든 작업이라면 해당 성분을 지운다.
		if (gaShow[iShow].flag & cOFLAG_DB) {
			gaShow[iShow].flag |= cOFLAG_DELETED;
		} else {
			gaShow.splice(iShow, 1);
		}

		// 테이블에서 giShow번째 행을 지운다.
		tblShowList.deleteRow(1 + giShow);	// 머리행은 건너뛴다.

		// giShow번째 행이 지우졌으므로,
		// 살아있는 작품 중에서 작품의 rowIndex값이 giShow보다 크거나 같은 작품의 rowIndex값을 하나 감소한다.
		for (var i = 0; i < gaShow.length; ++i) {
			var	oShow = gaShow[i];
			if (!(oShow & cOFLAG_DELETED) && oShow.rowIndex >= giShow) {
				--oShow.rowIndex;
			}
		}

		// giShow가 제일 마지막 행이였다면 giShow을 하나 감소한다.
		if (giShow == (tblShowList.rows.length - (1 + 1))) {	// 머리행과 꼬리행은 제외한다.
			--giShow;
		}
		// 해당 버튼의 상태를 CHECKED로 한다.
		var	bSubmit = fnbSubmit();
		if (giShow >= 0) {	// 작품이 있는 경우에만 이렇게 한다.
			tblShowList.rows[1 + giShow].children[0].children[0].checked = true;
			btnSubmit.disabled = !bSubmit;
		} else {	// 작품이 하나도 없다.
			// 꼬리행에서 지운다.
			tblShowList.deleteRow(1);
			if (!fnMakeTailRow(false)) {	// "지운다"버튼을 없앤다.
				alert('fnMakeTailRow(): Failure');
			}
			btnUpdateShow.disabled = true;
		}

		// giShow번째 작품으로 필요한 것들을 설정한다.
		if (!fnInit()) {
			alert('fnInit(): Failure');
		}
	}

	// giShow가 가리키는 작품을 수정한다.
	// (가정) giShow ∈ [0, gaShow.length)
	function fnUpdateShow() {
		// 장면, 인물/배경에서 같은 이름을 사용하는 지 확인한다.
		if (!fnbUniqueName()) {
			return;
		}

		// 테이블의 giShow번째 행에 해당하는 작업을 찾는다.
		var	iShow = fniShow();
		if (iShow == -1) {	// 찾을 수 없다.
			alert('giShow(= ' + giShow + ')에 해당하는 작품을 찾을 수 없다');
			return;
		}

		// gaShow배열에서 iShow번째 성분을 고친다.
////////////////////////////////////////////////////////////////////////
		// 작품의 기본정보(name, date_begin, date_end, desc)는 현재 값만을 복사해온다.
		// 그래야 나중에 어떤 변화가 있었는 지 알 수 있다.
		var	oShow = gaShow[iShow];
		oShow.name = gShow.name;
		oShow.date_begin = new Date(gShow.date_begin);
		oShow.date_end = new Date(gShow.date_end);
		oShow.desc = gShow.desc;

		// 작품을 구성하는 장면과 등장하는 인물/배경에 대한 정보는 있는 그대로 복사해야 한다.
		// 왜냐하면 사용되는 gShow의 장면, 인물/배경에는 원래값을 가지고 있기 때문이다.
		// 장면
		oShow.aScn = [];	// 초기화하고...
		for (var i = 0; i < gShow.aScn.length; ++i) {	// 복사한다.
			var	oScn = gShow.aScn[i];

			// (id, rowIndex, name, desc)을 복사하기위해,
			// objScene오브젝트를 만든다. ('_'로 시작하는 변수는 바뀌면 안된다.)
			oShow.aScn[i] = new objScene(oScn.name, oScn.desc, oScn._serial, oScn.id, oScn.rowIndex);
			// flag는 따로 복사한다. new를 통해서 만들면 기본값으로 설정되기 때문이다.
			oShow.aScn[i].flag = oScn.flag;
			// '_'로 시작하는 변수를 원래값으로 덮어쓴다.
			oShow.aScn[i]._name = oScn._name;
			oShow.aScn[i]._desc = oScn._desc;
			oShow.aScn[i]._serial = oScn._serial;
		}
		// 인물/배경
		var	aChrBg = {
			Chr: "인물",
			Bg: "배경"
		};	// 오브젝트
		for (ChrBg in aChrBg) {
			// ChrBg: "Chr", "Bg"
			// aChrBg[ChrBg]: "인물", "배경"
			eval("oShow.a" + ChrBg + " = []");
			eval("var a = gShow.a" + ChrBg);
			for (var i = 0; i < a.length; ++i) {
				var	oFrom = a[i];
				// (id, rowIndex, name, desc, aScnSerial)를 복사하기위해,
				// objChrBg오브젝트를 만든다. ('_'로 시작하는 변수는 바뀌면 안된다.)
				eval("oShow.a" + ChrBg + "[i] = new objChrBg(oFrom.name, oFrom.desc, oFrom.aScnSerial, oFrom.id, oFrom.rowIndex)");
				eval("var oTo = oShow.a" + ChrBg + "[i]");
				// flag는 따로 복사한다. new를 통해서 만들면 기본값으로 설정되기 때문이다.
				oTo.flag = oFrom.flag;
				// '_'로 시작하는 변수를 원래값으로 덮어쓴다.
				oTo._name = oFrom._name;
				oTo._desc = oFrom._desc;
				oTo._aScnId = [];
				for (var k = 0; k < oFrom._aScnId.length; ++k)
					oTo._aScnId[k] = oFrom._aScnId[k];
			}
		}
////////////////////////////////////////////////////////////////////////

		// giShow에 해당하는 행의 내용을 고친다.
		// 머리행까지 있으니 (1 + giShow)가 테이블의 해당하는 행에 대한 첨자이다.
		if ((1 + giShow) < 0 || tblShowList.rows.length <= (1 + giShow)) {
			alert('tblShowList테이블에서 (1 + giShow = ' + (1 + giShow) + ')에 해당하는 행이 없다');
			return;
		}
		oTd = tblShowList.rows[1 + giShow].children[0];
		if (oTd.nodeName != "TD") {
			alert('tblShowList.rows[' + (1 + giShow) + '].children[0].nodeName != "TD" ("' + oTd.nodeName + '")');
			return;
		}
		var	bModified = gaShow[iShow].bModified();
		var	bgColor = gaShow[iShow].id ? bModified ? cBGCOLOR_UPDATE : cBGCOLOR_SELECT : cBGCOLOR_INSERT;
		oTd.innerHTML = '&nbsp;<INPUT TYPE="RADIO" NAME="rdoiShow" VALUE="" CHECKED STYLE="background: ' + bgColor + ';" onClick="fnrdoiShowOnClick(this);">&nbsp;&nbsp;' + gaShow[iShow].name;

		if (gaShow[iShow].id)
			btnSubmit.disabled = !bModified;
	}
	// "반영하기"버튼의 활성화 여부를 결정한다.
	function fnbSubmit() {
		// "반영하기"버튼의 할성화여부를 결정한다.
		var	bSubmit = false;
		for (var i = 0; i < gaShow.length; ++i) {
			var	oShow = gaShow[i];
			if (!(oShow.flag & cOFLAG_DB)) {	// 추가된 작품이 있다.
				bSubmit = true;
				break;
			}
			// DB에서 가져온 작품이
			if (
				oShow.flag & cOFLAG_DELETED ||	// 지워졌거나
				oShow.bModified()		// 고쳐졌다.
			) {
				bSubmit = true;
				break;
			}
		}
		return bSubmit;
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/15 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * fmWorkSchedule.PHP처럼 작품들도 일정표를 통해서 관리하도록 한다.
 * 따라서 일정표를 관리하는 데에 필요한 많은 부분은 fmWorkSchedule.PHP에서 가져왔다.
 */

// 2. 작품 목록을 표시한다.
	// 작품목로에서 작품이 선택되었을 때 호출된다.
	function fnrdoiShowOnClick(o) {
		//        INPUT            TD            TR
		var	oTr = o.parentElement.parentElement;
		if (oTr.nodeName != "TR") {
			alert('oTr.nodeName(' + oTr.nodeName + ', o.nodeName = ' + o.nodeName + ') != "TR"');
			return;
		}
		var	iShow = oTr.rowIndex - 1;	// 머리행은 제외한다.
		if (iShow < 0 || gaShow.length <= iShow) {
			alert('giShow = ' + iShow + '에 해당하는 작품이 없다: 작품 수 = ' + gaShow.length);
			return;
		}
		if (iShow == giShow) {	// 현재 표시된 행이 선택되었다.
			// 아무일도 하지 않는다.
			return;
		}

		// 다른 작품을 설정하고,
		giShow = iShow;
		// 그 작품에 따른 것으로 모든 것을 초기화한다.
		fnInit();
	}
	function fnMakeShowListTable() {
		// 모든 행을 지운다.
		while (tblShowList.rows.length)
			tblShowList.deleteRow();

		// 머리행을 만든다.
		if (!fnMakeHeadRow()) {
			alert('fnMakeHeadRow(): Failure');
			return false;
		}

		if (giShow == -1) {	// 표시될 것이 없다.
			return true;
		}

		if (!fnMakeBodyRow()) {
			alert('fnMakeBodyRow(): Failure');
			return false;
		}

		if (!fnMakeTailRow(true)) {	// "지운다"버튼을 만든다.
			alert('fnMakeTailRow(): Failure');
			return false;
		}
		return true;
	}
	function fnMakeHeadRow() {
		var	oTr = tblShowList.insertRow();
		if (!oTr) {
			alert('tblShowList.insertRow(): Failure');
			return false;
		}
		var	oTd = oTr.insertCell();
		if (!oTd) {
			alert('oTr.insertCell(): Failure');
			tblShowList.deleteRow(oTr.rowIndex);
			return false;
		}
		oTd.align = "CENTER";
		oTd.width = "100%";
		oTd.innerHTML = "작품명";
		return true;
	}
	// 데이블 작품을 정보를 표시한다.
	// 몇 번째 행에 추가하는 지는 .rowIndex값을 본다.
	function fnInsertRow(oShow) {
		var	oTr = tblShowList.insertRow(1 + oShow.rowIndex);	// 머리행을 건너뛴다.
		if (!oTr) {
			alert('tblShowList.insertRow(): Failure');
			return false;
		}
		var	oTd = oTr.insertCell();
		if (!oTd) {
			alert('oTr.insertCell(): Failure');
			tblShowList.deleteRow(oTr.rowIndex);
			return false;
		}
//		oTd.align = "CENTER";
		oTd.width = "100%";
		var	bgColor = !(oShow.flag & cOFLAG_DB) ? cBGCOLOR_INSERT : cBGCOLOR_SELECT;
		oTd.innerHTML = '&nbsp;<INPUT TYPE="RADIO" NAME="rdoiShow" VALUE="" STYLE="background: ' + bgColor + ';" onClick="fnrdoiShowOnClick(this);">&nbsp;&nbsp;' + oShow.name;
		return true;
	}
	function fnMakeBodyRow() {
		for (var i = 0; i < gaShow.length; ++i) {
			if (!(fnInsertRow(gaShow[i]))) {
				alert('fnInsertRow(): Failure');
				return false;
			}
		}

		return true;
	}
	function fnMakeTailRow(bDeleteButton) {
		var	oTr = tblShowList.insertRow();
		if (!oTr) {
			alert('tblShowList.insertRow(): Failure');
			return false;
		}
		var	oTd = oTr.insertCell();
		if (!oTd) {
			alert('oTr.insertCell(): Failure');
			tblShowList.deleteRow(oTr.rowIndex);
			return false;
		}
		oTd.align = "CENTER";
		oTd.width = "100%";
		var	del = bDeleteButton ? '<INPUT TYPE="BUTTON" VALUE="지운다" onClick="fnDeleteShow();">&nbsp;&nbsp;&nbsp;&nbsp;' : '';
		oTd.innerHTML = del + '<INPUT ID="btnSubmit" TYPE="BUTTON" VALUE="반영한다" onClick="fnSubmit();">';
		return true;
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/14 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
 *
 * fmWorkSchedule.PHP처럼 작품들도 일정표를 통해서 관리하도록 한다.
 * 따라서 일정표를 관리하는 데에 필요한 많은 부분은 fmWorkSchedule.PHP에서 가져왔다.
 */

// 1. 데이터를 가져오거나 보낼 때, gShow(= new objShow(...))단위로 이루어지지 않고
// gaShow(= new Array(new objShow(...), ...))단위로 이루어지도록 한다.
	var	cBGCOLOR_INSERT = "<?PHP echo BGCOLOR_INSERT; ?>";	// 새로 추가할 작품
	var	cBGCOLOR_SELECT = "<?PHP echo BGCOLOR_SELECT; ?>";	// DB에서 가져온 작품
	var	cBGCOLOR_UPDATE = "<?PHP echo BGCOLOR_UPDATE; ?>";	// DB에서 가져와 수정한 작품
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/4 (EST)
 *		By Gwang-Ho Kim (iccentric@gmail.com)
 *
 * objTarget오브젝트에 bFromDb대신에 id를 사용한다.
 * id값이 0인 경우는 새로 만든 것이고, 1보다 크거나 같은 경우는 DB로부터 얻은 데이터이다.
 * 장면의 경우는 SCENE_ID, 인물/배경의 경우에는 TARGET_ID이다.
 */

	var	cOFLAG_DB	= <?PHP echo FLAG_DB ?>;	// 오브젝트에 대한 정보가 DB로부터 왔는가?
	var	cOFLAG_DELETED	= <?PHP echo FLAG_DELETED ?>;	// 해당 오브젝트가 지워졌는가?
	var	cOFLAG_NAME	= <?PHP echo FLAG_NAME ?>;	// 이름(NAME)이 바뀌었는가?
	var	cOFLAG_DESC	= <?PHP echo FLAG_DESC ?>;	// 설명(DESCRIPTION)이 바뀌었는가?
	var	cOFLAG_BEGIN	= <?PHP echo FLAG_BEGIN ?>;	// 작품을 시작하는 날짜가 바뀌었는가?
	var	cOFLAG_END	= <?PHP echo FLAG_END ?>;	// 작품을 끝내는 날짜가 바뀌었는가?
	var	cOFLAG_SERIAL	= <?PHP echo FLAG_SERIAL ?>;	// 장면의 일련번호가 바뀌었는가?
	var	cOFLAG_SCN_REF	= <?PHP echo FLAG_SCN_REF ?>;	// 인물/배경에서 장면에 대한 참조가 바뀌었는가?
	// DB로부터 만들어진 오브젝트가 지워졌다면
	// 오브젝트 자체를 없앤다.
	// 따라서 다음은 가능한 조합이다.
	//	00: DB로부터 오지 않은 즉 새로 추가한 오브젝트이다.
	//	01: DB로부터 만들어진 오브젝트로서 현재 살아있다.
	//	10: DB로부터 오지 않은 즉 새로 만든 오브젝트지만 이미 그 오브젝트를 지웠다. 따라서 이럴 수는 없다.
	//	11: DB로부터 만들어진 오브젝트지만 현재는 죽었다.
	function objTarget(name, desc, id, rowIndex) {
		this.name = name;
		this.desc = desc;

		this.id = id;
		if (id) {
			this.flag = cOFLAG_DB;

			// 나중에 비교하기 위하여 원본은 보관한다.
			this._name = this.name;
			this._desc = this.desc;
		} else {
			this.flag = 0;
		}

		// 테이블에서 몇 번재 행에 나타나는 지를 알려준다.
		this.rowIndex = rowIndex;
	}

	// 장면에 대한 정보를 담을 오브젝트에 대한 생성자이다.
	function objScene(name, desc, serial, id, rowIndex) {
		this.base = objTarget;
		this.base(name, desc, id, rowIndex);

		if (id) {
			// 나중에 비교하기 위하여 원본은 보관한다.
			this._serial = serial;
		} else {
			// "_XXX"값들은 정의되지 않았다. (undefined)
		}

		this.bModified = function fnbModified() {
			if (
				this.id == 0 ||			// 새로운 장면이 추가된 경우나
				this.flag & cOFLAG_DELETED	// DB에서 가져온 장면이 삭제된 경우에는
			) {
				// 당연하다.
				return true;
			}

			return this.name != this._name || this.desc != this._desc || this.rowIndex != this._serial;
		}
	}
	// 인물/배경에 대한 정보를 담을 오브젝트에 대한 생성자이다.
	function objChrBg(name, desc, aScnSerialByVal, id, rowIndex) {
		this.base = objTarget;
		this.base(name, desc, id, rowIndex);

		// aScnSerial배열이 의미있는 지 확인한다.
		var	bAllTheScenes = undefined;
		this.aScnSerial = new Array();
		var	o = this.aScnSerial;
		for (var i = 0; i < aScnSerialByVal.length; ++i) {
			var	iScn = parseInt(aScnSerialByVal[i]);
			if (
				!isNaN ||	// 정수가 아니거나
				iScn < -1	// 유효한 정수가 아니라면
			) {
				// 무시한다.
				alert(aScnSerialByVal[i] + ': 정수가 아니거나 -1보다 작은 정수이다');
				continue;
			}
			if (fnExistElement(o, iScn) >= 0) {	// 이미 등록된 정수라면
				// 무시한다.
				alert(aScnSerialByVal[i] + ': 이미 등록된 정수이다');
				continue;
			}
			if (iScn == -1) {	// 작품 전체에 쓰인다.
				if (bAllTheScenes === false) {
					// 섞여있다.
					alert('-1과 >= 0인 숫자가 섞여있다: [' + aScnSerialByVal.join(", ") + ']');
					return;
				}
				bAllTheScenes = true;
			} else {		// 작품 전체가 아닌 특정 장면(iScn)에서 쓰인다.
				if (bAllTheScenes === true) {
					// 섞여있다.
					alert('-1과 >= 0인 숫자가 섞여있다: [' + aScnSerialByVal.join(", ") + ']');
					return;
				}
				bAllTheScenes = false;
			}
			o.push(iScn);
		}
		if (o.length == 0) {	// 의미있는 숫자가 하나도 없다.
			alert('의미있는 숫자가 하나도 없다: [' + aScnSerialByVal.join(", ") + ']');
			return;
		}

		// 2005/11/5 (EST)
		//		By 김광호(Gwang-Ho Kim, iccentric@gmail.com)
		// 장면에 변화가 있다면 더 이상 이 장면을 참조하는 첨자는 의미가 없다.
		// 대신에 장면의 id(SCENE_ID)를 보관한다.
		// 이 오브젝트의 초기화 단계에서는 알 수 없기 때문에,
		// 장면에 대한 설정이 끝나는 gShow의 초기화 다음에 이루어져야 한다.
		// fnSetData의 제일 마지막 단계에서 fnSaveSceneId함수를 호출한다.
		this._aScnId = [];
/*
		if (id) {
			// 나중에 비교하기 위하여 원본은 보관한다.
			this._oScnSerial = [];
			for (var i = 0; i < o.length; ++i)
				this._oScnSerial[i] = o[i];
		} else {
			// "_XXX"값들은 정의되지 않았다. (undefined)
		}
 */
		this.bModified = function fnbModified() {
			if (
				this.id == 0 ||			// 새로운 인물/배경이 추가된 경우나
				this.flag & cOFLAG_DELETED	// DB에서 가져온 인물/배경이 삭제된 경우에는
			) {
				// 당연하다.
				return true;
			}

			return this.name != this._name || this.desc != this._desc;
		}
	}
	// 작품에 대한 정보를 담을 오브젝트를 만든다.
	function objShow(id, name, date_begin, date_end, desc, aScnByVal, aChrByVal, aBgByVal) {
		this.id = id;	// >= 1: SHOW_ID, 0: NEW
		this.name = name;
		this.date_begin = new Date(date_begin);
		this.date_end = new Date(date_end);
		this.desc = desc;
		if (id) {
			this.flag = cOFLAG_DB;

			// 나중에 비교하기 위하여 원본은 보관한다.
			this._name = name;
			this._date_begin = date_begin;
			this._date_end = date_end;
			this._desc = desc;
		} else {
			this.flag = 0;

			// "_XXX"값들은 정의되지 않았다. (undefined)
		}

		// 다음 값들의 변화여부는 해당 오브젝트(objScene, objChrBg)를 조사해서 알아낸다.
		// 오브젝트에 대한 참조를 하지 않고 값을 사용한다. 즉 복사한다.
if (0) {

		this.aScn = aScnByVal;
		this.aChr = aChrByVal;
		this.aBg = aBgByVal;
} else {
		this.aScn = [];
		for (var i = 0; i < aScnByVal.length; ++i) {
			var	oScn = aScnByVal[i];
			this.aScn.push(new objScene(oScn.name, oScn.desc, oScn._serial, oScn.id, oScn.rowIndex));
		}

		this.aChr = [];
		for (var i = 0; i < aChrByVal.length; ++i) {
			var	oChr = aChrByVal[i];
			this.aChr.push(new objChrBg(oChr.name, oChr.desc, oChr.aScnSerial, oChr.id, oChr.rowIndex));
		}
		this.aBg = [];
		for (var i = 0; i < aBgByVal.length; ++i) {
			var	oBg = aBgByVal[i];
			this.aBg.push(new objChrBg(oBg.name, oBg.desc, oBg.aScnSerial, oBg.id, oBg.rowIndex));
		}
}

		// 2005/11/15 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
		// 각 작품을 작품목록에 표시할 때, 몇 번째 행에 위치하는 지를 기억한다.
		// 정렬 등에 사용된다.
		this.rowIndex = -1;	// (정책) 기본값은 -1이다. 즉 아직 작품목록에 나타나지 않는다.

		this.bModified = function fnbModified() {
			if (this.id == 0)
				return false;

			if (
				this.name != this._name ||
				fnMakeDateString(this.date_begin) != fnMakeDateString(this._date_begin) ||
				fnMakeDateString(this.date_end) != fnMakeDateString(this._date_end) ||
				this.desc != this._desc
			)
				return true;

			for (var i = 0; i < this.aScn.length; ++i) {
				if (this.aScn[i].bModified())
					return true;
			}

			for (var i = 0; i < this.aChr.length; ++i) {
				var	aScnPseudoId = [];
				fnSetScenePseudoId(this, this.aChr[i], aScnPseudoId);
				if (this.aChr[i].bModified() || this.aChr[i]._aScnId.sort().join(', ') != aScnPseudoId.sort().join(', '))
					return true;
			}
			for (var i = 0; i < this.aBg.length; ++i) {
				var	aScnPseudoId = [];
				fnSetScenePseudoId(this, this.aBg[i], aScnPseudoId);
				if (this.aBg[i].bModified() || this.aBg[i]._aScnId.sort().join(', ') != aScnPseudoId.sort().join(', '))
					return true;
			}

			return false;
		}
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/4 (EST)
 *		By Gwang-Ho Kim (iccentric@gmail.com)
 *
 * 주어진 SHOW_ID를 통해서 DB로부터 작품에 대한 데이터를 읽어 gShow변수를 설정한다.
 * (DB)라고 된 부분을 채운다.
 */

	var	gaShow;		// gaShow = [new objShow(...), ...]: objShow들로 이루어지 배열이다.
	var	giShow;		// 현재 관리되고 있는 작품의, gaShow배열에 대한 첨자이다.
	var	gShow;		// 현재 관리되고 있는 작품을 가리킨다.
/*
	gShow = new objShow(
		1,	// >= 1: SHOW_ID
		"작품명",
		new Date(),
		new Date(),
		"시나리오...",
		// 장면
		new Array(
			new objScene("장면 #1", "#1: ...", 0, 1, 0),
			new objScene("장면 #2", "#2: ...", 1, 2, 1),
			new objScene("장면 #3", "#3: ...", 2, 3, 2)
		),
		// 인물
		new Array(
			new objChrBg("인물 #1", "인물#1: ...", [-1], 1, 0),
			new objChrBg("인물 #1", "인물#1(장면 #3): ...", [2], 2, 1),
			new objChrBg("인물 #2", "인물#2: ...", [-1], 3, 2)
		),
		// 배경
		new Array(
			new objChrBg("배경 #1", "배경#1: ...", [-1], 4, 0),
			new objChrBg("배경 #2", "배경#2: ...", [-1], 5, 1),
			new objChrBg("배경 #2", "배경#2(장면 #1): ...", [0], 6, 2),
			new objChrBg("배경 #3", "배경#3: ...", [-1], 7, 3)
		)
	);
 */
	function fnSetData() {
		var	JavaScriptEvalCode = "<?PHP echo $JavaScriptEvalCode ?>";
//		alert(JavaScriptEvalCode);
		eval(JavaScriptEvalCode);

		// 2005/11/5 (EST)
		//		By 김광호(Gwang-Ho Kim, iccentric@gmail.com)
		// 인물/배경에서 참조했던 장면에 대한 id를 보관해서,
		// 나중에 참조했던 장면들에 어떤 변화가 있었는 지 알 수 있도록 한다.
		for (var iShow = 0; iShow < gaShow.length; ++iShow) {
			var	oShow = gaShow[iShow];
			for (var i = 0; i < oShow.aChr.length; ++i) {
				fnSaveSceneId(oShow, oShow.aChr[i]);
			}
			for (i = 0; i < oShow.aBg.length; ++i) {
				fnSaveSceneId(oShow, oShow.aBg[i]);
			}
		}
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/11/1 (EST)
 *		By Gwang-Ho Kim (iccentric@gmail.com)
 *
 * 데이터를 모아 PHP코드로 만든다.
 */

	// 날짜를 yyyy-mm-dd형태로 문자열로 만든다.
	function fnMakeDateString(date) {
		return date.getYear() + '-' + (1 + date.getMonth()) + '-' + date.getDate();
	}

	// 주어진 오브텍트 o에 해당하는 인물/배경이 참조하는 장면의 pseudo ID에 대한 배열을 얻는다.
	// Pseudo ID는 다음과 같은 값을 가진다.
	//	1. 작품 전체에서 사용되는 경우:
	//		Pseudo ID의 값은 0이다.
	//	2. 특정 장면을 참조하는 경우:
	// 		1) 기존에 있던 장면이라면 pseudo ID의 값은 그 장면의 고유한 id(SCENE_ID)가 된다.
	//		즉 1보다 크다.
	//		2) 새로 만든 장면이라면 pseudo ID의 값은 0보다 작은 음수이다.
	//		절대값에서 1을 뺀 값이 참조하는 장면에 대한 첨자로 사용된다.
	function fnSetScenePseudoId(oShow, o, a) {
		a.length = 0;	// 초기화한다.
		if (o.aScnSerial.length == 1 && o.aScnSerial[0] == -1) {	// 작품 전체에서 사용된다.
			a[0] = 0;
			return;
		}
		// 특정 장면에서만 사용된다.
		for (var i = 0; i < o.aScnSerial.length; ++i) {
			var	index = o.aScnSerial[i];
			var	id = oShow.aScn[index].id;
			// id = 0이 경우는 새로 만들어진 장면을 참조한다는 뜻이다.
			a.push(id == 0 ? -(1 + oShow.aScn[index].rowIndex) : id);
		}
		return;
	}
	// 인물/배경에서 참조하는 장면에 대한 정보는 aScnSerial에 저장되어 있다.
	// 이 배열의 각 성분의 값은 -1인 경우를 제외하고는 참조하는 장면을 가리킨다.
	// 참조하는 장면이 바뀌었는 지를 알아보기 위해 데이터 "초기화 바로 후"에
	// 처음에 참조했던 장면들에 대한 첨자대신에 id(SCEND_ID)를 보관한다.
	function fnSaveSceneId(oShow, o) {
		fnSetScenePseudoId(oShow, o, o._aScnId);
	}

/*
		$oShow = new Show(
			0,			// 새로 추가한다.
			0,			// 새로 추가하므로 이 값은 사용되지 않는다.

			'작품 #0',
			'2005/10/12', '2005/12/10',
			'시나리오...',

			// 장면
			array(
				new TargetScene(
					0,		// 작품을 새로 추가하므로 이 값은 사용되지 않는다.
					0,		// 마찬가지로 사용되지 않는다.
					'장면 #0',
					'장면 #0: ...'
				),
				......
			),
			// 인물
			array(
				new TargetChrBg(
					0,		// 작품을 새로 추가하므로 이 값은 사용되지 않는다.
					0,		// 마찬가지로 사용되지 않는다.
					'인물 #0',
					'인물 #0: ...',
					array(array(), array(0))
				),
				......
			),
			// 배경
			array(
				new TargetChrBg(
					0,		// 작품을 새로 추가하므로 이 값은 사용되지 않는다.
					0,		// 마찬가지로 사용되지 않는다.
					'배경 #0',
					'배경 #0: ...',
					array(array(), array(0))
				),
				......
			)
		);
 */
	// 웹을 통해 처리한 데이터를 모아,
	// PHP코드형태로 만든다.
	// 2005/11/14 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
	// 모든 작품들에 대한 정보를 보낸다.
	//	$oShow = new Show(...)
	// 대신에
	//	$aShow = array(SHOW_ID => new Show(...))
	function fnMakePHPEvalCode() {
		var	aPHPEvalCode = [];
	for (var iShow = 0; iShow < gaShow.length; ++iShow) {
		var	oShow = gaShow[iShow];
/*
		$oShow = new Show(
			0,			// 새로 추가한다.
			0,			// 새로 추가하므로 이 값은 사용되지 않는다.
			'작품 #0',
			'2005/10/12', '2005/12/10',
			"시나리오...",
 */
		if (oShow.flag & cOFLAG_DELETED) {
			aPHPEvalCode.push(
				'\n' +
				'\t' + 'new Show(' + '\n' +
				'\t\t' + (oShow.id) + ',' + '\n' +
				'\t\t' + (oShow.flag) + ',' + '\n' +
				'\n' +
				'\t\t' + "''," + '\n' +
				'\t\t' + "'', " + "'', " + '\n' +
				'\t\t' + "''" + ',' + '\n' +
				'\n' +
				'\t\t' + '// 장면' + '\n' +
				'\t\t' + 'array(' + '\n' +
				'\t\t' + '),' + '\n' +
				'\n' +
				'\t\t' + '// 인물' + '\n' +
				'\t\t' + 'array(' + '\n' +
				'\t\t' + '),' + '\n' +
				'\t\t' + '// 배경' + '\n' +
				'\t\t' + 'array(' + '\n' +
				'\t\t' + ')' + '\n' +
				'\t' + ')'
				
			);
			continue;
		}
		var	PHPEvalCode = '';
		PHPEvalCode += '\n';
		PHPEvalCode += '\t' + 'new Show(' + '\n';
		PHPEvalCode += '\t\t' + (oShow.id) + ',' + '\n';	// SHOW_ID: 새로 만든다면 0, 수정하는 것이라면 기존 SHOW_ID (>= 1)
		var	flag = oShow.id == 0 ? 0 :	// 새로 만드는 경우에는 0이다.
			// DB에서 가져온 경우에 어떤 값들이 변했는 지 알아본다.
			(oShow.name == oShow._name ? 0 : cOFLAG_NAME) |
			(fnMakeDateString(oShow.date_begin) == fnMakeDateString(oShow._date_begin) ? 0 : cOFLAG_BEGIN) |
			(fnMakeDateString(oShow.date_end) == fnMakeDateString(oShow._date_end) ? 0 : cOFLAG_END) |
			(oShow.desc == oShow._desc ? 0 : cOFLAG_DESC)
		;
		PHPEvalCode += '\t\t' + (flag) + ',' + '\n';	// FLAG: SHOW_ID >= 1인 경우에
							// (name, date_begin,date_end, desc)중에서
							// 어느 부분이 바뀌었는 지를 말해준다.
		PHPEvalCode += '\n';

		// 작품명
		PHPEvalCode += '\t\t' + ("'" + (
			oShow.id == 0 ||	// 새로 만들거나
			flag & cOFLAG_NAME ?	// 이름이 변경된 경우에만
			oShow.name :		// 작품의 이름을 서버로 보낸다.
			''			// 그렇지 않은 경우에는 아무것도 보내지 않는다.
		) + "'") + ',' + '\n';
		// 작품을 시작하는 날짜(DATE_BEGIN)와 끝내는 날짜(DATE_END)
		PHPEvalCode += '\t\t' + ("'" + (
			oShow.id == 0 ||	// 새로 만들거나
			flag & cOFLAG_BEGIN ?	// 작품을 시작하는 날짜가 변경된 경우에만
			fnMakeDateString(oShow.date_begin) :	// 그 날짜를 서버로 보낸다.
			''			// 그렇지 않은 경우에는 아무것도 보내지 않는다.
		) + "'") + ', ' + ("'" + (
			oShow.id == 0 ||	// 새로 만들거나
			flag & cOFLAG_END ?	// 작품을 끝내는 날짜가 변경된 경우에만
			fnMakeDateString(oShow.date_end) :	// 그 날짜를 서버로 보낸다.
			''			// 그렇지 않은 경우에는 아무것도 보내지 않는다.
		) + "'") + ',' + '\n';
		// 시나리오
		// (정책) 일반적으로 여러 줄이므로 (')대신 (")로 문자열을 묶는다.
		// 따라서 시나리오 중에서 (")와 (\)는 각각 (\"), (\\)로 변환해야
		// 문자열을 둘러싸는 (")와 혼동되지 않는다.
		PHPEvalCode += '\t\t' + ('"' + (
			oShow.id == 0 ||	// 새로 만들거나
			flag & cOFLAG_DESC ?	// 시나리오의 내용이 바뀐 경우에만
			oShow.desc.replace(/([\\"])/g, "\\$1") :	// 시나리오의 내용을 서버로 보낸다.
			''			// 그렇지 않은 경우에는 아무것도 보내지 않는다.
		) + '"') + ',' + '\n';
		PHPEvalCode += '\n';

		// 장면에 대한 정보
/*
			// 장면
			array(
				new TargetScene(
					0,		// 작품을 새로 추가하므로 이 값은 사용되지 않는다.
					0,		// 마찬가지로 사용되지 않는다.
					'장면 #0',
					'장면 #0: ...'
				),
 */
		PHPEvalCode += '\t\t' + '// 장면' + '\n';
		PHPEvalCode += '\t\t' + 'array(';
		// 테이블에 나타나는 순서가 장면의 일련번호가 된다.
		// 일련번호가 바뀐 오브젝트, DB로부터 가져왔다가 지워진 오브젝트,
		// 새로 만든 오브젝트 등에 관한 정보를 모아야 한다.
		// 먼저 행에 대한 참조가 있는 오브젝트들로부터 행에 대한 참조를 얻는다.
		var	aScnRowIndex = [];
		for (var i = 0; i < oShow.aScn.length; ++i) {
			if (!(oShow.aScn[i].flag & cOFLAG_DELETED)) {	// 지워지지 않은 행에 대해서
				aScnRowIndex.push(oShow.aScn[i].rowIndex);
			}
		}
		// 정렬한다.
		aScnRowIndex.sort();
		var	aScnIndex = [];		// 테이블에 나타나는 순서에 따라 해당하는
						// 오브젝트들에 대한 참조를 얻는다.
		for (var i = 0; i < aScnRowIndex.length; ++i) {
			// i번째 행을 표시하기 위해 몇 번째 성분이 사용되었는 지를 안다.
			var	index = fnFindElement(oShow.aScn, i);
			aScnIndex.push(index);
		}
		// 이번에는 DB에서 가져왔으니 지원진 행에 대한 정보를 얻는다.
		for (var i = 0; i < oShow.aScn.length; ++i) {
			if (oShow.aScn[i].flag & cOFLAG_DELETED)
				aScnIndex.push(i);
		}
		var	oTargetScene = [];
		for (var i = 0; i < aScnIndex.length; ++i) {
			var	o = oShow.aScn[aScnIndex[i]];
			var	flag = o.id == 0 ? 0 :			// 새로 장면을 추가하면 무조건 0이다.
				(o.flag & cOFLAG_DELETED) |		// 삭제되었는가?
									// 삭제되었다면 다른 비트는 참고할 필요가 없다.
				(o.name == o._name ? 0 : cOFLAG_NAME) |	// 이름이 변경되었는가?
				(o.desc == o._desc ? 0 : cOFLAG_DESC) |	// 설명이 변경되었는가?
				(o._serial == o.rowIndex ? 0 : cOFLAG_SERIAL)	// 일련번호가 바뀌었는가?
			;
			oTargetScene.push(
				'\n' +
				'\t\t\t' + 'new TargetScene(' + '\n' +
				'\t\t\t\t' + (o.id) + ',' + '\n' +
				'\t\t\t\t' + (flag) + ',' + '\n' +
				'\t\t\t\t' + ("'" + (
					o.id == 0 ||			// 새로 만들거나
					!(flag & cOFLAG_DELETED) &&	// 삭제되지 않고,
					flag & cOFLAG_NAME ?		// 장면의 이름이 바뀐 경우에만
					o.name :			// 장면의 이름을 서버로 보낸다.
					''				// 그렇지 않은 경우에는 아무것도 보내지 않는다.
				) + "'") + ',' + '\n' +
				// 장면에 대한 설명
				// (정책) 일반적으로 여러 줄이므로 (')대신 (")로 문자열을 묶는다.
				// 따라서 시나리오 중에서 (")와 (\)는 각각 (\"), (\\)로 변환해야
				// 문자열을 둘러싸는 (")와 혼동되지 않는다.
				'\t\t\t\t' + ('"' + (
					o.id == 0 ||			// 새로 만들거나
					!(flag & cOFLAG_DELETED) &&	// 삭제되지 않고,
					flag & cOFLAG_DESC ?		// 장면에 대한 설명이 바뀐 경우에만
					o.desc.replace(/([\\"])/g, "\\$1") :	// 설명을 서버로 보낸다.
					''				// 그렇지 않은 경우에는 아무것도 보내지 않는다.
				) + '"') + '\n' +
				'\t\t\t' + ')'
			);
		}
//		alert(oTargetScene.join(',') + '\n');
		PHPEvalCode += oTargetScene.join(',') + '\n';
		PHPEvalCode += '\t\t' + '),' + '\n';

		// 인물/배경에 대한 정보
/*
			// 인물
			array(
				new TargetChrBg(
					0,		// 작품을 새로 추가하므로 이 값은 사용되지 않는다.
					0,		// 마찬가지로 사용되지 않는다.
					'인물 #0',
					'인물 #0: ...',
					array(array(), array(0))
				),
			// 배경
			array(
				new TargetChrBg(
					0,		// 작품을 새로 추가하므로 이 값은 사용되지 않는다.
					0,		// 마찬가지로 사용되지 않는다.
					'배경 #0',
					'배경 #0: ...',
					array(array(), array(0))
				),
 */
		var	aChrBg = {
			Chr: "인물",
			Bg: "배경"
		};	// 오브젝트
		PHPEvalCode += '\n';
		for (ChrBg in aChrBg) {
			PHPEvalCode += '\t\t' + '// ' + aChrBg[ChrBg] + '\n';
			PHPEvalCode += '\t\t' + 'array(';
			eval('var oTargetChrBg = []');
			eval('var aObj = oShow.a' + ChrBg);
			for (var i = 0; i < aObj.length; ++i) {
				var	o = aObj[i];
				// 인물/배경이 참조하는 장면에 대한 정보를 모은다.
				// 값이 어떤 것을 의미하는 지는 fnSetScenePseudoId함수를 보라.
				var	aScnPseudoId = [];
				fnSetScenePseudoId(oShow, o, aScnPseudoId);
//				alert(o.name + ': (' + o._aScnId.sort().join(', ') + ') -> (' + aScnPseudoId.sort().join(', ') + ')');
				var	flag = o.id == 0 ? 0 :			// 새로 장면을 추가하면 무조건 0이다.
					(o.flag & cOFLAG_DELETED) |		// 삭제되었는가?
										// 삭제되었다면 다른 비트는 참고할 필요가 없다.
					(o._aScnId.sort().join(', ') == aScnPseudoId.sort().join(', ') ? 0 : cOFLAG_SCN_REF) |										// 참조하는 장면에 변화가 있는가?
					(o.name == o._name ? 0 : cOFLAG_NAME) |	// 이름이 변경되었는가?
					(o.desc == o._desc ? 0 : cOFLAG_DESC)	// 설명이 변경되었는가?
				;
				oTargetChrBg.push(
					'\n' +
					'\t\t\t' + 'new TargetChrBg(' + '\n' +
					'\t\t\t\t' + (o.id) + ',' + '\n' +
					'\t\t\t\t' + (flag) + ',' + '\n' +
					'\t\t\t\t' + ("'" + (
						o.id == 0 ||			// 새로 만들거나
						!(flag & cOFLAG_DELETED) &&	// 삭제되지 않고,
						flag & cOFLAG_NAME ?		// 인물/배경의 이름이 바뀐 경우에만
						o.name :			// 인물/배경의 이름을 서버로 보낸다.
						''				// 그렇지 않은 경우에는 아무것도 보내지 않는다.
					) + "'") + ',' + '\n' +
					// 인물/배경에 대한 설명
					// (정책) 일반적으로 여러 줄이므로 (')대신 (")로 문자열을 묶는다.
					// 따라서 시나리오 중에서 (")와 (\)는 각각 (\"), (\\)로 변환해야
					// 문자열을 둘러싸는 (")와 혼동되지 않는다.
					'\t\t\t\t' + ('"' + (
						o.id == 0 ||			// 새로 만들거나
						!(flag & cOFLAG_DELETED) &&	// 삭제되지 않고,
						flag & cOFLAG_DESC ?		// 인물/배경에 대한 설명이 바뀐 경우에만
						o.desc.replace(/([\\"])/g, "\\$1") :	// 설명을 서버로 보낸다.
						''				// 그렇지 않은 경우에는 아무것도 보내지 않는다.
					) + '"') + ',' + '\n' +
//					'\t\t\t\t' + ('array(' + o.aScnSerial.sort().join(', ') + ')') + '\n' +
					'\t\t\t\t' + ('array(' + (
						o.id == 0 ||			// 새로 만들거나
						!(flag & cOFLAG_DELETED) &&	// 삭제되지 않고,
						flag & cOFLAG_SCN_REF ?		// 참조하는 장면이 바뀐 경우에만
						'array(' + o._aScnId.join(', ') + '), array(' + aScnPseudoId.join(', ') + ')' :										// 장면에 관한 "의미있는" 정보를 보내고,
						'array(), array()'		// 그렇지 않은 경우에는 "의미없는" 정보를 보낸다.
					) + ')') + '\n' +
					'\t\t\t' + ')'
				);
			}
//			alert(oTargetChrBg.join(',') + '\n');
			PHPEvalCode += oTargetChrBg.join(',') + '\n';
			PHPEvalCode += '\t\t' + ')' + (ChrBg == "Chr" ? "," : "") + '\n';
		}

		PHPEvalCode += '\t' + ')';

		aPHPEvalCode.push(PHPEvalCode);
	}

		return '$aShow = array(' + aPHPEvalCode.join(',') + "\n" + ');' + "\n";
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/10/29 (EST) - 2005/10/31 (EST)
 *		By Gwang-Ho Kim (iccentric@gmail.com)
 *
 * 데이터들을 모아 이를 처리하는 페이지를 호출한다.
 */

	function fnSubmit() {
//		var	oFORM = document.createElement('<FORM METHOD="POST" ACTION="fmShow.PHP" ENCTYPE="MULTIPART/FORM-DATA">');
		// 데이터를 서버로 보내는 데 필요한 FORM을 만든다.
		var	oFORM = document.createElement('<FORM METHOD="POST" ACTION="fmShow.PHP" ENCTYPE="APPLICATION/X-WWW-FORM-URLENCODED">');
		document.appendChild(oFORM);

		// .PHP에서 받아서 eval할 PHP코드를 포함하는 데이터를 만드는 데 필요한 TEXTAREA를 만든다.
		// 일반적으로 PHP코드는 여러 줄에 걸쳐있으므로 TEXTAREAD를 사용한다.
		var	oINPUT = document.createElement('<TEXTAREA NAME="PHP_EVAL_CODE">');
		var	oVal = '';
/* 연습용 */
/*
		oVal += '$a = array(' + '\n';
		oVal += '\t' + '"' + (
			// 다음은 데이터가 여러 줄에 걸쳐 있는 경우이다.
			'~!\\@#\'$%^&*()_+-=|;:/.,<>?' + '\n' +
			'강아지를 아세요?' + '\n'
		) + '",' + '\n';
		var	msg = '"저는 늘 \\집에만 있었어요."라고 그는 말했다. 나는 속으로 말한다. \'저두요...\'라고.';
		// 문자열을 큰 따옴표(")로 묶을 것이므로,
		// 큰 따옴표 자체는 (\")로 변환해야 한다.
		// 특수문자인 (\)는 그 전에 (\\)로 변환해야 한다.
//		alert(msg);
		oVal += '\t' + '"' + (
			// 다음은 데이터가 한 줄에 있는 경우이다. ('\n'은 포함되지 않았다)
//			'iccentric@gmail.com'
//			msg.replace(/\\/g, '\\\\').replace(/"/g, '\\\"')	// (\) -> (\\), (") -> (\")
			msg.replace(new RegExp('\\\\', 'g'), '\\\\').replace(new RegExp('"', 'g'), '\\"')
		) + '"' + '\n';
		oVal += ');' + '\n';
 */
		oVal = fnMakePHPEvalCode();
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
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/10/29 (EST) - 2005/10/30
 *		By Gwang-Ho Kim (iccentric@gmail.com)
 *
 * objTarget오브젝트에, 이 오브젝트가 테이블에 몇 번째 행에 표시되었는 지를 뜻하는,
 * rowIndex라는 property를 추가한다.
 * 이 property를 사용하면 rowIndex와 elementIndex사이의 참조의 관리가 편해진다.
 */
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/10/27 (EST) - 2005/10/28 (EST)
 *		By Gwang-Ho Kim (iccentric@gmail.com)
 *
 * 장면의 일련번호에 변화가 있다면,
 * 인물/배경에서 해당 장면의 참조도 적절하게 변화해야 한다.
 */

	// 배열의 몇 번째 성분이 테이블의 rowIndex행에 나타나는가 알아본다.
	function fnFindElement(aObj, rowIndex) {
		for (var i = 0; i < aObj.length; ++i) {
			if (aObj[i].flag & cOFLAG_DELETED) {
				// 지워졌다고 표시되는 성분은 테이블에 나타나지 않는다.
			} else {
				if (aObj[i].rowIndex == rowIndex) {
					// 이 i번째 성분이 테이블의 rowIndex행에 나타났다.
					return i;
				}
			}
		}
		// 테이블의 rowIndex행에 해당하는 배열의 성분을 찾지 못했다.
		return -1;
	}
	// 장면이 지워지려고 한다.
	// 이 장면을 참조하는 곳이 있는 지,
	// 있다면 지워도 되는 지 알아본다.
	function fnDeleteSceneIsOk(iScnElement) {
		var	aChrBg = {
			Chr: "인물",
			Bg: "배경"
		};	// 오브젝트
		// 인물/배경 중에서 해당 장면을 참조하는 인물/배경이 있는 지 알아본다.
		var	aCiteList = [];
		var	aUniqueCiteList = [];
		for (ChrBg in aChrBg) {
			eval('var aObj = gShow.a' + ChrBg);
			for (var i = 0; i < aObj.length; ++i) {
				if (aObj[i].flag & cOFLAG_DELETED) {	// 이미 없어진 오브젝트는
					// 건너뛴다.
					continue;
				}

				var	index = fnExistElement(aObj[i].aScnSerial, iScnElement);
				if (index == -1) {	//해당 장면을 참조하지 않는다.
					continue;
				}

				var	cite = aChrBg[ChrBg] + " #" + i + "(" + aObj[i].name + ")";
				if (aObj[i].aScnSerial.length == 1)
					aUniqueCiteList.push(cite);
				else
					aCiteList.push(cite);
			}
		}

		if (aCiteList.length + aUniqueCiteList.length == 0) {	// 이 장면을 참조하는 인물/배경이 없다.
			return true;
		}

		var	msg = '';
		msg += '>>>> 장면 #' + iScnElement + '(' + gShow.aScn[iScnElement].name + ')이 지워지려고 합니다 <<<<' + '\n';
		if (aCiteList.length) {
			msg += '\n';
			msg += '다음 인물/배경' + (aCiteList.length == 1 ? '' : '들') + '이 이 장면을 참조하고 있습니다:' + '\n';
			msg += '\n';
			msg += '\t' + aCiteList.join('\n' + '\t') + '\n';
		}
		msg += '\n';
		msg += '지워도 되겠습니까?' + '\n';
		msg += '"예"를 선택하면 이 장면에 대한 참조가 없어집니다.';
		if (aUniqueCiteList.length) {	// 지워지는 장면이 참조하는 유일한 장면이라면
			msg += '\n';
			msg += '\n';
			msg += '**************** 중요 ****************' + '\n';
			msg += '이 장면은 다음 인물/배경' + (aUniqueCiteList.length == 1 ? '' : '들') + '이 참조하는 유일한 장면입니다:' + '\n';
			msg += '\n';
			msg += '\t' + aUniqueCiteList.join('\n' + '\t') + '\n';
			msg += '\n';
			msg += '"예"를 선택했다면 다음에 위의 각 인물/배경에 대해서' + '\n';
			msg += '        ' + '1. 해당 인물/배경' + (aUniqueCiteList.length == 1 ? '' : '들') + '도 지울 것인지 아니면' + '\n';
			msg += '        ' + '2. 해당 인물/배경' + (aUniqueCiteList.length == 1 ? '' : '들') + '이 등장하는 장면을 작품 전체로 할 것인지' + '\n';
			msg += '다시 묻을 것입니다.' + '\n';
			msg += '이러한 것이 원하는 것이 아니려면 "아니오"를 선택하세요.' + '\n';
			msg += '**************************************';
		}
		return confirm(msg);
	}
	function fnUpdateSceneList() {
		var	aChrBg = {
			Chr: "인물",
			Bg: "배경"
		};	// 오브젝트
		// 인물/배경 중에서 해당 장면을 참조하는 인물/배경이 있는 지 알아본다.
		for (ChrBg in aChrBg) {
			eval('var aObj = gShow.a' + ChrBg);
			for (var i = 0; i < aObj.length; ++i) {
				if (aObj[i].flag & cOFLAG_DELETED) {	// 이미 없어진 오브젝트는
					// 건너뛴다.
					continue;
				}

				// 장면에 목록을 새로 만든다.
				fnAddOptionsToSelect(ChrBg == "Chr" ? cTARGET_CHR : cTARGET_BG, aObj[i]);
			}
		}
	}
	function fnChangeSceneReference(iFromElement, iToElement) {
		var	aChrBg = {
			Chr: "인물",
			Bg: "배경"
		};	// 오브젝트
		// 인물/배경 중에서 해당 장면을 참조하는 인물/배경이 있는 지 알아본다.
		for (ChrBg in aChrBg) {
			eval('var aObj = gShow.a' + ChrBg);
			for (var i = 0; i < aObj.length; ++i) {
				if (aObj[i].flag & cOFLAG_DELETED) {	// 이미 없어진 오브젝트는
					// 건너뛴다.
					continue;
				}

				var	index = fnExistElement(aObj[i].aScnSerial, iFromElement);
				if (index != -1) {	// iFromElement에 대한 참조가 있다.
					aObj[i].aScnSerial.splice(index, 1, iToElement);
				}
			}
		}
	}
	function fnDeleteSceneReference(iScnElement) {
		var	aChrBg = {
			Chr: "인물",
			Bg: "배경"
		};	// 오브젝트
		// 인물/배경 중에서 해당 장면을 참조하는 인물/배경이 있는 지 알아본다.
		for (ChrBg in aChrBg) {
			eval('var aObj = gShow.a' + ChrBg);
			for (var i = aObj.length - 1; i >= 0; --i) {
				if (aObj[i].flag & cOFLAG_DELETED) {	// 이미 없어진 오브젝트는
					// 건너뛴다.
					continue;
				}

				var	index = fnExistElement(aObj[i].aScnSerial, iScnElement);
				if (index == -1) {	//해당 장면을 참조하지 않는다.
					// 다음 인물/배경에 대해서 반복한다.
					continue;
				}

				if (aObj[i].aScnSerial.length != 1) {	// 장면에 대한 참조가 둘 이상일 때에는,
					// fnDeleteSceneIsOk에서 말했듯이,
					// 단지 현재 장면에 대한 참조만을 지운다.
					aObj[i].aScnSerial.splice(index, 1);

					// 다음 인물/배경에 대해서 반복한다.
					continue;
				}

				// 해당 장면이 참조되는 유일한 장면이다.
				var	msg = '';
				msg += '장면(' + gShow.aScn[iScnElement].name + ')은 ' + aChrBg[ChrBg] + '(' + aObj[i].name + ')이 참조하는 유일한 장면입니다.' + '\n';
				msg += '장면을 지우면 이 참조가 없어집니다.' + '\n';
				msg += aChrBg[ChrBg] + '도 함께 지우려면 "예를 선택하세요.' + '\n';
				msg += '또는 작품 전체에 등장하게 하려면 "아니오"를 선택하세요.';
				if (!confirm(msg)) {	// 작품 전체에 등장하게 한다.
					aObj[i].aScnSerial[0] = -1;

					// 다음 인물/배경에 대해서 반복한다.
					continue;
				}

				// 이 장면을 참조하는 인물/배경도 지운다.
				// 테이블에서 해당 인물/배경을 지운다.
				var	iNewRow = _fnAction(ChrBg == "Chr" ? cTARGET_CHR : cTARGET_BG, aObj[i].rowIndex, cACTION_DELETE);
				eval('var oTABLE = tbl' + ChrBg);
				// 해당하는 인덱스의 RADIO버튼을 CHECKED로 만든다.
				if (0 <= iNewRow && iNewRow < oTABLE.rows.length)
					oTABLE.rows[iNewRow].children[0].children[0].checked = true;

				if (oTABLE.rows.length == 0) {	// 행이 하나도 없다.
					eval('btn' + ChrBg + '.disabled = true');
					eval('giFlag &= ~cTABFLAG_' + ChrBg.toUpperCase());	// 강제로 해당 플래그를 해제한다.
				} else {
					eval('btn' + ChrBg + '.disabled = false');
				}
				fnDisplayButtons(ChrBg == "Chr" ? cTARGET_CHR : cTARGET_BG);
			}
		}
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/10/27 (EST)
 *		By Gwang-Ho Kim (iccentric@gmail.com)
 *
 * 입력값에 변화가 있을 때, 적절하게 해당 변수를 수정한다.
 */

	function fnRadioOnClick(target, o) {
		eval('gi' + (target == cTARGET_SCN ? "Scn" : target == cTARGET_CHR ? "Chr" : "Bg") + ' = o.parentElement.parentElement.rowIndex');

		fnDisplayButtons(target);
	}

	function fnValueOnChange(o) {
//		alert('id = ' + o.id);
//		alert('nodeName = ' + o.nodeName);
//		alert('value = ' + o.value);
		switch (o.id) {
		case "txtName":
			// (정책) 작품명에 특수문자(", ', \)는 허용되지 않는다.
			if (o.value.search(/["'\\]/) != -1) {
				alert('>' + o.value + '<: 작품명에 큭수문자(", \', \\)는 허용되지 않습니다');
				o.value = gShow.name;
				return;
			}
			gShow.name = o.value;
			break;
		case "txtDateBegin":
			var	val = o.value.replace(/-/g, '/');
			if (isNaN(new Date(val).valueOf())) {
				alert('>' + o.value + '<: 올바른 날짜 표현이 아닙니다');
			} else {
				gShow.date_begin = new Date(val);
			}
			// 날짜의 형태를 yyyy-mm-dd로 바꾼다.
			o.value = fnMakeDateString(gShow.date_begin);
			break;
		case "txtDateEnd":
			var	val = o.value.replace(/-/g, '/');
			if (isNaN(new Date(val).valueOf())) {
				alert('>' + o.value + '<: 올바른 날짜 표현이 아닙니다');
			} else {
				gShow.date_end = new Date(val);
			}
			// 날짜의 형태를 yyyy-mm-dd로 바꾼다.
			o.value = fnMakeDateString(gShow.date_end);
			break;
		case "txtaDescription":
			gShow.desc = o.value;
			break;
		default:
			break;
		}
	}

/*
		innerHTML += '<TABLE WIDTH="100%" BORDER="1" FRAME="LHS" RULER="ROWS" CELLSPACING="0" CELLPADDING="0"><TBODY>';
		innerHTML += '<TR>';
		innerHTML += '<TD ALIGN="RIGHT" WIDTH="10%">이름</TD>';
		var	tagName = target == cTARGET_CHR ? 'CHARACTER_NAME' : target == cTARGET_BG ? 'BACKGROUND_NAME' : 'SCENE_NAME';
		var	tagVal = o == undefined ? '' : o.name;
		innerHTML += '<TD ALIGN="LEFT" WIDTH="90%"><INPUT TYPE="TEXT" SIZE="32" MAXLENGTH="32" VALUE="' + tagVal + '" onChange="fnTargetValueOnChange(' + target + ', this);"></TD>';
		innerHTML += '</TR>';
		if (target != cTARGET_SCN) {	// 장면을 선택하도록 한다.
			innerHTML += '<TR>';
			innerHTML += '<TD ALIGN="RIGHT">장면</TD>';
			innerHTML += '<TD><SELECT MULTIPLE SIZE="4" STYLE="width: 400px;" onChange="fnTargetValueOnChange(' + target + ', this);">';
			innerHTML += '<OPTION' + (o != undefined && fnExistElement(o.aScnSerial, -1) != -1 ? ' SELECTED' : '') + '>작품 전체에 쓰인다.</OPTION>';
			var	aScn = gShow.aScn;
			for (var i = 0; i < aScn.length; ++i) {
				innerHTML += '<OPTION' + (o != undefined && fnExistElement(o.aScnSerial, i) != -1 ? ' SELECTED' : '') + '>특정 장면(' + aScn[i].name + ')에서만 쓰인다.</OPTION>';
			}
			innerHTML += '</SELECT></TD></TR>';
		}
		innerHTML += '<TR>';
		innerHTML += '<TD ALIGN="RIGHT" VALIGN="TOP">설명</TD>';
		tagName = target == cTARGET_CHR ? 'CHARACTER_DESCRIPTION' : target == cTARGET_BG ? 'BACKGROUND_DESCRIPTION' : 'SCENE_DESCRIPTION';
		tagVal = o == undefined ? '' : o.desc;
		innerHTML += '<TD ALIGN="LEFT"><TEXTAREA ROWS="5" COLS="72" onChange="fnTargetValueOnChange(' + target + ', this);">' + tagVal + '</TEXTAREA></TD>';
		innerHTML += '</TR>';
		innerHTML += '</TBODY></TABLE>';
 */
	// _fnAddTR에서 만드는 위와 같은 행에 대한 내용과 주어지는 target을 이용해서
	// 변화가 일어난 곳을 찾아내서 적절하게 데이터를 수정한다.
	function fnTargetValueOnChange(target, oInputTextareaSelect) {
		// 장면, 인물/배경 중 어디에서 변화가 일어났는 지 알아낸다.
		var	targetName = target == cTARGET_SCN ? "Scn" : target == cTARGET_CHR ? "Chr" : "Bg";
//		alert('target = ' + (target == cTARGET_SCN ? '장면' : target == cTARGET_CHR ? '인물' : '배경'));

		// 변화가 일어난 행을 알아낸다.
		//        INPUT            TD            TR         TBODY         TABLE            TD            TR
		//     TEXTAREA            TD            TR         TBODY         TABLE            TD            TR
		//       SELECT            TD            TR         TBODY         TABLE            TD            TR
		var	oTR = oInputTextareaSelect.parentElement.parentElement.parentElement.parentElement.parentElement.parentElement;
		if (oTR.nodeName !== "TR") {
			alert('oTR.nodeName != "TR"');
			return;
		}
		var	iRow = oTR.rowIndex;
//		alert('iRow = ' + iRow);

		// 어디에서 변화가 일어났는 지 알아낸다.
		var	nodeName = oInputTextareaSelect.nodeName;
		// nodeName인 "INPUT"이면 이름에서, "TEXTAREA"이면 설명에서 변화가 있음을 뜻한다.
		// 인물/배경의 경우에 "SELECT"라면 장면에서 변화가 있음을 뜻한다.
//		alert('nodeName = ' + nodeName);
		var	propertyName = nodeName == "INPUT" ? "name" : nodeName == "TEXTAREA" ? "desc" : "aScnSerial";

		eval('var o = gShow.a' + targetName);
		// iRow번째 행은 오브젝트 배열의 몇 번째 성분을 표시한 것인지 알아낸다.
		var	index = fnFindElement(o, iRow);
		if (index == -1) {
			alert('테이블의 ' + iRow + '행에 해당하는 배열의 성분을 찾지 못했다');
			return;
		}
		var	object = o[index];
		eval('var propertyValue = object.' + propertyName);
//		alert('propertyValue = ' + propertyValue);

		// 변화된 값은...
		var	value;
		if (oInputTextareaSelect.nodeName === "SELECT") {
			value = new Array();
			var	op = oInputTextareaSelect.options;
			// 2005/11/5 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
			// 작품 전체에서 사용되는 것과 특정 장면에서만 사용되는 것은 동시에 이루어질 수 없다.
			var	bAllTheScenes = false;
			for (var i = 0; i < op.length; ++i) {
				if (op[i].selected) {
					if (i == 0) {	// 작품 전체에 등장한다.
						bAllTheScenes = true;
						value.push(-1);
					} else {	// 특정 장면에서만 사용된다.
						if (bAllTheScenes == true) {
							// 동시에 두 종류를 선택할 수 없다.
							alert('작품 전체와 특정 장면을 동시에 선택할 수 없습니다');
							// 원래대로 한다.
							for (var k = 0; k < op.length; ++k)
								op[k].selected = fnExistElement(object.aScnSerial, k - 1) != -1;
							return;
						}
						value.push(i - 1);
					}
				}
			}
			object.aScnSerial = [];	// 장면에 대한 참조를 새로 설정한다.
			for (i = 0; i < value.length; ++i) {
				if (value[0] == -1) {	// 작품 전체에서 사용된다.
					object.aScnSerial.push(-1);
					break;
				}
				// 특정 장면에서 사용된다.
				// 테이블의 value[i]행에 해당하는 장면을 찾는다.
				index = fnFindElement(gShow.aScn, value[i]);
				if (index == -1) {
					alert('버그: fnTargetValueOnChange: ' + value[i] + '행에 해당하는 장면을 찾지 못했다.');
					return;
				}
				object.aScnSerial.push(index);
			}
		} else if (oInputTextareaSelect.nodeName == "TEXTAREA") {
			value = oInputTextareaSelect.value;
			object.desc = value;
		} else {	// "INPUT"
			value = oInputTextareaSelect.value;
			if (value == '') {
				alert('이름을 정해야합니다');
				oInputTextareaSelect.value = object.name;
				oInputTextareaSelect.select();
				return;
			}
			// (정책) 이름에 특수문자(", ', \)는 허용되지 않는다.
			if (value.search(/["'\\]/) != -1) {
				alert('>' + value + '<: 이름에 큭수문자(", \', \\)는 허용되지 않습니다');
				oInputTextareaSelect.value = object.name;
				oInputTextareaSelect.select();
				return;
			}
			// 2005/11/5 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
			// 이름이 같을 수는 없다.
			for (var i = 0; i < o.length; ++i) {
				if (i != index && o[i].name == value) {
					alert('같은 이름("' + value + '")을 사용하는 행이 있습니다. 다른 이름을 사용하세요.');
					oInputTextareaSelect.value = object.name;
					oInputTextareaSelect.select();
					return;
				}
			}
			object.name = value;

			if (target == cTARGET_SCN) {	// 장면의 이름이 바뀌었다면 인물/배경에서 참조하는 장면의 이름도 바꾸어야 한다.
				// 인물과 배경에서 참조하는 장면에 대한 목록을 다시 쓴다.
				fnUpdateSceneList();
			}
		}
//		alert('value = ' + value);
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/10/27 (EST)
 *		By Gwang-Ho Kim (iccentric@gmail.com)
 *
 * 쿠키로 필요한 변수를 저장한다.
 */

	var	giFlag;		// 장면, 인물/배경 중에서 어느 테이블이 활성화되였는 지를 말해준다.
				// 해당 플래그가 "설정"되었다면 그 테이블을 닫는다.
				// (정책) 기본값은 여는 것이다.
	var	cTABFLAG_SCN	= 1 << 0;
	var	cTABFLAG_CHR	= 1 << 1;
	var	cTABFLAG_BG	= 1 << 2;
	var	gaVarName = new Array(
		'giShow',
		'giScn',
		'giChr',
		'giBg',
		'giFlag'
	);
	function fnSetCookie() {
		var	aVarVal = new Array(gaVarName.length);
		var	cookie = '';
		for (var i = 0; i < gaVarName.length; ++i) {
			aVarVal[i] = eval(gaVarName[i]);
		}
		document.cookie = 'Show.Var=' + aVarVal;
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
			if (aNameVal[0] == "Show.Var") {
				var	aVarVal = aNameVal[1].split(",");
				for (var k = 0; k < gaVarName.length; ++k) {
					eval(gaVarName[k] + ' = ' + aVarVal[k]);
				}
			}
			if (aNameVal[0] == 'Storyboard.gindexCutRev') {
				gindexCutRev = unescape(aNameVal[1]).split(",");
			}
		}
	}

	function fnSetVar() {
		// giShow: number
		giShow = parseInt(giShow);
		if (
			isNaN(giShow) ||			// 정의되지 않았거나 숫자가 아닌 경우
			(giShow < 0 || gaShow.length <= giShow)	// 유효하지 않은 경우
		) {
			// (정책) 기본값은 마지막 작품이다.
			// 작품이 하나도 없는 경우 -1이다.
			giShow = gaShow.length - 1;
		}
	}
	function fnOnLoad() {
		fnGetCookie();

		fnSetData();

		// 설정된 전역변수가 없다면 전역변수를 정책에 따른 기본값으로 설정하고,
		// 있다면 설정된 데이터를 바탕으로 다시 확인한다.
		fnSetVar();

		// 2005/11/15 (EST)
		//		By 김광호(Gwang-Ho Kim, iccentric@gmail.com)
		// 각 작품이 작품목록의 몇 번째 행에 위치할 것인지를 설정한다.
		for (var i = 0; i < gaShow.length; ++i) {
			gaShow[i].rowIndex = i;
		}
		// 작품의 목록을 표시한다.
		if (!fnMakeShowListTable()) {
			alert('fnListShow(): Failure');
			return false;
		}
		// giShow에 해당하는 작품의 버튼의 상태를 CHECKED로 한다.
		if (giShow != -1) {	// 작품이 있는 경우에만 이렇게 한다.
			tblShowList.rows[1 + giShow].children[0].children[0].checked = true;
			btnSubmit.disabled = true;		// "반영하기"버튼을 비화성화한다.
		} else {
			btnUpdateShow.disabled = true;		// "고치기"버튼을 비활성화한다.
		}

		// giShow번째 작품으로 필요한 것들을 설정한다.
		if (!fnInit()) {
			alert('fnInit(): Failure');
		}
	}

	function fnOnUnload() {
		fnSetCookie();
	}
	window.onload = fnOnLoad;
	window.onunload = fnOnUnload;
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/10/25 (EST) - 2005/10/26 (EST)
 *		By Gwang-Ho Kim (iccentric@gmail.com)
 *
 * UI를 새로 작성했다.
 */
	var	cTARGET_SCN	= 0;
	var	cTARGET_CHR	= <?PHP echo TARGET_CHR ?>;
	var	cTARGET_BG	= <?PHP echo TARGET_BG ?>;

	var	cACTION_MOVEUP	= 0;
	var	cACTION_INSERT	= 1;
	var	cACTION_DELETE	= 2;
	var	cACTION_MOVEDN	= 3;

	function _fnTableDisplay(target, bOpen) {
		var	targetName = target == cTARGET_SCN ? "Scn" : target == cTARGET_CHR ? "Chr" : "Bg";
		eval('var oTABLE = tbl' + targetName);
		for (var i = 0; i < oTABLE.rows.length; ++i) {
			var	oTR = oTABLE.rows[i];
			oTR.style.display = bOpen ? "block" : "none";
		}
	}
	function fnToggleTableDisplay(target) {
		var	targetNameUpperCase = target == cTARGET_SCN ? "SCN" : target == cTARGET_CHR ? "CHR" : "BG";

		eval('giFlag ^= cTABFLAG_' + targetNameUpperCase);
		eval('var bOpened = giFlag & cTABFLAG_' + targetNameUpperCase);
		_fnTableDisplay(target, !bOpened);
		fnDisplayButtons(target);
	}
	function fnDisplayButtons(target) {
		var	targetName = target == cTARGET_SCN ? "Scn" : target == cTARGET_CHR ? "Chr" : "Bg";
		eval('var oTABLE = tbl' + targetName);

		if (oTABLE.rows.length == 0) {	// INSERT버튼만 제외하고 모든 버튼을 없앤다.
			eval('tr' + targetName + 'Move.style.display = "none"');
			eval('tr' + targetName + 'Delete.style.display = "none"');
		} else {
			eval('tr' + targetName + 'Move.style.display = "block"');
			eval('tr' + targetName + 'Delete.style.display = "block"');
		}

		eval('var bOpened = giFlag & cTABFLAG_' + targetName.toUpperCase());
		if (bOpened) {
			eval('btn' + targetName + 'MoveUp.disabled = true');
			eval('btn' + targetName + 'MoveDown.disabled = true');
			eval('btn' + targetName + 'Insert.disabled = true');
			eval('btn' + targetName + 'Delete.disabled = true');
		} else {
			eval('btn' + targetName + 'MoveUp.disabled = false');
			eval('btn' + targetName + 'MoveDown.disabled = false');
			eval('btn' + targetName + 'Insert.disabled = false');
			eval('btn' + targetName + 'Delete.disabled = false');
		}

		eval('var bDisabled = bOpened || gi' + targetName + ' == 0');
		eval('btn' + targetName + 'MoveUp.disabled = ' + bDisabled);
		eval('bDisabled = bOpened || gi' + targetName + ' == tbl' + targetName + '.rows.length - 1');
		eval('btn' + targetName + 'MoveDown.disabled = ' + bDisabled);
	}

	// target에 해당하는 테이블에서, 선택된 행에 action을 가한다.
	// 어떤 행이 선택되었는 지는 giScn, giChr/giBg값을 통해 알 수 있다.
	// action에 따라 선택된 행이 달라 질 수 있다.
	// 이 경우에 해당하는 값(giScn, giChr/giBg)을 바꾸어 준다.
	// 마지막으로 action을 가하는 버튼에 적절한 변화를 준다.
	function fnBtnAction(target, action) {
		var	targetName = target == cTARGET_SCN ? "Scn" : target == cTARGET_CHR ? "Chr" : "Bg";

		eval('var oTbl = tbl' + targetName);		// action이 가해지는 테이블 오브젝트
		// action이 가해지는 행을 알아낸다.
		var	bInvalid = false;
		eval('bInvalid |= typeof(gi' + targetName + ') == "undefined"');
		eval('bInvalid |= gi' + targetName + ' == undefined');
		eval('bInvalid |= gi' + targetName + ' < 0 || oTbl.rows.length <= gi' + targetName);
		eval('var iRow = bInvalid ? oTbl.rows.length - 1 : gi' + targetName);
//		alert('iRow = ' + iRow);
		// target에 해당하는 테이블의 iRow번째 행에 action을 가한다.
		// action에 따라 선택된 행을 말해주는 iRow값이 달라질 수 있다.
		var	iNewRow = _fnAction(target, iRow, action);
		if (iNewRow == undefined) {
//			alert('_fnAction(): Failure');
			return;
		}

		// 해당하는 변수를 새로 설정한다.
		eval('gi' + targetName + ' = iNewRow');
		// 해당하는 RADIO버튼을 CHECKED로 만든다.
		if (0 <= iNewRow && iNewRow < oTbl.rows.length)
			oTbl.rows[iNewRow].children[0].children[0].checked = true;

		// 버튼에 적절한 변화를 준다.
		if (oTbl.rows.length == 0) {	// 행이 하나도 없다.
			eval('btn' + targetName + '.disabled = true');
			eval('giFlag &= ~cTABFLAG_' + targetName.toUpperCase());	// 강제로 해당 플래그를 해제한다.
		} else {
			eval('btn' + targetName + '.disabled = false');
		}
		fnDisplayButtons(target);
	}

	// target에 해당하는 테이블의 rowIndex번째 행에 action을 가한다.
	// 대부분의 경우 rowIndex는 선택된 행을 말해준다.
	// action에 따라 선택된 행을 말해주는 rowIndex값이 달라질 수 있다.
	// 이 값을 리턴해서 이 함수를 호출할 함수가 이 값을 이용해서 적절한 조치를 취할 수 있도록 한다.
	function _fnAction(target, rowIndex, action) {
		var	targetName = target == cTARGET_SCN ? "Scn" : target == cTARGET_CHR ? "Chr" : "Bg";
		eval('var oTbl = tbl' + targetName);	// action이 가해지는 테이블 오브젝트
		eval('var aObj = gShow.a' + targetName);	// 테이블에 행을 표시할 때 참고하는 오브젝트 배열
		// rowIndex번째 행은 오브젝트 배열의 몇 번째 성분을 표시한 것인지 알아낸다.
		var	index = fnFindElement(aObj, rowIndex);
//		alert('_fnAction: rowIndex = ' + rowIndex + ', index = ' + index);

		// rowIndex행을 현재 선택한 상태라면,
		// action이 가해진 후 선택된 행에 변화가 일어날 수 있다.
		// 다음 변수 iRow는 변화된 행을 말해준다.
		var	iNewRow;
		switch (action) {
		case cACTION_MOVEUP:
			if (rowIndex == 0) {
				// -1이면 제일 뒤로 간다.
				alert('Cannot move UP');
				return;
			}

			iNewRow = rowIndex - 1;		// 선택된 행에도 변화가 있어야 한다면 이 값을 이용한다.
			// 두 행을 교환한다.
			oTbl.moveRow(rowIndex, iNewRow);
			// 두 행(rowIndex, iNewRow)에 해당하는 오브젝트들의, 행에 대한 참조도 바꾼다.
			var	newIndex = fnFindElement(aObj, iNewRow);
			aObj[index].rowIndex = iNewRow;
			aObj[newIndex].rowIndex = rowIndex;
			break;
		case cACTION_MOVEDN:
			if (rowIndex == oTbl.rows.length - 1) {
				alert('Cannot move DOWN');
				return;
			}

			iNewRow = rowIndex + 1;		// 선택된 행에도 변화가 있어야 한다면 이 값을 이용한다.
			// 두 행을 교환한다.
			oTbl.moveRow(rowIndex, iNewRow);
			// 두 행(rowIndex, iNewRow)에 해당하는 오브젝트들의, 행에 대한 참조도 바꾼다.
			var	newIndex = fnFindElement(aObj, iNewRow);
			aObj[index].rowIndex = iNewRow;
			aObj[newIndex].rowIndex = rowIndex;
			break;
		case cACTION_INSERT:
			// 새로운 오브젝트를 만든다.
			var	strEval = '';
			strEval += 'var o = new obj' + (target == cTARGET_SCN ? "Scene" : "ChrBg") + '(';
			// 이름(name) 속성이다.
			strEval += '"' + (target == cTARGET_SCN ? "장면" : target == cTARGET_CHR ? "인물" : "배경") + '의 이름을 정하세요", ';
			// 설명(desc) 속성이다.
			strEval += '"' + (target == cTARGET_SCN ? "장면" : target == cTARGET_CHR ? "인물" : "배경") + '에 대한 설명을 적으세요.", ';
			if (target == cTARGET_SCN) {	// 장면
				strEval += '-1, ';
			} else {	// 인물/배경
				// 인물/배경이 등장하는 장면을 지정해야 한다.
				strEval += '[-1], ';	// (정책) 작품 전체에 걸쳐 사용된다.
			}
			strEval += '0, ';		// 따라서 해당하는 id(장면: SCENE_ID, 인물/배경: TARGET_ID)가 없다.
			// 테이블의 rowIndex번째 행에 표시한다.
			strEval += rowIndex;
			strEval += ')';
			eval(strEval);

			// 새로 만든 오브젝트를 오브젝트 배열에 추가한다.
			aObj.splice(index, 0, o);
			if (target == cTARGET_SCN) {	// 장면이 추가된 경우에,
				// 인물/배경에서 장면들에 대한 참조도 바꾸어야 한다.
				// index번째에 새로운 장면이 추가되었으로,
				// 인물/배경의 장면에 대한 참조값이 index보다 크거나 같은 경우에,
				// 참조값을 하나씩 증가해야 한다.
				// 바로 앞에서 장면을 하나 추가했으므로,
				// 인물/배경에서 추가하기 전에 가질 수 있는 참조값의 최대값은 (장면의 갯수 - 2)이었다.
				// 2005/11/6 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
				// 처음으로 만든 장면의 경우 index값은 -1이다.
				// 이 경우에는 아무것도 하지 않아야 말아야 한다.
				for (var i = index; index >= 0 && i < gShow.aScn.length - 1; ++i) {
					fnChangeSceneReference(i, i + 1);
				}
			}

			// 만든 오브젝트를 테이블에 표시한다.
			_fnAddTR(target, o);
			// 테이블에 행이 추가되었으므로,
			for (var i = 0; i < aObj.length; ++i) {
				if (
					i != index &&				// 지금 추가한 행을 제외한 행들 중에서
					!(aObj[i].flag & cOFLAG_DELETED) &&	// 현재 살아있는 오브젝트에 한해서
					aObj[i].rowIndex >= rowIndex		// rowIndex번째 포함한 이후의
				) {
					++aObj[i].rowIndex;			// 행에 대한 참조를 하나씩 증가시킨다.
				}
			}

			// 선택된 행에도 변화가 있어야 한다면 다음 값을 이용한다.
			iNewRow = rowIndex + 1;
			break;
		case cACTION_DELETE:
			// 장면이 지워진다면,
			// 장면을 정말로 지워도 되는 지 확인한 후,
			// 이 장면을 참조하는 인물과 배경에서 이 장면에 대한 참조를 없앤다.
			if (target == cTARGET_SCN) {
				// 이 장면을 참조하는 인물/배경이 있는 지 알아본다.
				// 있다면 정말로 지워도 되는 지 다시 한 번 확인한다.
				if (!fnDeleteSceneIsOk(index)) {
					// 해당 장면을 지우고 싶지 않다.
					return;
				}

				// 인물과 배경에서 이 장면에 대한 참조를 지운다.
				// 유일한 참조라면 인물/배경도 함께 지울 것인지 아니면
				// 인물/배경이 작품 전체에 등장할 것인지를 묻는다.
				fnDeleteSceneReference(index);
			}
			// 오브젝트들의 배열에서 index번째 성분을 지운다.
			if (aObj[index].flag & cOFLAG_DB) {	// DB에서 만든 오브젝트라면
				// 지워졌다는 표시만 한다.
				aObj[index].flag ^= cOFLAG_DELETED;

				// 비록 장면이 지워지더라도 해당 오브젝트는 그래도 있으므로
				// 인물/배경에서 장면에 대한 참조값에는 변화가 없다.
			} else {				// 웹에서 만든 오브젝트라면
				// 정말로 지운다.
				aObj.splice(index, 1);

				if (target == cTARGET_SCN) {	// 장면이 지워진 경우
					// 인물/배경에서 장면들에 대한 참조도 바꾸어야 한다.

					// index번째 성분이 지워졌으므로,
					// 인물/배경의 장면에 대한 참조값이 index보다 큰 경우에는,
					// 참조값을 하나씩 감소해야 한다.
					// 바로 앞에서 장면을 하나 지웠으므로,
					// 인물/배경에서 삭제하기 전에 가질 수 있는 참조값의 최대값은 (장면의 갯수)이었다.
					for (var i = index + 1; i <= gShow.aScn.length; ++i) {
						fnChangeSceneReference(i, i - 1);
					}
				}
			}

			oTbl.deleteRow(rowIndex);
			// 테이블에서 행이 없어졌으므로
			for (var i = 0; i < aObj.length; ++i) {
				if (
					!(aObj[i].flag & cOFLAG_DELETED) &&	// 현재 살아있는 오브젝트에 한해서
					aObj[i].rowIndex > rowIndex		// rowIndex번째 이후의
				) {
					--aObj[i].rowIndex;			// 행에 대한 참조를 하나씩 감소시킨다.
				}
			}

			// 선택된 행에도 변화가 있어야 한다면 다음 값을 이용한다.
			iNewRow = rowIndex == oTbl.rows.length ? oTbl.rows.length - 1 : rowIndex;
			break;
		default:
			alert('Unknow action: ' + action);
			return;
		}
		if (target == cTARGET_SCN) {
			// 인물과 배경에서 참조하는 장면에 대한 목록을 다시 쓴다.
			fnUpdateSceneList();
		}

		return iNewRow;
	}

	// 인물/배경에서 참조하는 장면에 대한 목록은 다시 쓴다.
	// 해당 인물/배경에 대한 오브젝트는 o이다.
	function fnAddOptionsToSelect(target, o) {
		// 2005/11/6 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
		// 처음에 만든 인물/배경의 경우 rowIndex가 -1이다.
		// 이 경우는 0번째 행에 해당한다.
		eval('var oTR = ' + (target == cTARGET_CHR ? "tblChr" : "tblBg") + '.rows[' + (o.rowIndex == -1 ? 0 : o.rowIndex) + ']');
		//                             TD       TABLE       TBODY          TR          TD      SELECT
		var	oSELECT = oTR.children[1].children[0].children[0].children[1].children[1].children[0];
		// 초기화한다.
		oSELECT.options.length = 0;

		var	oOp = document.createElement("OPTION");
		oOp.text = "작품 전체에서 쓰인다";
		oOp.selected = o == undefined || fnExistElement(o.aScnSerial, -1) != -1;
		oSELECT.options.add(oOp);
		// 먼저 행에 대한 참조가 있는 오브젝트들로부터 행에 대한 참조를 얻는다.
		var	aScnRowIndex = [];
		for (var i = 0; i < gShow.aScn.length; ++i) {
			if (!(gShow.aScn[i].flag & cOFLAG_DELETED)) {	// 지워지지 않은 행에 대해서
				aScnRowIndex.push(gShow.aScn[i].rowIndex);
			}
		}
		// 정렬한다.
		aScnRowIndex.sort();
		for (var i = 0; i < aScnRowIndex.length; ++i) {
			// i번째 행을 표시하기 위해 몇 번째 성분이 사용되었는 지를 안다.
			var	index = fnFindElement(gShow.aScn, i);
			oOp = document.createElement("OPTION");
			oOp.text = "특정 장면(" + gShow.aScn[index].name + ")에서만 쓰인다";
			oOp.selected = o != undefined && fnExistElement(o.aScnSerial, index) != -1;
			oSELECT.options.add(oOp);
		}
	}
	function _fnAddTR(target, o) {
		var	oTABLE = target == cTARGET_SCN ? tblScn : target == cTARGET_CHR ? tblChr : tblBg;
/*
	<TR>
		<TD>...</TD>
		<TD WIDTH="100%">...</TD>
	</TR>
 */
		var	oTR = oTABLE.insertRow(o == undefined ? -1 : o.rowIndex);
		if (!oTR) {
			alert('.insertRow: Failure');
			return null;
		}
//		oTR.style.display = "block";

		var	oTD1 = oTR.insertCell();
		if (!oTD1) {
			alert('.insertCell: Failure');
			oTABLE.deleteRow(oTR.rowIndex);
			return null;
		}
		oTD1.align = "CENTER";
		oTD1.vAlign = "TOP";
		oTD1.innerHTML = '<INPUT TYPE="RADIO" NAME="rdo' + (target == cTARGET_SCN ? 'Scn' : target == cTARGET_CHR ? 'Chr' : 'Bg') + '" onClick="fnRadioOnClick(' + target + ', this);">';

		var	oTD2 = oTR.insertCell();
		if (!oTD2) {
			alert('.insertCell: Failure');
			oTABLE.deleteRow(oTR.rowIndex);
			return null;
		}
		oTD2.width = "100%";
		var	innerHTML;
/*
<TABLE WIDTH="100%" BORDER="1" CELLSPACING="0" CELLPADDING="0">
<TBODY>
<TR>
	<TD ALIGN="RIGHT" WIDTH="10%">이름</TD>
	<TD ALIGN="LEFT" WIDTH="90%"><INPUT TYPE="TEXT" SIZE="32" MAXLENGTH="32" NAME="BACKGROUND_NAME" VALUE=""></TD>
</TR>
<TR>
	<TD ALIGN="RIGHT" VALIGN="TOP" WIDTH="10%">설명</TD>
	<TD ALIGN="LEFT" WIDTH="90%"><TEXTAREA ROWS="5" COLS="72" NAME="BACKGROUND_DESCRIPTION"></TEXTAREA></TD>
</TR>
</TBODY>
</TABLE>
 */
		innerHTML = '';
		innerHTML += '<TABLE WIDTH="100%" BORDER="1" FRAME="LHS" RULER="ROWS" CELLSPACING="0" CELLPADDING="0"><TBODY>';
		innerHTML += '<TR>';
		innerHTML += '<TD ALIGN="RIGHT" WIDTH="10%">이름</TD>';
		var	tagName = target == cTARGET_CHR ? 'CHARACTER_NAME' : target == cTARGET_BG ? 'BACKGROUND_NAME' : 'SCENE_NAME';
		var	tagVal = o == undefined ? '' : o.name;
		innerHTML += '<TD ALIGN="LEFT" WIDTH="90%"><INPUT TYPE="TEXT" SIZE="32" MAXLENGTH="32" VALUE="' + tagVal + '" onChange="fnTargetValueOnChange(' + target + ', this);"></TD>';
		innerHTML += '</TR>';
		if (target != cTARGET_SCN) {	// 장면을 선택하도록 한다.
			innerHTML += '<TR>';
			innerHTML += '<TD ALIGN="RIGHT">장면</TD>';
			innerHTML += '<TD><SELECT MULTIPLE SIZE="4" STYLE="width: 400px;" onChange="fnTargetValueOnChange(' + target + ', this);">';
/*
 * 장면에 변화가 있을 때마다 OPTION을 새로 써야 하므로,
 * 함수(fnAddOptionsToSelect)를 따로 만들어 필요할 때마다 목록을 만든다.
			innerHTML += '<OPTION' + (o === undefined || fnExistElement(o.aScnSerial, -1) != -1 ? ' SELECTED' : '') + '>작품 전체에 쓰인다.</OPTION>';
			var	aScn = gShow.aScn;
			for (var i = 0; i < aScn.length; ++i) {
				innerHTML += '<OPTION' + (o != undefined && fnExistElement(o.aScnSerial, i) != -1 ? ' SELECTED' : '') + '>특정 장면(' + aScn[i].name + ')에서만 쓰인다.</OPTION>';
			}
 */
			innerHTML += '</SELECT></TD></TR>';
		}
		innerHTML += '<TR>';
		innerHTML += '<TD ALIGN="RIGHT" VALIGN="TOP">설명</TD>';
		tagName = target == cTARGET_CHR ? 'CHARACTER_DESCRIPTION' : target == cTARGET_BG ? 'BACKGROUND_DESCRIPTION' : 'SCENE_DESCRIPTION';
		tagVal = o == undefined ? '' : o.desc;
		innerHTML += '<TD ALIGN="LEFT"><TEXTAREA ROWS="5" COLS="72" onChange="fnTargetValueOnChange(' + target + ', this);">' + tagVal + '</TEXTAREA></TD>';
		innerHTML += '</TR>';
		innerHTML += '</TBODY></TABLE>';
		oTD2.innerHTML = innerHTML;

		if (target != cTARGET_SCN) {
			// 선택할 장면의 목록을 보여준다.
			fnAddOptionsToSelect(target, o);
		}

		return oTR;
	}
</SCRIPT>
<SCRIPT LANGUAGE="JavaScript1.3">
/*
 * 2005/10/25 (EST) - 2005/10/26 (EST)
 *		By Gwang-Ho Kim (iccentric@gmail.com)
 *
 * 1. 자바스크립트의 버젼을 1.3으로 명시한다.
 * 2. 오브젝트를 만들어 필요한 변수를 관리한다.
 * 3. 대상(인물/배경)을 작품 전체적으로 설명할 수 있지만,
 * 때로는 특정 장면(들)에 따라 다르게 묘사될 수 있다.
 * 4. DB부터 얻은 정보를 이용해서 해당 필드값을 설정한다.
 */

/*
	function fnA(aX) {
		this.aX = [];
		for (var i = 0; i < aX.length; ++i)
			this.aX.push(aX[i]);
//		this.aX = aX;
	}
	var	aX = [1, 2];
	var	aA = new fnA(aX);
	var	aB = new fnA(aA.aX);
	alert(aB.aX);
	aB.aX.splice(0, 1, 0);
	alert(aB.aX);
	alert(aA.aX);
 */
	function fnExistElement(oArray, oElement) {
		for (var i = 0; i < oArray.length; ++i) {
			if (oArray[i] === oElement)
				return i;
		}
		return -1;
	}
	var	giScn, giChr, giBg;
	function fnInit() {
		// 1. giShow에 해당하는 작품을 복사해서 gShow가 가리키게 한다.
		// 테이블의 giShow번째 행에 해당하는 작업을 찾는다.
		var	iShow = fniShow();
		if (iShow == -1) {
			gShow = new objShow(
				0,	// 0: NEW
				"작품의 이름을 적으세요",
				new Date(),
				new Date(),
				"시나리오를 입력하세요.",

				// 장면
				[],

				// 인물
				[],
				// 배경
				[]
			);
		} else {
			// gaShow[iShow]정보를 복사한다.
			// DB와 관계없는 새로운 것이어야 하므로 id = 0으로 한다.
			// 특히 장면, 인물/배경에 관한 정보는 있는 그대로 복사한다.
			var	oShow = gaShow[iShow];
			gShow = new objShow(0, oShow.name, oShow.date_begin, oShow.date_end, oShow.desc, oShow.aScn, oShow.aChr, oShow.aBg);
			for (var i = 0; i < oShow.aChr.length; ++i) {
				fnSaveSceneId(oShow, gShow.aChr[i]);
			}
			for (i = 0; i < oShow.aBg.length; ++i) {
				fnSaveSceneId(oShow, gShow.aBg[i]);
			}
			gShow.rowIndex = giShow;
		}

		// 2. giScn, giChr, giBg의 유효성을 검사해서 적절한 값으로 만든다.
		// giScn: number
		giScn = parseInt(giScn);
		if (
			isNaN(giScn) ||					// 정의되지 않았거나 숫자가 아닌 경우
			(giScn < 0 || gShow.aScn.length <= giScn)	// 유효하지 않은 경우
		) {
			// (정책) 제일 마지막을 가리키도록 한다.
			// 하나도 없는 경우에는 -1이다.
			giScn = -1;
		}
		// giChr: number
		giChr = parseInt(giChr);
		if (
			isNaN(giChr) ||					// 정의되지 않았거나 숫자가 아닌 경우
			(giChr < 0 || gShow.aChr.length <= giChr)	// 유효하지 않은 경우
		) {
			// (정책) 제일 마지막을 가리키도록 한다.
			// 하나도 없는 경우에는 -1이다.
			giChr = -1;
		}
		// giBg: number
		giBg = parseInt(giBg);
		if (
			isNaN(giBg) ||					// 정의되지 않았거나 숫자가 아닌 경우
			(giBg < 0 || gShow.aBg.length <= giBg)		// 유효하지 않은 경우
		) {
			// (정책) 제일 마지막을 가리키도록 한다.
			// 하나도 없는 경우에는 -1이다.
			giBg = -1;
		}
		// giFlag: number
		giFlag = parseInt(giFlag);
		if (isNaN(giFlag)) {	// 정의되지 않았거나 숫자가 아닌 경우
			// (정책) 어떤 플래그도 설정되지 않는 것으로 한다. 즉 모든 테이블은 연다.
			giFlag = 0;
		} else {
			// 의미있는 플래그만 그대로 둔다.
			giFlag &= cTABFLAG_SCN | cTABFLAG_CHR | cTABFLAG_BG;
		}

		// 3. gShow의 정보를 표시한다.
		fnSetShow();

		return true;
	}

	// gShow에 있는 정보를 표시한다.
	function fnSetShow() {
		txtName.value = gShow.name;
		txtDateBegin.value = fnMakeDateString(gShow.date_begin);
		txtDateEnd.value = fnMakeDateString(gShow.date_end);
		txtaDescription.value = gShow.desc;

		// 2005/11/20 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
		// 장면, 인물/배경을 .rowIndex에 따라 행에 순서대로 나타나야 한다.
		// 그렇지 않으면 _fnAddTR함수에서 .insertRow에서 "Invalid Argument"에러가 발생한다.
		// 다음은 .rowIndex에 따라 정렬하기 위한 오브젝트와 정렬규칙이다.
		function objAsso(index, rowIndex) {
			this.index = index;
			this.rowIndex = rowIndex;
		}
		function fnSortByRowIndex(a, b) {
			return a.rowIndex - b.rowIndex;
		}

		// 장면
		while (tblScn.rows.length) {
			tblScn.deleteRow();
		}
if (1) {
		var	aAsso = [];
		for (var i = 0; i < gShow.aScn.length; ++i)
			aAsso.push(new objAsso(i, gShow.aScn[i].rowIndex));
		aAsso.sort(fnSortByRowIndex);
		for (var i = 0; i < aAsso.length; ++i) {
			_fnAddTR(cTARGET_SCN, gShow.aScn[aAsso[i].index]);
		}
} else {
		for (var i = 0; i < gShow.aScn.length; ++i) {
			_fnAddTR(cTARGET_SCN, gShow.aScn[i]);
		}
}
		// 해당하는 인덱스의 RADIO버튼을 CHECKED로 만든다.
		if (
			typeof(giScn) == "undefined" ||			// 변수가 선언되지 았았거나
			giScn == undefined ||				// 값이 설정되지 않았거나
			(giScn < 0 || tblScn.rows.length <= giScn)	// 값이 유효하지 않을 때
		) {
			// (정책) 제일 마지막 행을 가리키도록 한다.
			giScn = tblScn.rows.length - 1;
		}
		// 해당하는 인덱스의 RADIO버튼을 CHECKED로 만든다.
		if (giScn == -1) {	// 행이 하나도 없다.
			giFlag &= ~cTABFLAG_SCN;	// 강제로 해당 플래그를 해제한다.
			btnScn.disabled = true;
		} else {		// 행이 적어도 하나가 있다.
			tblScn.rows[giScn].children[0].children[0].checked = true;
		}
		if (giFlag & cTABFLAG_SCN)
			_fnTableDisplay(cTARGET_SCN, false);
		fnDisplayButtons(cTARGET_SCN);

		// 인물
		while (tblChr.rows.length) {
			tblChr.deleteRow();
		}
if (1) {
		var	aAsso = [];
		for (var i = 0; i < gShow.aChr.length; ++i)
			aAsso.push(new objAsso(i, gShow.aChr[i].rowIndex));
		aAsso.sort(fnSortByRowIndex);
		for (var i = 0; i < aAsso.length; ++i) {
			_fnAddTR(cTARGET_CHR, gShow.aChr[aAsso[i].index]);
		}
} else {
		for (i = 0; i < gShow.aChr.length; ++i) {
			_fnAddTR(cTARGET_CHR, gShow.aChr[i]);
		}
}
		// 해당하는 인덱스의 RADIO버튼을 CHECKED로 만든다.
		if (
			typeof(giChr) == "undefined" ||			// 변수가 선언되지 았았거나
			giChr == undefined ||				// 값이 설정되지 않았거나
			(giChr < 0 || tblChr.rows.length <= giChr)	// 값이 유효하지 않을 때
		) {
			// (정책) 제일 마지막 행을 가리키도록 한다.
			giChr = tblChr.rows.length - 1;
		}
		// 해당하는 인덱스의 RADIO버튼을 CHECKED로 만든다.
		if (giChr == -1) {	// 행이 하나도 없다.
			btnChr.disabled = true;
			giFlag &= ~cTABFLAG_CHR;	// 강제로 해당 플래그를 해제한다.
		} else {		// 행이 적어도 하나가 있다.
			tblChr.rows[giChr].children[0].children[0].checked = true;
		}
		if (giFlag & cTABFLAG_CHR)
			_fnTableDisplay(cTARGET_CHR, false);
		fnDisplayButtons(cTARGET_CHR);

		// 배경
		while (tblBg.rows.length) {
			tblBg.deleteRow();
		}
if (1) {
		var	aAsso = [];
		for (var i = 0; i < gShow.aBg.length; ++i)
			aAsso.push(new objAsso(i, gShow.aBg[i].rowIndex));
		aAsso.sort(fnSortByRowIndex);
		for (var i = 0; i < aAsso.length; ++i) {
			_fnAddTR(cTARGET_BG, gShow.aBg[aAsso[i].index]);
		}
} else {
		for (i = 0; i < gShow.aBg.length; ++i) {
			_fnAddTR(cTARGET_BG, gShow.aBg[i]);
		}
}
		if (
			typeof(giBg) == "undefined" ||			// 변수가 선언되지 았았거나
			giBg == undefined ||				// 값이 설정되지 않았거나
			(giBg < 0 || tblBg.rows.length <= giBg)	// 값이 유효하지 않을 때
		) {
			// (정책) 제일 마지막 행을 가리키도록 한다.
			giBg = tblBg.rows.length - 1;
		}
		// 해당하는 인덱스의 RADIO버튼을 CHECKED로 만든다.
		if (giBg == -1) {	// 행이 하나도 없다.
			btnBg.disabled = true;
			giFlag &= ~cTABFLAG_BG;		// 강제로 해당 플래그를 해제한다.
		} else {		// 행이 적어도 하나가 있다.
			tblBg.rows[giBg].children[0].children[0].checked = true;
		}
		if (giFlag & cTABFLAG_BG)
			_fnTableDisplay(cTARGET_BG, false);
		fnDisplayButtons(cTARGET_BG);
	}
</SCRIPT>
</HEAD>
<BODY>
<P STYLE="text-decoration: underline;">작품목록</P>
<DIV ALIGN="CENTER">
<TABLE ID="tblShowList" WIDTH="72%" BORDER="1" CELLSPACING="0" CELLPADDING="0"><TBODY>
</TBODY></TABLE>
<TABLE WIDTH="72%" BORDER="0"><TBODY>
<TR><TD WIDTH="20px" STYLE="background: <?PHP echo BGCOLOR_INSERT ?>;">&nbsp;</TD><TD WIDTH="*">새로 만든 작품</TD></TR>
<TR><TD WIDTH="20px" STYLE="background: <?PHP echo BGCOLOR_SELECT ?>;">&nbsp;</TD><TD WIDTH="*">DB에서 가져온 작품</TD></TR>
<TR><TD WIDTH="20px" STYLE="background: <?PHP echo BGCOLOR_UPDATE ?>;">&nbsp;</TD><TD WIDTH="*">DB에서 가져와 수정한 작품</TD></TR>
</TBODY></TABLE>
</DIV>

<P STYLE="text-decoration: underline;">작품관리</P>
<INPUT ID="btnInsertShow" TYPE="BUTTON" VALUE="더하기" onClick="fnInsertShow();">
<INPUT ID="btnUpdateShow" TYPE="BUTTON" VALUE="고치기" onClick="fnUpdateShow();">
<DIV ALIGN="CENTER">
<TABLE WIDTH="85%" BORDER="1" CELLSPACING="0" CELLPADDING="0">
<TBODY>
<TR>
	<TD ALIGN="RIGHT" WIDTH="20%">작품명</TD>
	<TD WIDTH="80%"><INPUT ID="txtName" TYPE="TEXT" SIZE="32" MAXLENGTH="32" VALUE="" onChange="fnValueOnChange(this);"></TD>
</TR>
<TR>
	<TD ALIGN="RIGHT">제작기간</TD>
	<TD><INPUT ID="txtDateBegin" TYPE="TEXT" SIZE="10" MAXLENGTH="10" VALUE="" onChange="fnValueOnChange(this);">&nbsp;-&nbsp;<INPUT ID="txtDateEnd" TYPE="TEXT" SIZE="10" MAXLENGTH="10" VALUE="" onChange="fnValueOnChange(this);">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;예) 2005-7-17 - 2005-8-15</TD>
</TR>
<TR>
	<TD ALIGN="RIGHT" VALIGN="TOP">대본</TD>
	<TD><TEXTAREA ID="txtaDescription" ROWS="20" COLS="100" onChange="fnValueOnChange(this);"></TEXTAREA></TD>
</TR>
<TR>
	<TD WIDTH="20%" ALIGN="RIGHT" VALIGN="TOP"><TABLE WIDTH="80%" BORDER="1" FRAME="VOID" RULES="ROWS" CELLSPACING="0" CELLPADDING="0"><TBODY>
	<TR><TD ALIGN="RIGHT"><INPUT ID="btnScn" TYPE="BUTTON" VALUE="장면" onClick="fnToggleTableDisplay(cTARGET_SCN);"</TD></TR>
	<TR><TD ALIGN="RIGHT"><TABLE BORDER="0" CELLSPACING="0" CELLPADDING="0"><TBODY>
		<TR ID="trScnMove"><TD ALIGN="RIGHT"><INPUT ID="btnScnMoveUp" TYPE="BUTTON" VALUE=" ↑ " onClick="fnBtnAction(cTARGET_SCN, cACTION_MOVEUP);"><INPUT ID="btnScnMoveDown" TYPE="BUTTON" VALUE=" ↓ " onClick="fnBtnAction(cTARGET_SCN, cACTION_MOVEDN);"></TD></TR>
		<TR ID="trScnInser"><TD ALIGN="RIGHT"><INPUT ID="btnScnInsert" TYPE="BUTTON" VALUE="더하기" onClick="fnBtnAction(cTARGET_SCN, cACTION_INSERT);"></TD></TR>
		<TR ID="trScnDelete"><TD ALIGN="RIGHT"><INPUT ID="btnScnDelete" TYPE="BUTTON" VALUE="없애기" onClick="fnBtnAction(cTARGET_SCN, cACTION_DELETE);"></TD></TR>
	</TBODY></TABLE></TD></TR>
	</TBODY></TABLE></TD>
	<TD WIDTH="80%" VALIGN="TOP"><TABLE ID="tblScn" WIDTH="100%" BORDER="1" FRAME="VOID" RULES="ROWS" CELLSPACING="0" CELLPADDING="0"><TBODY></TBODY></TABLE></TD>
</TR>
<TR>
	<TD ALIGN="RIGHT" VALIGN="TOP"><TABLE WIDTH="80%" BORDER="1" FRAME="VOID" RULES="ROWS" CELLSPACING="0" CELLPADDING="0"><TBODY>
	<TR><TD ALIGN="RIGHT"><INPUT ID="btnChr" TYPE="BUTTON" VALUE="인물" onClick="fnToggleTableDisplay(cTARGET_CHR);"</TD></TR>
	<TR><TD ALIGN="RIGHT"><TABLE BORDER="0" CELLSPACING="0" CELLPADDING="0"><TBODY>
		<TR ID="trChrMove"><TD ALIGN="RIGHT"><INPUT ID="btnChrMoveUp" TYPE="BUTTON" VALUE=" ↑ " onClick="fnBtnAction(cTARGET_CHR, cACTION_MOVEUP);"><INPUT ID="btnChrMoveDown" TYPE="BUTTON" VALUE=" ↓ " onClick="fnBtnAction(cTARGET_CHR, cACTION_MOVEDN);"></TD></TR>
		<TR ID="trChrInsert"><TD ALIGN="RIGHT"><INPUT ID="btnChrInsert" TYPE="BUTTON" VALUE="더하기" onClick="fnBtnAction(cTARGET_CHR, cACTION_INSERT);"></TD></TR>
		<TR ID="trChrDelete"><TD ALIGN="RIGHT"><INPUT ID="btnChrDelete" TYPE="BUTTON" VALUE="없애기" onClick="fnBtnAction(cTARGET_CHR, cACTION_DELETE);"></TD></TR>
	</TBODY></TABLE></TD></TR>
	</TBODY></TABLE></TD>
	<TD VALIGN="TOP"><TABLE ID="tblChr" WIDTH="100%" BORDER="1" FRAME="VOID" RULES="ROWS" CELLSPACING="0" CELLPADDING="0"><TBODY></TBODY></TABLE></TD>
</TR>
<TR>
	<TD ALIGN="RIGHT" VALIGN="TOP"><TABLE WIDTH="80%" BORDER="1" FRAME="VOID" RULES="ROWS" CELLSPACING="0" CELLPADDING="0"><TBODY>
	<TR><TD ALIGN="RIGHT"><INPUT ID="btnBg" TYPE="BUTTON" VALUE="배경" onClick="fnToggleTableDisplay(cTARGET_BG);"</TD></TR>
	<TR><TD ALIGN="RIGHT"><TABLE BORDER="0" CELLSPACING="0" CELLPADDING="0"><TBODY>
		<TR ID="trBgMove"><TD ALIGN="RIGHT"><INPUT ID="btnBgMoveUp" TYPE="BUTTON" VALUE=" ↑ " onClick="fnBtnAction(cTARGET_BG, cACTION_MOVEUP);"><INPUT ID="btnBgMoveDown" TYPE="BUTTON" VALUE=" ↓ " onClick="fnBtnAction(cTARGET_BG, cACTION_MOVEDN);"></TD></TR>
		<TR ID="trBgInsert"><TD ALIGN="RIGHT"><INPUT ID="btnBgInsert" TYPE="BUTTON" VALUE="더하기" onClick="fnBtnAction(cTARGET_BG, cACTION_INSERT);"></TD></TR>
		<TR ID="trBgDelete"><TD ALIGN="RIGHT"><INPUT ID="btnBgDelete" TYPE="BUTTON" VALUE="없애기" onClick="fnBtnAction(cTARGET_BG, cACTION_DELETE);"></TD></TR>
	</TBODY></TABLE></TD></TR>
	</TBODY></TABLE></TD>
	<TD VALIGN="TOP"><TABLE ID="tblBg" WIDTH="100%" BORDER="1" FRAME="VOID" RULES="ROWS" CELLSPACING="0" CELLPADDING="0"><TBODY></TBODY></TABLE></TD>
</TR>
</TBODY>
</TABLE>
</DIV>
</BODY>
</HTML>
<?PHP
	}
	// Closing connection
	mysql_close($link);
?>
