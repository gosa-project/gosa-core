<div style="font-size:18px;">
  {t}Creating a new user using templates{/t}
</div>

<p>
  {t}Creating a new user can be assisted by using templates. Many database records will be filled automatically. Choose <b>none</b> to skip the usage of templates.{/t}
</p>

<table style="vertical-align:top; text-align:left;" cellpadding=4 border=0>
  <tr>
    <td><b>{t}Template{/t}</b></td>
    <td>
      <select size="1" name="template">
       {html_options options=$templates selected=$template}
      </select>
    </td>
  </tr>
  <tr>
    <td><b>{t}Name{/t}</b></td>
    <td><input name="sn" size="30" maxlength="40" value="{$sn}"></td>
  </tr>
  <tr>
    <td><b>{t}Given name{/t}</b></td>
    <td><input name="givenName" size="30" maxlength="40" value="{$givenName}"></td>
  </tr>
  {if $got_uid eq "true"}
  <tr>
    <td><b>{t}Login{/t}</b></td>
    <td>
      {if $edit_uid eq "false"}
      <select size="1" name="uid">
        {html_options output=$uids values=$uids selected=$uid}
      </select>
      {else}
      <input name="uid" size="30" maxlength="40" value="{$uid}">
      {/if}
    </td>
  </tr>
  {/if}
</table>

<br>
<p class="plugbottom">
  <input type=submit name="template_continue" value="{t}Continue{/t}">
  &nbsp;
  <input type=submit name="edit_cancel" value="{t}Cancel{/t}">
</p>



<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.sn.focus();
  -->
</script>
