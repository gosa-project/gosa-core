let scrollElem = document.querySelectorAll('.card-content-scroll');

if (scrollElem.length !== 0) {
    let cardContent = document.querySelector('.card-content');
    cardContent.style.overflow = "hidden"
}