{* GOsa dhcp host - smarty template *}
<p><b>{t}Generic{/t}</b></p>
<table width="100%">
 <tr>
  <td width="50%" style="vertical-align:top">
   <table>
    <tr>
     <td>{t}Name{/t}{$must}</td>
     <td>
      <input {if $realGosaHost} disabled {/if} id='cn' type='text' name='cn' size='25' maxlength='80' value='{$cn}'
             title='{t}Name of host{/t}'>
     </td>
    </tr>
    <tr>
     <td>{t}Fixed address{/t}</td>
     <td>
      <input {if $realGosaHost} disabled {/if} 
			type='text' name='fixedaddr' size='25' maxlength='80' value='{$fixedaddr}'
             title='{t}Use hostname or IP-address to assign fixed address{/t}'>
     </td>
    </tr>
   </table>
  </td>
  <td>
   <table>
    <tr>
     <td>{t}Hardware type{/t}</td>
     <td>
      <select name='hwtype'>
       {html_options options=$hwtypes selected=$hwtype}
      </select>
     </td>
    </tr>
    <tr>
     <td>{t}Hardware address{/t}{$must}</td>
     <td>
      <input type='text' name='dhcpHWAddress' size='20' maxlength='18' value='{$dhcpHWAddress}'>
     </td>
    </tr>
   </table>
  </td>
 </tr>
</table>

<p class="seperator"></p>

<!-- Place cursor in correct field -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	 focus_field('cn');
  -->
</script>
