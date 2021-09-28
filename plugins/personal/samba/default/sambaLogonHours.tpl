{if $acl}
<!-- Javacript function used to switch a complete row or col of selected hours -->
<script language="javascript" type="text/javascript">
    var $regex = new Array();
    function toggle_chk($reg) {
        if (!$regex[$reg]) {
            $regex[$reg] = 1;
        }
        $regex[$reg] *= -1;
        if ($regex[$reg] == 1) {
            chk_set_all($reg, true);
        } else {
            chk_set_all($reg, false);
        }
    }
</script>
{/if}

<h3>{t}Samba log on times{/t}</h3>
<p>{t}Specify the hours this user is allowed to log in{/t}</p>

<hr class="divider">

<table class="striped">
    <thead>
        <tr>
            <th>&nbsp;</th>
            <th class="center-align" colspan="24">
                <b>{t}Hour{/t}</b>
            </th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>&nbsp;</td>

            {foreach from=$Hours item=hours key=key_hours}
            <td class="center-align">
                {$hours}
            </td>
            {/foreach}
        </tr>

        {if $acl}
        <!-- Add toggle buttons for hours -->
        <tr>
            <td>&nbsp;</td>

            {foreach from=$Hours item=hours key=key_hours}
            <td class='center-align'>
                <input class="btn-extra-small primary white-text" type='button' onClick="toggle_chk('^day_[0-9]*_{$hours}$');" value='+/-' style='width:100%;'>
            </td>
            {/foreach}
            <td>
                <input class="btn-extra-small primary white-text" type='button' onClick="toggle_chk('^day_[0-9]*_[0-9]*$');" value='+/-' style='width:100%;'>
            </td>
        </tr>
        {/if}

        <!-- Add Entries -->
        {foreach from=$Matrix item=days key=key_day}
        <tr>
            <td><b>{$Days[$key_day]}</b></td>

            {foreach from=$days item=hours key=key_hour}
            <td class="center-align">
                <label>
                    <input type='checkbox' {if $acl} id='day_{$key_day}_{$key_hour}' name='day_{$key_day}_{$key_hour}' {/if} {if $Matrix[$key_day].$key_hour} checked {/if} {if !$acl} disabled {/if}>
                    <span></span>
                </label>
            </td>
            {/foreach}

            {if $acl}
            <!-- Add toggle button for days -->
            <td>
                <input class="btn-extra-small primary white-text" type='button' onClick="toggle_chk('^day_{$key_day}_[0-9]*$')" value='+/-' style='width:100%'>
            </td>
            {/if}
        </tr>
    </tbody>
    {/foreach}
</table>

<input type='hidden' name='sambaLogonHoursPosted' value='1'>

<div class="card-action">
    {if $acl}
    <button class="btn-small primary" type='submit' name='save_logonHours'>{msgPool type=saveButton}</button>
    {/if}
    <button class="btn-small primary" type='submit' name='cancel_logonHours'>{msgPool type=cancelButton}</button>
</div>

<!--
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
-->
