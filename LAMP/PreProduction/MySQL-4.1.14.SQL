USE PREPRODUCTION;

DROP TABLE IF EXISTS TREFERENCE_SERIES;
DROP TABLE IF EXISTS TREFERENCE;
DROP TABLE IF EXISTS TCUT;
DROP TABLE IF EXISTS TSTORYBOARD;
DROP TABLE IF EXISTS TDESIGN;
DROP TABLE IF EXISTS TWORK_IMAGE;
DROP TABLE IF EXISTS TIMAGE;
DROP TABLE IF EXISTS TWORKER;
DROP TABLE IF EXISTS TWORK;

DROP TABLE IF EXISTS TTARGET_SCENE;
DROP TABLE IF EXISTS TTARGET;
DROP TABLE IF EXISTS TSCENE;
DROP TABLE IF EXISTS TSHOW;

DROP TABLE IF EXISTS TGROUP_USER;
DROP TABLE IF EXISTS TUSER;
DROP TABLE IF EXISTS TGROUP;


/*
TGROUP
========================================================================
GROUP_ID	-- UNIQUE, >= 0
		-- 0: 관리자 그룹
NAME
========================================================================
 */
CREATE TABLE IF NOT EXISTS TGROUP
(
	GROUP_ID	INT UNSIGNED NOT NULL AUTO_INCREMENT UNIQUE, 
	NAME		CHAR(32) NOT NULL UNIQUE
)
	TYPE=INNODB	-- FOREIGN KEY를 사용하기 위하여
;
SET @@INSERT_ID = -1;	-- AUTO_INCREMENT로 설정된 값은 0부터 시작한다.
INSERT INTO TGROUP (NAME) VALUES ('관리자 그룹');
SET @@INSERT_ID = 0;	-- AUTO_INCREMENT로 설정된 값은 1부터 시작한다.
INSERT INTO TGROUP (NAME) VALUES ('Design');

/*
TUSER
========================================================================
USER_ID		-- UNIQUE, >= 0
		-- 0: 관리자
NICKNAME	-- Login ID
PASSWORD_SHA1	-- Login Password
FULLNAME
POSITION	-- 1: 팀장, 2: 팀원, 3: 방문
STATUS		-- 1: 사용중, 0: 정지, -1: 제거
========================================================================
 */
CREATE TABLE IF NOT EXISTS TUSER
(
	USER_ID		INT UNSIGNED NOT NULL AUTO_INCREMENT UNIQUE, 
	NICKNAME	CHAR(32) NOT NULL UNIQUE,
	PASSWORD_SHA1	CHAR(40) NOT NULL,		-- 12.9.2. Encryption Functions: SHA1()
	FULLNAME	CHAR(32) NOT NULL,
	POSITION	TINYINT UNSIGNED DEFAULT 2,	-- 1: 팀장, 2: 팀원, 3: 방문
	STATUS		TINYINT DEFAULT 1		-- -1: 제거, 0: 중지, 1: 사용중
)
	TYPE=INNODB	-- FOREIGN KEY를 사용하기 위하여
;
SET @@INSERT_ID = -1;	-- AUTO_INCREMENT로 설정된 값은 0부터 시작한다.
INSERT INTO TUSER (NICKNAME, PASSWORD_SHA1, POSITION) VALUES ('관리자', SHA1('관리자'), 1);
SET @@INSERT_ID = 0;	-- AUTO_INCREMENT로 설정된 값은 1부터 시작한다.
INSERT INTO TUSER (NICKNAME, FULLNAME, PASSWORD_SHA1) VALUES ('calani98', '고성욱', SHA1('flower'));
INSERT INTO TUSER (NICKNAME, PASSWORD_SHA1) VALUES ('coo03', SHA1('15321234'));
INSERT INTO TUSER (NICKNAME, FULLNAME, PASSWORD_SHA1) VALUES ('alwjd49', '박미정', SHA1('1315'));
INSERT INTO TUSER (NICKNAME, FULLNAME, PASSWORD_SHA1) VALUES ('r22022', '김리라', SHA1('1510'));
INSERT INTO TUSER (NICKNAME, PASSWORD_SHA1) VALUES ('무', SHA1('무'));
INSERT INTO TUSER (NICKNAME, PASSWORD_SHA1) VALUES ('기', SHA1('기'));
INSERT INTO TUSER (NICKNAME, PASSWORD_SHA1) VALUES ('경', SHA1('경'));
INSERT INTO TUSER (NICKNAME, PASSWORD_SHA1) VALUES ('신', SHA1('신'));
INSERT INTO TUSER (NICKNAME, PASSWORD_SHA1) VALUES ('임', SHA1('임'));
INSERT INTO TUSER (NICKNAME, PASSWORD_SHA1) VALUES ('계', SHA1('계'));

