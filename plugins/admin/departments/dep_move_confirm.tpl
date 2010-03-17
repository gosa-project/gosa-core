<div style="font-size:18px;">
 {image path="images/warning.png" align="top"}&nbsp;{t}Warning{/t} - {t}You are currently moving/renaming this department.{/t}

</div>
<p>
{t}Modifying a departments naming attribute 'ou' or base may corrupt acls and snapshot entries for all entire objects.{/t}
</p>
<p>
{t}GOsa can NOT fix this for you, yet.{/t}
</p>
<p>
{t}Before you confirm this action, ensure that everything will be as expected, possibly the best solution is a backup.{/t} 
</p>

<hr>
<div class="plugin-actions">
	<button type='submit' name='dep_move_confirm'>{msgPool type=saveButton}</button>

	<button type='submit' name='cancel_save'>{msgPool type=cancelButton}</button>

</div>
