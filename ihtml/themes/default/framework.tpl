<!-- Headline -->
<body>
{$php_errors}
<form action='main.php{$plug}' name='mainform' method='post' enctype='multipart/form-data'>
<table summary="" class="framework" cellspacing="0" cellpadding="0">
  <tr>
    <td style="height:36px; width:140px; vertical-align:bottom; text-align:center">
     <a href="main.php" style="vertical-align:bottom;"><img src='{$go_logo}' border="0" alt="GOSA2" style='vertical-align:bottom;'></a>
    </td>
    <td>
      <table style="margin-left:20px;" border="0" summary="">
       <tr>
        <td style="vertical-align:middle;">
	{if !$javascript}
	 <a class="maintitlebar" href='main.php?reset=1' onClick='return question("{t}You are currently editing a database entry. Do you want to dismiss the changes?{/t}", "main.php?reset=1");' >
	{else}
	 <a class="maintitlebar" onClick='return question("{t}You are currently editing a database entry. Do you want to dismiss the changes?{/t}", "main.php?reset=1");' >
	{/if}
	  <img src='{$go_home}' class='center' border="0" alt="Main">&nbsp;{t}Main{/t}
	 </a>
	</td>
        <td>
	 <a class="maintitlebar" style="margin-left:35px" {$help_method}>
          <img src='{$go_help}' class='center' border="0" alt="Help">&nbsp;{t}Help{/t}
         </a>
	</td>
	<td>
	{if !$javascript}
	 <a class="maintitlebar" href='logout.php?request' onClick='return question("{t}You are currently editing a database entry. Do you want to dismiss the changes?{/t}", "logout.php?request");' style="margin-left:35px;">
          <img src='{$go_out}' class='center' border="0" alt="Sign out">&nbsp;{t}Sign out{/t}
         </a>
	{else}
	 <a class="maintitlebar" onClick='return question("{t}You are currently editing a database entry. Do you want to dismiss the changes?{/t}", "logout.php?request");' style="margin-left:35px;">
          <img src='{$go_out}' class='center' border="0" alt="Sign out">&nbsp;{t}Sign out{/t}
         </a>
	{/if}
	</td>
       </tr>
      </table>
    </td>
    <td style="text-align:right;">
      <div class="maintitlebar" style="padding-right:10px;text-align:right">
        <a>{t}Signed in:{/t} <b>{$username}</b></a>
      </div>
    </td>
  </tr>

  <tr style="vertical-align:top;">
    <!-- Menu -->
    <td style="background-color:#EEFFE6; vertical-align:top;border-bottom:1px solid #778F6E;" id="menucell">
      <div style="background-color:#CDF6BD; height:12px; width:100%;">&nbsp;</div>
      <div style="height:650px">
      {$menu}
      </div>
	  <br />
    </td>
    <!-- Plugin window -->
    <td style="background-color:white; width:100%; vertical-align:top; height:15px; border-left:1px solid #778F6E; border-top:1px solid #778F6E" colspan="2">
      <table style="vertical-align:top; text-align:justify; background-color: #F8F8F8; margin-top:7px; margin-left:7px; margin-right:7px; width:99%" summary="">
       <tr>
        <td>
	 {$contents}
       </td>
      </tr>
     </table>
    </td>
  </tr>
</table></form></body></html>
