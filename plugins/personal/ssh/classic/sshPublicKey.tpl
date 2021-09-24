<p class="contentboxh" style="font-size:12px">
  <b>{t}List of SSH public keys for this user{/t}</b><br>
</p>
<p class="contentboxb" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
  <select style="width:100%; margin-top:4px; height:450px;" name="keylist[]" size="15" multiple>
     {html_options options=$keylist}
  </select>
</p>
{render acl=$sshPublicKeyACL}
<input type=file name="key">
&nbsp;
<button type='submit' name='upload_sshpublickey'>{t}Upload key{/t}</button>

&nbsp;
<button type='submit' name='remove_sshpublickey'>{t}Remove key{/t}</button>

{/render}

<hr>
<div class="plugin-actions">
  <button type='submit' name='save_sshpublickey'>{msgPool type=saveButton}</button>
  <button type='submit' name='cancel_sshpublickey'>{msgPool type=cancelButton}</button>
</div>

