<div class="faxaccount-wrapper">
	<div class="row">
		<div class="col s12 xl6">
			<h3>{t}Generic{/t}</h3>

			<div class="input-field add">
				<input name="facsimileTelephoneNumber" id="facsimileTelephoneNumber" size=40 maxlength=65 
					value="{$facsimileTelephoneNumber}" title="{t}Fax number for GOfax to trigger on{/t}">
				<label for="facsimileTelephoneNumber">{t}Fax{/t}{$must}</label>
				{t}Alternate fax numbers will not be copied{/t}
			</div>
		</div>
	</div>
</div>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  	// First input field on page
	focus_field('facsimileTelephoneNumber');
</script>
