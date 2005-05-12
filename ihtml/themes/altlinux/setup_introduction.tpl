<h1>{t}Welcome to the GOsa setup!{/t}</h1>

<p>
 {t}It looks like you're calling GOsa for the first time - no configuration was found. This setup like script will try to aid you in creating a working configuration by performing three major steps: first we'll check the PHP installation for required modules, than we're going to check for optional/needed programms and after you've provided basic informations for your LDAP connectivity, we're going to check your LDAP setup.{/t}
</p>

{$tests}

<div style="width:99%; text-align:right;">
 <input type='submit' name='continue' value='{t}Continue{/t}' {$mode}>
</div>

<br>
<br>
