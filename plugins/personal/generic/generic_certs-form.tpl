<input type="hidden" name="MAX_FILE_SIZE" value="2000000">
 <!-- Headline container -->
<h2><img alt="" class="center" align="middle" src="images/certs.png"> {t}Certificates{/t}</h2>

<table summary="" style="vertical-align:top; text-align:left;border-top: 1px solid rgb(176,176,176);" width="100%" cellpadding=4 border=0>
<TR><TD align="right">{t}Common Name{/t}{$must}</TD><TD>
<INPUT TYPE="TEXT" NAME="commonName" VALUE="{$commonName}" disabled SIZE=64>
<INPUT TYPE="hidden" NAME="commonName" VALUE="{$commonName}" >
</TD></TR>
<TR><TD align="right">{t}email{/t}{$must}</TD><TD>
<INPUT TYPE="TEXT" NAME="emailAddress" VALUE="{$emailAddress}" SIZE=40>
</TD></TR>
<TR><TD align="right">{t}Organization{/t}{$must}</TD><TD>
<INPUT TYPE="TEXT" NAME="organizationName" VALUE="{$organizationName}">
</TD></TR>
<TR><TD align="right">{t}Department{/t}{$must}</TD><TD>
<INPUT TYPE="TEXT" NAME="organizationalUnitName" VALUE="{$organizationalUnitName}">
</TD></TR>
<TR><TD align="right">{t}Location{/t}{$must}</TD><TD>
<INPUT TYPE="TEXT" NAME="localityName" VALUE="{$localityName}">
</TD align="right"></TR>
<TR><TD align="right">{t}State{/t}{$must}</TD><TD>
<INPUT TYPE="TEXT" NAME="stateOrProvinceName" VALUE="{$stateOrProvinceName}">
</TD></TR>
<TR><TD align="right">{t}Country{/t}{$must}</TD><TD>
<INPUT TYPE="TEXT" NAME="countryName" VALUE="{$countryName}" SIZE="2">
</TD></TR>
 </table>
<INPUT TYPE="hidden" NAME="SPKAC" VALUE="">
<p class="plugbottom">
  <input type=submit name="cert_gen_continue" value="{t}SPKAC{/t}">
  &nbsp;
  <input type=submit name="cert_gen_cancel" value="{t}Cancel{/t}">
</p>

