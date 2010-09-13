<h3>{t}User picture{/t}</h3>
<table summary="{t}The users picture{/t}">
 <tr>
  <td style='width:147px; height:200px; background-color:gray; vertical-align: middle;'>
   <img  src="getbin.php?rand={$rand}" alt='' style='max-width:147px; max-height: 200px; vertical-align: middle;' >
  </td>
 </tr>
</table>
<p>
 <input type="hidden" name="MAX_FILE_SIZE" value="2000000">
  <input id="picture_file" name="picture_file" type="file" size="20" maxlength="255" accept="image/*.jpg">
   &nbsp;
  <button type='submit' name='picture_remove'>{t}Remove picture{/t}</button>
 </p>
<hr>
<div class='plugin-actions'>
  <button type='submit' name='picture_edit_finish'>{msgPool type=saveButton}</button>
  <button type='submit' name='picture_edit_cancel'>{msgPool type=cancelButton}</button>
</div>

