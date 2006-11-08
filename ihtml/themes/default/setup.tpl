<body style='background-color:#A0A0A0;background-image:none'>
<form action="setup.php?check_globals=42" method="post" enctype="multipart/form-data">

<script language="JavaScript" type="text/javascript">
  <!--
  h=parseInt(window.innerHeight)-25;
  document.write('<div style="height:' + h + 'px";>');
  //-->
</script>

{$errors}

<table summary="" cellspacing=0 cellpadding=0 style="width:100%;height:100%;">
  <tr style="background-color:#CDF6C5">
    <td style="border-bottom:1px solid black; height:50px;">
      <img alt="" src="themes/default/images/go_logo.png">
    </td>
    <td style="text-align:right; vertical-align:top; border-bottom:1px solid black">
      setup.php
    </td>
  </tr>
  <tr style='height:100%;'>
    <td style="background-color: white;vertical-align:top;" colspan=2>

    {include file="$content"}

    </td>
  </tr>
</table>

<script language="JavaScript" type="text/javascript">
  <!--
    h=parseInt(window.innerHeight)-20;
    document.write('<\/div>');
  //-->
</script>

</form>
</body>
</html>
