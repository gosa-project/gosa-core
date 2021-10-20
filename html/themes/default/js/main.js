// Disable default event for scrolling the card
let scrollElem = document.querySelectorAll('.card-content-scroll');

if (scrollElem && scrollElem.length !== 0) {
    let cardContent = document.querySelector('.card-content');
    cardContent.style.overflowY = "visible";

    if (scrollElem.length > 1) {
        let outerScrollElem = document.querySelector('.card-content-scroll');
        outerScrollElem.style.overflowY = "visible";
        outerScrollElem.style.height = "auto";

        let checkSetup = document.querySelector('.content-area.setup');
        if (checkSetup && checkSetup.length !== 0) {
            outerScrollElem.classList.remove('card-content-scroll');
        }
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

// Creation of a submenu if the length of the navigation items of the mobile version is too long
let tabsElem = document.querySelectorAll('.view-xl');

if (tabsElem && tabsElem.length !== 0) {
    let mobileAreaSelection = document.querySelector('#mobile-area-selection');

    tabsElem.forEach(element => {
        let newElement = element.cloneNode(true);
        newElement.classList.remove('view-xl');
        newElement.classList.add('view-m');
        mobileAreaSelection.appendChild(newElement);
    });
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
var pwButton = document.getElementsByName("password_finish")

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