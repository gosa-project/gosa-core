<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4 border=0>
 <!-- Headline container -->
 <tr>
   <td colspan=2>
     <h2>{image path="plugins/generic/images/head.png"} {t}Personal picture{/t}
</h2>
   </td>
 </tr>
 <!-- Base container -->
 <tr>
 <!-- Image container -->
  <td>
   <table>
    <tr>
     <td width="147" height="200" bgcolor="gray">
      <img  src="getbin.php?rand={$rand}" alt='' style='width:147px;' >
     </td>
    </tr>
   </table>
  </td>
	</tr>
	<tr>
   <!-- Name, ... -->
   <td style="vertical-align:bottom; width:100%;">
     <input type="hidden" name="MAX_FILE_SIZE" value="2000000">
     <input id="picture_file" name="picture_file" type="file" size="20" maxlength="255" accept="image/*.jpg">
     &nbsp;
     <button type='submit' name='picture_remove'>{t}Remove picture{/t}</button>

   </td>
 </tr>
</table>
<br>

<div class='plugin-actions'>
  <button type='submit' name='picture_edit_finish'>{msgPool type=saveButton}</button>
  <button type='submit' name='picture_edit_cancel'>{msgPool type=cancelButton}</button>
</div>