/*
TGROUP_USER
========================================================================
GROUP_ID
USER_ID
========================================================================
 */
CREATE TABLE IF NOT EXISTS TGROUP_USER
(
	GROUP_ID	INT UNSIGNED NOT NULL,
	USER_ID		INT UNSIGNED NOT NULL,

	INDEX		(GROUP_ID),	-- 같은 GROUP_ID를 가진 USER_ID를 쉽게 찾을 수 있게 한다.
	INDEX		(USER_ID),	-- 같은 USER_ID를 가진 GROUP_ID를 쉽게 찾을 수 있게 한다.
	PRIMARY KEY	(GROUP_ID, USER_ID),	-- 두 조합은 UNIQUE해야 한다.
	FOREIGN KEY	(GROUP_ID)
		REFERENCES	TGROUP (GROUP_ID)
		ON DELETE	CASCADE
		ON UPDATE	CASCADE,
	FOREIGN KEY	(USER_ID)
		REFERENCES	TUSER (USER_ID)
		ON DELETE	CASCADE
		ON UPDATE	CASCADE
)
	TYPE=INNODB	-- GROUP_ID와 USER_ID를 FOREIGN KEY로 사용하기 위하여
;
INSERT INTO TGROUP_USER (GROUP_ID, USER_ID) VALUES (0, 0);
INSERT INTO TGROUP_USER (GROUP_ID, USER_ID) VALUES (1, 1);
INSERT INTO TGROUP_USER (GROUP_ID, USER_ID) VALUES (1, 2);
INSERT INTO TGROUP_USER (GROUP_ID, USER_ID) VALUES (1, 3);
INSERT INTO TGROUP_USER (GROUP_ID, USER_ID) VALUES (1, 4);
INSERT INTO TGROUP_USER (GROUP_ID, USER_ID) VALUES (1, 5);
INSERT INTO TGROUP_USER (GROUP_ID, USER_ID) VALUES (1, 6);
INSERT INTO TGROUP_USER (GROUP_ID, USER_ID) VALUES (1, 7);
INSERT INTO TGROUP_USER (GROUP_ID, USER_ID) VALUES (1, 8);
INSERT INTO TGROUP_USER (GROUP_ID, USER_ID) VALUES (1, 9);
INSERT INTO TGROUP_USER (GROUP_ID, USER_ID) VALUES (1, 10);


/*
TSHOW
========================================================================
SHOW_ID			-- UNIQUE, >= 1
NAME			-- UNIQUE
DATE_BEGIN
DATE_END
DESCRIPTION
========================================================================
 */
-- 13.1.5. CREATE TABLE Syntax
-- LAST_INSERT_ID()를 이용해서 AUTO_INCREMENT로 설정한 COLUMN의 값을 얻을 수 있다. (3.6.9. Using AUTO_INCREMENT)
CREATE TABLE IF NOT EXISTS TSHOW
-- 정의
(
	SHOW_ID		INT UNSIGNED NOT NULL AUTO_INCREMENT UNIQUE,
	NAME		CHAR(32) NOT NULL UNIQUE,
	DATE_BEGIN	DATE NOT NULL,
	DATE_END	DATE NOT NULL,
	DESCRIPTION	TEXT NOT NULL		-- L + 2 BYTEs where L < 2^{16} = 65536
)
-- 옵션
	TYPE=INNODB	-- FOREIGN KEY를 사용하기 위하여
;
-- 아래의 TSCENE테이블에서 SCENE_ID가 0인 행을 만들고자 한다.
-- 이유는 TTARGET_SCENE테이블에서 SCENE_ID가 0인 경우(인물/배경이 작품 전체에 등장하는 경우)를 위해서다.
-- SCENE_ID = 0인 경우가 없다면 TTARGET_SCENE테이블에서 SCENE_ID가 FOREIGN KEY로 설정되어 있기 때문에
-- 이러한 경우는 추가할 수 없기 때문이다.
-- 그런데, TSCENE에서 SHOW_ID도 TSHOW테이블을 참조하는 FOREIGN KEY로 사용했기 때문에,
-- TSCENE테이블에 SCENE_ID = 0인 행을 만들기 위해서는 의미있는 SHOW_ID가 TSHOW테이블에 있어야 한다.
-- 그래서 SHOW_ID인 가짜의 행을 만든다.
SET @@INSERT_ID = -1;	-- AUTO_INCREMENT로 설정된 값은 0부터 시작한다.
INSERT INTO TSHOW (NAME, DATE_BEGIN, DATE_END, DESCRIPTION) VALUES ("", "", "", "");
SET @@INSERT_ID = 0;	-- AUTO_INCREMENT로 설정된 값은 1부터 시작한다.
/*
INSERT INTO TSHOW (NAME, DATE_BEGIN, DATE_END, DESCRIPTION) VALUES ("작품명", "2005-10-7", "2005-12-10", "시나리오...");
-- 바로 앞의 데이터와 NAME이 같으므로, 다음 SQL은 실패한다.
INSERT INTO TSHOW (NAME, DATE_BEGIN, DATE_END, DESCRIPTION) VALUES ("작품명", "2005-10-7", "2005-12-10", "시나리오...");
INSERT INTO TSHOW (NAME, DATE_BEGIN, DATE_END, DESCRIPTION) VALUES ("작품명2", "2005-10-7", "2005-12-10", "시나리오...");
 */


