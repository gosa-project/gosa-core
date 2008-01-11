
<h2>{t}Devices{/t}</h2>

<table width="100%" summary="">
 <tr>
  <td width="50%" style="vertical-align:top">
    <table style="border-right:1px solid #B0B0B0;width:100%">
        <tr>
            <td><LABEL for="base">{t}Base{/t}</LABEL>
            </td>
            <td>
				<select name="base">
					{html_options options=$bases selected=$base}
				</select>
            </td>
        </tr>
        <tr>
            <td><LABEL for="cn">{t}Device name{/t}</LABEL>{$must}
            </td>
            <td>
                <input type="text" size=40 value="{$cn}" name="cn" id="cn">
            </td>
        </tr>
        <tr>
            <td><LABEL for="description">{t}Description{/t}</LABEL>
            </td>
            <td>
                <input type="text" size=40 value="{$description}" name="description" id="description">
            </td>
        </tr>
        <tr>
            <td><LABEL for="description">{t}Device type{/t}</LABEL>
            </td>
            <td>
		<select id="type" size="1" name="type" title="{t}Choose the device type{/t}">
			{html_options options=$types selected=$type}
		</select>
            </td>
        </tr>
    </table>
   </td>
   <td style="vertical-align:top">
    <table summary="">
        <tr>
            <td><LABEL for="dev_id">{t}Serial number{/t}&nbsp;{t}(iSerial){/t}</LABEL>{$must}
            </td>
            <td>
                <input type="text" value="{$dev_id}" name="dev_id" id="dev_id">
            </td>
            <td colspan="2">&nbsp;</td>
        </tr>
        <tr>
            <td><LABEL for="vendor">{t}Vendor-ID{/t}&nbsp;{t}(idVendor){/t}</LABEL>{$must}
            </td>
            <td>
                <input type="text" value="{$vendor}" name="vendor" id="vendor">
            </td>
        </tr>
        <tr>
          <td><LABEL for="produkt">{t}Product-ID{/t}&nbsp;{t}(idProduct){/t}</LABEL>{$must}
            </td>
            <td>
                <input type="text" value="{$serial}" name="serial" id="serial">
            </td>
        </tr>
    </table>
</table>
<input type='hidden' value="1" name="deviceGeneric_posted">
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
    focus_field('name');
  -->
</script>


