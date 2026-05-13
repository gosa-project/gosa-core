<input class='filter_textfield' placeholder='{t}Search{/t}...' id='{$tag}' name='{$tag}' type='text' size='30' maxlength='30' value='{$value}'>

{if $hasAutocomplete}
<div id='autocomplete{$tag}' class='autocomplete'></div>
<script type='text/javascript'>
    new Ajax.Autocompleter('{$tag}', 'autocomplete{$tag}', 'autocomplete.php', { minChars: {$characters}, frequency: {$frequency} });
</script>
{/if}