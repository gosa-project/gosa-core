<!--////////////////////
	//	LOCALITY (l)
    //////////////////// -->

<table summary="{t}Domain component{/t}" style="width:100%" cellpadding=4>
 <tr>
   <td style="width:50%">
     <h3>{t}Properties{/t}</h3>
     <table summary="{t}Properties{/t}">
      <tr>
       <td><LABEL for="dc">{t}Locality name{/t}</LABEL>{$must}</td>
       <td>
{render acl=$dcACL}
    	<input type='text' id="dc" name="dc" size=25 maxlength=60 value="{$dc}" title="{t}Name of locality to create{/t}">
{/render}
       </td>
      </tr>
      <tr>
       <td><LABEL for="description">{t}Description{/t}</LABEL>{$must}</td>
       <td>
{render acl=$descriptionACL}
        <input type='text' id="description" name="description" size=25 maxlength=80 value="{$description}" title="{t}Descriptive text for department{/t}">
{/render}
       </td>
      </tr>
	{if !$is_root_dse}
      <tr>
        <td><LABEL for="base">{t}Base{/t}</LABEL>{$must}</td>
        <td>
{render acl=$baseACL}
          {$base}
{/render}
	  </td>
	 </tr>
	{/if}

  {if $manager_enabled}
    <tr>
     <td><label for="manager">{t}Manager{/t}</label></td>
     <td>
{render acl=$managerACL}
        <input type='text' name='manager_name' id='manager_name' value='{$manager_name}' disabled
          title='{$manager}'>
{/render}
{render acl=$managerACL}
        {image path="images/lists/edit.png" action="editManager"}

{/render}
        {if $manager!=""}
{render acl=$managerACL}
        {image path="images/info_small.png" title="{$manager}"}

{/render}
{render acl=$managerACL}
        {image path="images/lists/trash.png" action="removeManager"}

{/render}
        {/if}
     </td>
    </tr>
  {/if}
	</table>
  </td>
 </tr>
</table>
<hr>
<h3>{t}Administrative settings{/t}</h3>
{render acl=$gosaUnitTagACL}
 <input id="is_administrational_unit" type=checkbox name="is_administrational_unit" value="1" {$gosaUnitTag}>
 <label for="is_administrational_unit">{t}Tag department as an independent administrative unit{/t}</label>
{/render}
<input type='hidden' name='dep_generic_posted' value='1'>
