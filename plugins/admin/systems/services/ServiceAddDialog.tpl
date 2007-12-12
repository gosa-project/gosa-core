<div style="height:5px">&nbsp;</div>
<div style="font-size:18px;">
	{t}Adding a new service to the current server{/t}
</div>
<br>
<p class="seperator">
{t}This dialog allows you to add new services to the currenty edited server object. The box below shows all available but not already used services.{/t}
<br>
<br>
</p>
{if $Services} 
<br>
<br>{t}Service to add{/t} &nbsp;
	<select name="ServiceName" >
		<option value="" >&nbsp;</option>
		{html_options options=$Services }
	</select>
{else}
	<br>
	{t}All available services are already in use.{/t}
	<br>
{/if}
<br>
<br>
<br>
<p class="seperator">
<div style="width:100%; text-align:right;padding-top:8px;padding-bottom:3px;">
    <input type='submit' name='SaveServiceAdd' value='{t}Continue{/t}' {if !$Services} disabled {/if}>
    &nbsp;
    <input type='submit' name='CancelServiceAdd' value='{t}Cancel{/t}'>
</div>

