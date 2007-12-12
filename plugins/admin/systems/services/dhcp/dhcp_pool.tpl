{* GOsa dhcp sharedNetwork - smarty template *}
<p><b>{t}Generic{/t}</b></p>
<table width="100%">
 <tr>
  <td width="50%">
   {t}Name{/t}{$must}&nbsp;
   <input id='cn' type='text' name='cn' size='25' maxlength='80' value='{$cn}'
        title='{t}Name of pool{/t}'>
  </td>
  <td>
   {t}Range{/t}{$must}&nbsp;
   <input type='text' name='range_start' size='25' maxlength='30' value='{$range_start}'>
   &nbsp;-&nbsp;
   <input type='text' name='range_stop' size='25' maxlength='30' value='{$range_stop}'>
  </td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<!-- Place cursor in correct field -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.cn.focus();
  -->
</script>
