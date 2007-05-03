
<table class='editing_surface'>
	<tr>
		<td class='editing_surface_menu'>
			
			<input type='submit' name='Save_Copy' value="{t}Export{/t}">
			<input type='submit' name='Import_Script' value="{t}Import{/t}">

			{if $Mode != "Source-Only"}			
				
				{if $Mode == "Source"}
				<input type='submit' name='View_Structured' value="{t}View structured{/t}">
				{else}
				<input type='submit' name='View_Source' value="{t}View source{/t}">
				{/if}
			{/if}
		</td>
	</tr>
	<tr>
		<td class='editing_surface_content'>

			{if $Script_Error != ""}
						<div  class='sieve_error_msgs'>
							{$Script_Error}
						</div>
			{/if}


			{if $Mode == "Structured"}
				{$Contents}
			{else}
				<textarea class='editing_source' name='script_contents'>{$Contents}</textarea>
			{/if}

		</td>
	</tr>
</table>
<div class='seperator' style='text-align:right; width:100%;'>
	<input type='submit' name='save_sieve_changes' value='{t}Save{/t}'>
        &nbsp;
    <input type='submit' name='cancel_sieve_changes' value='{t}Cancel{/t}'>
</div>
