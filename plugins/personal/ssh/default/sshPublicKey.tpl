<div class="ssh-wrapper">
  <div class="row">
    <div class="col s12 xl6">
      <h2>{t}Edit public ssh keys{/t}</h2>
  
      <div class="input-field">
        <select name="keylist[]" size="15" multiple>
           {html_options options=$keylist}
        </select>
        <label>{t}List of SSH public keys for this user{/t}</label>
      </div>

      {render acl=$sshPublicKeyACL}
      <div class="file-field input-field">
        <div class="btn-small">
          <span>File</span>
          <input type=file name="key">
        </div>
        <div class="file-path-wrapper">
          <input class="file-path validate" type="text" placeholder="Upload files">
        </div>
      </div>
      <button class="btn-small" type='submit' name='upload_sshpublickey'>{t}Upload key{/t}</button>
      <button class="btn-small" type='submit' name='remove_sshpublickey'>{t}Remove key{/t}</button>
      {/render}
    </div>
  </div>
</div>


<div class="card-action">
  <button class="btn-small primary" type='submit' name='save_sshpublickey'>{msgPool type=saveButton}</button>
  <button class="btn-small primary" type='submit' name='cancel_sshpublickey'>{msgPool type=cancelButton}</button>
</div>

