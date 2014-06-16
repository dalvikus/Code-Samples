var	gTimeout = 1 / 30;	// 단위: 초, 영화
var	gindexFrame = 0;
var	gtimeoutID = null;

/*
function fnSetFrameRate() {
	var	o = document.getElementsByName("rdFrameRateType");
	if (!o) {	// 오브젝트를 얻는 데 문제가 있다면
		// (정책) 기본값은 "천천히"이다.
		gsTimeout = cFRAME_FATE_TIMEOUT_SLOW;
		return;
	}
	for (var i = 0; i < o.length; ++i) {
		if (o[i].checked) {
			gTimeout = 1 / gcfrType[i][cFRAME_RATE_SPEED];		// gTimeout초당 1프레임
			break;
		}
	}
	if (
		o.length != gcfrType.length ||	// 오브젝트의 갯수가 일치하지 않거나
		i == i.length			// 선택된 것이 없다면
	) {
		// (정책) 기본값은 "TV"이다.
		gTimeout = 1 / gcfrType[cFRAME_RATE_TYPE_TV][cFRAME_RATE_SPEED];// gTimeout초당 1프레임
	}
}
 */
function fnPlayInit() {
	// 시작 프레임을 0으로 설정한다. 플레이 상태이다.
	gindexFrame = 0;
	// 플레이 상태를 중단상태로 바꾸다. (그래야 아래에서 시작할 수 있다.)
	gtimeoutID = null;

	btnPlay.disabled = true;
}

function fnPlayScene() {
	fnPlay();
}

function fnStop() {
	if (gtimeoutID) {
		clearTimeout(gtimeoutID);
		gtimeoutID = null;
	}
	btnPlay.disabled = false;
	btnStop.disabled = true;

	// 현재 보고 있는 프레임은 gindexFrame보다 한 프레임 앞선 프레임이다.
	gindexFrame -= 1;
	if (gindexFrame == -1) {
		gindexFrame = gaImg.length - 1;
	}

}
function fnPlay() {
	btnPlay.disabled = true;
	btnStop.disabled = false;

	var	imgLow = gaImg[gindexFrame];
	if (imgLow === undefined) {
		img = '/images/Thumbnail/_void.jpg';
	} else {
		img = '/images/Low/Work/Storyboard/' + imgLow + '.png';
	}
	var	alt = ' ALT="' + img + '"';
	tdImg.innerHTML = '<IMG HEIGHT="300px" WIDTH="300px" SRC="' + img + '">';

	// 다음 프레임을 현재 프레임으로 한다.
	gindexFrame += 1;
	if (gindexFrame == gaImg.length) {	// 제일 마지막 프레임이라면
		// (정책) 첫 프레임으로 돌아간다.
		gindexFrame = 0;
	}

	// gTimeout초 동안 멈춘 후에 다시 현재 프레임을 보여주도록 한다.
	gtimeoutID = setTimeout("fnPlay()", 1000 * gTimeout);
}
