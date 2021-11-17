{capture name='_smarty_debug' assign=debug_output}
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
    <title>Smarty Debug Console</title>
<style type="text/css">
{literal}
<<<<<<< HEAD
body, h1, h2, td, th, p {
=======
            body, h1, h2, h3, td, th, p {
>>>>>>> gosa-core_v2.8
    font-family: sans-serif;
    font-weight: normal;
    font-size: 0.9em;
    margin: 1px;
    padding: 0;
}

h1 {
    margin: 0;
    text-align: left;
    padding: 2px;
    background-color: #f0c040;
    color:  black;
    font-weight: bold;
    font-size: 1.2em;
 }

h2 {
    background-color: #9B410E;
    color: white;
    text-align: left;
    font-weight: bold;
    padding: 2px;
    border-top: 1px solid black;
}
<<<<<<< HEAD
=======
            h3 {
                text-align: left;
                font-weight: bold;
                color: black;
                font-size: 0.7em;
                padding: 2px;
            }
>>>>>>> gosa-core_v2.8

body {
    background: black; 
}

p, table, div {
    background: #f0ead8;
} 

p {
    margin: 0;
    font-style: italic;
    text-align: center;
}

table {
    width: 100%;
}

th, td {
    font-family: monospace;
    vertical-align: top;
    text-align: left;
<<<<<<< HEAD
    width: 50%;
=======
>>>>>>> gosa-core_v2.8
}

td {
    color: green;
}

.odd {
    background-color: #eeeeee;
}

.even {
    background-color: #fafafa;
}

.exectime {
    font-size: 0.8em;
    font-style: italic;
}

<<<<<<< HEAD
#table_assigned_vars th {
    color: blue;
=======
            #bold div {
                color: black;
                font-weight: bold;
            }
            #blue h3 {
                color: blue;
            }
            #normal div {
                color: black;
                font-weight: normal;
            }
            #table_assigned_vars th {
                color: blue;
                font-weight: bold;
>>>>>>> gosa-core_v2.8
}

#table_config_vars th {
    color: maroon;
}
<<<<<<< HEAD
=======

>>>>>>> gosa-core_v2.8
{/literal}
</style>
</head>
<body>

<<<<<<< HEAD
<h1>Smarty Debug Console  -  {if isset($template_name)}{$template_name|debug_print_var nofilter}{else}Total Time {$execution_time|string_format:"%.5f"}{/if}</h1>

{if !empty($template_data)}
<h2>included templates &amp; config files (load time in seconds)</h2>

<div>
{foreach $template_data as $template}
  <font color=brown>{$template.name}</font>
  <span class="exectime">
   (compile {$template['compile_time']|string_format:"%.5f"}) (render {$template['render_time']|string_format:"%.5f"}) (cache {$template['cache_time']|string_format:"%.5f"})
  </span>
  <br>
=======
    <h1>Smarty {Smarty::SMARTY_VERSION} Debug Console
        -  {if isset($template_name)}{$template_name|debug_print_var nofilter} {/if}{if !empty($template_data)}Total Time {$execution_time|string_format:"%.5f"}{/if}</h1>

    {if !empty($template_data)}
        <h2>included templates &amp; config files (load time in seconds)</h2>
        <div>
            {foreach $template_data as $template}
                <font color=brown>{$template.name}</font>
                <br />&nbsp;&nbsp;<span class="exectime">
                (compile {$template['compile_time']|string_format:"%.5f"}) (render {$template['render_time']|string_format:"%.5f"}) (cache {$template['cache_time']|string_format:"%.5f"})
                 </span>
                <br />
>>>>>>> gosa-core_v2.8
{/foreach}
</div>
{/if}

<h2>assigned template variables</h2>

<table id="table_assigned_vars">
    {foreach $assigned_vars as $vars}
       <tr class="{if $vars@iteration % 2 eq 0}odd{else}even{/if}">   
<<<<<<< HEAD
       <th>${$vars@key|escape:'html'}</th>
       <td>{$vars|debug_print_var nofilter}</td></tr>
    {/foreach}
</table>

<h2>assigned config file variables (outer template scope)</h2>
=======
                <td><h3><font color=blue>${$vars@key}</font></h3>
                    {if isset($vars['nocache'])}<b>Nocache</b><br />{/if}
                    {if isset($vars['scope'])}<b>Origin:</b> {$vars['scope']|debug_print_var nofilter}{/if}
                </td>
                <td><h3>Value</h3>{$vars['value']|debug_print_var:10:80 nofilter}</td>
                <td>{if isset($vars['attributes'])}<h3>Attributes</h3>{$vars['attributes']|debug_print_var nofilter} {/if}</td>
         {/foreach}
    </table>

    <h2>assigned config file variables</h2>
>>>>>>> gosa-core_v2.8

<table id="table_config_vars">
    {foreach $config_vars as $vars}
       <tr class="{if $vars@iteration % 2 eq 0}odd{else}even{/if}">   
<<<<<<< HEAD
       <th>{$vars@key|escape:'html'}</th>
       <td>{$vars|debug_print_var nofilter}</td></tr>
=======
                <td><h3><font color=blue>#{$vars@key}#</font></h3>
                    {if isset($vars['scope'])}<b>Origin:</b> {$vars['scope']|debug_print_var nofilter}{/if}
                </td>
                <td>{$vars['value']|debug_print_var:10:80 nofilter}</td>
            </tr>
>>>>>>> gosa-core_v2.8
    {/foreach}

</table>
</body>
</html>
{/capture}
<script type="text/javascript">
<<<<<<< HEAD
{$id = $template_name|default:''|md5}
    _smarty_console = window.open("","console{$id}","width=680,height=600,resizable,scrollbars=yes");
=======
    {$id = '__Smarty__'}
    {if $display_mode}{$id = "$offset$template_name"|md5}{/if}
    _smarty_console = window.open("", "console{$id}", "width=1024,height=600,left={$offset},top={$offset},resizable,scrollbars=yes");
>>>>>>> gosa-core_v2.8
    _smarty_console.document.write("{$debug_output|escape:'javascript' nofilter}");
    _smarty_console.document.close();
</script>
