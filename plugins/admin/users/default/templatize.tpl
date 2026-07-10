<div class="template-wrapper">
  <h2>{t}Applying a template{/t}</h2>

  <p>
    {t}Applying a template to several users will replace all user attributes defined in the template.{/t}
  </p>

  {if $templates}
    <div class="row">
      <div class="col s6">
        <div class="input-field">
          <select size="1" name="template" id="template">
            {html_options options=$templates}
          </select>
          <label for="template">{t}Template{/t}</label>
        </div>
      </div>
    </div>

    <div class="plugin-actions card-action">
      <button class="btn-small primary" type='submit' name='templatize_continue'>{msgPool type=applyButton}</button>
      <button class="btn-small primary" type='submit' name='edit_cancel'>{msgPool type=cancelButton}</button>
    </div>
  {else}
    <p class="no-templates">
      {t}No templates available!{/t}
    </p>

    <div class="plugin-actions card-action">
      <button class="btn-small primary" type='submit' name='edit_cancel'>{msgPool type=cancelButton}</button>
    </div>

  {/if}


  <!-- Place cursor -->
  <script language="JavaScript" type="text/javascript">
    <!-- // First input field on page
    focus_field('template');
    -->
  </script>
</div>
