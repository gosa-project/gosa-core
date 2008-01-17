<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top; width:600px">
   <div class="contentboxh" style="height:20px;">
    <p class="contentboxh" style="font-size:12px">
     <b><LABEL for="local_list">{t}Select addresses to add{/t} {$hint}</LABEL ></b><br>
    </p>
   </div>
   <div class="contentboxb">
    <p class="contentboxb" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
     <select style="width:600px; margin-top:4px; height:450px;" id="local_list" name="local_list[]" size="15" multiple>
		{html_options options=$mailusers}
		<option disabled>&nbsp;</option>
     </select>
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
   <div class="contentboxb">
     <table summary="" style="width:100%;border-top:1px solid #B0B0B0; background-color:#F8F8F8">
      {$alphabet}
     </table>
    <table summary="" width="100%" style="width:100%;border-top:1px solid #B0B0B0; background-color:#F8F8F8">
		<tr>
			<td>
				<img alt="" src="{$tree_image}" align=middle title="{t}Display addresses of department{/t}">&nbsp;
				<select name="depselect" size=1 onChange="mainform.submit()" title="{t}Choose the department the search will be based on{/t}">
      			{html_options options=$deplist selected=$depselect}
				</select>
			</td>
		</tr>
	</table>
    <table summary="" width="100%" style="width:100%;border-top:1px solid #B0B0B0; background-color:#F8F8F8">
		<tr>
			<td width="18">
				<img alt="" src="{$search_image}" align=middle title="{t}Display addresses matching{/t}">
			</td>
    		<td>
				<input type='text' name='regex' maxlength='20' style="width:99%"value='{$regex}' title='{t}Regular expression for matching addresses{/t}' onChange="mainform.submit()">
			</td>
		</tr>
	</table>
    <table summary="" width="100%"  style="width:100%;border-top:1px solid #B0B0B0; background-color:#F8F8F8">
		<tr>
			<td width="18">
				<img alt="" src="{$usearch_image}" align=middle title="{t}Display addresses of user{/t}">
			</td>
    		<td>
				<input type='text' name='muser' maxlength='20' value='{$muser}' style="width:99%" title='{t}User name of which addresses are shown{/t}' onChange="mainform.submit()">
			</td>
		</tr>
	</table>
   {$apply}
   </div>
  </td>
</tr>
</table>

<p class="plugbottom">
  <input type=submit name="add_locals_finish" value="{t}Add{/t}">
  &nbsp;
  <input type=submit name="add_locals_cancel" value="{t}Cancel{/t}">
</p>
