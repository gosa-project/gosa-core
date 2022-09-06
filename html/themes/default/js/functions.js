function setItemCount() {
    window.location.href = (document.getElementById('page-items').value);
}

// validate input for two factor authentication
function verify_input() {
    let code = document.getElementById("code").value;
    console.log(code)
    let input_data = code;
    let form_data = new FormData();
    let request = new XMLHttpRequest();

    form_data.append("secret", input_data);
    
    request.open('POST', window.location);
    request.send(form_data);
    
    if (input_data.length == 6) {
        request.onreadystatechange = function () {
            if (request.readyState == 4 && request.status == 200 && request.responseText == "true") {
                document.getElementById('VERIFY_OK').disabled = false;
            }
        }
    } else {
        document.getElementById('VERIFY_OK').disabled = true;
    }
}

function cancel_verification() {
    let form_data = new FormData();
    form_data.append('cancel_verification', '')
    let request = new XMLHttpRequest();

    request.open('POST', window.location);
    request.send(form_data);

    request.onreadystatechange = function() {
        if (request.readyState == 4 && request.status == 200) {
            next_msg_dialog();
            window.location.href = "main.php?global_check=1";
        }
    }
}