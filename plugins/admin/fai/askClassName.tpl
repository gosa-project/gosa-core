<div style="font-size: 18px;">
	{$headline}
</div>
<br>
<p class="seperator">
{t}Adding a new class to the FAI management, requires a class name. You have to specify a unique class name for unique types of FAI classes, while you can use the same class name for different types of FAI classes. In the laste case, FAI will automatically enclose all these different class types to one unique class name.{/t}
<br>
<br>
</p>
<p class="seperator">
<br>
<b>{t}Please use one of the following methods to choose the name for the new FAI class.{/t}</b>
<br>
<br>
</p>
<br>
<table summary="" style='width:100%'>
 <tr>
  <td style='width:49%'>
   <table summary="">
    <tr>
     <td>
       <input type=radio name="classSelector" value="1" checked>
     </td>
     <td> 
       <h1>{t}Enter FAI class name manually{/t}</h1>
     </td>
   </tr>
   <tr>
     <td>&nbsp;</td>
     <td>
      {t}Class name{/t}&nbsp;<input type="text"	 name="UseTextInputName" value="{$ClassName}" style="width:120px;">
     </td>
    </tr>
   </table>
   <br>
   <br>
  </td>
  <td style='border-left:1px solid #909090'>
  </td>
  <td>
   <table summary="" {$grey}>
    <tr>
     <td>
       <input type=radio name="classSelector" value="2" {$ClassNamesAvailable}>
     </td>
     <td>
      <h1 {$grey}>{t}Choose FAI class name from a list of existing classes{/t}</h1>
     </td>
    </tr>
    <tr>
     <td>&nbsp;</td>
     <td>
      {t}Class name{/t}&nbsp;<select name="SelectedClass" title="{t}Choose class name{/t}" style="width:120px;" {$ClassNamesAvailable}>
       {html_options options=$ClassNames}
      </select>
     </td>
    </tr>
   </table>
   <br>
   <br>
  </td>
 </tr>
</table>

<!--
<table summary="">
	<tr>
		<td>
		<input type="submit" name="UseSelectedClass" value="{t}Use{/t}" {$ClassNamesAvailable}> 
		</td>
	</tr>
	<tr>
		<td><b>{t}A new class name.{/t}</b>
		</td>
		<td>
		<input type="submit" name="UseNewClass" value="{t}Use{/t}"> 
		</td>
	</tr>
</table>
-->
<p class="seperator">&nbsp;</p>
	
<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
	<!--
	focus_field('UseTextInputName');
	-->
</script>
