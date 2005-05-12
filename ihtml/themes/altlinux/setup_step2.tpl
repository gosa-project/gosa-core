<h1>{t}Setup continued...{/t}</h1>

<p>
 {t}Step two looks for a set of helper programms and checks if they have the correct minimum version.{/t}
</p>

{$tests}

{if $mode ne "disabled"}
<p>
 <b>{t}We've inspected the webserver side now. Your setup seems to fit GOsa's needs. Lets go for the GOsa configuration part now...{/t}</b>
</p>
{/if}

<div style="width:99%; text-align:right;">
 <input type='submit' name='continue2' value='{t}Continue{/t}' {$mode}>
</div>
