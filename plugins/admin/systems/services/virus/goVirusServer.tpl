
<table style='width:100%;'>
 <tr>
  <td colspan=2>
	<h2>{t}Generic virus filtering{/t}</h2>
  </td>
 </tr>
 <tr>
  <td style="vertical-align:top;">
	<table>
	 <tr>
	  <td>
	 	{t}Database user{/t}
	  </td>
	  <td>
{render acl=$avUserACL}
	  	<input type='text' name='avUser' value='{$avUser}' style='width:220px;'>
{/render}
	  </td>
	 </tr>
	 <tr>
	  <td>
	 	{t}Database mirror{/t}
	  </td>
	  <td>
{render acl=$avDatabaseMirrorACL}
	  	<input type='text' name='avDatabaseMirror' value='{$avDatabaseMirror}' style='width:220px;'>
{/render}
	  </td>
	 </tr>
	 <tr>
	  <td>
	 	{t}Http proxy URL{/t}
	  </td>
	  <td>
{render acl=$avHttpProxyURLACL}
	  	<input type='text' name='avHttpProxyURL' value='{$avHttpProxyURL}' style='width:220px;'>
{/render}
	  </td>
	 </tr>
	 <tr>
	  <td>
	 	{t}Maximum threads{/t}
	  </td>
	  <td>
{render acl=$avMaxThreadsACL}
		<select name="avMaxThreads" title='{t}Select number of maximal threads{/t}'>
		 {html_options options=$ThreadValues selected=$avMaxThreads}
	 	</select>
{/render}
	  </td>
	 </tr>
	</table>
 
  </td>
  <td style='border-left:1px solid #A0A0A0;vertical-align:top;'>
  
	<table>
	 <tr>
	  <td>
	 	{t}Max directory recursions{/t}
	  </td>
	  <td>
{render acl=$avMaxDirectoryRecursionsACL}
	  	<input type='text' name='avMaxDirectoryRecursions' value='{$avMaxDirectoryRecursions}' >
{/render}
	  </td>
	 </tr>
	 <tr>
	  <td>
	 	{t}Checks per day{/t}
	  </td>
	  <td>
{render acl=$avChecksPerDayACL}
	  	<input type='text' name='avChecksPerDay' value='{$avChecksPerDay}'> 
{/render}
	  </td>
	 </tr>
	 <tr>
	  <td colspan=2>
{render acl=$avFlagsDACL}
	  	<input type='checkbox' name='avFlagsD' {$avFlagsDCHK} value='1'>
{/render}
		{t}Enable debugging{/t}
	  </td>
	 </tr>
	 <tr>
	  <td colspan=2>
{render acl=$avFlagsSACL}
	  	<input type='checkbox' name='avFlagsS' {$avFlagsSCHK} value='1'>
{/render}
		{t}Enable mail scanning{/t}
	  </td>
	 </tr>
	</table>
  </td>
 </tr>
 <tr>
  <td colspan=2>
   <p class='seperator'>&nbsp;</p>
	<h2>{t}Archive scanning{/t}</h2> 
  </td>
 </tr>
 <tr>
  <td style='vertical-align:top;'>
	<table>
	 <tr>
	  <td>
{render acl=$avFlagsAACL}
	  	<input type='checkbox' name='avFlagsA' {$avFlagsACHK} value='1'
			onClick=" changeState('avFlagsE') ; 
				  changeState('avArchiveMaxFileSize') ; 
				  changeState('avArchiveMaxRecursion') ; 
				  changeState('avArchiveMaxCompressionRatio') ; "
		>
{/render}
		{t}Enable scanning of archives{/t}
	  </td>
	 </tr>
	 <tr>
	  <td>
{render acl=$avFlagsEACL}
	  	<input type='checkbox' name='avFlagsE' {$avFlagsECHK} {$avFlagsAState} value='1' id='avFlagsE'>
{/render}
		{t}Block encrypted archives{/t}
	  </td>
	 </tr>
	</table>
  
  </td>
  <td  style="vertical-align:top;width:50%;border-left:1px solid #A0A0A0;">
	<table>
	 <tr>
	  <td>{t}Maximum file size{/t}
	  </td>
	  <td>
{render acl=$avArchiveMaxFileSizeACL}
	   <input name='avArchiveMaxFileSize' id='avArchiveMaxFileSize' value='{$avArchiveMaxFileSize}'  {$avFlagsAState} >
{/render}
	  </td>
	 </tr>
	 <tr>
	  <td>{t}Maximum recursion{/t}
	  </td>
	  <td>
{render acl=$avArchiveMaxRecursionACL}
	   <input name='avArchiveMaxRecursion' id='avArchiveMaxRecursion' value='{$avArchiveMaxRecursion}'  {$avFlagsAState} >
{/render}
	  </td>
	 </tr>
	 <tr>
	  <td>{t}Maximum compression ratio{/t}
	  </td>
	  <td>
{render acl=$avArchiveMaxCompressionRatioACL}
	   <input name='avArchiveMaxCompressionRatio' id='avArchiveMaxCompressionRatio' value='{$avArchiveMaxCompressionRatio}' {$avFlagsAState}>
{/render}
	  </td>
	 </tr>
	</table>
  
  </td>
 </tr>
</table>
<input type='hidden' name='goVirusServer' value='1'>
<p class='seperator'>&nbsp;</p>
<div style="width:100%; text-align:right;padding-top:10px;padding-bottom:3px;">
    <input type='submit' name='SaveService' value='{t}Save{/t}'>
    &nbsp;
    <input type='submit' name='CancelService' value='{t}Cancel{/t}'>
</div>

