<div class="content-wrapper card-content-scroll">
    <p>
        {t}The main data source used in GOsa is LDAP. In order to access the information stored there, please enter the required information.{/t}
    </p>

    {if $resolve_user}

        <b>{t}Please choose the LDAP user to be used by GOsa{/t}</b>

        <form class="col s12">
            <div class="row valign-wrapper">
                <div class="input-field col s6">
                    <select name="admin_to_use">
                        {html_options options=$resolved_users selected=$admin}
                    </select>
                </div>
                <div class="input-field col s5">
                    <input type="search" id="search" name="resolve_filter" value='{$resolve_filter}'>
                    <i class="material-icons">close</i>
                </div>
                <div class="col s1">
                    <button class="btn-small primary right" type='submit' name='resolve_search'>{t}Search{/t}</button>
                </div>
            </div>
            <div class="row">
                <div class="col s12">
                    <button class="btn-small primary" type='submit' name='use_selected_user'>{t}Apply{/t}</button>
                    <button class="btn-small primary right" type='submit' name='resolve_user'>{t}Cancel{/t}</button>
                </div>
            </div>
        </form>
    {else}
        <div class="connection-wrapper">
            <h2>{t}LDAP connection{/t}</h2>

            <div class="row">
                <div class="col s12 xl6 connection-setting">
                    <div class="input-field">
                        <input id="location-name" type='text' name='location' maxlength='80' size='40' value='{$location}'>
                        <label for="location-name">{t}Location name{/t}</label>
                    </div>
                </div>

                <div class="col s12 xl6 connection-setting">
                    <div class="input-field">
                        <input id="connection-uri" type='text' name='connection' maxlength='80' size='40' value='{$connection}'>
                        <label for="connection-uri">{t}Connection URI{/t}</label>
                    </div>
                </div>

                <div class="col s12 xl6 connection-setting">
                    <div class="input-field">
                        <select id="tls-connection" name="tls" size="1" title="">
                            {html_options options=$bool selected=$tls}
                        </select>
                        <label for="tls-connection">{t}TLS connection{/t}</label>
                    </div>
                </div>

                <div class="col s12 xl6 connection-setting">
                    <div class="input-field">
                        {if $namingContextsCount >= 1}
                            <select name='base' size=1>
                                {html_options values=$namingContexts output=$namingContexts selected=$base}
                            </select>
                        {else}
                            <input type='text' name='base' maxlength='80' size='40' value='{$base}'>
                        {/if}
                        <label>{t}Base{/t}</label>
                    </div>

                    <button class="btn-flat" type="submit" name="reload" title="{t}Reload{/t}"><i class="material-icons">refresh</i></button>
                </div>
            </div>
        </div>

        <hr class="divider">

        <div class="authentication-wrapper">
            <h2>{t}Authentication{/t}</h2>

            <div class="row">
                <div class="col s12 xl6 connection-setting">
                    <div class="input-field">
                        <input id="administrator-dn" type='text' name='admin_given' maxlength='160' size='40' value='{$admin_given}'>
                        <label for="administrator-dn">{t}Administrator DN{/t}</label>
                    </div>
                    <p>
                        {if $append_base_to_admin_dn},{$base_to_append}{/if}
                    </p>
                    <button class="btn-flat" type="submit" name="resolve_user" title="{t}Select user{/t}"><i class="material-icons">folder_open</i></button>
                </div>

                <div class="col s12 xl6 connection-setting">
                    <label class="checkbox">
                        <p>
                            <input onClick='document.mainform.submit();'
                            {if $append_base_to_admin_dn} checked {/if}
                            type='checkbox' name='append_base_to_admin_dn' value='1'/>
                            <span>{t}Automatically append LDAP base to administrator DN{/t}</span>
                        </p>
                    </label>
                </div>

                <div class="col s12 xl6 connection-setting">
                    <div class="input-field">
                        <input id="administrator-password" type='password' name='password' value='{$password}'>
                        <label for="administrator-password">{t}Administrator password{/t}</label>
                    </div>
                </div>
            </div>
        </div>

        <hr class="divider">

        <div class="schema-wrapper">
            <h2>{t}Schema based settings{/t}</h2>

            <div class="row">
                <div class="col s12 xl6 connection-setting">
                    <div class="input-field">
                        <select id="rfc" name="rfc2307bis" size="1" title="">
                            {html_options options=$bool selected=$rfc2307bis}
                        </select>
                        <label for="rfc">{t}Use RFC 2307 bis compliant groups{/t}</label>
                    </div>
                </div>
            </div>
        </div>

        {if !$resolve_user}
            <hr class="divider">

            <div class="status-wrapper">
                <h2>{t}Current status{/t}</h2>

                <div class="row">
                    <div class="col s12 connection-setting">
                        <p class="label">{t}Information{/t}</p>
                        <div class="connection-status">{$connection_status}</div>
                    </div>
                </div>
            </div>
        {/if}
    {/if}

    <!-- Place cursor -->
    <script language="JavaScript" type="text/javascript">
        focus_field('location');
    </script>
</div>
