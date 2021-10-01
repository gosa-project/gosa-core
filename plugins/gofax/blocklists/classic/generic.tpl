<h3>{t}Generic{/t}</h3>
<table summary="{t}Blacklist{/t}" style="width:100%;">

 <tr>
   <td style="width:50%;">
    <table summary="{t}Blacklist generic{/t}">
     <tr>
      <td><LABEL for="cn">{t}List name{/t}</LABEL>{$must}</td>
      <td>

{render acl=$cnACL}
       <input type='text' name="cn" id="cn" size=25 maxlength=60 value="{$cn}" title="{t}Name of blacklist{/t}">
{/render}
      </td>
     </tr>
     <tr>
      <td><LABEL for="base">{t}Base{/t}</LABEL>{$must}</td>
      <td>
{render acl=$baseACL}
        {$base}
{/render}
      </td>
     </tr>
    </table>
   </td>

  <td class='left-border'>
   &nbsp;
  </td>

   <td>
    <table summary="{t}Blacklist type{/t}">
     <tr>
       <td><LABEL for="type">{t}Type{/t}</LABEL></td>
       <td>
{render acl=$typeACL}
        <select size="1" id="type" name="type" title="{t}Select whether to filter incoming or outgoing calls{/t}">
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
         <input type='text' name="description" id="description" size=25 maxlength=80 value="{$description}" title="{t}Descriptive text for this blacklist{/t}">
{/render}
       </td>
      </tr>
    </table>
   </td>
 </tr>
</table>

<hr>

<table summary="{t}Blocked numbers{/t}" style="width:100%">
 <tr>
   <td style="width:50%;">

     <h3>{t}Blocked numbers{/t}</h3>
{render acl=$goFaxBlocklistACL}
     <select style="width:100%; height:200px;" name="numbers[]" size=15 multiple>
      {html_options values=$goFaxBlocklist output=$goFaxBlocklist}
	  <option disabled>&nbsp;</option>
     </select>
{/render}
     <br>
{render acl=$goFaxBlocklistACL}
     <input type='text' id="number" name="number" size=25 maxlength=60 >&nbsp;
{/render}
{render acl=$goFaxBlocklistACL}
     <button type='submit' name='add_number'>{msgPool type=addButton}</button>&nbsp;

{/render}
{render acl=$goFaxBlocklistACL}
     <button type='submit' name='delete_number'>{msgPool type=delButton}</button>

{/render}
   </td>
  <td class='left-border'>
   &nbsp;
  </td>
   <td>
     <h3>{t}Information{/t}</h3>
     <p>
      {t}Numbers can also contain wild cards.{/t}
     </p>
   </td>
 </tr>
</table>

<input type='hidden' name='blocklist_posted' value="1">
<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('n');
  -->
</script>
