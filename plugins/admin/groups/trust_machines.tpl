<table summary="" style="width:100%" cellpadding=4>
<tr>
  <td>
   <div class="contentboxh" style="height:20px;">
    <p class="contentboxh" style="font-size:12px">
     <b>{t}Select systems to add{/t} {$hint}</b><br>
    </p>
   </div>
   <div class="contentboxb">
    <p class="contentboxb" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
     <select style="width:600px; margin-top:4px; height:450px;" name="wslist[]" size="15" multiple>
	{html_options values=$wslist output=$wslist}
     </select>
    </p>
   </div>
  </td>
  <td>
   <div class="contentboxh" style="height:20px;">
    <p class="contentboxh" style="font-size:12px">{image path="{$launchimage}" align="right"}
<b>{t}Filters{/t}</b></p>
   </div>
   <div class="contentboxb" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
     <table summary="" style="width:100%;">
      {$alphabet}
     </table>
    <table style='background-color:#F8F8F8' summary="" width="100%">
<tr><td width="50%">{image path="{$tree_image}"}&nbsp;{t}Display systems of department{/t}
</td>
    <td><select name="depselect" size=1 onChange="mainform.submit()" title="{t}Choose the department the search will be based on{/t}">
      {html_options options=$deplist selected=$depselect}
    </select></td></tr></table>
    <table style='background-color:#F8F8F8' summary="" width="100%">
<tr><td width="50%">{image path="{$search_image}"}&nbsp;{t}Display systems matching{/t}
</td>
    <td><input type='text' name='regex' maxlength='20' value='{$regex}' title='{t}Regular expression for matching addresses{/t}' onChange="mainform.submit()"></td></tr></table>
   {$apply}
   </div>
  </td>
</tr>
</table>

<hr>
<div class="plugin-actions">
  <button type='submit' name='add_ws_finish'>{msgPool type=addButton}</button>

  <button type='submit' name='add_ws_cancel'>{msgPool type=cancelButton}</button>

</div>

