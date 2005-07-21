<h2><img alt="" align="middle" src="images/network.png"> {t}Network settings{/t}</h2>

<table summary="" width="100%">
 <tr>
  <td>
   <table summary="">
    <tr>
     <td><LABEL for="ipHostNumber">{t}IP-address{/t}{$staticAddress}</LABEL></td>
     <td><input id="ipHostNumber" name="ipHostNumber" size=25 maxlength=80 value="{$ipHostNumber}"></td>
    </tr>
   </table>
  </td>
  <td>
   <table summary="">
    <tr>
     <td><LABEL for="macAddress">{t}MAC-address{/t}</LABLE>{$staticAddress}</td>
     <td><input name="macAddress" id="macAddress" size=25 maxlength=80 value="{$macAddress}"></td>
    </tr>
   </table>
  </td>
 </tr>
</table>
<br>
