<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
	<tr>
		<td style="vertical-align:top;width=50%;width:600px">
			<div class="contentboxh" style="height:20px;">
				<p class="contentboxh" style="font-size:12px">
					<b>{t}Select objects to add{/t}</b>
				</p>
			</div>
			<div class="contentboxb">
          {$List}
					<input type=hidden name="edit_helper">
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
            <td style="width:18px">
              <img alt="" src="{$search_image}" align=middle title="{t}Display objects matching{/t}">
            </td>
            <td>
              <input type='text' style="width:99%" name='regex' maxlength='20' value='{$regex}' 
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
<input type="submit" name="ClosePPDSelection" value="{t}Close{/t}">
</p>
<!--
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
-->
