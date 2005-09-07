<h2>{t}FAI script set{/t}
<br><br>
<table>
    <tr>
        <td>
            {t}Name{/t}
        </td>
        <td>
            <input value="{$cn}" disabled>
        </td>
    </tr>
    <tr>
        <td>
            {t}Description{/t}
        </td>
        <td>
            <input value="{$description}" {$description} name="description">
        </td>
    </tr>
</table>
<br>
<br>
<h2>{t}Already defined scripts{/t}</h2>
<table width="100%">
    <tr>
        <td>
            <select name="SubObject" title="{t}Choose a script to delete or edit{/t}" style="width:450px;height:250px;" multiple>
                {html_options values=$SubObjectKeys output=$SubObjects}
            </select><br>
            <input type="submit" name="AddSubObject"     value="{t}Add{/t}">
            <input type="submit" name="EditSubObject"    value="{t}Edit{/t}">
            <input type="submit" name="DelSubObject"     value="{t}Delete{/t}">
        </td>
    </tr>
</table>
