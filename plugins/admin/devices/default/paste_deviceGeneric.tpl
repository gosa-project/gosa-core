<div class="device-wrapper">
    <div class="row">
        <h3>{t}Devices{/t}</h3>
        <div class="col s12 xl6">
            <div class="input-field">
                <input type="text" size=40 value="{$cn}" name="cn" id="cn">
                <label for="cn">{t}Device name{/t}{$must}</label>
            </div>
        </div>
    </div>
</div>

<input type='hidden' value="1" name="deviceGeneric_posted">

<script language="JavaScript" type="text/javascript">
    // First input field on page    
    focus_field('name');  
</script>
