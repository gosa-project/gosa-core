<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top;">
  <div class="contentboxh">
    <p class="contentboxh">
     {t}List of applications{/t} {$hint}
    </p>
  </div>
  <div class="contentboxb">
    <p class="contentboxb">
     <select style="width:500px; height:450px;" name="applist" size=15 ondblclick="this.form.edit_helper.value='1'; this.form.submit()">
      {html_options options=$applications}
		<option disabled></option>
    </select>
    <br>
    <input type=submit value="{t}New{/t}..." name="new_app">&nbsp;
    <input type=submit value="{t}Edit{/t}" name="select_app">&nbsp;
    <input type=submit value="{t}Delete{/t}" name="delete_app">&nbsp;
    <input type=hidden name="edit_helper">
   </p>
  </div>
  </td>
  <td style="vertical-align:top">
   <div class="contentboxh">
    <p class="contentboxh"><img src="{$infoimage}" align="right" alt="[i]">{t}Information{/t}</p>
   </div>
   <div class="contentboxb">
   <p class="contentboxb">
    {t}This menu allows you to add, edit and remove selected applications. You may want to use the range selector on top of the application listbox, when working with a large number of applications.{/t}
   </p>
   <p class="contentboxb">
    {t}-Edit- and -New...- will execute an assistant to aid you in editing properties. -Delete- will ask for confirmation before removing applications.{/t}
   </p>
   </div>
   <br>
   <div class="contentboxh">
    <p class="contentboxh"><img src="{$launchimage}" align="right" alt="[F]">{t}Filters{/t}</p>
   </div>
   <div class="contentboxb">
     <table style="width:100%;border-top:1px solid #B0B0B0;">
      {$alphabet}
     </table>
    <table style="width:100%;border-top:1px solid #B0B0B0;">
		<tr>
			<td>
				<img alt="" src="{$tree_image}" align=middle>&nbsp;{t}Display applications of department{/t}
			</td>
		    <td>
				<select name="depselect" size=1 onChange="mainform.submit()" title="{t}Choose the department the search will be based on{/t}">
				  {html_options options=$deplist selected=$depselect}
				</select>
			</td>
		</tr>
	</table>
    <table style="width:100%;border-top:1px solid #B0B0B0;">
		<tr>
			<td>
				<img alt="" src="{$search_image}" align=middle>&nbsp;{t}Display applications matching{/t}
			</td>
    		<td>
				<input type='text' name='regex' maxlength='20' value='{$regex}' title='{t}Regular expression for matching application names{/t}' onChange="mainform.submit()">
			</td>
		</tr>
	</table>
    {$apply}
   </div>
  </td>
</tr>
</table>

<input type="hidden" name="ignore">
