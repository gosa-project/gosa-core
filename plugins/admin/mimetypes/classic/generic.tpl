
<h3>{t}Generic{/t}</h3>

<table style="width:100%" summary="{t}Mime-type generic{/t}">
 <tr>
  <td style='width:50%; '>
 
   <table summary="{t}Mime-type settings{/t}">
    <tr>
     <td>{t}Mime type{/t}{$must}</td>
     <td>
      {render acl=$cnACL}
       <input type="text" name='cn' value="{$cn}" title='{t}Please enter a name for the mime-type here{/t}'>
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}Mime-group{/t}</td>
     <td>
      {render acl=$gotoMimeGroupACL}
       <select name='gotoMimeGroup' title='{t}Categorize this mime type{/t}' size=1>
        {html_options output=$gotoMimeGroups values=$gotoMimeGroups selected=$gotoMimeGroup}
       </select>
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}Description{/t}</td>
     <td>
      {render acl=$descriptionACL}
       <input type="text" name='description' value="{$description}" title='{t}Please specify a description{/t}'>
      {/render}
     </td>
    </tr>
    
    {if !$isReleaseMimeType}
     <tr>
      <td><br><LABEL for="base">{t}Base{/t}</LABEL>{$must}</td>
      <td>
       <br>
{render acl=$baseACL}
           {$base}
{/render}
      </td>
     </tr>
    {/if}
   </table>

  </td>
  <td class='left-border'>

   <table summary="{t}Picture settings{/t}">
    <tr>
     <td>
      <LABEL for="picture_file">{t}Icon{/t}</LABEL>
      <br>
      {if $IconReadable}
       <img src="{$gotoMimeIcon}" border=1 alt='{t}Mime-icon{/t}' 
         style="width:48px; height:48; background-color:white; vertical-align:bottom;">
       {else}
       {image path="images/empty.png"}
      {/if}
     </td>
     <td>&nbsp;
      <br>
      <input type="hidden" name="MAX_FILE_SIZE" value="1000000">
      {render acl=$gotoMimeIconACL}
       <input name="picture_file" type="file" size="20" maxlength="255" 							
        accept="image/*.png" id="picture_file">
      {/render}
      {render acl=$gotoMimeIconACL}
       <button type='submit' name='update_icon'title="{t}Update mime type icon{/t}">{t}Update{/t}</button>
      {/render}
     </td>
    </tr>
   </table>

  </td>
 </tr>
 <tr>
  <td colspan="2">

   <hr>

  </td>
 </tr>
 <tr>
  <td colspan="2">
   <h3>{t}Left click{/t}</h3>
  </td>
 </tr>
 <tr>
  <td style='width:50%; '>
    
   {t}File patterns{/t}{$must}

   {render acl=$gotoMimeFilePatternACL}
    {$gotoMimeFilePatterns}
   {/render}
   {render acl=$gotoMimeFilePatternACL}
    <input type='text'	 name='NewFilePattern'	  value='' title='{t}Please specify a new file pattern{/t}'>
   {/render}
   {render acl=$gotoMimeFilePatternACL}
    <button type='submit' name='AddNewFilePattern' title="{t}Add a new file pattern{/t}">
    {msgPool type=addButton}</button>
   {/render}

  </td>
  <td class='left-border'>

   {t}Applications{/t}

   {render acl=$gotoMimeApplicationACL}
    {$gotoMimeApplications}
   {/render}
   {render acl=$gotoMimeApplicationACL}
    <select name="NewApplicationSelect" size=1><option value="">-</option>
     {html_options options=$ApplicationList}
    </select>
   {/render}
   {render acl=$gotoMimeApplicationACL}
    <input type='text'	 name='NewApplication'	  value='' title='{t}Enter an application name here{/t}'>
   {/render}
   {render acl=$gotoMimeApplicationACL}
    <button type='submit' name='AddNewApplication' title="{t}Add application{/t}">
    {msgPool type=addButton}</button>
   {/render}

  </td>
 </tr>
 <tr>
  <td colspan="2">

   <hr>

  </td>
 </tr>
 <tr>
  <td colspan="2">

   <h3>{t}Embedding{/t}</h3>

  </td>
 </tr>
 <tr>
  <td style='width:50%; '>

   {render acl=$gotoMimeLeftClickActionACL}
    <input type='radio' name='gotoMimeLeftClickAction_IE' value='I' 							
       {if $gotoMimeLeftClickAction_I} checked {/if}>
   {/render}{t}Show file in embedded viewer{/t}
   <br>
   {render acl=$gotoMimeLeftClickActionACL}
    <input type='radio' name='gotoMimeLeftClickAction_IE' value='E' 							
       {if $gotoMimeLeftClickAction_E} checked {/if}>
   {/render}{t}Show file in external viewer{/t}
   <br>
   {render acl=$gotoMimeLeftClickActionACL}
    <input type='checkbox' name='gotoMimeLeftClickAction_Q' value='1' 							
       {if $gotoMimeLeftClickAction_Q} checked {/if}>
   {/render}{t}Ask whether to save to local disk{/t}

  </td>
  <td class='left-border'>

   {t}Applications{/t}
   {render acl=$gotoMimeEmbeddedApplicationACL}
    {$gotoMimeEmbeddedApplications}
   {/render}
   {render acl=$gotoMimeEmbeddedApplicationACL}
    <select name="NewEmbeddedApplicationSelect" size=1><option value="">-</option>
     {html_options options=$ApplicationList}
    </select>
   {/render}
   {render acl=$gotoMimeEmbeddedApplicationACL}
    <input type='text' name='NewEmbeddedApplication' value=''	title='{t}Enter an application name here{/t}'>
   {/render}
   {render acl=$gotoMimeEmbeddedApplicationACL}
    <button type='submit' name='AddNewEmbeddedApplication' 
      title='{t}Add application{/t}'>{msgPool type=addButton}</button>
   {/render}

  </td>
 </tr>
</table>
<input type="hidden" name="MimeGeneric" value="1"><!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  // First input field on page	
  focus_field('cn'); 
</script>
