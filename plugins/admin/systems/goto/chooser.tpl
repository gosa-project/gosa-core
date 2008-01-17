<h1>{t}Choose the kind of system component you want to create{/t}</h1>

<p>
 {t}Linux terminals and workstations are autocreated on bootup. For this reason you're only be able to create templates for a specific tree. Servers are normally automatically added too, but in some special cases you may need to create a faked server entry to provide GOsa with some informations. Other network components may be used for Nagios setups to create component dependencies.{/t}
</p>

<input type="radio" name="system" value="terminal" checked>{t}Linux thin client template{/t}<br>
<input type="radio" name="system" value="workstation">{t}Linux workstation template{/t}<br>
<input type="radio" name="system" value="server">{t}Linux Server{/t}<br>
<!--<input type="radio" name="system" value="machineaccount">{t}Windows workstation{/t}<br>-->
<input type="radio" name="system" value="printer">{t}Network printer{/t}<br>
<input type="radio" name="system" value="phone">{t}Phone{/t}<br>
<input type="radio" name="system" value="component">{t}Other network component{/t}<br>

<p class="plugbottom">
 <input type=submit name="create_system" value="{t}Create{/t}">
 &nbsp;
 <input type=submit name="cancel" value="{t}Cancel{/t}">
</p>

<input type="hidden" name="ignore">
