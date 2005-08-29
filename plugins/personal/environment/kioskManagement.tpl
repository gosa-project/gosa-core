<h2><img alt="" src="images/house.png" align="middle">&nbsp;<LABEL for="gotoKioskProfile">{t}Kiosk profile management{/t}</LABEL></h2>

    <input type="hidden" name="dialogissubmitted" value="1">

	<table summary="{t}Kiosk profile management{/t}">
		<tr>
			<td>
            	<select style="width:350px;" size=8 name="gotoKioskProfile" {$gotoKioskProfileACL} id="gotoKioskProfile">
                {html_options values=$gotoKioskProfileKeys output=$gotoKioskProfiles}
                <option disabled>&nbsp;</option>
                </select><br>
				<input type="file" 	 name="newProfile" value="{t}Browse{/t}">			
    		<input type="submit" name="profileAdd" value="{t}Add{/t}">
			  <input type="submit" name="profileDel" value="{t}Remove{/t}">
			</td>
			<td>
			</td>
		</tr>
	</table>

    <p align="right">
    <input type="submit" name="KioskClose"     value="{t}Close{/t}">
    </p>

