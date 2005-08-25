<h2><img alt="" src="images/house.png" align="middle">&nbsp;{t}Select hotplug entry{/t}</h2>

    <input type="hidden" name="dialogissubmitted" value="1">

  	<table summary="">
        <tr>
            <td width="50%">
                <select style="width:350px;" size=20 name="hotplugName">
                {html_options values=$hotplugDeviceKeys output=$hotplugDevices}
                <option disabled>&nbsp;</option>
                </select><br>
            </td>
		</tr>
	</table>

    <p align="right">
    <input type="submit" name="HotPlugSave"     value="{t}Use{/t}">
    <input type="submit" name="HotPlugCancel"   value="{t}Cancel{/t}">
    </p>

