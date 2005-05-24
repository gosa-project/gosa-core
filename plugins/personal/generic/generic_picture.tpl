<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=4 border=0>

 <!-- Headline container -->
 <tr>
   <td colspan=2>
     <h2><img alt="" align="middle" src="images/head.png"> {t}Personal picture{/t}</h2>
   </td>
 </tr>

 <!-- Base container -->
 <tr>

   <!-- Image container -->
   <td>
     <img align="bottom" border="1" width="145" height="200" src="getpic.php?rand=$rand">
   </td>

   <!-- Name, ... -->
   <td style="vertical-align:bottom; width:100%;">
     <input type="hidden" name="MAX_FILE_SIZE" value="2000000">
     <input name="picture_file" type="file" size="20" maxlength="255" accept="image/*.jpg">
     &nbsp;
     <input type=submit name="picture_remove" value="{t}Remove picture{/t}">
   </td>
 </tr>
</table>
<br>
<p class="plugbottom">
  <input type=submit name="picture_edit_finish" value="{t}Save{/t}">
  &nbsp;
  <input type=submit name="picture_edit_cancel" value="{t}Cancel{/t}">
</p>

