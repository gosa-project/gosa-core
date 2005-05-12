<table style="width:100%;">
 <tr>
  <td style="width:50%; vertical-align:top">
   <input type="hidden" name="ogroupedit" value="1">
   <table>
    <tr>
     <td>{t}Group name{/t}{$must}</td>
     <td>
       <input name="cn" size=25 maxlength=60 {$cnACL} value="{$cn}" title="{t}Name of the group{/t}">
     </td>
    </tr>
    <tr>
     <td>{t}Description{/t}</td>
     <td><input name="description" size=40 maxlength=60 {$descriptionACL} value="{$description}" title="{t}Descriptive text for this group{/t}">
     </td>
    </tr>
    <tr>
     <td colspan=2><div style="height:20px;"></td>
    </tr>
    <tr>
     <td>{t}Base{/t}{$must}</td>
     <td>
      <select size="1" name="base" {$selectmode} title="{t}Choose subtree to place group in{/t}">
       {html_options options=$bases selected=$base_select}
      </select>
     </td>
    </tr>
   </table>

  </td>
  <td style="border-left:1px solid #A0A0A0; padding-left:10px;">
   <b>{t}Member objects{/t}</b>&nbsp;({$combinedObjects})
   <br>
   <select style="width:400px; height:300px;" name="members[]" size=15 multiple>
    {$members}
   </select>
   <br>
   <input type=submit value="{t}Add{/t}" name="edit_membership" {$membersACL}>&nbsp;
   <input type=submit value="{t}Delete{/t}" name="delete_membership" {$membersACL}>
  </td>
 </tr>
</table>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.cn.focus();
  -->
</script>
