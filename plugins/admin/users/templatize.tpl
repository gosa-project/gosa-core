<div style="font-size:18px;">
  {t}Applying a template{/t}
</div>

<p>
 {t}Applying a template to several users will replace all user attributes defined in the template.{/t}
</p>

<hr>
<br>

{if $templates}
<table summary="{t}Apply user template{/t}" cellpadding=4 border=0>
  <tr>
    <td><b><LABEL for="template">{t}Template{/t}</LABEL></b></td>
    <td>
      <select size="1" name="template" id="template">
       {html_options options=$templates}
      </select>
    </td>
  </tr>
</table>

<hr>
<div class="plugin-actions">
  <button type='submit' name='templatize_continue'>{msgPool type=applyButton}</button>
  <button type='submit' name='edit_cancel'>{msgPool type=cancelButton}</button>
</div>

{else}

  {t}No templates available!{/t}

  <hr>
  <div class="plugin-actions">
    <button type='submit' name='edit_cancel'>{msgPool type=cancelButton}</button>
  </div>

{/if}


<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('template');
  -->
</script>
