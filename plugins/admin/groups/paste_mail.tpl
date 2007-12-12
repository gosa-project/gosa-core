<h2>{t}Mail settings{/t}</h2>
<table summary="" style="width:100%; vertical-align:top; text-align:left;">
 	<tr>
  		<td colspan=2>   
   			<table summary="">
    			<tr>
     				<td>
						<LABEL for="mail">{t}Primary address{/t}</LABEL>{$must}
					</td>
     				<td>
						<input id="mail" name="mail" size="25" maxlength="65" 
							value="{$mail}" title="{t}Primary mail address for this shared folder{/t}">
					</td>
    			</tr>
   			</table>
  		</td>
 	</tr>
 	<tr>
		<td>
			<table style="width:100%; vertical-align:top; text-align:left;">
				<tr>
					<td style="width:50%; border-right:1px solid #A0A0A0; vertical-align:top">
							{t}Alternative addresses{/t}
						<select style="width:100%;" name="alternates_list[]" size=10 multiple title="{t}List of alternative mail addresses{/t}">
							{html_options values=$gosaMailAlternateAddress output=$gosaMailAlternateAddress}
							<option disabled>&nbsp;</option>
						</select>
						<br>
						<input name="alternate_address" size="30" align=middle maxlength="60" value="">
						<input type=submit value="{t}Add{/t}" name="add_alternate">&nbsp;
						<input type=submit value="{t}Delete{/t}" name="delete_alternate">
					</td>
					<td style="vertical-align:top;width:50%">
							{t}Forward messages to non group members{/t}
						<select style="width:100%;" name="forwarder_list[]" size=10 multiple>
							{html_options values=$gosaMailForwardingAddress output=$gosaMailForwardingAddress}
							<option disabled>&nbsp;</option>
						</select>
						<br>
						<input name="forward_address" size=20 align=middle maxlength=65 value="">
						<input type=submit value="{t}Add{/t}" name="add_forwarder">&nbsp;
						<input type=submit value="{t}Add local{/t}" name="add_local_forwarder">&nbsp;
						<input type=submit value="{t}Delete{/t}" name="delete_forwarder">
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
