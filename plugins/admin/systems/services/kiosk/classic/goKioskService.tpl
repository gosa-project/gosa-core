<h3><LABEL for="gotoKioskProfile">{t}Kiosk profile management{/t}</LABEL></h3>

{if $baseDir == ""}

  <b>{msgPool type=invalidConfigurationAttribute param=KIOSKPATH}</b>
  <hr>
  <div class="plugin-actions">
   <button type='submit' name='CancelService'>{msgPool type=cancelButton}</button>
  </div>

{else}

<input type="hidden" name="dialogissubmitted" value="1">

{t}Server path{/t}&nbsp;<input type='text' name="server_path" style="width:300px;" value="{$server_path}">

{render acl=$ThisACL}
 {$kioskList}
{/render}

{render acl=$ThisACL}
 <input type="file" size=50 name="newProfile" value="{t}Browse{/t}">
{/render}

{render acl=$ThisACL}
 <button type='submit' name='profileAdd'>{msgPool type=addButton}</button>
{/render}

<hr>
<div class="plugin-actions">
 <button type='submit' name='SaveService'>{msgPool type=saveButton}</button>
 <button type='submit' name='CancelService'>{msgPool type=cancelButton}</button>
</div>

<input type="hidden" name="goKioskPosted" value="1">

<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
    focus_field('gotoKioskProfile');
  -->
</script>
{/if}
