<div class="content-wrapper card-content-scroll">
    {if $method == "default"}
        <p>{t}During the LDAP inspection, we're going to check for several common pitfalls that may occur when migration to GOsa base LDAP administration. You may want to fix the problems below, in order to provide smooth services.{/t}
        </p>

        <div class="setup-ldap-table card-content-scroll">
            <div class="row">
                <div class="col s12">
                    <table class="striped">
                        <caption>{t}Checks{/t}</caption>
                        <tbody>
                            {foreach from=$checks item=val key=key}
                                <tr>
                                    <td>
                                        <b>{$checks.$key.TITLE}</b>
                                    </td>
                                    <td>
                                        {$checks.$key.STATUS_MSG}
                                    </td>
                                    <td>
                                        {if $checks.$key.ERROR_MSG}
                                            {$checks.$key.ERROR_MSG}
                                        {/if}
                                    </td>
                                </tr>
                            {/foreach}
                        </tbody>
                    </table>
                </div>
            </div>
        </div>

    {elseif $method == "rootOC_migrate_dialog"}
        <h2>{t}Add required object classes to the LDAP base{/t}</h2>

        <div class="migrate-wrapper card-content-scroll">
            <div class="row">
                <div class="col s6">
                    <h3>{t}Current{/t}</h3>
                    <pre>{$details.current}</pre>
                </div>
                <div class="col s6">
                    <h3>{t}After migration{/t}</h3>
                    <pre>{$details.target}</pre>
                </div>
            </div>
        </div>

        <div class="plugin-actions card-action">
            <button class="btn-small primary" type='submit' name='rootOC_migrate_start'>{t}Migrate{/t}</button>
            <button class="btn-small primary" type='submit' name='rootOC_dialog_cancel'>{t}Close{/t}</button>
        </div>

    {elseif $method == "create_acls"}

        <h2>{t}Create a new GOsa administrator account{/t}</h2>
        <p>{t}This dialog will automatically add a new super administrator to your LDAP tree.{/t}</p>

        <div class="row">
            <div class="col s12 xl6 create-admin-account">
                <div class="input-field">
                    <input id="admin-name" type='text' name='admin-name' value='System administrator' disabled>
                    <label for="admin-name">{t}Name{/t}:</label>
                </div>
            </div>
            <div class="col s12 xl6 connection-setting">
                <div class="input-field">
                    <input id="user-id" type='text' value='{$new_user_uid}' name='new_user_uid'>
                    <label for="user-id">{t}User ID{/t}:</label>
                </div>
            </div>
            <div class="col s12 xl6 connection-setting">
                <div class="input-field">
                    <input id="admin-pw" type='password' value='{$new_user_password}' name='new_user_password'>
                    <label for="admin-pw">{t}Password{/t}:</label>
                </div>
            </div>
            <div class="col s12 xl6 connection-setting">
                <div class="input-field">
                    <input id="admin-pw-again" type='password' value='{$new_user_password2}' name='new_user_password2'>
                    <label for="admin-pw-again">{t}Password (again){/t}:</label>
                </div>
            </div>
        </div>

        <script language="JavaScript" type="text/javascript">
            // First input field on page
            focus_field('new_user_password');
        </script>

        <div class="plugin-actions card-action setup-exception">
            <button class="btn-small primary" type='submit' name='create_admin_user'>{t}Apply{/t}</button>
            <button class="btn-small primary" type='submit' name='create_acls_cancel'>{t}Cancel{/t}</button>
        </div>
    {/if}
</div>
