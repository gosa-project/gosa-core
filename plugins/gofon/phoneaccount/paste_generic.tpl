<h2>{t}Phone settings{/t}</h2>
<table summary="{t}Phone numbers{/t}" style="width:100%" cellspacing=0>
 <tr>
  <td style="vertical-align:top; border-right:1px solid #A0A0A0; width:50%">
   <h2>
    <img class="center" alt="" align="middle" src="images/banana.png" />&nbsp;{t}Phone numbers{/t}
   </h2>
  </td>
 </tr>
 <tr>
  <td style="vertical-align:top; border-right:1px solid #A0A0A0; width:50%">
   <select style="width:100%;" name="phonenumber_list[]" size=7 multiple>
    {html_options options=$phoneNumbers}
     <option disabled>&nbsp; </option>
   </select>
   <br>
   <input name="phonenumber" size=20 align=middle maxlength=60 value="">
   <input type="submit" value="{t}Add{/t}" name="add_phonenumber"> 
   <input type="submit" value="{t}Delete{/t}" name="delete_phonenumber">
  </td>
  <td style="vertical-align:top; width:50%;">
   <table summary="" style="width:100%" border=0>
    <tr>
     <td style="vertical-align:top;">
      <h2> 
       <img class="center" alt="" align="middle" src="images/hardware.png" />
       &nbsp;{t}Telephone hardware{/t}
      </h2>
      <table summary="{t}Telephone{/t}" border=0>
       <tr>
        <td>
         <label for="goFonVoicemailPIN">{t}Voicemail PIN{/t}{$must}</label>
        </td>
        <td>
         <input type="password" id="goFonVoicemailPIN" name="goFonVoicemailPI" value="{$goFonVoicemailPIN}">
        </td>
       </tr>
       <tr>
        <td>
         <label for="goFonPIN">{t}Phone PIN{/t}{$must}</label>
        </td>
        <td>
         <input type="password" id="goFonPIN" name="goFonPIN" value="{$goFonPIN}">
        </td>
       </tr>
      </table>
     </td>
    </tr>
   </table>
  </td>
 </tr>
</table>
<input type="hidden" name="phoneTab" value="phoneTab">
<br>
<br>