/*
TSCENE
========================================================================
SCENE_ID		-- UNIQUE, >= 1
SHOW_ID			-- 장면이 속한 작품
			-- INDEX로 사용하여, 주어진 작품에 어떤 장면이 있는 지 쉽게 알 수 있다.
SERIAL			-- 몇 번째 장면인가를 말해준다. (>= 0)
NAME			-- (SHOW_ID, SERIAL, NAME)의 조합은 UNIQUE하다.
DESCRIPTION
========================================================================
 */
CREATE TABLE IF NOT EXISTS TSCENE
(
	SCENE_ID	INT UNSIGNED NOT NULL AUTO_INCREMENT UNIQUE, 
	SHOW_ID		INT UNSIGNED NOT NULL,
	SERIAL		TINYINT UNSIGNED NOT NULL,	-- [0, 255]
	NAME		CHAR(32) NOT NULL,
	DESCRIPTION	TEXT NOT NULL,

	PRIMARY KEY	(SHOW_ID, SERIAL, NAME),	-- 세 조합은 UNIQUE해야 한다.
	-- 7.4.5. How MySQL Uses Indexes
	-- (PRIMARY KEY, UNIQUE)로 설정된 것들은 INDEX로 사용된다.
	-- TSHOW에서는 SHOW_ID이나 NAME이 INDEX로 사용된다.
	INDEX		(SHOW_ID),			-- 많은 경우 SHOW_ID를 통해서 이 테이블에 접근한다.
	-- 15.7.4. FOREIGN KEY Constraints
	FOREIGN KEY	(SHOW_ID)			-- SHOW_ID열은
		REFERENCES	TSHOW (SHOW_ID)		-- TSHOW테이블의 SHOW_ID을 참조하므로
							-- 해당하는 TSHOW테이블의 SHOW_ID가 지우려고 할 때,
		ON DELETE	CASCADE			-- 이 테이블에서 같은 SHOW_ID를 가진 행들은 모두 삭제된다.
							--	CASCADE: 같은 동작을 한다.
							--	NO ACTION = RESTRICT = 생략: 해당하는 SHOW_ID를 지울 수 없다.
							-- TSHOW테이블에서 해당하는 SHOW_ID를 바꾸려고 할 때,
		ON UPDATE	CASCADE			-- 이 테이블에서 같은 SHOW_ID를 가지 행들에서, SHOW_ID를 똑같이 바꾼다.
)
	TYPE=INNODB	-- FOREIGN KEY를 사용하기 위하여
;
SET @@INSERT_ID = -1;	-- AUTO_INCREMENT로 설정된 값은 0부터 시작한다.
-- TARGET_ID = 0인 데이터를 만들어 놓아야 TTARGET_SCENE에서 TARGET_ID = 0일 수 있다.
INSERT INTO TSCENE (SHOW_ID, SERIAL, NAME, DESCRIPTION) VALUES (0, 0, "", "");
SET @@INSERT_ID = 0;	-- AUTO_INCREMENT로 설정된 값은 1부터 시작한다.
/*
INSERT INTO TSCENE (SHOW_ID, SERIAL, NAME, DESCRIPTION) VALUES (1, 0, "장면 #1", "장면 #1: ...");
INSERT INTO TSCENE (SHOW_ID, SERIAL, NAME, DESCRIPTION) VALUES (2, 0, "장면 #1", "장면 #1: ...");
INSERT INTO TSCENE (SHOW_ID, SERIAL, NAME, DESCRIPTION) VALUES (2, 1, "장면 #1", "장면 #1: ...");
-- 바로 위의 데이터와 (2, 1, "장면 #1")의 조합이 같다. 따라서 다음 SQL은 실패한다.
INSERT INTO TSCENE (SHOW_ID, SERIAL, NAME, DESCRIPTION) VALUES (2, 1, "장면 #1", "장면 #1: ...");
-- 위의 TSHOW의 데이터를 보면 SHOW_ID = 3인 행은 없다. 따라서 FOREIGN KEY에 의해서 다음 SQL은 실패한다.
INSERT INTO TSCENE (SHOW_ID, SERIAL, NAME, DESCRIPTION) VALUES (3, 0, "장면 #1", "장면 #1: ...");
 */


