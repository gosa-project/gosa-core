{* GOsa dhcp subnet - smarty template *}
<br>
<table width="100%">
 <tr>
  <td width="50%">
   <b>{t}Network address{/t}{$must}</b>&nbsp;
   <input type='text' name='na0' size='3' maxlength='3' value='{$na0}' {$mode}><b>.</b>
   <input type='text' name='na1' size='3' maxlength='3' value='{$na1}' {$mode}><b>.</b>
   <input type='text' name='na2' size='3' maxlength='3' value='{$na2}' {$mode}><b>.</b>
   <input type='text' name='na3' size='3' maxlength='3' value='{$na3}' {$mode}>
  </td>
  <td>
   <b>{t}Netmask{/t}{$must}</b>&nbsp;
   <input type='text' name='nm0' size='3' maxlength='3' value='{$nm0}'><b>.</b>
   <input type='text' name='nm1' size='3' maxlength='3' value='{$nm1}'><b>.</b>
   <input type='text' name='nm2' size='3' maxlength='3' value='{$nm2}'><b>.</b>
   <input type='text' name='nm3' size='3' maxlength='3' value='{$nm3}'>
  </td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<table width="100%">
 <tr>
  <td>
   <b>{t}Range for dynamic address assignement{/t}</b>&nbsp;
   <input type='text' name='r00' size='3' maxlength='3' value='{$r00}'><b>.</b>
   <input type='text' name='r01' size='3' maxlength='3' value='{$r01}'><b>.</b>
   <input type='text' name='r02' size='3' maxlength='3' value='{$r02}'><b>.</b>
   <input type='text' name='r03' size='3' maxlength='3' value='{$r03}'>&nbsp;<b>-</b>&nbsp;
   <input type='text' name='r10' size='3' maxlength='3' value='{$r10}'><b>.</b>
   <input type='text' name='r11' size='3' maxlength='3' value='{$r11}'><b>.</b>
   <input type='text' name='r12' size='3' maxlength='3' value='{$r12}'><b>.</b>
   <input type='text' name='r13' size='3' maxlength='3' value='{$r13}'>
  </td>
 </tr>
</table>

<div style="height:150px;"></div>

<!-- Place cursor in correct field -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.na0.focus();
  -->
</script>
