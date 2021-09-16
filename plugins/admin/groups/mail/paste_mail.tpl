<h3>{t}Mail settings{/t}</h3>
<table style='width:100%; ' summary="{t}Paste mail settings{/t}">

 	<tr>
  		<td colspan=2>   
   			<table summary="{t}Address settings{/t}">
    			<tr>
     				<td>
						<LABEL for="mail">{t}Primary address{/t}</LABEL>{$must}
					</td>
     				<td>
						<input id="mail" name="mail" size="25" maxlength="65" type='text' 
							value="{$mail}" title="{t}Primary mail address for this shared folder{/t}">
					</td>
    			</tr>
   			</table>
  		</td>
 	</tr>
 	<tr>
		<td>
			<table style='width:100%;' summary="{t}Additional mail settings{/t}">

				<tr>
					<td style='width:50%; ' class='right-border'>

							{t}Alternative addresses{/t}
						<select style="width:100%;" name="alternates_list[]" size=10 multiple title="{t}List of alternative mail addresses{/t}">
							{html_options values=$gosaMailAlternateAddress output=$gosaMailAlternateAddress}
							<option disabled>&nbsp;</option>
						</select>
						<br>
						<input type='text' name="alternate_address" size="30" align=middle maxlength="60" value="">
						<button type='submit' name='add_alternate'>{msgPool type=addButton}</button>&nbsp;

						<button type='submit' name='delete_alternate'>{msgPool type=delButton}</button>

					</td>
					<td style='width:50%'>

							{t}Forward messages to non group members{/t}
						<select style="width:100%;" name="forwarder_list[]" size=10 multiple>
							{html_options values=$gosaMailForwardingAddress output=$gosaMailForwardingAddress}
							<option disabled>&nbsp;</option>
						</select>
						<br>
						<input type='text' name="forward_address" size=20 align=middle maxlength=65 value="">
						<button type='submit' name='add_forwarder'>{msgPool type=addButton}</button>&nbsp;

						<button type='submit' name='add_local_forwarder'>{t}Add local{/t}</button>&nbsp;

						<button type='submit' name='delete_forwarder'>{msgPool type=delButton}</button>

					</td>
				</tr>
			</table>
		<td>
	</tr>
</table>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('mail');
  -->
</script>