/*
TTARGET
========================================================================
TARGET_ID		-- UNIQUE, >= 1
SHOW_ID			-- 대상이 등장하는 작품
TYPE			-- 1: 인물, 2: 배경
			-- (SHOW_ID, TYPE)의 조합을 INDEX로 사용하여,
			-- 주어진 작품에 사용되는 인물/배경을 쉽게 찾을 수 있다.
NAME			-- 대상의 이름
			-- (SHOW_ID, TYPE, NAME)의 조합은 UNIQUE하다.
DESCRIPTION		-- 대상에 대한 설명이다.
========================================================================
 */
CREATE TABLE IF NOT EXISTS TTARGET
(
	TARGET_ID	INT UNSIGNED NOT NULL AUTO_INCREMENT UNIQUE, 
	SHOW_ID		INT UNSIGNED NOT NULL,
	TYPE		TINYINT UNSIGNED NOT NULL,	-- [0, 255] 0: 인물/배경도 아니다. 1: 인물, 2: 배경
							-- 0: 의미있는 데이터가 아니다.
	NAME		CHAR(32) NOT NULL,
	DESCRIPTION	TEXT NOT NULL,

	PRIMARY KEY	(SHOW_ID, TYPE, NAME),	-- UNIQUE
	INDEX		(SHOW_ID, TYPE),
	FOREIGN KEY	(SHOW_ID)
		REFERENCES	TSHOW (SHOW_ID)
		ON DELETE	CASCADE
		ON UPDATE	CASCADE
)
	TYPE=INNODB				-- SHOW_ID를 FOREIGN KEY로 사용하기 위해서
;
/*
INSERT INTO TTARGET (SHOW_ID, TYPE, NAME, DESCRIPTION) VALUES (2, 1, "인물 #1", "인물 #1: ...");
-- 바로 위와 같은 데이터이므로 다음 SQL은 실패한다.
INSERT INTO TTARGET (SHOW_ID, TYPE, NAME, DESCRIPTION) VALUES (2, 1, "인물 #1", "인물 #1: ...");
INSERT INTO TTARGET (SHOW_ID, TYPE, NAME, DESCRIPTION) VALUES (2, 1, "인물 #2", "인물 #2:(장면 #1, 장면 #2) ...");
INSERT INTO TTARGET (SHOW_ID, TYPE, NAME, DESCRIPTION) VALUES (2, 2, "배경 #1", "배경 #1: ...");
INSERT INTO TTARGET (SHOW_ID, TYPE, NAME, DESCRIPTION) VALUES (2, 2, "배경 #2", "배경 #2: ...");
INSERT INTO TTARGET (SHOW_ID, TYPE, NAME, DESCRIPTION) VALUES (2, 2, "배경 #3", "배경 #3: ...");
 */


/*
TTARGET_SCENE
========================================================================
TARGET_ID		-- 같은 TARGET_ID라면 같은 대상에 대한 정보이다.
			-- INDEX로 사용하여, 주어진 대상이 참조하는 SCENE_ID를 쉽게 찾을 수 있다.
SCENE_ID		-- SCENE_ID = 0(기본값): 대상(TARGET_ID: 인물/배경)은 작품 전체에 등장한다.
			-- SCENE_ID >= 1: 대상(TARGET_ID: 인물/배경)는 특정 장면(SCENE_ID)에서만 등장한다.
========================================================================
 */
