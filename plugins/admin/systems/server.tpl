<table summary="{t}Server{/t}" width="100%">
 <tr>
  <td style='width:50%;'>

  <h3>{t}Generic{/t}</h3>
	<table summary="{t}Server settings{/t}">
	 <tr>
	  <td><LABEL for="cn">{t}Server name{/t}</LABEL>{$must}</td>
	  <td>
{render acl=$cnACL}
	   <input type='text' name="cn" id="cn" size=20 value="{$cn}">
{/render}
	  </td>
	 </tr>
	 <tr>
	  <td><LABEL for="description">{t}Description{/t}</LABEL></td>
	  <td>
{render acl=$descriptionACL}
           <input type='text' name="description" id="description" size=25 maxlength=80 value="{$description}">
{/render}
          </td>
	 </tr>
   	<tr>
     <td>{t}Mode{/t}</td>
     <td>
{render acl=$gotoModeACL}
      <select name="gotoMode" title="{t}Select terminal mode{/t}" size=1>
       {html_options options=$modes selected=$gotoMode}
      </select>
{/render}
     </td>
    </tr>
 	 <tr>
	  <td><br><LABEL for="base">{t}Base{/t}</LABEL>{$must}</td>
	  <td>
	   <br>
{render acl=$baseACL}
           {$base}
{/render}
	   </td>
	  </tr>
	</table>

  </td>
  <td class='left-border'>
	{$host_key}
  </td>
 </tr>
</table>

<hr>
<br>

{$netconfig}

{if $fai_activated && $si_activated}
  <hr>

  <h3>{t}Action{/t}</h3>

  {if $currently_installing}
    <i>{t}System installation in progress, the FAI state cannot be changed right now.{/t}</i>
  {else}
    {render acl=$FAIstateACL}
       <select size="1" name="saction" title="{t}Select action to execute for this server{/t}">
        <option>&nbsp;</option>
        {html_options options=$actions}
       </select>
    {/render}
    {render acl=$FAIstateACL}
       <button type='submit' name='action'>{t}Execute{/t}</button>

    {/render}
  {/if}
{/if}

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('cn');
  -->
</script>
