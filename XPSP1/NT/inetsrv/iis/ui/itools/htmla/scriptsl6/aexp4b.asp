<%@Language="VBScript"%>
<HTML>

<title>IIS - Authentication Manager</title>

<FONT COLOR=FFFFFF>
<STYLE>
  A  {text-decoration: none}
</STYLE>
</FONT>

<BODY BGCOLOR=#FFFFFF LINK=000000 VLINK=000000>

<%On Error goto 0%>
<%if Request.Form("cancel") <> "" then
	Response.Redirect(Request.QueryString) 
end if
Response.End
%>

<!-- Windows NT Server with IIS  -->
<%if Instr(1,Request.ServerVariables("SERVER_SOFTWARE"), "IIS") > 0 then%>
	<TABLE BORDER=0 CELLPADDING=0 CELLSPACING=0>
	<TR VALIGN=CENTER>
		<TD></TD>
		<TD WIDTH=20> </TD>
		<TD><FONT SIZE=+3 COLOR=#000000><B>Internet Service Manager<BR> <FONT SIZE=-1>for Internet Information Server 5.0<FONT></B></FONT></TD>
	</TR>
	</Table>
<%end if%>   

<!-- Windows NT Workstation with PWS  -->
<%if Instr(1,Request.ServerVariables("SERVER_SOFTWARE"), "PWS") then%>
<TABLE BORDER=0 CELLPADDING=0 CELLSPACING=0>
<TR VALIGN=CENTER>
	<TD></TD>
	<TD WIDTH=20> </TD>
	<TD><FONT SIZE=+3 COLOR=#000000><B>Internet Service Manager<BR> <FONT SIZE=-1>for Peer Web Services<FONT></B></FONT></TD>
</TR>
</Table>
<%end if%>

<p>

<form method="POST" action="http://<%=Request.ServerVariables("SERVER_NAME")%>/iisadmpwd/achg.asp?<%=Request.QueryString%>">

<table>
<tr>
<td>Domain</td><td><input type="text" name="domain" value="
<%if Request.Form("domain") <> "" then %>
	<%=Request.Form("domain")%>
<%else%>
	<%
	Set objNet = Server.CreateObject("WScript.Network")
	Response.Write objNet.UserDomain
%>
<%end if%>"></td></tr>
<tr>
<td>Account</td><td><input type="text" name="acct" value="
<%if Request.Form("acct") <> "" then%>
	<%=Request.Form("acct")%>
<%else%>
	<%=Right(Request.ServerVariables("REMOTE_USER"),len(Request.ServerVariables("REMOTE_USER")) - Instr(1,Request.ServerVariables("REMOTE_USER"),"\"))%>
<%end if%>"></td>
</tr>
<tr>
<td>Old password</td><td><input type="password" name="old" value=""></td>
</tr>
<tr>
<td>New password</td><td><input type="password" name="new" value=""></td>
</tr>
<tr>
<td>Confirm new password</td><td><input type="password" name="new2" value=""></td>
</tr>
</table>

<p>

<input type="submit" value="     OK     ">
<input type="submit" name="cancel" value=" Cancel ">
<input type="reset" value="  Reset  ">

</form>
</body>
</html>
