
<table style="width:100%;">
	<tr>
		<td style="width:50%;vertical-align:top;">
			<h2>{t}Heimdal options{/t}</h2>
			<i>{t}Use empty values for infinite{/t}</i>
			<table>
				<tr>
					<td>
						<label for="krb5MaxLife">{t}Ticket max life{/t}</label>
					</td>
					<td colspan="6">
						<input id="krb5MaxLife" type="text" name="krb5MaxLife" value="{$krb5MaxLife}"> 
					</td>
				</tr>
				<tr>
					<td>
						<label for="krb5MaxRenew">{t}Ticket max renew{/t}</label>
					</td>
					<td colspan="6">
						<input id="krb5MaxRenew" type="text" name="krb5MaxRenew" value="{$krb5MaxRenew}">
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
						<label for="krb5ValidStart">{t}Valid ticket start time{/t}</label>
					</td>
					<td>
						<input type="checkbox" name="krb5ValidStart_clear" 
							onClick="	changeState('krb5ValidStart_y');
									  	changeState('krb5ValidStart_m');
									  	changeState('krb5ValidStart_d');
									  	changeState('krb5ValidStart_h');
									  	changeState('krb5ValidStart_i');"
							{if $krb5ValidStart_clear} checked {/if}
						>
					</td>
					<td>
						<select name="krb5ValidStart_h" id="krb5ValidStart_h" {if $krb5ValidStart_clear} disabled {/if}>
							{html_options options=$hours selected=$krb5ValidStart_h}
						</select>
					</td>
					<td>
						<select name="krb5ValidStart_i" id="krb5ValidStart_i" {if $krb5ValidStart_clear} disabled {/if}>
							{html_options options=$minutes selected=$krb5ValidStart_i}
						</select>
					</td>
					<td>
						<select name="krb5ValidStart_d" id="krb5ValidStart_d" {if $krb5ValidStart_clear} disabled {/if}>
							{html_options options=$days selected=$krb5ValidStart_d}
						</select>
					</td>
					<td>
						<select name="krb5ValidStart_m" id="krb5ValidStart_m" {if $krb5ValidStart_clear} disabled {/if}>
							{html_options options=$month selected=$krb5ValidStart_m}
						</select>
					</td>
					<td>
						<select name="krb5ValidStart_y" id="krb5ValidStart_y" {if $krb5ValidStart_clear} disabled {/if}> 
							{html_options options=$years selected=$krb5ValidStart_y}
						</select>
					</td>
				</tr>
				<tr>
					<td>
						<label for="krb5ValidEnd">{t}Valid ticket end time{/t}</label>
					</td>
					<td>
						<input type="checkbox" name="krb5ValidEnd_clear" 
							onClick="	changeState('krb5ValidEnd_y');
									  	changeState('krb5ValidEnd_m');
									  	changeState('krb5ValidEnd_d');
									  	changeState('krb5ValidEnd_h');
									  	changeState('krb5ValidEnd_i');"
							{if $krb5ValidEnd_clear} checked {/if}
						>
					</td>
					<td>
						<select name="krb5ValidEnd_h" id="krb5ValidEnd_h" {if $krb5ValidEnd_clear} disabled {/if}>
							{html_options options=$hours selected=$krb5ValidEnd_h}
						</select>
					</td>
					<td>
						<select name="krb5ValidEnd_i" id="krb5ValidEnd_i" {if $krb5ValidEnd_clear} disabled {/if}>
							{html_options options=$minutes selected=$krb5ValidEnd_i}
						</select>
					</td>
					<td>
						<select name="krb5ValidEnd_d" id="krb5ValidEnd_d" {if $krb5ValidEnd_clear} disabled {/if}>
							{html_options options=$days selected=$krb5ValidEnd_d}
						</select>
					</td>
					<td>
						<select name="krb5ValidEnd_m" id="krb5ValidEnd_m" {if $krb5ValidEnd_clear} disabled {/if}>
							{html_options options=$month selected=$krb5ValidEnd_m}
						</select>
					</td>
					<td>
						<select name="krb5ValidEnd_y" id="krb5ValidEnd_y" {if $krb5ValidEnd_clear} disabled {/if}>
							{html_options options=$years selected=$krb5ValidEnd_y}
						</select>
					</td>
				</tr>
				<tr>
					<td>
						<label for="krb5PasswordEnd">{t}Password end{/t}</label>
					</td>
					<td>
						<input type="checkbox" name="krb5PasswordEnd_clear" 
							onClick="	changeState('krb5PasswordEnd_y');
									  	changeState('krb5PasswordEnd_m');
									  	changeState('krb5PasswordEnd_d');
									  	changeState('krb5PasswordEnd_h');
									  	changeState('krb5PasswordEnd_i');"
							{if $krb5PasswordEnd_clear} checked {/if}
						>
					</td>
					<td>
						<select name="krb5PasswordEnd_h" id="krb5PasswordEnd_h" {if $krb5PasswordEnd_clear} disabled {/if}>
							{html_options options=$hours selected=$krb5PasswordEnd_h}
						</select>
					</td>
					<td>
						<select name="krb5PasswordEnd_i" id="krb5PasswordEnd_i" {if $krb5PasswordEnd_clear} disabled {/if}>
							{html_options options=$minutes selected=$krb5PasswordEnd_i}
						</select>

					</td>
					<td>
						<select name="krb5PasswordEnd_d" id="krb5PasswordEnd_d" {if $krb5PasswordEnd_clear} disabled {/if}>
							{html_options options=$days selected=$krb5PasswordEnd_d}
						</select>
					</td>
					<td>
						<select name="krb5PasswordEnd_m" id="krb5PasswordEnd_m" {if $krb5PasswordEnd_clear} disabled {/if}>
							{html_options options=$month selected=$krb5PasswordEnd_m}
						</select>
					</td>
					<td>
						<select name="krb5PasswordEnd_y" id="krb5PasswordEnd_y" {if $krb5PasswordEnd_clear} disabled {/if}>
							{html_options options=$years selected=$krb5PasswordEnd_y}
						</select>
					</td>
				</tr>
			</table>
		</td>	
		<td>
			<h2>Flags</h2>
			<table>
				<tr>
					<td style="width:120px;">
