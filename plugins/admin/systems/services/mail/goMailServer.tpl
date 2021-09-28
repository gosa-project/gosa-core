<h3>{t}Generic{/t}</h3>

<table  style="width:100%;" summary="{t}Mail settings{/t}">
	<tr>
		<td>

			<table summary="{t}Generic settings{/t}">
				<tr>
					<td>{t}Visible fully qualified host name{/t}</td>
					<td>
{render acl=$postfixMyhostnameACL}
						<input type="text" name='postfixMyhostname' value='{$postfixMyhostname}' title='{t}The fully qualified host name.{/t}'>
{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Max mail header size{/t}
					</td>
					<td>
{render acl=$postfixMyhostnameACL}
						<input type="text" name='postfixHeaderSizeLimit' value='{$postfixHeaderSizeLimit}' 
									title='{t}This value specifies the maximal header size.{/t}'>&nbsp;{t}KB{/t}
{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Max mailbox size{/t}
					</td>
					<td>
{render acl=$postfixMailboxSizeLimitACL}
						<input type="text" name='postfixMailboxSizeLimit' value='{$postfixMailboxSizeLimit}' 
									title='{t}Defines the maximal size of mail box.{/t}'>&nbsp;{t}KB{/t}
{/render}					</td>
				</tr>
				<tr>
					<td>{t}Max message size{/t}
					</td>
					<td>
{render acl=$postfixMessageSizeLimitACL}
						<input type="text" name='postfixMessageSizeLimit' value='{$postfixMessageSizeLimit}' 
									title='{t}Specify the maximal size of a message.{/t}'>&nbsp;{t}KB{/t}
{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Relay host{/t}
					</td>
					<td>
{render acl=$postfixRelayhostACL}
						<input type="text" name='postfixRelayhost' value='{$postfixRelayhost}' 
									title='{t}Relay messages to following host:{/t}'>
{/render}
					</td>
				</tr>
			</table>
		</td>
		<td class='left-border'>

			<table summary="{t}Network settings{/t}">
				<tr>
					<td>
						{t}Local networks{/t}<br>
{render acl=$postfixMyNetworksACL}
						<select name='Select_postfixMyNetworks[]' multiple size=6 style='width:100%;' title='{t}Postfix networks{/t}'>
							{html_options options=$postfixMyNetworks}
						</select>
{/render}
{render acl=$postfixMyNetworksACL}
						<input type="text" name="NewString_postfixMyNetworks" value="">
{/render}
{render acl=$postfixMyNetworksACL}
						<button type='submit' name='AddpostfixMyNetworks'>{msgPool type=addButton}</button>

{/render}
{render acl=$postfixMyNetworksACL}
						<button type='submit' name='DelpostfixMyNetworks'>{t}Remove{/t}</button>

{/render}
					</td>
				</tr>
			</table>
		</td>
	</tr>
	<tr>
		<td colspan="2">
			<hr>
			<h3>{t}Domains and routing{/t}</h3>
		</td>
	</tr>	
	<tr>
		<td>
			<table summary="{t}Domains and routing{/t}">
                <tr>
                    <td>
                        {t}Domains to accept mail for{/t}<br>
{render acl=$postfixMyDestinationsACL}
                        <select name='Select_postfixMyDestinations[]' multiple size=6 style='width:100%;' title='{t}Postfix is responsible for the following domains:{/t}'>
{/render}
                            {html_options options=$postfixMyDestinations}
                        </select>
{render acl=$postfixMyDestinationsACL}
                        <input type="text" name="NewString_postfixMyDestinations" value="">
{/render}
{render acl=$postfixMyDestinationsACL}
                        <button type='submit' name='AddpostfixMyDestinations'>{msgPool type=addButton}</button>

{/render}
{render acl=$postfixMyDestinationsACL}
                        <button type='submit' name='DelpostfixMyDestinations'>{t}Remove{/t}</button>

{/render}
                    </td>
                </tr>
            </table>
		</td>
		<td class='left-border'>

			  <table style="width:100%;" summary="{t}Transports{/t}">
                <tr>
                    <td>
                        {t}Transports{/t}<br>
{render acl=$postfixTransportTableACL}
						{$postfixTransportTableList}
{/render}

{render acl=$postfixTransportTableACL}
                        <input type="text" name="Source_postfixTransportTable" value="">
{/render}
{render acl=$postfixTransportTableACL}
                        <select name='TransportProtocol' title='{t}Select a transport protocol.{/t}' size=1>
                            {html_options options=$TransportProtocols}
                        </select>
{/render}
{render acl=$postfixTransportTableACL}
                        <input type="text" name="Destination_postfixTransportTable" value="">
{/render}
{render acl=$postfixTransportTableACL}
                        <button type='submit' name='AddpostfixTransportTable'>{msgPool type=addButton}</button>

{/render}
                    </td>
                </tr>
            </table>
		</td>
	</tr>
	<tr>
		<td colspan="2">
			<hr>
			<h3>{t}Restrictions{/t}</h3>
		</td>
	</tr>	
	<tr>
		<td>
            <table style="width:100%;" summary="{t}Restrictions for sender{/t}">
                <tr>
                    <td>
                        {t}Restrictions for sender{/t}<br>
{render acl=$postfixSenderRestrictionsACL}
						{$postfixSenderRestrictionsList}
{/render}
{render acl=$postfixSenderRestrictionsACL}
                        <input type="text" name="Source_postfixSenderRestrictions" value="">
{/render}
{render acl=$postfixSenderRestrictionsACL}
                        <select name='SenderRestrictionFilter' title='{t}Restriction filter{/t}' size=1>
                            {html_options options=$RestrictionFilters}
                        </select>
{/render}
{render acl=$postfixSenderRestrictionsACL}
                        <input type="text" name="Destination_postfixSenderRestrictions" value="">
{/render}
{render acl=$postfixSenderRestrictionsACL}
                        <button type='submit' name='AddpostfixSenderRestrictions'>{msgPool type=addButton}</button>

{/render}
                    </td>
                </tr>
            </table>
		</td>
		<td class='left-border'>

            <table style="width:100%;" summary="{t}Restrictions for recipient{/t}">
                <tr>
                    <td>
                        {t}Restrictions for recipient{/t}<br>
{render acl=$postfixRecipientRestrictionsACL}
						{$postfixRecipientRestrictionsList}
{/render}
{render acl=$postfixRecipientRestrictionsACL}
                        <input type="text" name="Source_postfixRecipientRestrictions" value="">
{/render}
{render acl=$postfixRecipientRestrictionsACL}
                        <select name='RecipientRestrictionFilter' title='{t}Restriction filter{/t}' size=1>
                            {html_options options=$RestrictionFilters}
                        </select>
{/render}
{render acl=$postfixRecipientRestrictionsACL}
                        <input type="text" name="Destination_postfixRecipientRestrictions" value="">
{/render}
{render acl=$postfixRecipientRestrictionsACL}
                        <button type='submit' name='AddpostfixRecipientRestrictions'>{msgPool type=addButton}</button>

{/render}
                    </td>
                </tr>
            </table>
		</td>
	</tr>	
</table>

<hr>
<br>
<h3>Action</h3>
{if $is_new == "new"}
	{t}The server must be saved before you can use the status flag.{/t}
{elseif !$is_acc}
	{t}The service must be saved before you can use the status flag.{/t}
{/if}
<br>
<select name="action" title='{t}Set new status{/t}' 
	{if $is_new == "new" || $is_acc == false} disabled {/if}
>
	<option value="none">&nbsp;</option>
	{html_options options=$Actions}	
</select>
<button type='submit' name='ExecAction' title="{t}Set status{/t}"
	{if $is_new == "new" || $is_acc == false} disabled {/if}
>{t}Execute{/t}</button>

<hr>

<div class="plugin-actions">
    <button type='submit' name='SaveService'>{msgPool type=saveButton}</button>
    <button type='submit' name='CancelService'>{msgPool type=cancelButton}</button>
</div>

