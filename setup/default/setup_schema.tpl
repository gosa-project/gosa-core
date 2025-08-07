<div class="content-wrapper card-content-scroll">
    <h2>{t}Schema specific settings{/t}</h2>

    {if !$checkFailed}
        <div class="success-message">
            <i class="material-icons green-text">check</i>
            <p class="green-text">{t}Schema check succeeded{/t}</p>
        </div>
    {else}
        <div class="row valign-wrapper">
            <div class="col s2 attention-icon">
                <i class="material-icons orange-text">warning</i>
            </div>

            <div class="col s10 attention-content">
                <h3>{t}Attention{/t}</h3>

                {t}Schema check failed{/t}

                {if $ocCount == 0}
                    <p>
                        {t}Could not read any schema information, all checks skipped. Adjust your LDAP ACLs.{/t}
                    {if !$database_initialised}
                        <br>
                        {t}It seems that your LDAP database wasn't initialized yet. This maybe the reason, why GOsa can't read your schema configuration!{/t}
                    {/if}
                    </p>
                {else}
                    {$message}
                {/if}
            </div>
        </div>
    {/if}


    <input type='hidden' value='1' name='step7_posted'>
</div>
