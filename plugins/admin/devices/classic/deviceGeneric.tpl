
<h3>{t}Devices{/t}</h3>

<table width="100%" summary="{t}Edit devices{/t}">
 <tr>
  <td>

   <table style='width:100%' class='right-border' summary="{t}Generic settings{/t}">
    <tr>
     <td><LABEL for="base">{t}Base{/t}</LABEL></td>
     <td>
      {render acl=$baseACL}
       {$base}
      {/render}
     </td>
    </tr>
    <tr>
     <td><LABEL for="cn">{t}Device name{/t}</LABEL>{$must}</td>
     <td>
      {render acl=$cnACL}
       <input type="text" size=40 value="{$cn}" name="cn" id="cn">
      {/render}
     </td>
    </tr>
    <tr>
     <td><LABEL for="description">{t}Description{/t}</LABEL></td>
     <td>
      {render acl=$descriptionACL}
       <input type="text" size=40 value="{$description}" name="description" id="description">
      {/render}
     </td>
    </tr>
    <tr>
     <td><LABEL for="description">{t}Device type{/t}</LABEL></td>
     <td>
      {render acl=$typeACL}
       <select id="type" size="1" name="type" title="{t}Choose the device type{/t}">
        {html_options options=$types selected=$type}
       </select>
      {/render}
     </td>
    </tr>
   </table>
  </td>
  <td>
   <table summary="{t}Serial settings{/t}">
    <tr>
     <td><LABEL for="devID">{t}Serial number{/t}&nbsp;{t}(iSerial){/t}</LABEL>{$must}</td>
     <td>
      {render acl=$devIDACL}
       <input type="text" value="{$devID}" name="devID" id="devID">
      {/render}
     </td>
     <td colspan="2">&nbsp;</td>
    </tr>
    <tr>
     <td><LABEL for="vendor">{t}Vendor-ID{/t}&nbsp;{t}(idVendor){/t}</LABEL>{$must}</td>
     <td>
      {render acl=$vendorACL}
       <input type="text" value="{$vendor}" name="vendor" id="vendor">
      {/render}
     </td>
    </tr>
    <tr>
     <td><LABEL for="produkt">{t}Product-ID{/t}&nbsp;{t}(idProduct){/t}</LABEL>{$must}</td>
     <td>
      {render acl=$serialACL}
       <input type="text" value="{$serial}" name="serial" id="serial">
      {/render}
     </td>
    </tr>
   </table>
  </td>
 </tr>
</table>
<input type='hidden' value="1" name="deviceGeneric_posted">
<script language="JavaScript" type="text/javascript">
 <!-- // First input field on page    
  focus_field('name');  
 -->
</script>
