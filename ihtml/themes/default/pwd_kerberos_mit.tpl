<table style="width:100%;">
	<tr>
		<td style="width:50%;vertical-align:top;">
			<h2>{t}Heimdal options{/t}</h2>
			<i>{t}Use empty values for infinite{/t}</i>
			<table>
				<tr>
					<td>
						<label for="MAXLIFE">{t}Ticket max life{/t}</label>
					</td>
					<td colspan="6">
						<input id="MAXLIFE" type="text" name="MAXLIFE" value="{$MAXLIFE}"> 
					</td>
				</tr>
				<tr>
					<td>
						<label for="MAXRENEWLIFE">{t}Ticket max renew{/t}</label>
					</td>
					<td colspan="6">
						<input id="MAXRENEWLIFE" type="text" name="MAXRENEWLIFE" value="{$MAXRENEWLIFE}">
					</td>
				</tr>
				<tr>
					<td colspan="7">
						&nbsp;
					</td>
				</tr>
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
						<label for="EXPIRE">{t}Valid ticket end time{/t}</label>
					</td>
					<td>
						<input type="checkbox" name="EXPIRE_clear" 
							onClick="	changeState('EXPIRE_y');
									  	changeState('EXPIRE_m');
									  	changeState('EXPIRE_d');
									  	changeState('EXPIRE_h');
									  	changeState('EXPIRE_i');"
							{if $EXPIRE_clear} checked {/if}
						>
					</td>
					<td>
						<select name="EXPIRE_h" id="EXPIRE_h" {if $EXPIRE_clear} disabled {/if}>
							{html_options options=$hours selected=$EXPIRE_h}
						</select>
					</td>
					<td>
						<select name="EXPIRE_i" id="EXPIRE_i" {if $EXPIRE_clear} disabled {/if}>
							{html_options options=$minutes selected=$EXPIRE_i}
						</select>
					</td>
					<td>
						<select name="EXPIRE_d" id="EXPIRE_d" {if $EXPIRE_clear} disabled {/if}>
							{html_options options=$days selected=$EXPIRE_d}
						</select>
					</td>
					<td>
						<select name="EXPIRE_m" id="EXPIRE_m" {if $EXPIRE_clear} disabled {/if}>
							{html_options options=$month selected=$EXPIRE_m}
						</select>
					</td>
					<td>
						<select name="EXPIRE_y" id="EXPIRE_y" {if $EXPIRE_clear} disabled {/if}>
							{html_options options=$years selected=$EXPIRE_y}
						</select>
					</td>
				</tr>
				<tr>
					<td>
						<label for="PWEXPIRE">{t}Password end{/t}</label>
					</td>
					<td>
						<input type="checkbox" name="PWEXPIRE_clear" 
							onClick="	changeState('PWEXPIRE_y');
									  	changeState('PWEXPIRE_m');
									  	changeState('PWEXPIRE_d');
									  	changeState('PWEXPIRE_h');
									  	changeState('PWEXPIRE_i');"
							{if $PWEXPIRE_clear} checked {/if}
						>
					</td>
					<td>
						<select name="PWEXPIRE_h" id="PWEXPIRE_h" {if $PWEXPIRE_clear} disabled {/if}>
							{html_options options=$hours selected=$PWEXPIRE_h}
						</select>
					</td>
					<td>
						<select name="PWEXPIRE_i" id="PWEXPIRE_i" {if $PWEXPIRE_clear} disabled {/if}>
							{html_options options=$minutes selected=$PWEXPIRE_i}
						</select>

					</td>
					<td>
						<select name="PWEXPIRE_d" id="PWEXPIRE_d" {if $PWEXPIRE_clear} disabled {/if}>
							{html_options options=$days selected=$PWEXPIRE_d}
						</select>
					</td>
					<td>
						<select name="PWEXPIRE_m" id="PWEXPIRE_m" {if $PWEXPIRE_clear} disabled {/if}>
							{html_options options=$month selected=$PWEXPIRE_m}
						</select>
					</td>
					<td>
						<select name="PWEXPIRE_y" id="PWEXPIRE_y" {if $PWEXPIRE_clear} disabled {/if}>
							{html_options options=$years selected=$PWEXPIRE_y}
						</select>
					</td>
				</tr>
			</table>
		</td>	
		<td>
			<h2>Flags</h2>
			<table width="100%">
				<tr>
					<td style="width:50%;">
<input {if $KRB5_KDB_DISALLOW_SVR} checked {/if} class="center"
        name="KRB5_KDB_DISALLOW_SVR" value="1" type="checkbox">{t}Prohibit issuance of service tickets{/t}<br>
<input {if $KRB5_KDB_DISALLOW_FORWARDABLE} checked {/if} class="center" 
		name="KRB5_KDB_DISALLOW_FORWARDABLE" value="1" type="checkbox">{t}Prohibit forwardable tickets{/t}<br>
<input {if $KRB5_KDB_DISALLOW_PROXIABLE} checked {/if} class="center" 
		name="KRB5_KDB_DISALLOW_PROXIABLE" value="1" type="checkbox">{t}Disallow proxiable tickets{/t}<br>
<input {if $KRB5_KDB_DISALLOW_RENEWABLE} checked {/if} class="center" 
		name="KRB5_KDB_DISALLOW_RENEWABLE" value="1" type="checkbox">{t}Prohibit renewable tickets{/t}<br>
<input {if $B5_KDB_DISALLOW_POSTDATED} checked {/if} class="center" 
		name="B5_KDB_DISALLOW_POSTDATED" value="1" type="checkbox">{t}Pohibit postdated tickets{/t}<br>
<input {if $KRB5_KDB_DISALLOW_TGT_BASED} checked {/if} class="center" 
		name="KRB5_KDB_DISALLOW_TGT_BASED" value="1" type="checkbox">{t}Disallow Ticket-Granting Service{/t}<br>
<input {if $KRB5_KDB_PWCHANGE_SERVICE} checked {/if} class="center" 
		name="KRB5_KDB_PWCHANGE_SERVICE" value="1" type="checkbox">{t}Password change service{/t}<br>
<input {if $KRB5_KDB_REQUIRES_PRE_AUTH} checked {/if} class="center" 
		name="KRB5_KDB_REQUIRES_PRE_AUTH" value="1" type="checkbox">{t}Preauthentication required{/t}<br>
<input {if $KRB5_KDB_REQUIRES_PWCHANGE} checked {/if} class="center" 
		name="KRB5_KDB_REQUIRES_PWCHANGE" value="1" type="checkbox">{t}Force a password change{/t}<br>
<input {if $KRB5_KDB_REQUIRES_HW_AUTH} checked {/if} class="center" 
		name="KRB5_KDB_REQUIRES_HW_AUTH" value="1" type="checkbox">{t}Hardware preauthentication{/t}<br>
<input {if $KRB5_KDB_DISALLOW_DUP_SKEY} checked {/if} class="center" 
		name="KRB5_KDB_DISALLOW_DUP_SKEY" value="1" type="checkbox">{t}Disallow user to user authentification{/t}<br>
<input {if $KRB5_KDB_DISALLOW_ALL_TIX} checked {/if} class="center" 
		name="KRB5_KDB_DISALLOW_ALL_TIX" value="1" type="checkbox">{t}Forbid ticket issuance{/t}<br>
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
