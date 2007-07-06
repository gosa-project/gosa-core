<h2><img class="center" alt="" src="images/hotplug.png" align="middle">&nbsp;{t}Add hotplug devices{/t}</h2>

    <input type="hidden" name="dialogissubmitted" value="1">

  	<table summary="{t}Hotplug management{/t}" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4 cellspacing=2>
        <tr>
            <td width="50%" style="vertical-align:top;">
	 			<div class="contentboxh" style="height:20px;">
    				<p class="contentboxh" style="font-size:12px">
     					<b><LABEL for="hotplugName">{t}Select hotplug device to add{/t}</LABEL></b>
    				</p>
   				</div>
   				<div class="contentboxb">
    				<p class="contentboxb" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
						<select style="width:600px;height:450px" size=20 name="hotplugName[]" id="hotplugName" multiple>
							{html_options values=$hotplugDeviceKeys output=$hotplugDevices}
						<option disabled>&nbsp;</option>
						</select><br>
					</p>
				</div>
            </td>
		 	<td style="vertical-align:top;">
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
                            <select name="depselectHot" size=1 onChange="mainform.submit()" title="{t}Choose the department the search will be based    on{/t}">
                              {html_options options=$deplistHot selected=$depselectHot}
                            </select>
                        </td>
                    </tr>
                </table>
                        <table summary="" width="100%" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
                            <tr>
                                <td>
                                    <LABEL for='filter'><img class="center" alt="Search" src="{$search_image}" align="middle" title="{t}Display users matching{/t}">
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

    <p class="seperator">&nbsp;</p>

    <p align="right">
    <input type="submit" name="HotPlugSave"     value="{t}Add{/t}">
    <input type="submit" name="HotPlugCancel"   value="{t}Cancel{/t}">
    </p>
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('hotplugName');
  -->
</script>
