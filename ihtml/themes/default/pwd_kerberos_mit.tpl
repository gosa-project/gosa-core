<table style="width:100%;">
	<tr>
		<td style="width:50%;vertical-align:top;">
			<h2>{t}Heimdal options{/t}</h2>
			<i>{t}Use empty values for infinite{/t}</i>
			<table style="width:100%;">
				<tr>
					<td>
						<label for="goKrbRealm">{t}Realm{/t}</label>
					</td>
					<td>
						<select name="goKrbRealm" onChange="document.mainform.submit();">
						{foreach from=$server_list item=item key=key}
							{if $item.goKrbRealm==$goKrbRealm}
							<option selected value="{$item.goKrbRealm}">{$item.goKrbRealm} - {$key}</option>
							{else}
							<option value="{$item.goKrbRealm}">{$item.goKrbRealm} - {$key}</option>
							{/if}
						{/foreach}
						</select>
					</td>
				<tr>
				</tr>
					<td>
						<label for="POLICY">{t}POLICY{/t}</label>
					</td>
					<td>
						<select name="POLICY">
						{foreach from=$POLICIES item=item key=key}
							{if $POLICY==$item}
							<option selected value="{$item}">{$item}</option>
							{else}
							<option value="{$item}">{$item}</option>
							{/if}
						{/foreach}
						</select>
					</td>
				</tr>
				<tr>
					<td>
						<label for="MAX_LIFE">{t}Ticket max life{/t}</label>
					</td>
					<td>
						<input id="MAX_LIFE" type="text" name="MAX_LIFE" value="{$MAX_LIFE}"> 
					</td>
				</tr>
				<tr>
					<td>
						<label for="MAX_RENEWABLE_LIFE">{t}Ticket max renew{/t}</label>
					</td>
					<td>
						<input id="MAX_RENEWABLE_LIFE" type="text" name="MAX_RENEWABLE_LIFE" value="{$MAX_RENEWABLE_LIFE}">
					</td>
				</tr>
			</table>
			<br>
			<br>
			<table>
				<tr>
					<td>
					</td>
					<td style="width:40px;"><i>{t}infinite{/t}</i>
					</td>
					<td><i>{t}Hour{/t}</i>
					</td>
					<td style="width:60px;"><i>{t}Minute{/t}</i>
					</td>
					<td><i>{t}Day{/t}</i>
					</td>
					<td><i>{t}Month{/t}</i>
					</td>
					<td><i>{t}Year{/t}</i>
					</td>
				</tr>
				<tr>
					<td>
						<label for="PRINC_EXPIRE_TIME">{t}Valid ticket end time{/t}</label>
					</td>
					<td>
						<input type="checkbox" name="PRINC_EXPIRE_TIME_clear" 
							onClick="	changeState('PRINC_EXPIRE_TIME_y');
									  	changeState('PRINC_EXPIRE_TIME_m');
									  	changeState('PRINC_EXPIRE_TIME_d');
									  	changeState('PRINC_EXPIRE_TIME_h');
									  	changeState('PRINC_EXPIRE_TIME_i');"
							{if $PRINC_EXPIRE_TIME_clear} checked {/if}
						>
					</td>
					<td>
						<select name="PRINC_EXPIRE_TIME_h" id="PRINC_EXPIRE_TIME_h" {if $PRINC_EXPIRE_TIME_clear} disabled {/if}>
							{html_options options=$hours selected=$PRINC_EXPIRE_TIME_h}
						</select>
					</td>
					<td>
						<select name="PRINC_EXPIRE_TIME_i" id="PRINC_EXPIRE_TIME_i" {if $PRINC_EXPIRE_TIME_clear} disabled {/if}>
							{html_options options=$minutes selected=$PRINC_EXPIRE_TIME_i}
						</select>
					</td>
					<td>
						<select name="PRINC_EXPIRE_TIME_d" id="PRINC_EXPIRE_TIME_d" {if $PRINC_EXPIRE_TIME_clear} disabled {/if}>
							{html_options options=$days selected=$PRINC_EXPIRE_TIME_d}
						</select>
					</td>
					<td>
						<select name="PRINC_EXPIRE_TIME_m" id="PRINC_EXPIRE_TIME_m" {if $PRINC_EXPIRE_TIME_clear} disabled {/if}>
							{html_options options=$month selected=$PRINC_EXPIRE_TIME_m}
						</select>
					</td>
					<td>
						<select name="PRINC_EXPIRE_TIME_y" id="PRINC_EXPIRE_TIME_y" {if $PRINC_EXPIRE_TIME_clear} disabled {/if}>
							{html_options options=$years selected=$PRINC_EXPIRE_TIME_y}
						</select>
					</td>
				</tr>
				<tr>
					<td>
						<label for="PW_EXPIRATION">{t}Password end{/t}</label>
					</td>
					<td>
						<input type="checkbox" name="PW_EXPIRATION_clear" 
							onClick="	changeState('PW_EXPIRATION_y');
									  	changeState('PW_EXPIRATION_m');
									  	changeState('PW_EXPIRATION_d');
									  	changeState('PW_EXPIRATION_h');
									  	changeState('PW_EXPIRATION_i');"
							{if $PW_EXPIRATION_clear} checked {/if}
						>
					</td>
					<td>
						<select name="PW_EXPIRATION_h" id="PW_EXPIRATION_h" {if $PW_EXPIRATION_clear} disabled {/if}>
							{html_options options=$hours selected=$PW_EXPIRATION_h}
						</select>
					</td>
					<td>
						<select name="PW_EXPIRATION_i" id="PW_EXPIRATION_i" {if $PW_EXPIRATION_clear} disabled {/if}>
							{html_options options=$minutes selected=$PW_EXPIRATION_i}
						</select>

					</td>
					<td>
						<select name="PW_EXPIRATION_d" id="PW_EXPIRATION_d" {if $PW_EXPIRATION_clear} disabled {/if}>
							{html_options options=$days selected=$PW_EXPIRATION_d}
						</select>
					</td>
					<td>
						<select name="PW_EXPIRATION_m" id="PW_EXPIRATION_m" {if $PW_EXPIRATION_clear} disabled {/if}>
							{html_options options=$month selected=$PW_EXPIRATION_m}
						</select>
					</td>
					<td>
						<select name="PW_EXPIRATION_y" id="PW_EXPIRATION_y" {if $PW_EXPIRATION_clear} disabled {/if}>
							{html_options options=$years selected=$PW_EXPIRATION_y}
						</select>
					</td>
				</tr>
			</table>
			<br>
			<br>
			<h2>{t}Status{/t}</h2>
			<table>
				<tr>
					<td style='width:160px;'>{t}Failed logins{/t}</td>
					<td>{if !$FAIL_AUTH_COUNT} {t}none{/t} {else} {$FAIL_AUTH_COUNT} {/if}</td>
				</tr>
				<tr>
					<td>{t}Key version number{/t}</td>
					<td>{$KVNO}</td>
				</tr>
				<tr>
					<td>{t}Last failed login{/t}</td>
					<td>{if !$LAST_FAILED} {t}none{/t} {else} {$LAST_FAILED} {/if}</td>
				</tr>
				<tr>
					<td>{t}Last password change{/t}</td>
					<td>{if !$LAST_PWD_CHANGE} {t}none{/t} {else} {$LAST_PWD_CHANGE} {/if}</td>
				</tr>
				<tr>
					<td>{t}Last successful login{/t}</td>
					<td>{if !$LAST_SUCCESS} {t}none{/t} {else} {$LAST_SUCCESS} {/if}</td>
				</tr>
				<tr>
					<td>{t}Last modification date{/t}</td>
					<td>{if !$MOD_DATE} {t}none{/t} {else} {$MOD_DATE} {/if}</td>
				</tr>
			</table>
	
	
		</td>	
		<td style="vertical-align:top;">
			<h2>Flags</h2>
			<table width="100%">
				<tr>
					<td style="width:50%;">
