{if $objectList ne ""}
<table style='width:100%;' summary="{t}Object references{/t}">


 <colgroup>
  <col width="20%">
  <col width="20%">
  <col width="60%">
 </colgroup>

 <tr style="background-color: #E8E8E8; height:26px;font-weight:bold;">
  <td>{t}Object name{/t}</td><td>{t}Description{/t}</td><td>{t}Contents{/t}</td>
 </tr>

 {$objectList}

</table>
{else}
<b>{t}This object has no relationship to other objects.{/t}</b>
{/if}
