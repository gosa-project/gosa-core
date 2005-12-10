<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
	<tr>
  		<td style="vertical-align:top;" width="50%">
   
			<div class="contentboxh" style="width:610px; height:20px;">
    			<p class="contentboxh" style="font-size:12px">
     				<b>{t}Repositories{/t}</b>
    			</p>
   			</div>

			<div class="contentboxb">
				<p class="contentboxb" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
					{$Repositories}
					<input type="submit" value="{t}Add{/t}" name="AddRepository">
				</p>
			</div>

  		</td>

		<td style="vertical-align:top;">
   			<div class="contentboxh" style="height:20px;">
    			<p class="contentboxh" style="font-size:12px">
					<img src="{$launchimage}" align="right" alt="[F]">
					<b>{t}Filters{/t}</b>
				</p>
   			</div>

		 	<div class="contentboxb" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
				<table summary="" style="width:100%;">
					{$alphabet}
				</table>
    		
				<table summary="" width="100%" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
					<tr>
						<td width="100%">
							<img alt="" src="{$search_image}" align=middle>&nbsp;
							<input type='text' name='regex' style='width:90%' maxlength='20' value='{$regex}' 
								title='{t}Search filter{/t}' onChange="mainform.submit()">
						</td>
					</tr>
				</table>
				{$apply}
   			</div>
  		</td>
	</tr>
</table>