CREATE TABLE IF NOT EXISTS TTARGET_SCENE
(
	TARGET_ID	INT UNSIGNED NOT NULL,
	SCENE_ID	INT UNSIGNED DEFAULT 0,

	-- SCENE_ID = 0이면 TARGET_ID는 작품 전체에 등장한다.
	-- 실제로 TSCENE테이블에서 SCENE_ID가 0인 경우는 없으므로
	-- SCENE_ID를 이용해서 PRIMARY KEY를 만들거나 FOREIGN KEY를 만들어도
	-- PRIMARY KEY나 FOREIGN KEY의 사용에 위배되지 않는다.
	PRIMARY KEY	(TARGET_ID, SCENE_ID),	-- UNIQUE
						-- TSCENE테이블에서 SCENE_ID = 0인 경우는 없으므로,
						-- PRIMARY KEY의 조건에 위배되지 않는다.
	INDEX		(TARGET_ID),
	FOREIGN KEY	(TARGET_ID)
		REFERENCES	TTARGET (TARGET_ID)
		ON DELETE	CASCADE,
	FOREIGN KEY	(SCENE_ID)		-- TSCENE테이블에서 SCENE_ID = 0인 경우는 없으므로,
						-- FOREIGN KEY의 조건에 위배되지 않는다.
		REFERENCES	TSCENE (SCENE_ID)
		ON DELETE	CASCADE
		ON UPDATE	CASCADE
)
	TYPE=INNODB	-- TARGET_ID와 SCENE_ID를 FOREIGN KEY로 사용하기 위하여
;
/*
INSERT INTO TTARGET_SCENE (TARGET_ID, SCENE_ID) VALUES (1, 0);
-- 해당 장면(장면 #3)이 없기 때문에 다음 SQL은 실패한다.
INSERT INTO TTARGET_SCENE (TARGET_ID, SCENE_ID) VALUES (1, 3);
INSERT INTO TTARGET_SCENE (TARGET_ID, SCENE_ID) VALUES (2, 1);
INSERT INTO TTARGET_SCENE (TARGET_ID, SCENE_ID) VALUES (2, 2);
 */


/*
TWORK
========================================================================
TYPE			-- 1: 디자인, 2: 스토리보드
WORK_ID			-- UNIQUE, >= 1
			-- TYPE = 1: WORK_ID = DESIGN_ID, TYPE = 2: WORK_ID = STORYBOARD_ID
SHOW_ID
SCENE_ID		-- TYPE = 2; SCENE_ID >= 1
			-- TYPE = 1; 0: 작품 전체의 맞는 디자인, >= 1: 특정 장면에 국한된 디자인
WORKER_ID
DATE_BEGIN
DATE_END
YOUR_ORDER
STATUS			-- 1: Least, 10: Most
========================================================================
 */
CREATE TABLE IF NOT EXISTS TWORK
(
	TYPE		TINYINT UNSIGNED DEFAULT 1,			-- 1: DESIGN, 2: STORYBOARD
	WORK_ID		INT UNSIGNED NOT NULL AUTO_INCREMENT UNIQUE,	-- TYPE = 1; DESIGN_ID, TYPE = 2; STORYBOARD_ID
	SHOW_ID		INT UNSIGNED NOT NULL,
	SCENE_ID	INT UNSIGNED NOT NULL,
	DATE_BEGIN	DATE NOT NULL,
	DATE_END	DATE NOT NULL,
	YOUR_ORDER	TEXT NOT NULL,		-- L + 2 BYTEs where L < 2^{16} = 65536
	STATUS		TINYINT UNSIGNED DEFAULT 1,

	-- 2005/11/11 (EST) - 김광호(Gwang-Ho Kim, iccentric@gmail.com)
	-- 디자인 작업의 경우, (DESIGN_TYPE, TARGET_ID)의 조합은 UNIQUE하다.
	-- 스토리보드 작업의 경우, (SHOW_ID, SCENE_ID)의 조합은 UNIQUE하다.
	-- 디자인 작업의 경우,
	-- TDESIGN테이블에서 (DESIGN_TYPE, TARGET_ID)의 조합이
	-- UNIQUE로 설정되었기 때문에 같은 작업이 반복되는 것을 막을 수는 있다.
	-- 비록 TWORK테이블에 넣을 수 있더라도.
	-- 하지만 스토리보드 작업의 경우에는 TSTORYBOARD테이블에
	-- 두 COLUMNS(SHOW_ID, SCENE_ID)을 추가하고,
	-- (SHOW_ID, SCENE_ID)의 조합을 UNIQUE로 설정되지 않는 한
	-- 디자인 작업에서와 같은 반응을 볼 수는 없다.
	-- 그렇다고 사용하지 않는 두 COLUMN을 추가할 수도 없고...
	-- 어느 경우든 이 TWORK테이블에 등록되는 것은 막을 수는 없다.
	-- 어떤 방법이 있을까...
	-- 제일 쉬운 방법은 STORED PROCEDURE를 이용하는 것인데,
	-- 그럴려면 MySQL을 5.0이상으로 업그레이드해야 한다.
	-- 지금은 PHP에서 처리하도록 한다.
	-- 단, 스토리보드에도 이를 위해서, 앞에서 언급한 두 행(SHOW_ID, SCENE_ID)을 넣고
	-- 중복되는 데이터가 들어올 결우 QUERY가 실패하게 해서,
	-- 이를 PHP에서 알 수 있도록 한다.
	INDEX		(TYPE),			-- 디자인/스토리보드 작업을 쉽게 찾게 한다.
	FOREIGN KEY	(SHOW_ID)
		REFERENCES	TSHOW (SHOW_ID)
		ON DELETE	CASCADE
		ON UPDATE	CASCADE,
	FOREIGN KEY	(SCENE_ID)
		REFERENCES	TSCENE (SCENE_ID)
		ON DELETE	CASCADE
		ON UPDATE	CASCADE
)
	TYPE=INNODB	-- FOREIGN KEY를 사용하기 위하여
