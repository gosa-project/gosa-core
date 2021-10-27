// Disable default event for scrolling the card
let scrollElem = document.querySelectorAll('.card-content-scroll');

if (scrollElem && scrollElem.length !== 0) {
    let cardContent = document.querySelector('.card-content');
    cardContent.style.overflowY = "visible";

    if (scrollElem.length > 1) {
        let outerScrollElem = document.querySelector('.card-content-scroll');
        outerScrollElem.style.overflowY = "visible";
        outerScrollElem.style.height = "auto";

        // let checkSetup = document.querySelector('.content-area.setup');
        // if (checkSetup && checkSetup.length !== 0) {
        //     outerScrollElem.classList.remove('card-content-scroll');
        // }
    }
}

// Disable empty container
let cardActionElem = document.querySelectorAll('.card-action');

if (cardActionElem && cardActionElem.length > 1) {
    cardActionArray = Array.from(cardActionElem);
    cardActionArray.forEach(element => {
        if (!element.querySelector("button")) {
            element.style.padding = "0";
        }
    });
}

// Adjustment of scroll height, because the card-action is outside of content-wrapper card-content-scroll
let contentWrapper = document.querySelector('.content-wrapper.card-content-scroll');
let pwresetWrapper = document.querySelectorAll('.pwreset-wrapper.card-content-scroll');
let sambaLoginWrapper = document.querySelectorAll('.samba-login-wrapper.card-content-scroll');
let cardActionElement = document.querySelectorAll('.card-action');

if (pwresetWrapper && pwresetWrapper.length == 1 && cardActionElement && cardActionElement.length == 1) {
    contentWrapper.style.maxHeight = "calc(100vh - 202px)";
} else if (sambaLoginWrapper && sambaLoginWrapper.length == 1 && cardActionElement && cardActionElement.length == 1) {
    contentWrapper.style.maxHeight = "calc(100vh - 202px)";
}

// Debounce Function
// Returns a function, that, as long as it continues to be invoked, will not
// be triggered. The function will be called after it stops being called for
// N milliseconds. If `immediate` is passed, trigger the function on the
// leading edge, instead of the trailing.
function debounce(func, wait, immediate) {
    var timeout;
    return function() {
        var context = this,
            args = arguments;
        var later = function() {
            timeout = null;
            if (!immediate) func.apply(context, args);
        };
        var callNow = immediate && !timeout;
        clearTimeout(timeout);
        timeout = setTimeout(later, wait);
        if (callNow) func.apply(context, args);
    };
};

// Automatic adjustment of content navigation during resize
function automaticAdjustmentNavigation() {
    const contentNavigationWrapper = document.querySelector('.object-info .card-tabs');
    const navigationPointer = document.querySelectorAll('.tabs .tab.pointer.menu-pointer');

    if (contentNavigationWrapper && contentNavigationWrapper.length !== 0 &&
        navigationPointer && navigationPointer.length !== 0) {
        const advancedAreaSelection = document.querySelector('#advanced-area-selection');
        const advancedTabPoint = document.querySelector('.advanced-menu');
        const tabsWrapper = document.querySelector('.object-info .card-tabs .tabs');

        let cardWrapper = contentNavigationWrapper.offsetWidth - 54;
        let tabPointers = Array.from(navigationPointer);
        let currentWidth = 0;

        tabPointers.forEach(element => {
            let previewWidth = currentWidth + element.offsetWidth + 1;

            if (currentWidth <= cardWrapper && previewWidth <= cardWrapper) {
                currentWidth += element.offsetWidth + 1;
            } else {
                advancedAreaSelection.appendChild(element);
                advancedTabPoint.style.display = "inline-block";
            }
        });

        let advancedAreaPointer = document.querySelectorAll('#advanced-area-selection .tab.pointer.menu-pointer');
        let advancedPointers = Array.from(advancedAreaPointer);

        advancedPointers.forEach(element => {
            tabsWrapper.insertBefore(element, advancedTabPoint);

            let previewWidth = currentWidth + element.offsetWidth + 1;

            if (currentWidth <= cardWrapper && previewWidth <= cardWrapper) {
                currentWidth += element.offsetWidth + 1;
            } else {
                advancedAreaSelection.appendChild(element);
            }
        });

        if (advancedPointers.length == 0) {
            advancedTabPoint.style.display = "none";
        }
    }
}

automaticAdjustmentNavigation();

let debounceAutomaticAdjustmentNavigation = debounce(automaticAdjustmentNavigation, 50);

window.addEventListener('resize', debounceAutomaticAdjustmentNavigation);

