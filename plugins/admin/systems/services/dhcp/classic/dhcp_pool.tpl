{* GOsa dhcp sharedNetwork - smarty template *}

<h3>{t}Generic{/t}</h3>
<table width="100%" summary="{t}DHCP pool settings{/t}">
 <tr>
  <td width="50%">
   {t}Name{/t}{$must}
   {render acl=$acl}
    <input id='cn' type='text' name='cn' size='25' maxlength='80' value='{$cn}'
      title='{t}Name of pool{/t}'>
   {/render}
  </td>
  <td>{t}Range{/t}
   {$must}&nbsp;
   {render acl=$acl}
    <input type='text' name='range_start' size='25' maxlength='30' value='{$range_start}'>
   {/render}&nbsp;-&nbsp;
   {render acl=$acl}
    <input type='text' name='range_stop' size='25' maxlength='30' value='{$range_stop}'>
   {/render}
  </td>
 </tr>
</table>

<hr>

<!-- Place cursor in correct field -->
<script language="JavaScript" type="text/javascript">
<!-- // First input field on page  
  document.mainform.cn.focus();  
-->
</script>