;


/*
TWORKER
========================================================================
WORKER_ID	-- 같은 WORKER_ID라면 같은 작업에 참여하는 USER들에 대한 정보이다.
		-- INDEX로 사용하여, USER_ID를 쉽게 찾을 수 있게 한다.
USER_ID		-- INDEX로 사용하여, USER가 참여하는 작업을 WORKER_ID를 통해서 쉽게 얻도록 한다.
========================================================================
 */
CREATE TABLE IF NOT EXISTS TWORKER
(
	WORKER_ID	INT UNSIGNED NOT NULL,	-- WORK_ID
	USER_ID		INT UNSIGNED NOT NULL,

	PRIMARY KEY	(WORKER_ID, USER_ID),	-- UNIQUE
	INDEX		(WORKER_ID),
	INDEX		(USER_ID),
	FOREIGN KEY	(WORKER_ID)
		REFERENCES	TWORK (WORK_ID)
		ON DELETE	CASCADE
		ON UPDATE	CASCADE,
	FOREIGN KEY	(USER_ID)
		REFERENCES	TUSER (USER_ID)
		ON DELETE	CASCADE
		ON UPDATE	CASCADE
)
	TYPE=INNODB	-- TARGET_ID와 SCENE_ID를 FOREIGN KEY로 사용하기 위하여
;

/*
TIMAGE
========================================================================
IMAGE_ID		-- UNIQUE, >= 1
========================================================================
 */
CREATE TABLE IF NOT EXISTS TIMAGE
(
	IMAGE_ID	INT UNSIGNED NOT NULL AUTO_INCREMENT UNIQUE
)
	TYPE=INNODB	-- FOREIGN KEY를 사용하기 위하여
;
-- 작업이 등록되는 단계에서는 아직 완성된 이미지가 없다.
-- 그런 경우에 각 작업(디자인 작업: TDESGIN, 스토리보드 작업: TSTORYBOARD_CUT)의 IMAGE_ID를 0으로 설정한다.
-- 그런데 각 테이블에서 IMAGE_ID는 이 TIMAGE테이블을 참조하는 FOREIGN KEY로 설정되었기 때문에
-- IMAGE_ID = 0이 행을 만들어야 한다.
SET @@INSERT_ID = -1;	-- AUTO_INCREMENT로 설정된 값은 0부터 시작한다.
INSERT INTO TIMAGE () VALUES ();
SET @@INSERT_ID = 0;	-- AUTO_INCREMENT로 설정된 값은 1부터 시작한다.

/*
TWORK_IMAGE
========================================================================
IMAGE_ID		-- IMAGE_ID가 같다면 같은 작업(디자인 작업또는 스토리보드의 컷 작업)에 속한다.
REVISION		-- 해당 작업의 버젼을 말한다. (>= 1)
YOUR_COMMENT
FILENAME		-- 본래 주어진 파일이름
FILENAME_UUID		-- UNIQUE한 파일이름
			-- PHP: sha1(uniqid(rand(), true)): 40 characters, md5(...): 32 characters
USER_ID			-- 누가 작업했는가?
========================================================================
 */
CREATE TABLE IF NOT EXISTS TWORK_IMAGE
(
	IMAGE_ID	INT UNSIGNED NOT NULL,
	REVISION	TINYINT UNSIGNED DEFAULT 0,
	YOUR_COMMENT	TEXT NOT NULL,
	FILENAME	CHAR(128) NOT NULL,	-- 원래 파일이름
	FILENAME_UUID	CHAR(40) NOT NULL,	-- PHP: sha1(uniqid(rand(), true))
	USER_ID		INT UNSIGNED NOT NULL,

	INDEX		(IMAGE_ID),
	INDEX		(USER_ID),
	FOREIGN KEY	(IMAGE_ID)
		REFERENCES	TIMAGE (IMAGE_ID)
		ON DELETE	CASCADE
		ON UPDATE	CASCADE,
	FOREIGN KEY	(USER_ID)
		REFERENCES	TUSER (USER_ID)
		ON DELETE	CASCADE
		ON UPDATE	CASCADE
)
	TYPE=INNODB	-- FOREIGN KEY를 사용하기 위하여
