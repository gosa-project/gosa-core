<div class="policy-wrapper">
    {if !$policy_config && !$twoFactor_config}
    <div class="row">
        <h2>Activate account policy overlay</h2>
        <div class="col s12 xl6 policy-overlay">
            <label>
                <input onclick="document.mainform.submit()" type="checkbox" {if $policy} checked {/if} id="policy-overlay" name="policy" value="1">
                <span>Use account policy?</span>
            </label>
        </div>

        <div class="col s23 xl6">
            <button {if !$policy} disabled {/if} class="btn-small" type="submit" name="config_policy">Config policies</button>
        </div>
    </div>

    <div class="row">
        <h2>Activate two factor authentication</h2>
        <div class="col s12 xl6 two-factor-overlay">
            <label>
                <input onclick="document.mainform.submit()" type="checkbox" {if $twoFactor} checked {/if} id="two-factor-overlay" name="twoFactor" value="1">
                <span>Use two factor authentication?</span>
            </label>
        </div>
    </div>
    {elseif $policy_config}
    <h2>Create the password policy </h2>
    
    <div class="row">
        <div class="col s12 xl6">
            <div class="input-field">
                <input type="text" id="pwdFailureCountInterval" {if $pwdFailureCountInterval}
                    value="{$pwdFailureCountInterval}" {/if} name="pwdFailureCountInterval">
                <label for="pwdFailureCountInterval">Failure count interval</label>
            </div>
    
            <div class="valign-wrapper" style="display: flex;">
                <label>
                    <input type="checkbox" name="enable_pwdInHistory" id="enable_pwdInHistory"
                        onclick="enableHistoryInput()">
                    <span>Set maximum number of used Passwords stored</span>
                </label>
                <div class="input-field" style="margin-left: 15px;">
                    <input type="text" id="pwdInHistory" name="pwdInHistory" disabled {if $pwdInHistory}
                        value="{$pwdInHistory}" {/if}>
                </div>
            </div>
    
            <div class="input-field">
                <input type="text" id="pwdMaxFailure" name="pwdMaxFailure" {if $pwdMaxFailure} value="{$pwdMaxFailure}"
                    {/if}>
                <label for="pwdMaxFailure">Maximum login attempts</label>
            </div>
    
            <div class="input-field">
                <input type="text" id="pwdLockoutDuration" name="pwdLockoutDuration" {if $pwdLockoutDuration}
                    value="{$pwdLockoutDuration}" {/if}>
                <label for="pwdLockoutDuration">Waiting time in seconds, after too many consecutive failed connection
                    attempts.</label>
            </div>
    
            <div class="input-field">
                <input type="text" id="pwdMinLength" name="pwdMinLength" {if $pwdMinLength} value="{$pwdMinLength}" {/if}>
                <label for="pwdMinLength">Minimum number of characters for password</label>
            </div>
        </div>
    </div>

    <div class="plugin-actions card-action setup-exception">
        <button class="btn-small primary" type='submit' name='create_policy'>{t}Apply{/t}</button>
        <button class="btn-small primary" type='submit' name='create_policy_cancel'>{t}Cancel{/t}</button>
    </div>
    {elseif $twoFactor_config}
        <div>
            2FA works!
        </div>
    {/if}
</div>


<script language="JavaScript" type="text/javascript">
    // First input field on page
    focus_field('pwdFailureCountInterval');

    function enablePolicy() {
        var enable_policy = document.getElementById('policy-overlay')
    }

    function enableHistoryInput() {
        var chkEnableHistory = document.getElementById('enable_pwdInHistory');
        var enableHistory = document.getElementById('pwdInHistory');
        enableHistory.disabled = !chkEnableHistory.checked;
    }

</script>

