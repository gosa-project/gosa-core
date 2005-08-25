<h2><img alt="" src="images/house.png" align="middle">&nbsp;{t}Select hotplug entry{/t}</h2>

    <input type="hidden" name="dialogissubmitted" value="1">

  	<table summary="">
        <tr>
            <td width="50%">
                <select style="width:350px;" size=20 name="gotoPrinter">
                {html_options values=$gotoPrinterKeys output=$gotoPrinters}
                <option disabled>&nbsp;</option>
                </select><br>
            </td>
            <td>
				<div class="contentboxh">
    				<p class="contentboxh"><img src="{$infoimage}" align="right" alt="[i]">{t}Information{/t}</p>
  	 			</div>
   				<div class="contentboxb">
   					<p class="contentboxb">
    					{t}This menu allows you to create, edit and delete selected users. 
							Having a great number of users, you may want to use the range selectors on top of the user list.{/t}
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
							<img alt="" src="{$tree_image}" align=middle>&nbsp;{t}Display groups of department{/t}
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
									<LABEL for='filter'><img alt="Search" src="{$search_image}" align="middle" title="{t}Display users matching{/t}">
									</LABEL>
								</td>
    							<td width="99%">
									<input id='filter' style='width:99%' type='text' name='regexPrinter' maxlength='20' value='{$regexPrinter}' 
											title='{t}Regular expression for        matching user names{/t}' onChange="mainform.submit()">
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

