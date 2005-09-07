<h2>{t}FAI script set{/t}
<br><br>
<input type="hidden" name="SubObjectFormSubmitted" value="1">
<table>
    <tr>
        <td>
            {t}Name{/t}
        </td>
        <td>
            <input value="{$Object_cn}" name="Object_cn">
        </td>
    </tr>
    <tr>
        <td>
            {t}Description{/t}
        </td>
        <td>
            <input value="{$Object_description}" name="Object_description">
        </td>
    </tr>
</table>
<p class="seperator">&nbsp;</p>
<br>
<h2>{t}Script{/t}</h2>
<table width="100%">
    <tr>
        <td>
        	<input type="text" name="Object_FAIscript" value="{$Object_FAIscript}">
		</td>
    </tr>
    <tr>
        <td>
        	<input type="text" name="Object_FAItask" value="{$Object_FAItask}">
		</td>
    </tr>
</table>
<br>
<p class="seperator">&nbsp;</p>
<br>
<div style="align:right;" align="right">
	<input type="submit" value="{t}Save{/t}" 	name="SaveSubObject">
	<input type="submit" value="{t}Abort{/t}" 	name="CancelSubObject">
</div>

