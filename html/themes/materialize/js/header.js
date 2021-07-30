document.addEventListener('DOMContentLoaded', function() {
    var elems = document.querySelectorAll('.dropdown-nav-more');
    var instances = M.Dropdown.init(elems, {
        'constrainWidth': false,
        'coverTrigger': false,
        'hover': false,
        'alignment': 'right',
    });
});