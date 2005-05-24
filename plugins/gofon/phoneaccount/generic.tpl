<table style="width:100%" border=0>
 <tr>

  <td style="vertical-align:top; border-right:1px solid #A0A0A0;width:33%">
   <h2><img alt="" align="middle" src="images/hardware.png"> {t}Telephone hardware{/t}</h2>

   <table style="width:100%" border=0>
    <tr>
     <td>{t}Telephone{/t}&nbsp;
	  {$hardware_list}
     </td>
    </tr>
   </table>
  </td>
  <td style="width:33%;">
   <h2><img alt="" align="middle" src="images/sound.png"> {t}Answering machine{/t}</h2>
    <table border=0 width="100%">
     <tr>
      <td>
       {if $has_mailaccount eq "false"}
       <input disabled type=checkbox name="fon_to_mail" value="1" >
        <a style="color:#909090">{t}Deliver missed calls as mail{/t}</a>
       {else}
       <input type=checkbox name="fon_to_mail" value="1"  {$goFonDeliveryModeACL}>
        {t}Deliver missed calls as mail{/t}
       {/if}
      </td>
     </tr>
    </table>
  </td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<table style="width:100%">
 <tr>
  <td style="vertical-align:top; border-right:1px solid #A0A0A0; width:50%">
   <h2><img alt="" align="middle" src="images/banana.png"> {t}Phone numbers{/t}</h2>
      <select style="width:380px; height:120px;" name="phonenumber_list[]" size=6 multiple {$telephoneNumberACL}>
       {html_options options=$phoneNumbers}
      </select>
      <br>
      <input name="phonenumber" size=20 align=middle maxlength=60 {$telephoneNumberACL} value="">
      <input type="submit" value="{t}Add{/t}" name="add_phonenumber"
	{$telephoneNumberACL}>&nbsp;
      <input type="submit" value="{t}Delete{/t}" name="delete_phonenumber"
        {$telephoneNumberACL}>
  </td>

  <td style="vertical-align:top; width:50%;">

   <table>
	<tr>
    <td style="width:50%; vertical-align:top;">
     <h2><img alt="" align="middle" src="images/rightarrow.png"> {t}Forward calls to{/t}</h2>

     <table>
      <tr> 
       <td>{t}Phone number{/t}</td><td>{t}Timeout (s){/t}</td><td>&nbsp;</td>
      </tr>
      {$forwarder_list}
     </table>
    </td>
   </table>
  </td>

 </tr>
</table>

<input type="hidden" name="phoneTab" value="phoneTab">
