<table summary="" style="width:100%; vertical-align:top; text-align:left; border:1px solid #B0B0B0;" cellpadding=4 cellspacing=0 border=0>
	<tr style="background-color: #E8E8E8; height:26px;font-weight:bold;">
		<td align=left width="100%">
			<LABEL for="storage_base">
				{$storage_info}
			</LABEL>
			<select id="storage_base" name="storage_base" size="1" title="{t}Choose the department to store entry in{/t}">
				{html_options options=$deplist selected=$storage_base}
			</select>
		</td>
		<td>
			<a href="{$url}">
				X
			</a>
		</td>
	</tr>
	<tr style="background-color: white">
		<td colspan=2>
			<table summary="" width="100%" cellspacing=2 cellpadding=4>
				<tr>
					<td style="width:50%; vertical-align:top; background-color: #F0F0F0">
						<h2>
							<img class="center" alt="" src="{$personal_image}" align=middle>
								&nbsp;{t}Personal{/t}
						</h2>
 						<table summary="" width="100%">
  							<tr>
   								<td>
									<LABEL for="sn">
										{t}Last name{/t}{$must}
									</LABEL>, 
									<LABEL for="givenName">
										{t}First name{/t}{$must}
									</LABEL>
								</td>
   								<td>
{render acl=$snACL}	
									<input id="sn" name="sn" size=10 maxlength=60 value="{$info_sn}">, 
{/render}
{render acl=$givenNameACL}
									<input id="givenName" name="givenName" size=10 maxlength=60 value="{$info_givenName}">
{/render}
								<td>
  							</tr>
  							<tr>
   								<td>
									<LABEL for="initials">
										{t}Initials{/t}
									</LABEL>
								</td>
								<td>
{render acl=$initialsACL}
									<input id="initials" name="initials" size=5 maxlength=20 value="{$info_initials}">
{/render}
								</td>
  							</tr>
  							<tr>
   								<td>
									<LABEL for="title">
										{t}Personal title{/t}
									</LABEL>
								</td>
								<td>
{render acl=$titleACL}
									<input id="title" name="title" size=10 maxlength=20 value="{$info_title}">
{/render}
								</td>
  							</tr>
 						</table>
					</td>
					<td style="width:50%; vertical-align:top;background-color:#E8E8E8">
 						<h2>
							<img class="center" alt="" src="{$home_image}" align=middle>
							&nbsp;{t}Private{/t}
						</h2>
						<table summary="" width="100%">
						  	<tr>
						   		<td>
									<LABEL for="homePostalAddress">
										{t}Address{/t}
									</LABEL>
								<br>
								<br>
								</td>
						   		<td style="vertical-align:top;">
{render acl=$homePostalAddressACL}
									<textarea id="homePostalAddress" name="homePostalAddress" rows=1 cols=20>{$info_homePostalAddress}</textarea>
{/render}
								</td>
						  	</tr>
						  	<tr>
						   		<td>
									<LABEL for="homePhone">
										{t}Phone{/t}
									</LABEL>
								</td>
								<td>
{render acl=$homePhoneACL}
									<input id="homePhone" name="homePhone" size=15 maxlength=60 value="{$info_homePhone}">
{/render}
								</td>
						  	</tr>
						  	<tr>
						   		<td>
									<LABEL for="mobile">
										{t}Mobile{/t}
									</LABEL>
								</td>
								<td>
{render acl=$mobileACL}
									<input id="mobile" name="mobile" size=15 maxlength=60 value="{$info_mobile}">
{/render}
								</td>
						  	</tr>
						  	<tr>
						   		<td>
									<LABEL for="mail">
										{t}Email{/t}
									</LABEL>
								</td>
								<td>
{render acl=$mailACL}
									<input id="mail" name="mail" size=15 maxlength=60 value="{$info_mail}">
{/render}
								</td>
						  	</tr>
						 </table>
					</td>
				</tr>
				<tr>
					<td style="width:100%; vertical-align:top; background-color: #E4E4E4" colspan=2>
 						<h2>
							<img class="center" alt="" src="{$company_image}" align=middle>
							&nbsp;{t}Organizational{/t}
						</h2>
 						<table summary="" width="100%">
							<tr>
						   		<td>
									<table summary="" width="100%">
									 	<tr>
									  		<td>
												<LABEL for="">	
													{t}Company{/t}
												</LABEL>
											</td>
											<td>
{render acl=$oACL}
												<input id="o" name="o" size=15 maxlength=60 value="{$info_o}">
{/render}
											</td>
									 	</tr>
									 	<tr>
									  		<td>
												<LABEL for="">
													{t}Department{/t}
												</LABEL>
											</td>
											<td>
{render acl=$ouACL}
												<input id="ou" name="ou" size=15 maxlength=60 value="{$info_ou}">
{/render}
											</td>
									 	</tr>
									 	<tr>
									  		<td>
												<LABEL for="">
													{t}City{/t}
												</LABEL>
											</td>
											<td>
{render acl=$lACL}
												<input id="l" name="l" size=15 maxlength=60 value="{$info_l}">
{/render}
											</td>
									 	</tr>
									 	<tr>
									  		<td>
												<LABEL for="">
													{t}Postal code{/t}
												</LABEL>
											</td>
											<td>
{render acl=$postalCodeACL}
												<input id="postalCode" name="postalCode" size=15 maxlength=60 value="{$info_postalCode}">
{/render}
											</td>
									 	</tr>
									 	<tr>
									  		<td>
												<LABEL for="">
													{t}Country{/t}
												</LABEL>
											</td>
											<td>
{render acl=$stACL}
												<input id="st" name="st" size=15 maxlength=60 value="{$info_st}">
{/render}
											</td>
									 	</tr>
									</table>
								   	</td>
								   	<td style="vertical-align:top">
									<table summary="" width="100%">
										<tr>
											<td>
												<LABEL for="">
													{t}Address{/t}
												</LABEL>
												<br>
												<br>
											</td>
											<td style="vertical-align:top;">
{render acl=$postalAddressACL}
												<textarea id="postalAddress" name="postalAddress" rows=1 cols=20 >{$info_postalAddress}</textarea>
{/render}
											</td>
										</tr>
										<tr>
										  	<td>
												<LABEL for="">
													{t}Phone{/t}	
												</LABEL>
											</td>
											<td>
{render acl=$telephoneNumberACL}
												<input id="telephoneNumber" name="telephoneNumber" size=15 maxlength=60 value="{$info_telephoneNumber}">
{/render}
											</td>
										</tr>
										<tr>
										  	<td>
												<LABEL for="">
													{t}FAX{/t}
												</LABEL>
											</td>
											<td>
{render acl=$facsimileTelephoneNumberACL}
												<input id="facsimileTelephoneNumber" name="facsimileTelephoneNumber"
													size=15 maxlength=60 value="{$info_facsimileTelephoneNumber}">
{/render}
											</td>
										</tr>
										<tr>
											<td>
												<LABEL for="">
													{t}Pager{/t}
												</LABEL>
											</td>
											<td>
{render acl=$pagerACL}
												<input id="pager" name="pager" size=15 maxlength=60 value="{$info_pager}">
{/render}
											</td>
										</tr>
									</table>
								</td>
							</tr>
						</table>
					</td>
				</tr>
			</table>

			<p align=right>
			<input type="submit" name="save" value="{t}Save{/t}">
			<input type="submit" name="cancel" value="{t}Cancel{/t}">
			</p>

		</td>
	</tr>
</table>

<!--
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
-->
