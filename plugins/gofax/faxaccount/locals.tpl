<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top; width:600px">
   <div class="contentboxh" style="height:20px;">
    <p class="contentboxh" style="font-size:12px">
     <b>{t}Select numbers to add{/t} {$hint}</b><br>
    </p>
   </div>
   <div class="contentboxb">
    <p class="contentboxb" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
     <select style="width:100%; margin-top:4px; height:450px;" name="local_list[]" size="15" multiple>
    	{html_options options=$list}
     </select>
    </p>
   </div>
  </td>
  <td style="vertical-align:top;">
   <div class="contentboxh" style="height:20px;">
    <p class="contentboxh" style="font-size:12px"><img src="{$launchimage}" align="right" alt="[F]"><b>{t}Filters{/t}</b></p>
   </div>
   <div class="contentboxb"  style="background-color:#F8F8F8">
     <table summary="" style="width:100%;border-top:1px solid #B0B0B0; background-color:#F8F8F8">
      {$alphabet}
     </table>
    <table summary="" width="100%" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
		<tr>
			<td>
				<img class="center" alt="" src="{$tree_image}" align="middle" title="{t}Display numbers of department{/t}">&nbsp;
				<select name="depselect" size="1" onChange="mainform.submit()" title="{t}Choose the department the search will be based on{/t}">
			      {html_options options=$deplist selected=$depselect}
			    </select>
			</td>
		</tr>
	</table>
    <table summary="" width="100%" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
		<tr>
			<td width="18">
				<img class="center" alt="" src="{$search_image}" align="middle" title="{t}Display numbers matching{/t}">
			</td>
    		<td>
				<input type='text' name='regex' maxlength='20' value='{$regex}' style="width:99%" title='{t}Regular expression for matching numbers{/t}' onChange="mainform.submit()">	</td>
		</tr>
	</table>
    <table summary="" width="100%" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
		<tr>
			<td width="18">
				<img class="center" alt="" src="{$usearch_image}" align="middle" title="{t}Display numbers of user{/t}">
			</td>
		    <td>
				<input type='text' name='fuser' style="width:99%" maxlength='20' value='{$fuser}' title='{t}User name of which numbers are shown{/t}' onChange="mainform.submit()">	</td>
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

