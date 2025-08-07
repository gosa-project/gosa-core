<div style="font-size:18px;">
  {t}Creating a new user using templates{/t}
</div>

<p>
 {t}Creating a new user can be assisted by using templates. Many database records will be filled automatically. Choose 'none' to skip the usage of templates.{/t}
</p>

<hr>

<br>

<table summary="{t}User template selection dialog{/t}" cellpadding=4 border=0>
  <tr>
    <td><b><LABEL for="template">{t}Template{/t}</LABEL></b></td>
    <td>
      <select size="1" name="template" id="template">
       {html_options options=$templates selected=$template}
      </select>
    </td>
  </tr>
  <tr>
    <td><b><LABEL for="sn">{t}Last name{/t}</LABEL></b></td>
    <td><input type='text' name="sn" id="sn" size="30" maxlength="40" value="{$sn}"></td>
  </tr>
  <tr>
    <td><b><LABEL for="givenName">{t}First name{/t}</LABEL></b></td>
    <td><input type='text' name="givenName" id="givenName" size="30" maxlength="40" value="{$givenName}"></td>
  </tr>
  {if $got_uid eq "true"}
  <tr>
    <td><b>{t}Login{/t}</b></td>
    <td>
      {if $edit_uid eq "false"}
        {if $allowUidProposalModification == "true"}
           <select size="1" name="uidSelect" id='uidSelect' onChange="
                document.getElementById('uid').value = document.getElementById('uidSelect').value;">
            {html_options output=$uids values=$uids selected=$uid}
          </select>
          <input type='text' id="uid" name="uid" size="30" maxlength="40" value="{$uid}" 
                style="display: none;">
          <img src="images/lists/edit.png" onClick="$('uid').toggle();" 
                title="{t}Modify the uid proposal{/t}">
        {else}
           <select size="1" name="uid">
            {html_options output=$uids values=$uids selected=$uid}
          </select>
        {/if}
      {else}                    
          <input type='text' id="uid" name="uid" size="30" maxlength="40" value="{$uid}">
      {/if}
    </td>
  </tr>
  {/if}
</table>


<hr>
<div class="plugin-actions">
 <button type='submit' name='template_continue'>{t}Continue{/t}</button>
 <button type='submit' name='edit_cancel'>{msgPool type=cancelButton}</button>
</div>



<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('sn');
  -->
</script>
