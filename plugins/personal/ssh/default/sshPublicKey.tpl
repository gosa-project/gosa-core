<div class="ssh-wrapper">
  <h2>{t}Edit public ssh keys{/t}</h2>

  <div class="row">
    <div class="col s12 xl6">
      <div class="input-field">
        <select name="keylist[]" size="15" multiple>
           {html_options options=$keylist}
        </select>
        <label>{t}List of SSH public keys for this user{/t}</label>
      </div>

      {render acl=$sshPublicKeyACL}
        <div class="ssh-action">
          <div class="file-field input-field">
            <div class="btn-small">
              <span>{t}Browse{/t}</span>
              <input type=file name="key" onchange="enable_upload(this.value)">
            </div>
            <div class="file-path-wrapper">
              <input class="file-path validate" type="text">
            </div>
          </div>
          <button class="btn-small" type='submit' name='upload_sshpublickey' id="upload_sshpublickey" disabled>{t}Upload key{/t}</button>
          <button class="btn-small" type='submit' name='remove_sshpublickey'>{t}Remove key{/t}</button>
        </div>
      {/render}
    </div>
  </div>
</div>


<div class="card-action">
  <button class="btn-small primary" type='submit' name='save_sshpublickey'>{msgPool type=saveButton}</button>
  <button class="btn-small primary" type='submit' name='cancel_sshpublickey'>{msgPool type=cancelButton}</button>
</div>
