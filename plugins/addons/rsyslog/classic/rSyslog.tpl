
<div id="mainlist">
 <div class="mainlist-header">
  <p>{t}System logs{/t}</p>

  <div class="mainlist-nav">
   <table summary="{t}Filter{/t}" style="width: 100%;" 
      id="t_scrolltable" cellpadding="0" cellspacing="0">
    <tr>
     <td colspan="2" style='width:25%;'>{t}Server{/t}:
      <select name='selected_server' onChange='document.mainform.submit();' size=1>
       {foreach from=$servers item=item key=key}
        <option value='{$key}' {if $key == $selected_server} selected {/if}>{$item.cn}</option>
       {/foreach}
      </select>
     </td>
     <td colspan="2" style='width:25%;'>{t}Host{/t}:
      <select name='selected_host' onChange='document.mainform.submit();' size=1>
       {foreach from=$hosts item=item key=key}
        <option value='{$key}' {if $key == $selected_host} selected {/if}>{$item}</option>
       {/foreach}
      </select>
     </td>
     <td colspan="2">{t}Severity{/t}:
      <select name='selected_priority' onChange='document.mainform.submit();' size=1>
       {html_options values=$priorities options=$priorities selected=$selected_priority}
      </select>
     </td>
    </tr>
    <tr>
     <td>{t}From{/t}:</td>
     <td>
      <input type="text" id="startTime" name="startTime" class="date" style='width:100px' value="{$startTime}">
      <script type="text/javascript">  
       {literal}
        var datepicker  = new DatePicker(
         { 
          relative : 'startTime', 
          language : '{/literal}{$lang}{literal}', 
          keepFieldEmpty : true,           
          enableCloseEffect : false, 
          enableShowEffect : false 
         });
       {/literal}
      </script>
     </td>
     <td>{t}till{/t}:</td>
     <td>
      <input type="text" id="stopTime" name="stopTime" class="date" style='width:100px' value="{$stopTime}">
      <script type="text/javascript">  
       {literal}
        var datepicker  = new DatePicker(
         { 
          relative : 'stopTime', 
          language : '{/literal}{$lang}{literal}', 
          keepFieldEmpty : true,           
          enableCloseEffect : false, 
          enableShowEffect : false 
         });
       {/literal}
      </script>
     </td>
     <td>{t}Search{/t}:
      <input type='text' name='search_for' value='{$search_for}' style='width:250px;'>
     </td>
     <td>
      <button type='submit' name='search'>{t}Search{/t}</button>
     </td>
    </tr>
   </table>
  </div>
 </div>

 {if $result.status != 'ok'}
  <b>{t}Error{/t}: &nbsp;{$result.status}</b>
  <br>
  {$result.error}
  <br>

 {else}

  <div class="listContainer" id="d_scrollbody" style="min-height: 475px; height: 444px;">

   <table summary="{t}Entry list{/t}" style="width:100%;" cellpadding="0" cellspacing="0">
    <thead class="fixedListHeader listHeaderFormat">
     <tr>
      <td class='listheader'>
       <a href='?plug={$plug_id}&amp;sort_value=DeviceReportedTime'>{t}Date{/t}</a>      
       {if $sort_value=="DeviceReportedTime"}
        {if $sort_type=="DESC"}
         {$downimg}
         {else}
         {$upimg}
        {/if}
       {/if}
      </td>
      <td  class='listheader'>
       <a href='?plug={$plug_id}&amp;sort_value=FromHost'>{t}Source{/t}</a>
       {if $sort_value=="FromHost"}
        {if $sort_type=="DESC"}
         {$downimg}
         {else}
         {$upimg}
        {/if}
       {/if}
      </td>
      <td class='listheader'>
       <a href='?plug={$plug_id}&amp;sort_value=SysLogTag'>{t}Header{/t}</a>
       {if $sort_value=="SysLogTag"}
        {if $sort_type=="DESC"}
         {$downimg}
         {else}
         {$upimg}
        {/if}
       {/if}
      </td>
      <td class='listheader'>
       <a href='?plug={$plug_id}&amp;sort_value=Facility'>{t}Facility{/t}</a>
       {if $sort_value=="Facility"}
        {if $sort_type=="DESC"}
         {$downimg}
         {else}
         {$upimg}
        {/if}
       {/if}
      </td>
      <td class='listheader'>
       <a href='?plug={$plug_id}&amp;sort_value=Priority'>{t}Severity{/t}</a>
       {if $sort_value=="Priority"}
        {if $sort_type=="DESC"}
         {$downimg}
         {else}
         {$upimg}
        {/if}
       {/if}
      </td>
      <td class='listheader'>
       <a href='?plug={$plug_id}&amp;sort_value=Message'>{t}Message{/t}</a>
       {if $sort_value=="Message"}
        {if $sort_type=="DESC"}
         {$downimg}
         {else}
         {$upimg}
        {/if}
       {/if}
      </td>
     </tr>
    </thead>
  
    <tbody class="listScrollContent listBodyFormat" id="t_nscrollbody">
     {foreach from=$result.entries item=item key=key}
      <tr>
       <td title="{$item.DeviceReportedTime}" style='width:120px' class='list1'>
        {$item.DeviceReportedTime}
       </td>
       <td title="{$item.FromHost}" class='list1'>
        {$item.FromHost}
       </td>
       
       <td title="{$item.SysLogTag}" class='list1'>
        {$item.SysLogTag}
       </td>
       
       <td title="{$item.Facility}" class='list1'>
        {$item.Facility}
       </td>
       
       <td title="{$item.Priority}" class='list1'>
        {$item.Priority}
       </td>
       
       <td title="{$item.Message}" style="width:400px" class='list1'>
        <div style='overflow:hidden; width:400px'>
         {$item.Message}
        </div>
       </td>
      </tr>
     {/foreach}
     <tr>
      <td class="list0">&nbsp;</td>
      <td class="list0">&nbsp;</td>
      <td class="list0">&nbsp;</td>
      <td class="list0">&nbsp;</td>
      <td class="list0">&nbsp;</td>
      <td class="list0">&nbsp;</td>
     </tr>
    </tbody>
   </table>
  </div >

  <div class="nlistFooter">

    <div style='width:40%;float:left;'>
     {$matches}
    </div>
   
    <div style='width:80px;float:right;'>
     <select name='limit' onChange='document.mainform.submit();' size=1>
      {html_options options=$limits selected=$limit}
     </select>
    </div>
    
    <div style='width:300px;float:left;'>
     {$page_sel}
    </div>
    <div class='clear'></div>
  </div>
 {/if}
</div>
<br>
