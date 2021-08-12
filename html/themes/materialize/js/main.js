// Disable default event for scrolling the card
let scrollElem = document.querySelectorAll('.card-content-scroll');

if (scrollElem.length !== 0) {
    let cardContent = document.querySelector('.card-content');
    cardContent.style.overflowY = "hidden"
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
var pwLowercase = document.getElementById("pw-lowercase");
var pwUppercase = document.getElementById("pw-uppercase");
var pwNumbers = document.getElementById("pw-numbers");
var pwSpecial = document.getElementById("pw-special");

// When the user starts to type something inside the password field
input.onkeyup = function() {
    testPasswordCss(input.value);

    // Validate length
    if (input.value.length >= 8) {
        pwLength.innerText = "check";
        pwLength.style.color = "green";
    } else {
        pwLength.innerText = "close";
        pwLength.style.color = "red";
    }

    // Validate lowercase letters
    var lowerCaseLetters = /[a-z]/g;
    if (input.value.match(lowerCaseLetters)) {
        pwLowercase.innerText = "check";
        pwLowercase.style.color = "green";
    } else {
        pwLowercase.innerText = "close";
        pwLowercase.style.color = "red";
    }

    // Validate capital letters
    var upperCaseLetters = /[A-Z]/g;
    if (input.value.match(upperCaseLetters)) {
        pwUppercase.innerText = "check";
        pwUppercase.style.color = "green";
    } else {
        pwUppercase.innerText = "close";
        pwUppercase.style.color = "red";
    }

    // Validate numbers
    var numbers = /[0-9]/g;
    if (input.value.match(numbers)) {
        pwNumbers.innerText = "check";
        pwNumbers.style.color = "green";
    } else {
        pwNumbers.innerText = "close";
        pwNumbers.style.color = "red";
    }

    // Validate numbers
    var specialChars = /[!,@,#,$,%,^,&,*,?,_,~]/g;
    if (input.value.match(specialChars)) {
        pwSpecial.innerText = "check";
        pwSpecial.style.color = "green";
    } else {
        pwSpecial.innerText = "close";
        pwSpecial.style.color = "red";
    }
}