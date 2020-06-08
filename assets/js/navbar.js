/*
 * Display the menu items on smaller screens
 */
$(function () {
    var pull = $('#pull');
    menu = $('nav ul');
    //menuHeight = menu.height();

    $(pull).on('click', function (e) {
        menu.slideToggle();
    });
});

/*
 * Display the navbar back to normal after resize
 */
$(window).resize(function () {
    var w = $(window).width();
    if (w > 320 && menu.is(':hidden')) {
        menu.removeAttr('style');
    }
});


/*
 * Make the header images move on scroll
 */
$(window).scroll(function () {
    var x = $(this).scrollTop();   
    $('#main').css('background-position', '100% ' + parseInt(-x/3) + 'px' + ', 0%, center top');
});
