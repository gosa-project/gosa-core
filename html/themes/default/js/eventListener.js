// js for select dropdown
let select = document.querySelectorAll('select');
let dropdownOptions = {
    dropdownOptions: {
        'constrainWidth': true,
        'coverTrigger': false,
        'hover': false,
        'alignment': 'right',
    }
}
let selectInstances = M.FormSelect.init(select, dropdownOptions);

// js for multiple select dropdown
let multiSelect = document.querySelectorAll('select[multiple]');
let dropdownOptionsMultipleSelect = {
    classes: "multiple-select",
    dropdownOptions: {
        'constrainWidth': true,
        'coverTrigger': false,
        'hover': false,
        'alignment': 'right',
    }
}
let multiSelectInstances = M.FormSelect.init(multiSelect, dropdownOptionsMultipleSelect);

// js for accordion menu
let collaps = document.querySelectorAll('.collapsible');
let collapsInstances = M.Collapsible.init(collaps, {});

// js for tooltip
let ttip = document.querySelectorAll('.tooltipped');
let ttipInstances = M.Tooltip.init(ttip, {});

// js for images
let mbox = document.querySelectorAll('.materialboxed');
let mboxInstances = M.Materialbox.init(mbox, {});

let ddmenu = document.querySelectorAll('.dropdown-menu');
let ddmenuInstances = M.Dropdown.init(ddmenu, {
    'constrainWidth': false,
    'coverTrigger': false,
    'hover': false,
    'alignment': 'right',
});

let ddfilter = document.querySelectorAll('.dropdown-filter');
let ddfilterInstances = M.Dropdown.init(ddfilter, {
    'constrainWidth': false,
    'coverTrigger': false,
    'hover': false,
    'alignment': 'right',
});

let nav = document.querySelectorAll('.dropdown-nav-more');
let navInstances = M.Dropdown.init(nav, {
    'constrainWidth': false,
    'coverTrigger': false,
    'hover': false,
    'alignment': 'right',
});

// js for input
let autocomplete = document.querySelector('#autocomplete-input');
let acInstances = M.Dropdown.init(autocomplete, {
    'constrainWidth': false,
    'coverTrigger': false,
    'hover': false
});

let lang = '';
if (document.getElementById('lang')) {
    lang = document.getElementById('lang').value;
}
let languages = language();
let dp = document.querySelectorAll('.datepicker');
let currentDate = new Date();
let minDate = new Date(1920, 1, 1);
let dateOptions = {
    'format': 'dd mmm, yyyy',
    'minDate': minDate,
    'maxDate': currentDate,
    'firstDay': 1,
    'defaultDate': currentDate,
    'yearRange': [minDate.getFullYear(), currentDate.getFullYear()],
    'i18n': languages[lang]
}
let dpInstances = M.Datepicker.init(dp, dateOptions);