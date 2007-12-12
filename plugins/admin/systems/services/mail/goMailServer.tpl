<h2><img class="center" alt="" align="middle" src="images/rightarrow.png" /> {t}Generic{/t}</h2>
<table  style="width:100%;">
	<tr>
		<td width="50%" style="vertical-align:top">
			<table >
				<tr>
					<td>{t}Visible full qualified hostname{/t}
					</td>
					<td>
{render acl=$postfixMyhostnameACL}
						<input type="text" name='postfixMyhostname' value='{$postfixMyhostname}' title='{t}The full qualified host name.{/t}'>
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
		<td style="border-left:1px solid #A0A0A0; ">
			<table style="width:100%;">
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
						<input type="submit" name="AddpostfixMyNetworks" value="{t}Add{/t}">
{/render}
{render acl=$postfixMyNetworksACL}
						<input type="submit" name="DelpostfixMyNetworks" value="{t}Remove{/t}">
{/render}
					</td>
				</tr>
			</table>
		</td>
	</tr>
	<tr>
		<td colspan="2">
			<p class='seperator'>&nbsp;</p>
			<h2>{t}Domains and routing{/t}</h2>
		</td>
	</tr>	
	<tr>
		<td>
			  <table style="width:100%;">
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
                        <input type="submit" name="AddpostfixMyDestinations" value="{t}Add{/t}">
{/render}
{render acl=$postfixMyDestinationsACL}
                        <input type="submit" name="DelpostfixMyDestinations" value="{t}Remove{/t}">
{/render}
                    </td>
                </tr>
            </table>
		</td>
		<td style="border-left:1px solid #A0A0A0; ">
			  <table style="width:100%;">
                <tr>
                    <td>
                        {t}Transports{/t}<br>
{render acl=$postfixTransportTableACL}
						{$Div_postfixTransportTable}
{/render}

{render acl=$postfixTransportTableACL}
                        <input type="text" name="Source_postfixTransportTable" value="">
{/render}
{render acl=$postfixTransportTableACL}
                        <select name='TransportProtocol' title='{t}Select a transport protocol.{/t}'>
                            {html_options options=$TransportProtocols}
                        </select>
{/render}
{render acl=$postfixTransportTableACL}
                        <input type="text" name="Destination_postfixTransportTable" value="">
{/render}
{render acl=$postfixTransportTableACL}
                        <input type="submit" name="AddpostfixTransportTable" value="{t}Add{/t}">
{/render}
                    </td>
                </tr>
            </table>
		</td>
	</tr>
	<tr>
		<td colspan="2">
			<p class='seperator'>&nbsp;</p>
			<h2>{t}Restrictions{/t}</h2>
		</td>
	</tr>	
	<tr>
		<td>
            <table style="width:100%;">
                <tr>
                    <td>
                        {t}Restrictions for sender{/t}<br>
{render acl=$postfixSenderRestrictionsACL}
						{$Div_postfixSenderRestrictions}
{/render}
{render acl=$postfixSenderRestrictionsACL}
                        <input type="text" name="Source_postfixSenderRestrictions" value="">
{/render}
{render acl=$postfixSenderRestrictionsACL}
                        <select name='SenderRestrictionFilter' title='{t}Restriction filter{/t}'>
                            {html_options options=$RestrictionFilters}
                        </select>
{/render}
{render acl=$postfixSenderRestrictionsACL}
                        <input type="text" name="Destination_postfixSenderRestrictions" value="">
{/render}
{render acl=$postfixSenderRestrictionsACL}
                        <input type="submit" name="AddpostfixSenderRestrictions" value="{t}Add{/t}">
{/render}
                    </td>
                </tr>
            </table>
		</td>
		<td style="border-left:1px solid #A0A0A0;">
            <table style="width:100%;">
                <tr>
                    <td>
                        {t}Restrictions for recipient{/t}<br>
{render acl=$postfixRecipientRestrictionsACL}
						{$Div_postfixRecipientRestrictions}
{/render}
{render acl=$postfixRecipientRestrictionsACL}
                        <input type="text" name="Source_postfixRecipientRestrictions" value="">
{/render}
{render acl=$postfixRecipientRestrictionsACL}
                        <select name='RecipientRestrictionFilter' title='{t}Restriction filter{/t}'>
                            {html_options options=$RestrictionFilters}
                        </select>
{/render}
{render acl=$postfixRecipientRestrictionsACL}
                        <input type="text" name="Destination_postfixRecipientRestrictions" value="">
{/render}
{render acl=$postfixRecipientRestrictionsACL}
                        <input type="submit" name="AddpostfixRecipientRestrictions" value="{t}Add{/t}">
{/render}
                    </td>
                </tr>
            </table>
		</td>
	</tr>	
</table>

<p class='seperator'>&nbsp;</p>
<br>
<h2><img class="center" alt="" src="images/rocket.png" align="middle"> Action</h2>
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
<input type='submit' name='ExecAction' title='{t}Set status{/t}' value='{t}Execute{/t}' 
	{if $is_new == "new" || $is_acc == false} disabled {/if}
>

<p class="seperator">&nbsp;</p>
<div style="width:100%; text-align:right;padding-top:10px;padding-bottom:3px;">
    <input type='submit' name='SaveService' value='{t}Save{/t}'>
    &nbsp;
    <input type='submit' name='CancelService' value='{t}Cancel{/t}'>
</p>

