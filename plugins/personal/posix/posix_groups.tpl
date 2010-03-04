<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
 <tr>
  <td style="vertical-align:top;">
   <div class="contentboxh" style="height:20px;">
    <p class="contentboxh" style="font-size:12px">
    <b>{t}Select groups to add{/t} {$hint}</b>
    </p>
   </div>
   <div class="contentboxb" style="">
    <p class="contentboxb" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
    <select style="width:100%; height:430px; margin-top:4px;" name="groups[]" size="15" multiple>
     {html_options options=$groups}
    </select>
    <input type=hidden name="edit_helper">
    </p>
   </div>
  </td>
  <td style="vertical-align:top; width:350px;">
   <div class="contentboxh" style="height:20px;">
    <p class="contentboxh" style="font-size:12px">
    {image path="{$launchimage}" align="right"}
    <b>{t}Filters{/t}</b>
    </p>
   </div>
   <div class="contentboxb" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
    <table style="width:100%;">
     {$alphabet}
    </table>
    
    <table width="100%" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
     <tr>
      <td width="50%">
       {image path="{$tree_image}"}&nbsp;{t}Display groups of department{/t}
       
      </td>
      <td>
       <select name="depselect" size=1 onChange="mainform.submit()" title="{t}Choose the department the search will be based on{/t}">
        {html_options options=$deplist selected=$depselect}
       </select>
      </td>
     </tr>
    </table>
    <table width="100%" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
     <tr>
      <td width="50%">
       {image path="{$search_image}"}&nbsp;{t}Display groups matching{/t}
       
      </td>
      <td>
       <input type='text' name='regex' maxlength='20' value='{$regex}' 
       title='{t}Regular expression for matching group names{/t}' onChange="mainform.submit()">
      </td>
     </tr>
    </table>
    <table width="100%" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
     <tr>
      <td width="50%">
       {image path="{$search_image}"}&nbsp;{t}Display groups of user{/t}
       
      </td>
      <td>
       <input type='text' name='guser' maxlength='20' value='{$guser}' 
       title='{t}User name of which groups are shown{/t}' onChange="mainform.submit()">
      </td>
     </tr>
    </table>
    <table width="100%" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
     <tr>
      <td>
       <input type="checkbox" name="SubSearch" value="1" {$SubSearchCHK}
       onClick="mainform.submit();">
       {t}Search in subtrees{/t}
      </td>
     </tr>
    </table>
    {$apply}
   </div>
  </td>
 </tr>
</table>
<input type="hidden" value="1" name="PosixGroupDialogPosted">
<p class="plugbottom">
<button type='submit' name='add_groups_finish'>{msgPool type=addButton}</button>

&nbsp;
<button type='submit' name='add_groups_cancel'>{msgPool type=cancelButton}</button>

</p>

