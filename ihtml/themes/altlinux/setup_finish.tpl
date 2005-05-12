<h1>{t}Setup finished{/t}</h1>

<p>
 {t}GOsa setup has collected all data needed to create an initial configuration file. Save the the link below as your gosa.conf and place that file in /etc/gosa. Change it as needed.{/t}
</p>

<p>
 &gt; <b><a target='_new' href='setup.php?getconfig=1'>gosa.conf</a></b>
</p>

<p>
 {t}After placing the file under /etc/gosa, place make sure that the webserver user is able to read gosa.conf, while other users shouldn't. You may want to execute these commands to achieve this requirement:{/t}
</p>

<tt>
# chown root.{$webgroup} /etc/gosa/gosa.conf<br>
# chmod 640 /etc/gosa/gosa.conf
</tt>

<div style="width:99%; text-align:right;">
 {if $mode ne 'ready'}
 <input type='submit' name='retry' value='{t}Retry{/t}'>
 {else}
 <input type='submit' name='login' value='{t}Sign in{/t}'>
 {/if}
</div>
