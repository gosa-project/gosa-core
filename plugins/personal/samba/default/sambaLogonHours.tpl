<div class="samba-login-wrapper">
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

    <table id="samba-login-table" class="striped">
        <thead>
            <tr>
                <th>&nbsp;</th>
                <th class="center-align" colspan="24">
                    <b>{t}Hour{/t}</b>
                </th>
            </tr>
        </thead>
        <tbody>
            <tr class="hours">
                <td>&nbsp;</td>

                {foreach from=$Hours item=hours key=key_hours}
                <td class="center-align">
                    {$hours}
                </td>
                {/foreach}
            </tr>

            {if $acl}
            <!-- Add toggle buttons for hours -->
            <tr class="toggle-buttons">
                <td>&nbsp;</td>

                {foreach from=$Hours item=hours key=key_hours}
                <td class='toggle-button'>
                    <input class="btn-extra-small" type='button' onClick="toggle_chk('^day_[0-9]*_{$hours}$');" value='+ / -'>
                </td>
                {/foreach}
                <td class='toggle-button'>
                    <input class="btn-extra-small" type='button' onClick="toggle_chk('^day_[0-9]*_[0-9]*$');" value='+ / -'>
                </td>
            </tr>
            {/if}

            <!-- Add Entries -->
            {foreach from=$Matrix item=days key=key_day}
            <tr class="check-days">
                <td><b>{$Days[$key_day]}</b></td>

                {foreach from=$days item=hours key=key_hour}
                <td class="check-option">
                    <label>
                        <input type='checkbox' {if $acl} id='day_{$key_day}_{$key_hour}' name='day_{$key_day}_{$key_hour}' {/if} {if $Matrix[$key_day].$key_hour} checked {/if} {if !$acl} disabled {/if}>
                        <span></span>
                    </label>
                </td>
                {/foreach}

                {if $acl}
                <!-- Add toggle button for days -->
                <td class="toggle-button">
                    <input class="btn-extra-small" type='button' onClick="toggle_chk('^day_{$key_day}_[0-9]*$')" value='+ / -'>
                </td>
                {/if}
            </tr>
            {/foreach}
        </tbody>
    </table>

    <input type='hidden' name='sambaLogonHoursPosted' value='1'>
</div>

<div class="card-action">
    {if $acl}
    <button class="btn-small primary" type='submit' name='save_logonHours'>{msgPool type=saveButton}</button>
    {/if}
    <button class="btn-small primary" type='submit' name='cancel_logonHours'>{msgPool type=cancelButton}</button>
</div>

<!--
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
-->
