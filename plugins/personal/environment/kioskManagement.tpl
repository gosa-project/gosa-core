<h2><img alt="" class="center" src="images/house.png" align="middle">&nbsp;<LABEL for="gotoKioskProfile">{t}Kiosk profile management{/t}</LABEL></h2>

    <input type="hidden" name="dialogissubmitted" value="1">

	<table summary="{t}Kiosk profile management{/t}" width="100%">
		<tr>
			<td>
				{$divlist}
<!--            	<select style="width:100%;" size=18 name="gotoKioskProfile" {$gotoKioskProfileACL} id="gotoKioskProfile">
                {html_options values=$gotoKioskProfileKeys output=$gotoKioskProfiles}
                <option disabled>&nbsp;</option>
                </select>-->
		<input type="file" size=50 name="newProfile" value="{t}Browse{/t}">			
    		<input type="submit" name="profileAdd" value="{t}Add{/t}">
		<!--	  <input type="submit" name="profileDel" value="{t}Remove{/t}">-->
			</td>
			<td>
			</td>
		</tr>
	</table>

    <p class="seperator">&nbsp;</p>

    <p align="right">
    <input type="submit" name="KioskClose"     value="{t}Close{/t}">
    </p>
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('gotoKioskProfile');
  -->
</script>
