<h2><img class="center" alt="" src="images/select_printer.png" align="middle">&nbsp;{t}Add printer devcies{/t}</h2>

    <input type="hidden" name="dialogissubmitted" value="1">

  	<table summary="{t}Select printer to add{/t}" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
        <tr>
            <td width="50%">
	  			<div class="contentboxh" style="height:20px;">
    				<p class="contentboxh" style="font-size:12px">
     					<b><LABEL for="gotoPrinter">{t}Select printer to add{/t}</LABEL></b>
    				</p>
   				</div>
   				<div class="contentboxb">
    				<p class="contentboxb" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
		                <select style="width:600px;height:450px" size=20 name="gotoPrinter[]" id="gotoPrinter" multiple>
        		        {html_options values=$gotoPrinterKeys output=$gotoPrinters}
                		<option disabled>&nbsp;</option>
               			 </select><br>
					</p>
				</div>
            </td>
            <td valign="top">
		   		<div class="contentboxh" style="height:20px">
    				<p class="contentboxh" style="font-size:12px"><img src="{$launchimage}" align="right" alt="[F]">{t}Filters{/t}</p>
   				</div>
   				<div class="contentboxb">
     				<table summary="" style="width:100%;border-top:1px solid #B0B0B0;background-color:#F8F8F8">
      				{$alphabet}
     				</table>
				<table summary="" width="100%" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
					<tr>
						<td>
							<img class="center" alt="" src="{$tree_image}" align=middle>&nbsp;
							<select name="depselectPrinter" size=1 onChange="mainform.submit()" title="{t}Choose the department the search will be based on{/t}">
							  {html_options options=$deplistPrinter selected=$depselectPrinter}
							</select>
						</td>
					</tr>
				</table>
     				<table summary="" style="width:100%;border-top:1px solid #B0B0B0;background-color:#F8F8F8">
				<tr><td>
      				<input type=checkbox name="subtrees" value="1" {$subtrees} onClick="mainform.submit()" title="{t}Select to search within subtrees{/t}">{t}Search in subtrees{/t}
				</tr></td>
     				</table>
    					<table summary="" width="100%" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
							<tr>
								<td>
									<LABEL for='filter'><img class="center" alt="Search" src="{$search_image}" align="middle" title="{t}Display printers matching{/t}">
									</LABEL>
								</td>
    							<td width="99%">
									<input id='filter' style='width:99%' type='text' name='regexPrinter' maxlength='20' value='{$regexPrinter}' 
											title='{t}Regular expression for matching printer names{/t}' onChange="mainform.submit()">
								</td>
							</tr>
						</table>
					{$apply}
				</div>
        	</td>
    	</tr>
    </table>

    <p class="seperator">&nbsp;</p>
    <p align="right">
    <input type="submit" name="PrinterSave"     value="{t}Add{/t}">
    <input type="submit" name="PrinterCancel"   value="{t}Cancel{/t}">
    </p>
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('gotoPrinter');
  -->
</script>
