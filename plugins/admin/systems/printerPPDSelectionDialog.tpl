<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
	<tr>
		<td style="vertical-align:top;">
			<div class="contentboxh" style="height:20px;">
				<p class="contentboxh" style="font-size:12px">
					<b>{t}Select objects to add{/t}</b>
				</p>
			</div>
			<div class="contentboxb">
				<p class="contentboxb" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
					<select style="width:500px; height:450px; margin-top:4px;" name="PPDselection" size="15">
            {html_options values=$objectKeys options=$objects}
					</select>
					<input type=hidden name="edit_helper">
				</p>	
			</div>
		</td>
		<td style="vertical-align:top;">
			<div class="contentboxh" style="height:20px;">
				<p class="contentboxh" style="font-size:12px">
          <img src="{$launchimage}" align="right" alt="[F]">
          <b>{t}Filters{/t}</b>
        </p>
      </div>
      <div class="contentboxb">
        <table summary="" style="width:100%;border-top:1px solid #B0B0B0; background-color:#F8F8F8">
          {$alphabet}
        </table>
        <table summary="" width="100%" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
          <tr>
            <td width="50%">
              <img alt="" src="{$tree_image}" align=middle> 
              &nbsp;{t}Display objects of department{/t}
            </td>
          </tr>
        </table>
        <table summary="" width="100%" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
          <tr>
            <td width="50%">
              <img alt="" src="{$search_image}" align=middle>
              &nbsp;{t}Display objects matching{/t}
            </td>
            <td>
              <input type='text' name='regex' maxlength='20' value='{$regex}' 
                title='{t}Regular expression for matching object names{/t}' onChange="mainform.submit();">
            </td>
          </tr>
        </table>
              {$apply}
      </div>
    </td>
  </tr>
</table>

<p class="plugbottom">
<input type="submit" name="SavePPDSelection" value="{t}Use{/t}">
  &nbsp;
<input type="submit" name="ClosePPDSelection" value="{t}Close{/t}">
</p>
<!--
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
-->
