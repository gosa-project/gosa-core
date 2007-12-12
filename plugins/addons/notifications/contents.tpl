{if $finished eq "false"}
<h2>{t}Notification target{/t}</h2>

<table style="width:100%">
 <tr>
  <td style="width:48%; vertical-align:top;">
   {t}Use target from{/t}
   <select name="target" onChange="document.mainform.submit()">
        {html_options options=$targets selected=$target}
        <option disabled>&nbsp;</option>
   </select>
   {if $javascript eq 'false'}<input type="submit" value="{t}Apply{/t}" name="refresh">{/if}<br><br>
  </td>
  <td>
  </td>
  <td>
  </td>
 </tr>
 <tr>
  <td>
   {t}Available recipients{/t}<br>
   <select style="width:100%;height:180px;" name="source[]" size="20" multiple title="{t}List message possible targets{/t}">
            {html_options options=$sources}
	    <option disabled>&nbsp;</option>
   </select>
  </td>
  <td style="vertical-align:center; text-align:center">
   <input type="submit" value="&gt;" name="add">
   <br><br>
   <input type="submit" value="&lt;" name="del">
  </td>
  <td style="width:48%; vertical-align:top;">
   {t}Recipients{/t}<br>
   <select style="width:100%;height:180px;" name="recipient[]" size="20" multiple title="{t}List message recipients{/t}">
            {html_options options=$recipients}
	    <option disabled>&nbsp;</option>
   </select>
  </td>
 </tr>
</table>


<p class="seperator">&nbsp;</p>
<h2>{t}Message{/t}</h2>
{render acl=$nmessageACL}
  <textarea id="nmessage" style="width:99%; height:180px;" name="nmessage" rows="4" cols="512" >{$nmessage}</textarea>
{/render}

{if $show_templates eq "true"}
<select name="nmessage_template">
	{html_options options=$message_templates selected=$template}
	<option disabled>&nbsp;</option>
</select>
<input type="submit" value="{t}Import{/t}" name="import_template">
{/if}

<p class="seperator">&nbsp;</p>
<div style='text-align:right;margin-top:5px'>
{render acl=$nmessageACL}
        <input type="submit" name="send" value="{t}Send message{/t}">
{/render}
</div>


<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.nmessage.focus();
  -->
</script>
{else}
<h2>{t}Notification send!{/t}</h2>
<p class="seperator">&nbsp;</p>
<br>
{t}Your message has been sent successfully. Press the continue button to get back to the notification plugin.{/t}
<p class="seperator">&nbsp;</p>
<div style='text-align:right;margin-top:5px'>
        <input type="submit" name="continue" value="{t}Continue{/t}">
</div>
{/if}
