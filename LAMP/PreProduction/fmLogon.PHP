<?PHP
//phpinfo();
	// Connecting, selecting database
	$link = mysql_connect('localhost', 'root', 'MySQL-4.1.14') or die('Could not connect: ' . mysql_error());
	mysql_select_db('PREPRODUCTION') or die('Could not select database');
	if (isset($_POST['LOGOFF'])) {
		setcookie('User', '', time() - 3600);
		unset($_COOKIE['User']);
	}
	if (isset($_COOKIE['User'])) {
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML>
<HEAD>
<TITLE>개인</TITLE>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; CHARSET=euc-kr">
<SCRIPT LANGUAGE="JavaScript1.3">
	function fnLogOff() {
		// 데이터를 서버로 보내는 데 필요한 FORM을 만든다.
		var	oLogoff = document.createElement('<FORM METHOD="POST" ACTION="fmLogon.PHP">');
		document.appendChild(oLogoff);

		// Submit Type:
		var	oSubmitType = document.createElement('<INPUT TYPE="TEXT" NAME="LOGOFF" VALUE="">');
		oLogoff.appendChild(oSubmitType);

		oLogoff.submit();
	}
</SCRIPT>
</HEAD>
<BODY>
<UL>
<LI><A HREF="http://203.241.190.23/PreProduction/fmDesign.PHP">디자인 작업</A></LI>
<LI><A HREF="http://203.241.190.23/PreProduction/fmStoryboard.PHP">스토리보드 작업</A></LI>
</UL>
<INPUT TYPE="BUTTON" VALUE="로그오프" onClick="fnLogOff();">
</BODY>
</HTML>
<?PHP
	} else if (isset($_GET['LOGON'])) {
		global	$gMsg;
		if (isset($_GET['NICKNAME']) && isset($_GET['PASSWORD'])) {
			$query = sprintf(
				"SELECT USER_ID FROM TUSER WHERE NICKNAME = '%s' AND PASSWORD_SHA1 = SHA1('%s')",
				mysql_real_escape_string($_GET['NICKNAME']),
				mysql_real_escape_string($_GET['PASSWORD'])
			);
			if (!($result = mysql_query($query))) {
				$message = '';
				$message .= 'errno: |' . mysql_errno() . '|' . "\n";
				$message .= 'error: |' . mysql_error() . '|' . "\n";
				$message .= 'query: |' . $query . '|' . "\n";
				die($message);
			}
			if (!($row = mysql_fetch_array($result, MYSQL_ASSOC))) {
				$gMsg = '아이디("' . $_GET['NICKNAME'] . '")와 패스워드("' . $_GET['PASSWORD'] . '")가 일치하지 않습니다';
			} else {
				setcookie('User', $row['USER_ID'] . ':' . $_GET['NICKNAME']);
			}
			mysql_free_result($result);
		} else {
			$gMsg = '아이디와 패스워드가 존재해야 합니다';
		}
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML>
<HEAD>
<TITLE>fmLogon: 로그온...</TITLE>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; CHARSET=euc-kr">
</HEAD>
<BODY>
<PRE>
</PRE>
<SCRIPT LANGUAGE="JavaScript1.3">
	var	msg = '<?PHP echo $gMsg ?>';
	if (msg != '') {
		alert(msg);
	}
	location.replace("fmLogon.PHP");
</SCRIPT>
</BODY>
</HTML>
<?PHP
	} else {
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML>
<HEAD>
<TITLE>fmLogon.PHP</TITLE>
<META HTTP-EQUIV="Content-Type" CONTENT="text/html; CHARSET=euc-kr">
<SCRIPT LANGUAGE="JavaScript1.3">
</SCRIPT>
</HEAD>
<BODY>
<FORM ACTION="fmLogon.PHP">
<DIV ALIGN="CENTER">
<TABLE WIDTH="40%" BORDER="1" CELLSPACING="0" CELLPADDING="0"><TBODY>
<TR><TD WIDTH="100%"><TABLE WIDTH="100%" BORDER="1" CELLSPACING="0" CELLPADDING="0"><TBODY>
	<TR>
	<TD ALIGN="RIGHT" WIDTH="30%">아이디</TD>
	<TD WIDTH="70%"><INPUT TYPE="TEXT" NAME="NICKNAME" VALUE=""></TD>
	</TR>
	<TR>
	<TD ALIGN="RIGHT" WIDTH="30%">패스워드</TD>
	<TD WIDTH="70%"><INPUT TYPE="PASSWORD" NAME="PASSWORD" VALUE=""></TD>
	</TR>
	<TR><TD COLSPAN="2" ALIGN="CENTER"><INPUT TYPE="SUBMIT" NAME="LOGON" VALUE="로그온"></TD></TR>
</TBODY></TABLE></TD></TR>
</TBODY></TABLE>
</DIV>
</FORM>
</BODY>
</HTML>
<?PHP
	}
	// Closing connection
	mysql_close($link);
?>