<input {if $DISALLOW_SVR} checked {/if} class="center"
        name="DISALLOW_SVR" value="1" type="checkbox">{t}Prohibit issuance of service tickets{/t}<br>
<input {if $DISALLOW_FORWARDABLE} checked {/if} class="center" 
		name="DISALLOW_FORWARDABLE" value="1" type="checkbox">{t}Prohibit forwardable tickets{/t}<br>
<input {if $DISALLOW_PROXIABLE} checked {/if} class="center" 
		name="DISALLOW_PROXIABLE" value="1" type="checkbox">{t}Disallow proxiable tickets{/t}<br>
<input {if $DISALLOW_RENEWABLE} checked {/if} class="center" 
		name="DISALLOW_RENEWABLE" value="1" type="checkbox">{t}Prohibit renewable tickets{/t}<br>
<input {if $DISALLOW_POSTDATED} checked {/if} class="center" 
		name="DISALLOW_POSTDATED" value="1" type="checkbox">{t}Pohibit postdated tickets{/t}<br>
<input {if $DISALLOW_TGT_BASED} checked {/if} class="center" 
		name="DISALLOW_TGT_BASED" value="1" type="checkbox">{t}Disallow Ticket-Granting Service{/t}<br>
<input {if $PWCHANGE_SERVICE} checked {/if} class="center" 
		name="PWCHANGE_SERVICE" value="1" type="checkbox">{t}Password change service{/t}<br>
<input {if $REQUIRES_PRE_AUTH} checked {/if} class="center" 
		name="REQUIRES_PRE_AUTH" value="1" type="checkbox">{t}Preauthentication required{/t}<br>
<input {if $REQUIRES_PWCHANGE} checked {/if} class="center" 
		name="REQUIRES_PWCHANGE" value="1" type="checkbox">{t}Force a password change{/t}<br>
<input {if $REQUIRES_HW_AUTH} checked {/if} class="center" 
		name="REQUIRES_HW_AUTH" value="1" type="checkbox">{t}Hardware preauthentication{/t}<br>
<input {if $DISALLOW_DUP_SKEY} checked {/if} class="center" 
		name="DISALLOW_DUP_SKEY" value="1" type="checkbox">{t}Disallow user to user authentification{/t}<br>
<input {if $DISALLOW_ALL_TIX} checked {/if} class="center" 
		name="DISALLOW_ALL_TIX" value="1" type="checkbox">{t}Forbid ticket issuance{/t}<br>
 	 				</td>
				</tr>
			</table>
		</td>
	</tr>
</table>
<input type="hidden" name="pwd_heimdal_posted" value="1">
<p class="seperator"></p>
<p style="text-align:right;">
	<input type="submit" name="pw_save" value="{t}Save{/t}">
	&nbsp;
	<input type="submit" name="pw_abort" value="{t}Cancel{/t}">
</p>
