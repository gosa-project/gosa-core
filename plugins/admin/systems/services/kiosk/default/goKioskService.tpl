<div class="gokioskservice-wrapper">
	<div class="row">
		<div class="col s12 xl6">
      <h3>{t}Kiosk profile management{/t}</h3>

      {if $baseDir == ""}
        <p><b>{msgPool type=invalidConfigurationAttribute param=KIOSKPATH}</b></p>

        <div class="plugin-actions card-action">
          <button class="btn-small primary" type='submit' name='CancelService'>{msgPool type=cancelButton}</button>
        </div>
      {else}
        <input type="hidden" name="dialogissubmitted" value="1">

        <div class="input-field">
            <input type='text' id="server_path" name="server_path" value="{$server_path}">
            <label for="server_path">{t}Server path{/t}</label>
        </div>

        {render acl=$ThisACL}
          {$kioskList}

          <div class="input-field add">
            <input type="file" size=50 name="newProfile" value="{t}Browse{/t}">
            <button type='submit' name='profileAdd'>{msgPool type=addButton}</button>
          </div>
        {/render}

        <div class="plugin-actions card-action">
          <button class="btn-small primary" type='submit' name='SaveService'>{msgPool type=saveButton}</button>
          <button class="btn-small primary" type='submit' name='CancelService'>{msgPool type=cancelButton}</button>
        </div>

        <input type="hidden" name="goKioskPosted" value="1">

        <script language="JavaScript" type="text/javascript">
          <!-- // First input field on page
            focus_field('gotoKioskProfile');
          -->
        </script>
      {/if}
	  </div>
	</div>
</div>
