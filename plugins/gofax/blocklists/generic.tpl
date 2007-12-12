<h2><img class="center" alt="" align="middle" src="images/rightarrow.png"> {t}Generic{/t}</h2>
<table summary="" style="width:100%; vertical-align:top; text-align:left;">

 <tr>
   <td style="width:50%; vertical-align:top;">
    <table summary="">
     <tr>
      <td><LABEL for="cn">{t}List name{/t}</LABEL>{$must}</td>
      <td>

{render acl=$cnACL}
       <input name="cn" id="cn" size=25 maxlength=60 value="{$cn}" title="{t}Name of blocklist{/t}">
{/render}
      </td>
     </tr>
     <tr>
      <td><LABEL for="base">{t}Base{/t}</LABEL>{$must}</td>
      <td>
{render acl=$baseACL}
      <select id="base" size="1" name="base" title="{t}Choose subtree to place group in{/t}">
       {html_options options=$bases selected=$base}
      </select>
{/render}

{render acl=$baseACL disable_picture='images/folder_gray.png'}
        <input type="image" name="chooseBase" src="images/folder.png" class="center" title="{t}Select a base{/t}">
{/render}
      </td>
     </tr>
    </table>
   </td>

  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>

   <td style="vertical-align:top;">
    <table summary="">
     <tr>
       <td><LABEL for="type">{t}Type{/t}</LABEL></td>
       <td>
{render acl=$typeACL}
        <select size="1" id="type" name="type" title="{t}Select wether to filter incoming or outgoing calls{/t}">
	        {html_options options=$types selected=$type}
		<option disabled>&nbsp;</option>
        </select>
{/render}
        </td>
      </tr>
      <tr>
       <td><LABEL for="description">{t}Description{/t}</LABEL></td>
       <td>
{render acl=$descriptionACL}
         <input name="description" id="description" size=25 maxlength=80 value="{$description}" title="{t}Descriptive text for this blocklist{/t}">
{/render}
       </td>
      </tr>
    </table>
   </td>
 </tr>
</table>

<p class="plugbottom" style="margin-bottom:0px; padding:0px;">&nbsp;</p>

<table summary="" style="width:100%">
 <tr>
   <td style="width:50%;">
     <h2><img class="center" alt="" align="middle" src="images/false.png"> {t}Blocked numbers{/t}</h2>
{render acl=$goFaxBlocklistACL}
     <select style="width:100%; height:200px;" name="numbers[]" size=15 multiple>
      {html_options values=$goFaxBlocklist output=$goFaxBlocklist}
	  <option disabled>&nbsp;</option>
     </select>
{/render}
     <br>
{render acl=$goFaxBlocklistACL}
     <input id="number" name="number" size=25 maxlength=60 >&nbsp;
{/render}
{render acl=$goFaxBlocklistACL}
     <input type=submit value="{t}Add{/t}" name="add_number">&nbsp;
{/render}
{render acl=$goFaxBlocklistACL}
     <input type=submit value="{t}Delete{/t}" name="delete_number">
{/render}
   </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
   <td style="vertical-align:top;" >
     <h2><img class="center" alt="" align="middle" src="images/info_small.png"> {t}Information{/t}</h2>
     <p>
      {t}Numbers can also contain wild cards.{/t}
     </p>
   </td>
 </tr>
</table>

<p class="plugbottom">
  <input type=submit name="edit_finish" style="width:80px" value="{t}Ok{/t}">
  <input type=submit name="edit_apply" value="{t}Apply{/t}">
  <input type=submit name="edit_cancel" value="{t}Cancel{/t}">
</p>
<input type='hidden' name='blocklist_posted' value="1">
<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('n');
  -->
</script>
