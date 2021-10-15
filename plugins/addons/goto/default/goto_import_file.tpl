<div class="goto-import-wrapper">
    <h3>{t}Import jobs{/t}</h3>
    <p>{t}You can import a list of jobs into the GOsa job queue. This should be a semicolon separated list of items in the following format:{/t}</p>
    <span style="font-style: italic;">{t}time stamp{/t} ; {t}MAC-address{/t} ; {t}job type{/t} ; {t}object group{/t} [ ; {t}import base{/t} ; {t}full host name{/t} ; {t}IP address{/t} ; {t}DHCP group{/t} ]</span>

    <hr class="divider">

	{if !$count}
    <h5>{t}Example{/t}:</h5>
    <p>20080626162556 <b>;</b> 00:0C:29:99:1E:37 <b>;</b> job_trigger_activate_new <b>;</b> goto-client <b>;</b> dc=test,dc=gonicus,dc=de</p>
    {/if}

    <h5>{t}Select list to import{/t}</h5>
    <div class="row">
        <div class="input-field add valign-wrapper col s12 xl6">
            <div class="file-field">
                <div class="btn-small">
                    <span>{t}Browse{/t}</span>
                    <input type='file' name='file' value="">
                </div>
                <div class="file-path-wrapper">
                    <input class="file-path validate" type="text">
                </div>
            </div>
            <button class="btn-small" type='submit' name='import'>{t}Upload{/t}</button>
        </div>
    </div>


	{if $count}
    <hr class="divider">

    <table id="goto-import-table" class="striped">
		<thead>
			<tr>
				<th>{t}Time stamp{/t}</th>
				<th>{t}MAC{/t}</th>
				<th>{t}Event{/t}</th>
				<th>{t}Object group{/t}</th>
				<th>{t}Base{/t}</th>
				<th>{t}FQDN{/t}</th>
				<th>{t}IP{/t}</th>
				<th>{t}DHCP{/t}</th>
			</tr>
		</thead>
		<tbody>
			{foreach from=$info item=item key=key}
			{if !$item.ERROR}
			<tr>
			{else}
			<tr style='background-color: #F0BBBB;'>
			{/if}
				<td>{$item.TIMESTAMP}</td>
				<td>{$item.MAC}</td>
				<td>{$item.HEADER}</td>
				<td>{$item.OGROUP}</td>
				<td>{$item.BASE}</td>
				<td>{$item.FQDN}</td>
				<td>{$item.IP}</td>
				<td>{$item.DHCP}</td>
			</tr>
			{if $item.ERROR}
			<tr style='background-color: #F0BBBB;'>
				<td colspan="7"><b>{$item.ERROR}</b></td>
			</tr>
			{/if}
			{/foreach}
		</tbody>
    </table>
    {/if}
</div>

<div class="card-action">
    <button class="btn-small primary" type='submit' name='start_import'>{t}Import{/t}</button>
    <button class="btn-small primary" type='submit' name='import_abort'>{msgPool type=backButton}</button>
</div>