;

/*
TDESIGN
========================================================================
DESIGN_ID		-- WORK_ID
SHOW_ID
SCENE_ID		-- TSTORYBOARD와 마찬가지로 중복 작업을 피하기 위해서 두 COLUMN(SHOW_ID, SCENE_ID)를 넣었다.
			-- 디자인 작업의 경우 SCENE_ID만으로 UNIQUE한 값을 만들 수 없다.
			-- 왜냐하면 SCENE_ID = 0인 경우가 있기 때문이다.
			-- 하지만 이 경우에도 SHOW_ID와 결합하면 UNIQUE한 조합이 된다.
TARGET_ID
TYPE			-- 1: 구체적인 디자인, 2: 컨셉 디자인
			-- (SHOW_ID, SCENE_ID, TARGET_ID, TYPE)의 조합은 UNIQUE하다.
IMAGE_ID		-- 결과 이미지에 대한 참조이다.
			-- 0: 아직 어떤 결과도 없다., >= 1: TIMAGE테이블의 해당 IMAGE_ID에 결과에 대한 정보가 있다.
========================================================================
 */
CREATE TABLE IF NOT EXISTS TDESIGN
(
	DESIGN_ID	INT UNSIGNED NOT NULL UNIQUE,	-- WORK_ID
	SHOW_ID		INT UNSIGNED NOT NULL,
	SCENE_ID	INT UNSIGNED NOT NULL,
	TARGET_ID	INT UNSIGNED NOT NULL,
	TYPE		TINYINT UNSIGNED NOT NULL,
	IMAGE_ID	INT UNSIGNED DEFAULT 0,

	PRIMARY KEY	(SHOW_ID, SCENE_ID, TARGET_ID, TYPE),	-- UNIQUE
	FOREIGN KEY	(SHOW_ID)
		REFERENCES	TSHOW (SHOW_ID)
		ON DELETE	CASCADE
		ON UPDATE	CASCADE,
	FOREIGN KEY	(SCENE_ID)
		REFERENCES	TSCENE (SCENE_ID)
		ON DELETE	CASCADE
		ON UPDATE	CASCADE,
	FOREIGN KEY	(DESIGN_ID)
		REFERENCES	TWORK (WORK_ID)
		ON DELETE	CASCADE
		ON UPDATE	CASCADE,
	FOREIGN KEY	(TARGET_ID)
		REFERENCES	TTARGET (TARGET_ID)
		ON DELETE	CASCADE
		ON UPDATE	CASCADE,
	FOREIGN KEY	(IMAGE_ID)
		REFERENCES	TIMAGE (IMAGE_ID)
		ON DELETE	CASCADE
		ON UPDATE	CASCADE
)
	TYPE=INNODB	-- FOREIGN KEY를 사용하기 위하여
;

/*
TSTORYBOARD
========================================================================
STORYBOARD_ID		-- WORK_ID
SCENE_ID		-- SCENE_ID = 0인 경우의 스토리보드 작업은 없으므로
			-- 이 값은 UNIQUE하다.
			-- 단지, 중복 작업을 피하기 위해서 사용한 COLUMN이다.
========================================================================
*/
CREATE TABLE IF NOT EXISTS TSTORYBOARD
(
	STORYBOARD_ID	INT UNSIGNED NOT NULL UNIQUE,	-- WORK_ID
	SCENE_ID	INT UNSIGNED NOT NULL UNIQUE,

	FOREIGN KEY	(SCENE_ID)
		REFERENCES	TSCENE (SCENE_ID)
		ON DELETE	CASCADE
		ON UPDATE	CASCADE,
	FOREIGN KEY	(STORYBOARD_ID)
		REFERENCES	TWORK (WORK_ID)
		ON DELETE	CASCADE
		ON UPDATE	CASCADE
)
	TYPE=INNODB	-- FOREIGN KEY를 사용하기 위하여
;

/*
STORYBOARD_ID를 만들 때, 적어도 하나의 컷(CUT #1)은 만든다.
TCUT
========================================================================
CUT_ID			-- UNIQUE
STORYBOARD_ID		-- (= WORK_ID) 같은 STORYBOARD_ID라면 같은 작업에 대한 결과이다.
SERIAL			-- 일련번호이다. (>= 1)
			-- (STORYBOARD_ID, SERIAL)쌍은 오직 하나만 있어야 한다.
IMAGE_ID		-- 결과 이미지에 대한 참조이다.
			-- 0: 아직 어떤 결과도 없다., >= 1: TIMAGE테이블의 해당 IMAGE_ID에 결과에 대한 정보가 있다.
========================================================================
 */
