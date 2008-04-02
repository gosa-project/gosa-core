
<select name='option'>
{foreach from=$options item=item key=key}
	<option value='{$key}'>{$item.NAME} ({$map[$item.TYPE]})</option>
{/foreach}
</select>
