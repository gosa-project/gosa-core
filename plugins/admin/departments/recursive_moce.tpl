<h2>{t}Processing requested operation, the status of this operation will be displayed in an iframe below.{/t}</h2>
<iframe src="{$src}" style="width:100%;height:450px;background-color:#FFFFFF;" name="status">
	{t}Your browser isn't able to display this iframe ... use the link below intead{/t}<br>
	<a href="{$src}">{$src}</a>
</iframe>

<p class="seperator">&nbsp;</p>
<div style="text-align:right;">
	<p>
		<input type="submit" name="back" value="{t}Back{/t}">
	</p>
</div>
