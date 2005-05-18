<!-- Headline -->
{$php_errors}
<form action='main.php{$plug}' name='mainform' method='post' enctype='multipart/form-data'>
<script language="JavaScript" type="text/javascript">
  <!--
  h=parseInt(window.innerHeight)-25;
  document.write('<div style="height:' + h + 'px">');
  //-->
</script>
<table width="100%" border="0" cellspacing="0" cellpadding="3" bgcolor="#CDF6BD">
  <tr>
    <td>
      <a class=maintitlebar href='main.php?reset=1'>
        {t}Main{/t}
      </a> | <a class=maintitlebar >
        {t}Help{/t}
      </a> | <a class=maintitlebar href='logout.php'>
        {t}Sign out{/t}
      </a>
    </td>
    <td align=right>
      <div class=maintitlebar>
        <a>{t}Signed in:{/t} <b>{$username}</b></a>
      </div>
    </td>
  </tr>
</table>

<!-- Menu and plugin window -->
<table class='maintab' cellspacing='0' cellpadding='0'>
  <tr style="vertical-align:top;">
    <!-- Menu -->
    <td style="background-color:#EEFFE6; vertical-align:top;">
      <a href="main.php?version=1"><img src='{$go_logo}' border="0" alt="GOSA2"></a>
      {$menu}
      <div style="height:100px;"></div>
    </td>

    <!-- Spacer with shadow -->
    <td style="background:url({$go_left}); vertical-align:top; height:100%;">
      <img src="{$go_corner}" border="0" ALT="*">
      <div style="height:1px;">
    </td>

    <!-- Plugin window -->
    <td style="background-color:white; width:100%; vertical-align:top; height:15px;">
      <div style="width:100%; background:url({$go_top}); background-repeat:repeat-x;">
        &nbsp;
      </div>
      <div style="vertical-align:top; text-align:justify; margin-right:10px; padding: 5px; background-color: #F5F5F5; margin-top:0px;">
       {$contents}
     </div>
    </td>
  </tr>
</table>

<script language="JavaScript" type="text/javascript">
  <!--
  h=parseInt(window.innerHeight)-20;
  document.write('</div>');
  //-->
</script>
</form>

</body>
</html>