// Adjust start menu column width to specific display sizes
let startpageMenu = document.querySelectorAll('.row.startpage-iconmenu');
let startpagePointer = document.querySelectorAll('.startpage-iconmenu .pointer');

if (startpageMenu && startpageMenu.length !== 0 && startpagePointer && startpagePointer.length !== 0) {
    let windowWidth = window.innerWidth;
    pointers = Array.from(startpagePointer);

    if (windowWidth >= 1201 && windowWidth <= 1400) {
        pointers.forEach(element => {
            if (element.classList.contains('xl2')) {
                newElement.classList.remove('xl2');
                newElement.classList.add('xl3');
            }
        });
    } else {
        pointers.forEach(element => {
            if (element.classList.contains('xl3')) {
                newElement.classList.remove('xl3');
                newElement.classList.add('xl2');
            }
        });
    }
}

// Function to change the visibility of the password
document.querySelectorAll('.toggle-pw').forEach(icon => {
    icon.addEventListener('click', function() {
        let input = this.parentElement.querySelector('input');

        if (input.getAttribute('type') == 'password') {
            input.setAttribute('type', 'text');
            icon.innerText = "visibility_off";
        } else {
            input.setAttribute('type', 'password');
            icon.innerText = "visibility";
        }
    })
});

// Adjustment of the paddings when the control panel is visible
let listWrapperElem = document.querySelector('.card-content.first-level .list-head-wrapper');
let cardContentFirstLevelElem = document.querySelector('.card-content.first-level');

if (listWrapperElem && cardContentFirstLevelElem) {
    cardContentFirstLevelElem.style.padding = "0";
}

// Check password conditions
var input = document.getElementById("new_password");
var pwLength = document.getElementById("pw-length");
var pwLowercase_eval = document.getElementById("pw-lowercase") != null;
var pwLowercase = document.getElementById("pw-lowercase");
var pwUppercase_eval = document.getElementById("pw-uppercase") != null;
var pwUppercase = document.getElementById("pw-uppercase");
var pwNumber_eval = document.getElementById("pw-numbers") != null;
var pwNumbers = document.getElementById("pw-numbers");
var pwSpecial_eval = document.getElementById("pw-special") != null;
var pwSpecial = document.getElementById("pw-special");
let pwStrength = [false, false, false, false, false];
var pwButton = document.getElementById("password_finish")

// When the user starts to type something inside the password field
if (input) {
    input.onkeyup = function() {
        testPasswordCss(input.value);

        // Validate length
        if (input.value.length >= 8) {
            pwLength.innerText = "check";
            pwLength.style.color = "green";
            pwStrength[0] = true;
        } else {
            pwLength.innerText = "close";
            pwLength.style.color = "red";
            pwStrength[0] = false;
        }

        // Validate lowercase letters
        if (pwLowercase_eval) {
            var lowerCaseLetters = /[a-z]/g;
            if (input.value.match(lowerCaseLetters)) {
                pwLowercase.innerText = "check";
                pwLowercase.style.color = "green";
                pwStrength[1] = true;
            } else {
                pwLowercase.innerText = "close";
                pwLowercase.style.color = "red";
                pwStrength[1] = false;
            }
        } else {
            pwStrength[1] = true;
        }


        // Validate capital letters
        if (pwUppercase_eval) {
            var upperCaseLetters = /[A-Z]/g;
            if (input.value.match(upperCaseLetters)) {
                pwUppercase.innerText = "check";
                pwUppercase.style.color = "green";
                pwStrength[2] = true;
            } else {
                pwUppercase.innerText = "close";
                pwUppercase.style.color = "red";
                pwStrength[2] = false;
            }
        } else {
            pwStrength[2] = true;
        }

        // Validate numbers
        if (pwNumber_eval) {
            var numbers = /[0-9]/g;
            if (input.value.match(numbers)) {
                pwNumbers.innerText = "check";
                pwNumbers.style.color = "green";
                pwStrength[3] = true;
            } else {
                pwNumbers.innerText = "close";
                pwNumbers.style.color = "red";
                pwStrength[3] = false;
            }
        } else {
            pwStrength[3] = true;

        }


        // Validate numbers
        if (pwSpecial_eval) {
            var specialChars = /[!,@,#,$,%,^,&,*,?,_,~]/g;
            if (input.value.match(specialChars)) {
                pwSpecial.innerText = "check";
                pwSpecial.style.color = "green";
                pwStrength[4] = true;
            } else {
                pwSpecial.innerText = "close";
                pwSpecial.style.color = "red";
                pwStrength[4] = false;
            }
        } else {
            pwStrength[4] = true;

        }

        if (pwStrength.every(Boolean)) {
            pwButton.disabled = false;
        } else {
            pwButton.disabled = true;
        }
    }
}

