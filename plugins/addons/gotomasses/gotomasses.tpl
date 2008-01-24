{$div}
<div style="width:65%; text-align: center; float:left;" >
{if $start == 0 }
	<input type="button" name="dummy" value="&lt;&nbsp;{t}Back{/t}" disabled>&nbsp;
	{$start+1}&nbsp;
	<input type="submit" name="next_page" value="{t}Next{/t}&nbsp;&gt;">&nbsp;
{else}
	<input type="submit" name="last_page" value="&lt;&nbsp;{t}Back{/t}">&nbsp;
	{$start_real}&nbsp;
	<input type="submit" name="next_page" value="{t}Next{/t}&nbsp;&gt;">&nbsp;
{/if}
</div>
<div >
<select name="range" onChange="document.mainform.submit();">
	 {html_options values=$ranges options=$ranges selected=$range}
</select>
</div>
