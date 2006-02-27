<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
	<tr>
  		<td style="vertical-align:top; width:50%;">
			<div class="contentboxh">
				<p class="contentboxh">
				 	{$Summary} 
				</p>
			</div>
  			<div class="contentboxb">
      			{$Header}
  			</div>
  			<div style='height:4px;'>
			</div>
  			<div class="contentboxb" style="border-top:1px solid #B0B0B0;">
      			{$DivList}
    			<input type=hidden name="edit_helper">
  			</div>
  		</td>
  		<td style="vertical-align:top;">
   			<div class="contentboxh" style="border-bottom:1px solid #B0B0B0;height:22px;">
    			<p class="contentboxh">
					<img src="{$infoimage}" align="right" alt="[i]">{t}Information{/t}
				</p>
   			</div>
   			<div class="contentboxb" style="padding:5px;">
				{$Information}
   			</div>
   			<br>
   			<div class="contentboxh" style="height:22px;">
    			<p class="contentboxh">
					<img src="{$launchimage}" align="right" alt="[F]">{t}Filters{/t}
				</p>
   			</div>
			<div class="contentboxb">
				{if $Display_alphabet}
			 	<table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
			  		{$alphabet}
			 	</table>
				{/if}
				{ if $CheckBoxes != ""}
				<table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
					<tr>
						<td>
							{$CheckBoxes}
						</td>
					</tr>
				</table>   
				{/if}
			{$regexes}
			{$apply}
   			</div>
  		</td>
	</tr>
</table>
<p class="seperator">&nbsp;
</p>
<div style="text-align:right;" align="right">
	<p>
		{if $Display_Save}
			<input type="submit" name="SaveMultiSelectWindow" value="{$SaveButtonString}"> 
		{/if}
		{if $Display_Close}
			<input type="submit" name="CloseMultiSelectWindow" value="{$CloseButtonString}"> 
		{/if}
	</p>
</div>

<input type="hidden" name="ignore">
<input type="hidden" name="MultiSelectWindow" value="1">

