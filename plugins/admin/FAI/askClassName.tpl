<div style="font-size: 18px;">
	{$headline}
	<br><br>
</div>
	{t}Please select a class name for this object. The class name specifies a set of configurations.{/t}
	<br>
	{t}Every Class name must be unique. But it can be used for diffrent objects at the same time.\n
	   If you use this class name in a profile, all object with this class name will be included to this profile.{/t}
	<br>
	<br>
	{t}The list box below shows all available class names you can use, for this object, to avoid duplicates.{/t}
<br>
<br>
<table >
	<tr>
		<td colspan="2"><b>
			{t}You can select one the following class names for this object.{/t}
		</b></td>
	</tr>
	<tr>
		<td><b>{t}Available class names.{/t}</b>
		</td>
		<td>
	    <select name="SelectedClass" title="{t}Choose class name{/t}" style="width:120px;">
       		{html_options values=$ClassNameKeys output=$ClassNames}
      	</select>
		<input type="submit" name="UseSelectedClass" value="{t}Use{/t}" {$ClassNamesAvailable}> 
		</td>
	</tr>
	<tr>
		<td colspan="2"><br>
		</td>
	</tr>
	<tr>
		<td colspan="2"><b>
			{t}Please enter your class name here, if want to create a new class.{/t}
		</b></td>
	</tr>
	<tr>
		<td><b>{t}A new class name.</b>{/t}
		</td>
		<td>
		<input type="text"	 name="UseTextInputName" value="" style="width:120px;">
		<input type="submit" name="UseNewClass" value="{t}Use{/t}"> 
		</td>
	</tr>
</table>
<p class="seperator">&nbsp;</p>
	
