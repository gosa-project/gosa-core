{* GOsa dhcp sharedNetwork - smarty template *}

<h3>{t}Generic{/t}</h3>

<table summary="{t}DHCP group settings{/t}">
 <tr>
  <td>{t}Name{/t}{$must}</td>
  <td>
   {render acl=$acl}
    <input id='cn' type='text' name='cn' value='{$cn}' title='{t}Name of group{/t}'>
   {/render}
  </td>
 </tr>
</table>

<hr>

<!-- Place cursor in correct field -->
<script language="JavaScript" type="text/javascript">
 <!-- // First input field on page	 
  focus_field('cn');  
 -->
</script>
