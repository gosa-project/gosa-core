<h2><img alt="" src="images/house.png" align="middle">&nbsp;{t}Select printer entry{/t}</h2>

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
		                <select style="width:550px;" size=20 name="gotoPrinter" id="gotoPrinter">
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
   					<p class="contentboxb">
    					{t}This menu allows you to create, edit and delete selected printers. 
							Having a great number of printers, you may want to use the range selectors on top of the printers list.{/t}
    				</p>
   				</div>
   				<br>

		   		<div class="contentboxh">
    				<p class="contentboxh"><img src="{$launchimage}" align="right" alt="[F]">{t}Filters{/t}</p>
   				</div>
   				<div class="contentboxb">
				<table summary="" width="100%" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
					<tr>
						<td width="50%">
							<img alt="" src="{$tree_image}" align=middle>&nbsp;{t}Display printers of department{/t}
						</td>
						<td>
							<select name="depselectPrinter" size=1 onChange="mainform.submit()" title="{t}Choose the department the search will be based on{/t}">
							  {html_options options=$deplistPrinter selected=$depselectPrinter}
							</select>
						</td>
					</tr>
				</table>
   				</div>
   				<div class="contentboxb">
     				<table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
      				{$alphabet}
     				</table>
    					<table summary="" width="100%" style="border-top:1px solid #B0B0B0;">
							<tr>
								<td>
									<LABEL for='filter'><img alt="Search" src="{$search_image}" align="middle" title="{t}Display printers matching{/t}">
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

    <p align="right">
    <input type="submit" name="PrinterSave"     value="{t}Use{/t}">
    <input type="submit" name="PrinterCancel"   value="{t}Cancel{/t}">
    </p>

