// js for select dropdown
var select = document.querySelectorAll('select');
var dropdownOptions = {
    dropdownOptions: {
        'constrainWidth': true,
        'coverTrigger': false,
        'hover': false,
        'alignment': 'right',
    }
}
var selectInstances = M.FormSelect.init(select, dropdownOptions);

// js for accordion menu
var collaps = document.querySelectorAll('.collapsible');
var collapsInstances = M.Collapsible.init(collaps, {});

// js for tooltip
var ttip = document.querySelectorAll('.tooltipped');
var ttipInstances = M.Tooltip.init(ttip, {});

// js for images
var mbox = document.querySelectorAll('.materialboxed');
var mboxInstances = M.Materialbox.init(mbox, {});

// js for datepicker
var dp = document.querySelectorAll('.datepicker');
let currentDate = new Date();
let minDate = new Date(1920, 1, 1);
let dateOptions = {
    'format': 'dd.mm.yyyy',
    'minDate': minDate,
    'maxDate': currentDate,
    'firstDay': 1,
    'defaultDate': currentDate,
    'yearRange': [minDate.getFullYear(), currentDate.getFullYear()]
}
var dpInstances = M.Datepicker.init(dp, dateOptions);

var ddmenu = document.querySelectorAll('.dropdown-menu');
var ddmenuInstances = M.Dropdown.init(ddmenu, {
    'constrainWidth': false,
    'coverTrigger': false,
    'hover': false,
    'alignment': 'right',
});

var ddfilter = document.querySelectorAll('.dropdown-filter');
var ddfilterInstances = M.Dropdown.init(ddfilter, {
    'constrainWidth': false,
    'coverTrigger': false,
    'hover': false,
    'alignment': 'right',
});

var nav = document.querySelectorAll('.dropdown-nav-more');
var navInstances = M.Dropdown.init(nav, {
    'constrainWidth': false,
    'coverTrigger': false,
    'hover': false,
    'alignment': 'right',
});

// js for input
var autocomplete = document.querySelector('#autocomplete-input');
var acInstances = M.Dropdown.init(autocomplete, {
    'constrainWidth': false,
    'coverTrigger': false,
    'hover': false
});