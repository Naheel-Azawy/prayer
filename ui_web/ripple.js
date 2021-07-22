// Ripple effect, based on https://codepen.io/jakob-e/pen/XZoZWQ
export function ripple_apply() {
    [].map.call(document.getElementsByClassName("ripple"), function (el) {
        var listener = function (e) {
            e = e.touches ? e.touches[0] : e;
            var r = el.getBoundingClientRect(), d = Math.sqrt(Math.pow(r.width, 2) + Math.pow(r.height, 2)) * 2;
            el.style.cssText = "--s:0; --o:1;"; el.offsetTop;
            el.style.cssText = "--t:1; --o:0; --d:" + d + "; --x:" + (e.clientX - r.left) + "; --y:" + (e.clientY - r.top) + ";";
        };
        el.addEventListener("mousedown", listener);
        el.addEventListener("touchstart", e => {
            el.removeEventListener("mousedown", listener);
            listener(e);
        });
    });
}
