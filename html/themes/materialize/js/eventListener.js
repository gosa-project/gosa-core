// js for select dropdown
var select = document.querySelectorAll('select');
var instances = M.FormSelect.init(select, {
    belowOrigin: true
});

// js for accordion menu
var collaps = document.querySelectorAll('.collapsible');
var instances = M.Collapsible.init(collaps, {});

// js for tooltip
var ttip = document.querySelectorAll('.tooltipped');
var instances = M.Tooltip.init(ttip, {});

// js for images
var mbox = document.querySelectorAll('.materialboxed');
var instances = M.Materialbox.init(mbox, {});

// js for datepicker
var dp = document.querySelectorAll('.datepicker');
var instances = M.Datepicker.init(dp, {});

// js for dropdowns
var ddmenu = document.querySelectorAll('.dropdown-menu');
var instances = M.Dropdown.init(ddmenu, {
    'constrainWidth': false,
    'coverTrigger': false,
    'hover': false,
    'alignment': 'right',
});

// js for dropdowns
var ddfilter = document.querySelectorAll('.dropdown-filter');
var instances = M.Dropdown.init(ddfilter, {
    'constrainWidth': false,
    'coverTrigger': false,
    'hover': false,
    'alignment': 'right',
});

var elems = document.querySelectorAll('.dropdown-nav-more');
var instances = M.Dropdown.init(elems, {
    'constrainWidth': false,
    'coverTrigger': false,
    'hover': false,
    'alignment': 'right',
});