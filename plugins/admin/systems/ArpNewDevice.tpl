<h2><img class="center" alt="" align="middle" src="images/rightarrow.png">{t}Properties{/t}</h2>
<table summary="">
<tr>
 <td style>
  <LABEL for="cn">
   {t}Dns name{/t}
  </LABEL>
 </td>
 <td>
  <input name="cn" id="cn" size=18 maxlength=60 value="{$cn}">
 </td>
</tr>
<tr>
 <td style>
  <LABEL for="description">
   {t}Description{/t}
  </LABEL>
 </td>
 <td>
  <input name="description" id="description" size=18 maxlength=60 value="{$description}">
 </td>
</tr>
</table>
{$netconfig}

<input type='hidden' name='ArpNewDevice_posted' value='1'>