<input {if $krb5KDCFlags_0} checked {/if} class="center" name="krb5KDCFlags_0" value="1" type="checkbox">initial<br>
<input {if $krb5KDCFlags_1} checked {/if} class="center" name="krb5KDCFlags_1" value="1" type="checkbox">forwardable<br>
<input {if $krb5KDCFlags_2} checked {/if} class="center" name="krb5KDCFlags_2" value="1" type="checkbox">proxiable<br>
<input {if $krb5KDCFlags_3} checked {/if} class="center" name="krb5KDCFlags_3" value="1" type="checkbox">renewable<br>
<input {if $krb5KDCFlags_4} checked {/if} class="center" name="krb5KDCFlags_4" value="1" type="checkbox">postdate<br>
<input {if $krb5KDCFlags_5} checked {/if} class="center" name="krb5KDCFlags_5" value="1" type="checkbox">server<br>
<input {if $krb5KDCFlags_6} checked {/if} class="center" name="krb5KDCFlags_6" value="1" type="checkbox">client<br>
					</td>
					<td>
<input {if $krb5KDCFlags_7} checked {/if} class="center" name="krb5KDCFlags_7" value="1" type="checkbox">invalid<br>
<input {if $krb5KDCFlags_8} checked {/if} class="center" name="krb5KDCFlags_8" value="1" type="checkbox">require-preauth<br>
<input {if $krb5KDCFlags_9} checked {/if} class="center" name="krb5KDCFlags_9" value="1" type="checkbox">change-pw<br>
<input {if $krb5KDCFlags_10} checked {/if} class="center" name="krb5KDCFlags_10" value="1" type="checkbox">require-hwauth<br>
<input {if $krb5KDCFlags_11} checked {/if} class="center" name="krb5KDCFlags_11" value="1" type="checkbox">ok-as-delegate<br>
<input {if $krb5KDCFlags_12} checked {/if} class="center" name="krb5KDCFlags_12" value="1" type="checkbox">user-to-user<br>
<input {if $krb5KDCFlags_13} checked {/if} class="center" name="krb5KDCFlags_13" value="1" type="checkbox">immutable<br>
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
