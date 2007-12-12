<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  	<td style="vertical-align:top;width:600px">
  		<div class="contentboxh">
    		<p class="contentboxh">
     			{t}List of available packages{/t}
    		</p>
  		</div>
  		<div class="contentboxb">
      		{$faihead}
  		</div>
  		<div style='height:4px;'>
		</div>
  		<div class="contentboxb" style="border-top:1px solid #B0B0B0;">
      		{$failist}
    		<input type="hidden" name="edit_helper">
  		</div>
  	</td>
  	<td style="vertical-align:top;">
		<div class="contentboxh" style="border-bottom:1px solid #B0B0B0;">
    		<p class="contentboxh">
				<img src="{$infoimage}" align="right" alt="[i]">{t}Information{/t}
			</p>
   		</div>
   		<div class="contentboxb" style="padding:4px;">
			{t}This menu allows you to select multiple packages to add them to the currently edited package list.{/t}
   		</div>
   		<br>
   		<div class="contentboxh">
    		<p class="contentboxh">
				<img src="{$launchimage}" align="right" alt="[F]">
				{t}Filters{/t}
			</p>
   		</div>
   		<div class="contentboxb"> 
	 	 	{$CHKS}
		</div>
		<div class="contentboxb">
     		<table summary=""  style="width:100%;">
     			<tr>
     				<td>
						<img alt="{t}Display objects matching{/t}" src="{$search_image}" align=middle title='{t}Display objects matching{/t}'>
     				</td>
     				<td width="99%">
     					<input type='text' name='searchStrPKG' maxlength='40' style='width:99%' value='{$searchStrPKG}' id='filter' 
					     title='{t}Regular expression for matching object names{/t}' onClick="mainform.filter.value='';" onChange="mainform.submit()">
    	 			</td>
     			</tr>
     		</table>
		 {$apply}
   		</div>
	</td>
</tr>
</table>
<div align="right" style="align:right;">
	<input id="SaveSubObject" name="SaveSubObject" value="{t}Apply{/t}" type="submit">
	<input id="CancelSubObject" name="CancelSubObject" value="{t}Cancel{/t}" type="submit">
</div>
<input type="hidden" name="ignore">
<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('filter');
  -->
</script>

