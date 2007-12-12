<h2><img alt="" class="center" src="images/house.png" align="middle">&nbsp;<LABEL for="gotoKioskProfile">{t}Kiosk profile management{/t}</ LABEL></h2>

    <input type="hidden" name="dialogissubmitted" value="1">

{t}Server path{/t}&nbsp;<input name="server_path" style="width:300px;" value="{$server_path}">
<br>
<br>
{$divlist}
<input type="file" size=50 name="newProfile" value="{t}Browse{/t}">
<input type="submit" name="profileAdd" value="{t}Add{/t}">

<p class='seperator'>&nbsp;</p>
<div style="width:100%; text-align:right;padding-top:10px;padding-bottom:3px;">
    <input type='submit' name='SaveService' value='{t}Save{/t}'>
    &nbsp;
    <input type='submit' name='CancelService' value='{t}Cancel{/t}'>
</div>
<input type="hidden" name="goKioskPosted" value="1">

<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
    focus_field('gotoKioskProfile');
  -->
</script>
