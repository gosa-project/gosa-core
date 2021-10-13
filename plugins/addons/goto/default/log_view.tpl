<div class="system-wrapper">
    <div class="goto-log-wrapper">
        {if !$ACL}
        <h2>{msgPool type=permView}</h2>
        {else}

        {if $logs_available}
        <h2>{t}Available logs{/t}</h2>

        {$listing}

        <hr class="divider">

        <h3>{t}Selected log{/t}: {$selected_log}</h3>
        {$log_file}

        {else}
        <h2>{t}No logs for this host available!{/t}</h2>
        {/if}

        {/if}

        {if $standalone}
        <input type="hidden" name="ignore" value="1">

        <div class="card-action">
            <button class="btn-small primary" type="submit" name="abort_event_dialog">{msgPool type=backButton}</button>
        </div>
        {/if}
    </div>
</div>