CREATE TABLE IF NOT EXISTS TCUT
(
	CUT_ID		INT UNSIGNED NOT NULL AUTO_INCREMENT UNIQUE,
	STORYBOARD_ID	INT UNSIGNED NOT NULL,
	SERIAL		INT UNSIGNED DEFAULT 1,
	IMAGE_ID	INT UNSIGNED DEFAULT 0,

	PRIMARY KEY	(STORYBOARD_ID, SERIAL),
	INDEX		(STORYBOARD_ID),
	FOREIGN KEY	(STORYBOARD_ID)
		REFERENCES	TSTORYBOARD (STORYBOARD_ID)
		ON DELETE	CASCADE
		ON UPDATE	CASCADE,
	FOREIGN KEY	(IMAGE_ID)
		REFERENCES	TIMAGE (IMAGE_ID)
		ON DELETE	CASCADE
		ON UPDATE	CASCADE
)
	TYPE=INNODB	-- FOREIGN KEY를 사용하기 위하여
;


/*
-- 참고자료의 갯수와 무관한 정보이다.
TREFERENCE
========================================================================
REF_ID			-- UNIQUE
CITE_TYPE		-- 1: 작품, 2: 장면, 3: 대상(인물/배경), 4: 작업
CITE_ID			-- 1: CITE_ID = SHOW_ID, 2: CITE_ID = SCENE_ID, 3: CITE_ID = TARGET_ID, 4: CITE_ID = WORK_ID
			-- (CYTE_TYPE, CITE_ID)쌍은 오직 하나만 있어야 한다.
========================================================================
 */
CREATE TABLE IF NOT EXISTS TREFERENCE
(
	REF_ID		INT UNSIGNED NOT NULL AUTO_INCREMENT UNIQUE, 
	CITE_TYPE	TINYINT UNSIGNED NOT NULL,	-- 1: 작품, 2: 장면, 3: 대상(인물/배경), 4: 작업(디자인/스토리보드)
	CITE_ID		INT UNSIGNED NOT NULL,		-- 1: SHOW_ID, 2: SCENE_ID, 3: TARGET_ID, 4: WORK_ID

	PRIMARY KEY	(CITE_TYPE, CITE_ID)		-- UNIQUE
)
	TYPE=INNODB		-- 사실 CITE_ID는 FOREIGN KEY이다. 하지만 어떻게 이를 알릴 수 있을까...
				-- 따라서 해당 CITE_ID가 변경되거나 없어지더라도 그와 관련된 레코드는 여전히 남아있다.
;

/*
-- 참고자료의 갯수와 관련된 정보이다.
-- REF_ID를 통해 TREFERENCE와 함께 사용해야만 의미가 있다.
TREFERENCE_SERIES
========================================================================
REF_ID			-- 같은 REF_ID라면 같은 장면이나 대상(인물/배경)에 대한 참고자료이다.
SERIAL			-- 일련번호이다. (>= 0)
			--
			-- (REF_ID, SERIAL)쌍은 오직 하나만 있어야 한다.
TYPE			-- 1: 그림, 2: HTML 문서
CONTENT			-- TYPE = 1: CONTENT = (저장된 그림 파일의 FILENAME_UUID, 원래 파일이름)
			-- TYPE = 2: CONTENT = HTML 문서의 내용
USER_ID			-- 누가 등록했는가?
========================================================================
 */
CREATE TABLE IF NOT EXISTS TREFERENCE_SERIES
(
	REF_ID		INT UNSIGNED NOT NULL,
	SERIAL		TINYINT UNSIGNED NOT NULL,	-- [0, 255]
	TYPE		TINYINT UNSIGNED NOT NULL,	-- 1: 그림, 2: HTML
	CONTENT		TEXT NOT NULL,
	USER_ID		INT UNSIGNED NOT NULL,

	PRIMARY KEY	(REF_ID, SERIAL),	-- UNIQUE
	INDEX		(REF_ID),
	INDEX		(USER_ID),
	FOREIGN KEY	(REF_ID)
		REFERENCES	TREFERENCE (REF_ID)
		ON DELETE	CASCADE,
	FOREIGN KEY	(USER_ID)
		REFERENCES	TUSER (USER_ID)
		ON DELETE	CASCADE
)
	TYPE=INNODB	-- REF_ID와 USER_ID를 FOREIGN KEY로 사용하기 위하여
;
