<h2><img alt="" src="images/house.png" align="middle">&nbsp;{t}Select hotplug entry{/t}</h2>

    <input type="hidden" name="dialogissubmitted" value="1">

  	<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4 cellspacing=2>
        <tr>
            <td width="50%" style="vertical-align:top;">
	 			<div class="contentboxh" style="height:20px;">
    				<p class="contentboxh" style="font-size:12px">
     					<b><LABEL for="hotplugName">Select hotplug to add</LABEL></b>
    				</p>
   				</div>
   				<div class="contentboxb">
    				<p class="contentboxb" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
						<select style="width:550px;" size=20 name="hotplugName" id="hotplugName">
						{html_options values=$hotplugDeviceKeys output=$hotplugDevices}
						<option disabled>&nbsp;</option>
						</select><br>
					</p>
				</div>
            </td>
		 	<td style="vertical-align:top;">
                <div class="contentboxh">
                    <p class="contentboxh"><img src="{$infoimage}" align="right" alt="[i]">{t}Information{/t}</p>
                </div>
                <div class="contentboxb">
                    <p class="contentboxb">
                        {t}This menu allows you to select an existing hotplug configuration.{/t}
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
                            <img alt="" src="{$tree_image}" align=middle>&nbsp;{t}Display hotplugs of department{/t}
                        </td>
                        <td>
                            <select name="depselectHot" size=1 onChange="mainform.submit()" title="{t}Choose the department the search will be based    on{/t}">
                              {html_options options=$deplistHot selected=$depselectHot}
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
                                    <input id='filter' style='width:99%' type='text' name='regexHot' maxlength='20' value='{$regexHot}'
                                            title='{t}Regular expression for        matching hotplugs{/t}' onChange="mainform.submit()">
                                </td>
                            </tr>
                        </table>
                    {$apply}
                </div>
            </td>


		</tr>
	</table>

    <p align="right">
    <input type="submit" name="HotPlugSave"     value="{t}Use{/t}">
    <input type="submit" name="HotPlugCancel"   value="{t}Cancel{/t}">
    </p>

