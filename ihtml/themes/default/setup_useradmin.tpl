
{if $exists == TRUE}
   <h1>{t}You already have an Administrative Account and a Group.{/t}</h1>
{else}

<h1>{t}You're missing an administrative account for GOsa, you'll not be able to administrate anything!{/t}</h1>
{/if}

<p>
{t}To provide full administrative access to GOsa, you need to create a special group which contains the administrative user. The setup program can assist you with this task. Enter the desired username and the password below to create the missing entries.{/t}
</p>

<p class="seperator">
  &nbsp;
</p>

<br>

<table>
	<tr>
		<td>
		{t}Username{/t}
		</td>
		<td>
			<input type="text" value="" name="admin_name">	
		</td>
	</tr>
	<tr>
		<td>
		{t}Password{/t}
		</td>
		<td>
            <input type="password" value="" name="admin_pass">
		</td>
	</tr>
</table>
{$str_there}
<p class="plugbottom">
<input type="hidden" name="next" value="6">
<input type='submit' name='back' value='{t}Back{/t}' >
<input type="submit" name="new_admin" value="{t}Continue{/t}">
</p>

