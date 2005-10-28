    <input type="hidden" name="dialogissubmitted" value="1">

  	<table summary="{t}Select printer to add{/t}" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
        <tr>
            <td width="50%">
	  			<div class="contentboxh" style="height:20px;">
    				<p class="contentboxh" style="font-size:12px">
     					<b><LABEL for="gotoPrinter">{t}Select entries to add{/t}</LABEL></b>
    				</p>
   				</div>
   				<div class="contentboxb">
    				<p class="contentboxb" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
		                <select style="width:500px;height:480px;" size=20 name="gotoPrinter[]" id="gotoPrinter" multiple>
        		        {html_options values=$gotoPrinterKeys output=$gotoPrinters}
                		<option disabled>&nbsp;</option>
               			 </select><br>
					</p>
				</div>
            </td>
            <td valign="top">
				<div class="contentboxh">
    				<p class="contentboxh"><img src="{$infoimage}" align="right" alt="[i]">{t}Information{/t}</p>
  	 			</div>
   				<div class="contentboxb">
   					<p class="contentboxb" style="background-color:#F8F8F8">
    					{t}This menu allows you to select one or multiple entries to be added to the printer object for specific tasks. Having a great number of entries, you may want to use the range selectors below.{/t}
    				</p>
   				</div>
   				<br>

		   		<div class="contentboxh">
    				<p class="contentboxh"><img src="{$launchimage}" align="right" alt="[F]">{t}Filters{/t}</p>
   				</div>
   				<div class="contentboxb">
     				<table summary="" style="width:100%;border-top:1px solid #B0B0B0;background-color:#F8F8F8">
      				{$alphabet}
     				</table>
				<table summary="" width="100%" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
					<tr>
						<td>
							<label for="dspselectPrinter"><img alt="" src="{$tree_image}" align="middle" title="{t}Display members of department{/t}"></label>
						</td>
						<td style="width:99%">
							<select style="width:99%" name="depselectPrinter" size=1 onChange="mainform.submit()" title="{t}Choose the department the search will be based on{/t}">
							  {html_options options=$deplistPrinter selected=$depselectPrinter}
							</select>
						</td>
					</tr>
				</table>
    					<table summary="" width="100%" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
							<tr>
								<td>
									<LABEL for='filter'><img alt="Search" src="{$search_image}" align="middle" title="{t}Display members matching{/t}">
									</LABEL>
								</td>
    							<td width="99%">
									<input id='filter' style='width:99%' type='text' name='regexPrinter' maxlength='20' value='{$regexPrinter}' 
											title='{t}Regular expression for matching member names{/t}' onChange="mainform.submit()">
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

