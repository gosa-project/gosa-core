// js for select dropdown
document.addEventListener('DOMContentLoaded', function() {
    var elems = document.querySelectorAll('select');
    var instances = M.FormSelect.init(elems, {
        belowOrigin: true
    });
});

// js for accordion menu
document.addEventListener('DOMContentLoaded', function() {
    var elems = document.querySelectorAll('.collapsible');
    var instances = M.Collapsible.init(elems, {});
});

// js for tooltip
document.addEventListener('DOMContentLoaded', function() {
    var elems = document.querySelectorAll('.tooltipped');
    var instances = M.Tooltip.init(elems, {});
});

// js for images
document.addEventListener('DOMContentLoaded', function() {
    var elems = document.querySelectorAll('.materialboxed');
    var instances = M.Materialbox.init(elems, {});
});

// js for datepicker
document.addEventListener('DOMContentLoaded', function() {
    var elems = document.querySelectorAll('.datepicker');
    var instances = M.Datepicker.init(elems, {});
});

// js for dropdowns
document.addEventListener('DOMContentLoaded', function() {
    var elems = document.querySelectorAll('.dropdown-menu');
    var instances = M.Dropdown.init(elems, {
        'constrainWidth': false,
        'coverTrigger': false,
        'hover': false,
        'alignment': 'right',
    });
});

// js for dropdowns
document.addEventListener('DOMContentLoaded', function() {
    var elems = document.querySelectorAll('.dropdown-filter');
    var instances = M.Dropdown.init(elems, {
        'constrainWidth': false,
        'coverTrigger': false,
        'hover': false,
        'alignment': 'right',
    });
});