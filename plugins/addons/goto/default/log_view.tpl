<div class="goto-log-wrapper">
    {if !$ACL}
    <h3>{msgPool type=permView}</h3>
    {else} 
    
    {if $logs_available}
    <h3>{t}Available logs{/t}</h3>
    
    <table style='width:100%;height:40px;' summary="{t}View system logs{/t}">
        <tr>
            <td>
                {$listing}
            </td>
        </tr>
    </table>
    <hr class="divider">
    <h3>{t}Selected log{/t}: {$selected_log}</h3>
    {$log_file} 
    
    {else}
    <h3>{t}No logs for this host available!{/t}</h3>
    {/if} 
    
    {/if} 
    
    {if $standalone}
    <input type="hidden" name="ignore" value="1">
    
    <div class="card-action">
        <button class="btn-small primary" type='submit' name='abort_event_dialog'>{msgPool type=backButton}</button>
    </div>
    {/if}
</div>