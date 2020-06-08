var elements = document.getElementsByTagName('script');

Array.prototype.forEach.call(elements, function (element) {
    if (element.type.indexOf('math/tex') !== -1) {
        // Extract math markdown
        var textToRender = element.innerText || element.textContent;

        // Kramdown and KaTeX workaround
        // https://github.com/rohanchandra/type-theme/issues/47
        textToRender = textToRender.replace(/%.*/g, '');

        // Create span for KaTeX
        var katexElement = document.createElement('span');

        // Support inline and display math
        if (element.type.indexOf('mode=display') !== -1) {
            katexElement.className += "math-display";
            textToRender = '\\displaystyle {' + textToRender + '}';
        } else {
            katexElement.className += "math-inline";
        }

        katex.render(textToRender, katexElement);
        element.parentNode.insertBefore(katexElement, element);
    }
});
