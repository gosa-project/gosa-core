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
var dpInstances = M.Datepicker.init(dp, {});

// js for dropdowns
var ddmenu = document.querySelectorAll('.dropdown-menu');
var ddmenuInstances = M.Dropdown.init(ddmenu, {
    'constrainWidth': false,
    'coverTrigger': false,
    'hover': false,
    'alignment': 'right',
});

// js for dropdowns
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

var autocomplete = document.querySelector('#autocomplete-input');
var acInstances = M.Dropdown.init(autocomplete, {
    'coverTrigger': false,
    'hover': false,
    'constrainWidth': false,
});