<h1>{t}Welcome to the GOsa setup!{/t}</h1>

<p>
 {t}It looks like you're calling GOsa for the first time - no configuration was found. This setup like script will try to aid you in creating a working configuration by performing three major steps: first we'll check the PHP installation for required modules, than we're going to check for optional/needed programms and after you've provided basic information for your LDAP connectivity, we're going to check your LDAP setup.{/t}
</p>
<p class="seperator">&nbsp;</p>
<div id='oldbox' style='background-color:#ffffff;'>
{$tests}
</div>
<p class="plugbottom">
 <input type="hidden" name="next" value="2">
 <input type='submit' name='continue' value='{t}Continue{/t}' {$mode}>
</p>

<p class="seperator">&nbsp;</p>
<p style="width:90%;">
 {t}You can also use the new and more-advanced way of inspecting your PHP-installation. It will give you information about the exact function that could not be found (functions that could be found are not mentioned). This is useful if you know what you're doing.{/t}
</p>
<button type='button' onClick='toggle("functionbox")'>{t}Toggle Show/Hide{/t}</button>
<div id='functionbox' style='background-color:#ffffff;position:absolute; z-index:0; visibility:hidden'>
{$detailed_tests}
</div>
