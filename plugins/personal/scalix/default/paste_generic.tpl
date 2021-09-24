<div class="scalix-wrapper">
  <div class="row">
    <h3>{t}SCALIX email addresses{/t}</h3>

    <div class="input-field">
      <select id="emails_list" name="emails_list[]" size="15" multiple title="{t}List of SCALIX email addresses{/t}">
        {html_options values=$scalixEmailAddress output=$scalixEmailAddress}
      </select>
    </div>

    <div class="input-field add">
      <input type='text' name="email_address" size="30" align="middle" maxlength="65" value="">
      <button class="btn-small" type='submit' name='add_email'>{msgPool type=addButton}</button>
      <button class="btn-small" type='submit' name='delete_email'>{msgPool type=delButton}</button>
    </div>
  </div>
</div>

<input type="hidden" name="scalixTab" value="scalixTab">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  // First input field on page
  document.mainform.scalixMailnode.focus();
</script>
