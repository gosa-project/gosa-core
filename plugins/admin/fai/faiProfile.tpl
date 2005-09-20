<h2><img alt="" src="images/house.png" align="middle" title="{t}Generic{/t}">&nbsp;{t}Generic{/t}</h2>
<table summary="" cellspacing="4">
	<tr>
		<td>
			<LABEL for="cn">
			{t}Name{/t}{$must}
			</LABEL>
		</td>
		<td>
			<input value="{$cn}" id="cn" name="cn">
		</td>
	</tr>
	<tr>
		<td>
			<LABEL for="description">
			{t}Description{/t}{$must}
			</LABEL>
		</td>
		<td>
			<input value="{$description}" {$description} name="description" id="description">
		</td>
	</tr>
</table>
<br>

<table summary="" style="vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top;">
  <div class="contentboxh">
    <p class="contentboxh">
    Das ist zu groß hier... Macht keinen Sinn - 
     <LABEL for="userlist">{t}List of used class names{/t}</LABEL> 
    </p>
  </div>
			<div class="contentboxb" style="border-top:1px solid #B0B0B0;">
				
				{$divlisthead}
			</div>
			<div style='height:4px;'>
			</div>
			<div class="contentboxb" style="border-top:1px solid #B0B0B0;">
				{$divlist}
				<input type="hidden" name="edit_helper">
			</div>
		</td>
	</tr>
</table>
