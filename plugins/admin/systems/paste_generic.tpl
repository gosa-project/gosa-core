{if $object == "server"}
    <table summary="{t}Paste server{/t}">
     <tr>
      <td><LABEL for="cn">{t}Server name{/t}</LABEL>{$must}</td>
      <td>
       <input type='text' name="cn" id="cn" size=20 maxlength=60 value="{$cn}">
      </td>
     </tr>
	</table>
{/if}
{if $object == "workstation"}
    <table summary="{t}Paste workstation{/t}">
     <tr>
      <td><LABEL for="cn">{t}workstation name{/t}</LABEL>{$must}</td>
      <td>
       <input type='text' name="cn" id="cn" size=20 maxlength=60 value="{$cn}">
      </td>
     </tr>
	</table>
{/if}
{if $object == "terminal"}
    <table summary="{t}Paste terminal{/t}">
     <tr>
      <td><LABEL for="cn">{t}Terminal name{/t}</LABEL>{$must}</td>
      <td>
       <input type='text' name="cn" id="cn" size=20 maxlength=60 value="{$cn}">
      </td>
     </tr>
	</table>
{/if}
{if $object == "printer"}
    <table summary="{t}Paste printer{/t}">
     <tr>
      <td><LABEL for="cn">{t}Printer name{/t}</LABEL>{$must}</td>
      <td>
       <input type='text' name="cn" id="cn" size=20 maxlength=60 value="{$cn}">
      </td>
     </tr>
	</table>
{/if}
{if $object == "component"}
    <table summary="{t}Paste network component{/t}">
     <tr>
      <td><LABEL for="cn">{t}Component name{/t}</LABEL>{$must}</td>
      <td>
       <input type='text' name="cn" id="cn" size=20 maxlength=60 value="{$cn}">
      </td>
     </tr>
	</table>
{/if}
