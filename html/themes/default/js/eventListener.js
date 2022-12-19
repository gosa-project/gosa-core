// js for select dropdown
let select = document.querySelectorAll('select');
let dropdownOptions = {
    dropdownOptions: {
        'constrainWidth': false,
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
let collaps = document.querySelectorAll('.collapsible.expandable');
let collapsInstances = M.Collapsible.init(collaps, {
    accordion: false,
});

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

let advancedareaSelection = document.querySelectorAll('.dropdown-advanced-area-selection');
let advancedareaInstances = M.Dropdown.init(advancedareaSelection, {
    'constrainWidth': false,
    'coverTrigger': false,
    'hover': false,
    'alignment': 'right'
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
var dpInstances = M.Datepicker.init(dp, dateOptions);

function language() {
    return language = {
        en: {
            cancel: 'Cancel',
            done: 'Ok',
            months: ['January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December'],
            monthsShort: ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'],
            weekdays: ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'],
            weekdaysShort: ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat'],
            weekdaysAbbrev: ['S', 'M', 'T', 'W', 'T', 'F', 'S']
        },
        de: {
            cancel: 'Abbrechen',
            done: 'OK',
            months: ['Januar', 'Februar', 'März', 'April', 'Mai', 'Juni', 'Juli', 'August', 'September', 'Oktober', 'November', 'Dezember'],
            monthsShort: ['Jan', 'Feb', 'Mär', 'Apr', 'Mai', 'Jun', 'Jul', 'Aug', 'Sep', 'Okt', 'Nov', 'Dez'],
            weekdays: ['Sonntag', 'Montag', 'Dienstag', 'Mittwoch', 'Donnerstag', 'Freitag', 'Samstag'],
            weekdaysShort: ['So', 'Mo', 'Di', 'Mi', 'Do', 'Fr', 'Sa'],
            weekdaysAbbrev: ['S', 'M', 'D', 'M', 'D', 'F', 'S'],
        },
        es: {
            cancel: 'Cancelar',
            done: 'OK',
            months: ['enero', 'febrero', 'marzo', 'abril', 'mayo', 'junio', 'julio', 'agosto', 'septiembre', 'octubre', 'noviembre', 'diciembre'],
            monthsShort: ['ene', 'feb', 'mar', 'abr', 'may', 'jun', 'jul', 'ago', 'sep', 'oct', 'nov', 'dic'],
            weekdays: ['domingo', 'lunes', 'martes', 'miércoles', 'jueves', 'viernes', 'sábado'],
            weekdaysShort: ['dom', 'lun', 'mar', 'mié', 'jue', 'vie', 'sáb'],
            weekdaysAbbrev: ["D", "L", "M", "M", "J", "V", "S"]
        },
        fr: {
            cancel: 'Annuler',
            done: 'OK',
            months: ['Janvier', 'Février', 'Mars', 'Avril', 'Mai', 'Juin', 'Juillet', 'Août', 'Septembre', 'Octobre', 'Novembre', 'Décembre'],
            monthsShort: ['Jan', 'Fev', 'Mar', 'Avr', 'Mai', 'Jun', 'Jul', 'Aou', 'Sep', 'Oct', 'Nov', 'Dec'],
            weekdays: ['Dimanche', 'Lundi', 'Mardi', 'Mercredi', 'Jeudi', 'Vendredi', 'Samedi'],
            weekdaysShort: ['Dim', 'Lun', 'Mar', 'Mer', 'Jeu', 'Ven', 'Sam'],
            weekdaysAbbrev: ['D', 'S', 'T', 'Q', 'Q', 'S', 'S'],
        },
        it: {
            cancel: 'Cancella',
            done: 'OK',
            months: ['gennaio', 'febbraio', 'marzo', 'aprile', 'maggio', 'giugno', 'luglio', 'agosto', 'settembre', 'ottobre', 'novembre', 'dicembre'],
            monthsShort: ['gen', 'feb', 'mar', 'apr', 'mag', 'giu', 'lug', 'ago', 'set', 'ott', 'nov', 'dic'],
            weekdays: ['domenica', 'lunedì', 'martedì', 'mercoledì', 'giovedì', 'venerdì', 'sabato'],
            weekdaysShort: ['dom', 'lun', 'mar', 'mer', 'gio', 'ven', 'sab'],
            weekdaysAbbrev: ['D', 'L', 'M', 'M', 'G', 'V', 'S'],
        },
        nl: {
            cancel: 'Verwijderen',
            done: 'OK',
            months: ['januari', 'februari', 'maart', 'april', 'mei', 'juni', 'juli', 'augustus', 'september', 'oktober', 'november', 'december'],
            monthsShort: ['jan', 'feb', 'maa', 'apr', 'mei', 'jun', 'jul', 'aug', 'sep', 'okt', 'nov', 'dec'],
            weekdays: ['zondag', 'maandag', 'dinsdag', 'woensdag', 'donderdag', 'vrijdag', 'zaterdag'],
            weekdaysShort: ['zo', 'ma', 'di', 'wo', 'do', 'vr', 'za'],
            weekdaysAbbrev: ['Z', 'M', 'D', 'W', 'D', 'V', 'Z'],
        },
        pl: {
            cancel: 'Usunąć',
            done: 'zrobione',
            months: ['styczeń', 'luty', 'marzec', 'kwiecień', 'maj', 'czerwiec', 'lipiec', 'sierpień', 'wrzesień', 'październik', 'listopad', 'grudzień'],
            monthsShort: ['sty', 'lut', 'mar', 'kwi', 'maj', 'cze', 'lip', 'sie', 'wrz', 'paź', 'lis', 'gru'],
            weekdays: ['niedziela', 'poniedziałek', 'wtorek', 'środa', 'czwartek', 'piątek', 'sobota'],
            weekdaysShort: ['niedz.', 'pn.', 'wt.', 'śr.', 'cz.', 'pt.', 'sob.'],
            weekdaysAbbrev: ['N', 'P', 'W', 'Ś', 'C', 'P', 'S'],
        },
        pt: {
            cancel: 'limpar',
            done: 'feito',
            months: ['janeiro', 'fevereiro', 'março', 'abril', 'maio', 'junho', 'julho', 'agosto', 'setembro', 'outubro', 'novembro', 'dezembro'],
            monthsShort: ['jan', 'fev', 'mar', 'abr', 'mai', 'jun', 'jul', 'ago', 'set', 'out', 'nov', 'dez'],
            weekdays: ['Domingo', 'Segunda', 'Terça', 'Quarta', 'Quinta', 'Sexta', 'Sábado'],
            weekdaysShort: ['dom', 'seg', 'ter', 'qua', 'qui', 'sex', 'sab'],
            weekdaysAbbrev: ['D', 'S', 'T', 'Q', 'Q', 'S', 'S'],
        },
        pt_BR: {
            cancel: 'limpar',
            done: 'feito',
            months: ['janeiro', 'fevereiro', 'março', 'abril', 'maio', 'junho', 'julho', 'agosto', 'setembro', 'outubro', 'novembro', 'dezembro'],
            monthsShort: ['jan', 'fev', 'mar', 'abr', 'mai', 'jun', 'jul', 'ago', 'set', 'out', 'nov', 'dez'],
            weekdays: ['domingo', 'segunda-feira', 'terça-feira', 'quarta-feira', 'quinta-feira', 'sexta-feira', 'sábado'],
            weekdaysShort: ['dom', 'seg', 'ter', 'qua', 'qui', 'sex', 'sab'],
            weekdaysAbbrev: ['D', 'S', 'T', 'Q', 'Q', 'S', 'S'],
        },
        ru: {
            cancel: 'Отмена',
            done: 'сделано',
            months: ['января', 'февраля', 'марта', 'апреля', 'мая', 'июня', 'июля', 'августа', 'сентября', 'октября', 'ноября', 'декабря'],
            monthsShort: ['янв', 'фев', 'мар', 'апр', 'май', 'июн', 'июл', 'авг', 'сен', 'окт', 'ноя', 'дек'],
            weekdays: ['воскресенье', 'понедельник', 'вторник', 'среда', 'четверг', 'пятница', 'суббота'],
            weekdaysShort: ['вс', 'пн', 'вт', 'ср', 'чт', 'пт', 'сб'],
            weekdaysAbbrev: ['вс', 'пн', 'вт', 'ср', 'чт', 'пт', 'сб'],
        },
        vi: {
            cancel: 'Hủy bỏ',
            done: 'VÂNG',
            months: ['Tháng Một', 'Tháng Hai', 'Tháng Ba', 'Tháng Tư', 'Tháng Năm', 'Tháng Sáu', 'Tháng Bảy', 'Tháng Tám', 'Tháng Chín', 'Tháng Mười', 'Tháng Mười Một', 'Tháng Mười Hai'],
            monthsShort: ['Một', 'Hai', 'Ba', 'Tư', 'Năm', 'Sáu', 'Bảy', 'Tám', 'Chín', 'Mưới', 'Mười Một', 'Mười Hai'],
            weekdays: ['Chủ Nhật', 'Thứ Hai', 'Thứ Ba', 'Thứ Tư', 'Thứ Năm', 'Thứ Sáu', 'Thứ Bảy'],
            weekdaysShort: ['C.Nhật', 'T.Hai', 'T.Ba', 'T.Tư', 'T.Năm', 'T.Sáu', 'T.Bảy'],
        },
        zh: {
            cancel: '取消',
            done: '完成',
            months: ['一月', '二月', '三月', '四月', '五月', '六月', '七月', '八月', '九月', '十月', '十一月', '十二月'],
            monthsShort: ['一', '二', '三', '四', '五', '六', '七', '八', '九', '十', '十一', '十二'],
            weekdays: ['星期日', '星期一', '星期二', '星期三', '星期四', '星期五', '星期六'],
            weekdaysShort: ['日', '一', '二', '三', '四', '五', '六'],
            weekdaysAbbrev: ['日', '一', '二', '三', '四', '五', '六'],
        }
    }
}

function enable_upload(val) {
    console.log(val);
    var upload_ssh_btn = document.getElementById('upload_sshpublickey');
    if (val) {
        upload_ssh_btn.disabled = false;
    } else {
        upload_ssh_btn.disabled = true;
    }
}