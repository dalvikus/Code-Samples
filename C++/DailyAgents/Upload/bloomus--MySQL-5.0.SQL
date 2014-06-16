USE DBBloomUs;

CREATE TABLE IF NOT EXISTS TDaily
(
	DailyID		INT UNSIGNED NOT NULL AUTO_INCREMENT UNIQUE, 

	Flag		SMALLINT UNSIGNED NOT NULL DEFAULT 0,	--	
						--	1:= A recored is deleted

	DateTime	DATETIME NOT NULL,
	DelDateTime	DATETIME,		--	Default:= NULL	Tell when the record is deleted.

	GroupName	CHAR(8) NOT NULL,	-- GroupID
	UserName	CHAR(16) NOT NULL,	-- UserID

	ShowName	CHAR(16) NOT NULL,	-- ShowID	[A-Za-z]{2,16}
	SeqName		CHAR(17) NOT NULL DEFAULT '',	-- SeqID	[A-Za-z0-9]{1,8}[\.[A-Za-z0-9]{1,8}]
	ShotName	CHAR(8) NOT NULL,	-- ShotID	[A-Za-z]{2,}[1-9]\d?

												/* linux/limits.h */
--	PathName	VARCHAR(4096) NOT NULL,	-- (PathID)	NFS Path...			PATH_MAX
--	GivenFileName	CHAR(255) NOT NULL,	--		Without \d{4}.FileType		NAME_MAX
	Revision	CHAR(3) NOT NULL,	--		r[1-9]\d?
	Frame		CHAR(64) NOT NULL,	--		1[-10[:2]][,...]
	UniqueFileName0	CHAR(255) NOT NULL,	-- (FileID)	Date.ShowName[.SeqName].ShotName.GroupName.UserName.Revision.UUID
						--						NAME_MAX
	ImageType	CHAR(4) NOT NULL DEFAULT 'TIFF',	-- (TypeID)	TIFF, GIF

	Comment		CHAR(255)
);
