const HijrahDate = require("hijrah-date");

import {
    pt_init, prayertimes, next_time,
    remaining_to, time_str_nearest, TIMES_NAMES,
    calc_methods, asr_methods, high_lat_methods
} from "./pt_iface";

import {
    config, load_config, save_config
} from "./configs";

import {
    strings_map
} from "./strings";
let strings; // updated in main

import {
    $div, $span, $table, $tr, $td, $br, $hr, $p, $h3,
    $input, $select, $option, $get, $button
} from "elemobj";

// [place, place_ar, gmt_offset, lat, lng]
const places = require("./places.json");

import {ripple_apply} from "./ripple";

import "./style.css";
import "./ripple.css";

const isdroid = typeof droid != "undefined";

// time utils =====

function remaining_time_human(time, time_name) {
    if (time == 0) {
        return strings.time_for + time_name;
    }

    let passed = false;
    if (time < 0) {
        passed = true;
        time *= -1;
    }

    let str_rem;
    let str_hr, str_hr_2, str_hr_many;
    let str_min, str_min_2, str_min_many;
    let str_and;

    let hours = (time / 60) | 0;
    let minutes = time % 60;

    let hours_str = hours.toString();
    let minutes_str = minutes.toString();

    let res;
    if (config.lang == "ar") {
        str_rem = passed ? "مضت " : "المتبقي ";
        str_hr = "ساعة";
        str_hr_2 = "ساعتان";
        str_hr_many = "ساعات";
        str_min = "دقيقة";
        str_min_2 = "دقيقتان";
        str_min_many = "دقائق";
        str_and = " و ";

        if (minutes > 10) {
            str_min_many = str_min;
        }
        if (hours > 10) {
            str_hr_many = str_hr;
        }
        if (minutes == 1) {
            str_min_many = str_min;
            minutes_str = "";
        }
        if (hours == 1) {
            str_hr_many = str_hr;
            hours_str = "";
        }
        if (minutes == 2) {
            str_min_many = str_min_2;
            minutes_str = "";
        }
        if (hours == 2) {
            str_hr_many = str_hr_2;
            hours_str = "";
        }
        if (minutes == 0) {
            str_min_many = minutes_str = str_and = "";
        }
        if (hours == 0) {
            str_hr_many = hours_str = str_and = "";
        }

        res = str_rem +
            hours_str +
            (hours_str == "" ? "" : " ") +
            str_hr_many + str_and + minutes_str +
            (minutes_str == "" ? "" : " ") +
            str_min_many;
    } else {
        str_rem = passed ? " passed" : " remaining";
        str_hr = str_hr_2 = str_hr_many = "hr";
        str_min = str_min_2 = str_min_many = "min";
        str_and = " and ";

        if (minutes == 0) {
            str_min_many = minutes_str = str_and = "";
        }
        if (hours == 0) {
            str_hr_many = hours_str = str_and = "";
        }

        res = hours_str +
            (hours_str == "" ? "" : " ") +
            str_hr_many + str_and + minutes_str +
            (minutes_str == "" ? "" : " ") +
            str_min_many +
            str_rem;
    }

    if (time_name.endsWith("iqama")) {
        res += " " + strings.to_iqama;
    }

    return res;
}

function time_human(date) {
    let h = date.getHours();
    let m = date.getMinutes();

    let suffix;
    if (config.time12h) {
        suffix = strings.am;
        if (h > 12) {
            suffix = strings.pm;
            h -= 12;
        }
    }

    if (h < 10) h = "0" + h;
    if (m < 10) m = "0" + m;

    let res = `${h}:${m}`;
    if (config.time12h)
        res += " " + suffix;
    return res;
}

// main items =====

function prayer_cards(times) {
    times = times || prayertimes(new Date(), config);
    let cards = {};
    let really_active;
    let real_next, next;
    let activate_timer;

    class PrayCard {
        constructor(times, i) {
            this.times = times;
            this.i = i;
            this.active = false;

            this.elem = $div({
                className: "button-card ripple",
                content: [
                    this.elem_name = $span({
                        style: {textAlign: "center", width: "50%", float: "left"}
                    }),
                    this.elem_time = $span({
                        style: {textAlign: "center", width: "50%", float: "right"}
                    }),
                    this.elem_rema = $span({
                        style: {textAlign: "center", width: "100%"}
                    })
                ],
                onclick: () => {
                    if (!this.active) {
                        this.activate();
                        if (activate_timer) {
                            clearTimeout(activate_timer);
                        }
                        activate_timer = setTimeout(() => {
                            if (really_active != undefined) {
                                cards[really_active].activate();
                            }
                        }, 5000);
                    }
                }
            });

            this.update();

            cards[i] = this;
        }

        update() {
            this.elem_name.innerText = strings[this.i];
            this.elem_time.innerText = time_human(this.times[this.i]);
            if (this.active) {
                let i = this.i == next ? real_next : this.i;
                this.elem_rema.innerText = remaining_time_human(
                    remaining_to(this.times, new Date(), i), i);
                this.elem.classList.add("active");
            } else {
                this.elem_rema.innerText = "";
                this.elem.classList.remove("active");
            }
        }

        activate() {
            if (this.active) return;
            for (let i in cards) {
                cards[i].deactivate();
            }
            this.active = true;
            this.update();
        }

        deactivate() {
            if (!this.active) return;
            this.active = false;
            this.update();
        }
    }

    let elem = $div();
    for (let i of TIMES_NAMES) {
        let c = new PrayCard(times, i);
        elem.appendChild(c.elem);
    }

    function activate(i) {
        really_active = i;
        cards[i].activate();
    }

    function update() {
        real_next = next_time(times);
        next = time_str_nearest(real_next);
        activate(next);
        for (let i in cards) {
            cards[i].update();
        }
    }

    update();
    setInterval(update, 30000);

    return elem;
}

function calendar() {
    let header_text;
    let calendar_table;
    let month_diff = 0;

    function set_day(diff) {
        if (diff == 0) {
            month_diff = 0;
        } else {
            month_diff += diff;
        }
        calendar_table.innerHTML = "";
        if (config.lang == "ar") {
            calendar_table.appendChild($tr([$td("أح"), $td("إث"),
                                            $td("ث"), $td("أر"),
                                            $td("خ"), $td("ج"),
                                            $td("س")]));
        } else {
            calendar_table.appendChild($tr([$td("S"), $td("M"),
                                            $td("T"), $td("W"),
                                            $td("T"), $td("F"),
                                            $td("S")]));
        }

        let now = new HijrahDate();
        let hdate = new HijrahDate();
        hdate.plusMonths(month_diff);
        let hdate_tmp = new HijrahDate(hdate.getFullYear(), hdate.getMonth(), 1);
        let weekday = hdate_tmp.getDay();
        let start_month = hdate_tmp.getMonth();

        header_text.innerText = hdate_tmp.format("MMMM yyyy", config.lang);

        let count = 0;
        let row, item;
        while (hdate_tmp.getMonth() == start_month) {
            if (count++ % 7 == 0) {
                calendar_table.appendChild(row = $tr());
            }

            if (weekday > 0) {
                row.appendChild($td());
                --weekday;
            } else {
                row.appendChild(item = $td({
                    className: "calendar-item ripple",
                    content: hdate_tmp.getDate().toString(),
                    onclick: event => {
                        let h = new HijrahDate(hdate.getFullYear(), hdate.getMonth(),
                                               event.target.innerText);
                        let g = h.toGregorian();
                        let title = h.format("dd MMMM yyyy", config.lang) +
                            "<br>" +
                            g.getDate() + " " +
                            g.toLocaleString(config.lang,
                                             { month: 'long' }) + " " +
                            g.getFullYear();
                        let times = prayertimes(g, config);

                        let times_table = [];
                        for (let i of TIMES_NAMES) {
                            times_table.push($tr([
                                $td({style: {width: "50%"}},
                                    strings[i]), $td(time_human(times[i]))
                            ]));
                        }

                        modal_show($div({
                            style: {textAlign: "center"}
                        }, [
                            $h3(title),
                            $hr(),
                            $table({
                                style: {
                                    margin: "auto",
                                    width:  "100%"
                                }
                            }, times_table)
                        ]));
                    }
                }));
                if (hdate_tmp.getFullYear() == now.getFullYear() &&
                    hdate_tmp.getMonth() == now.getMonth() &&
                    hdate_tmp.getDate() == now.getDate()) {
                    item.classList.add("active");
                }
                hdate_tmp.plusDays(1);
            }
        }

        try { ripple_apply(); } catch (e) {}
    }

    let elem = $div({
        className: "card",
        content: [
            $table({
                className: "calendar-header",
                content: [
                    $td({
                        style: {textAlign: "left"},
                        content: $span({
                            className: "calendar-item ripple",
                            content: '<i class="arrow-left"></i>'
                        }),
                        onclick: () => set_day(-1)
                    }),
                    $td(header_text = $span({
                        className: "calendar-item ripple",
                        onclick: () => set_day(0)
                    })),
                    $td({
                        style: {textAlign: "right"},
                        content: $span({
                            className: "calendar-item ripple",
                            content: '<i class="arrow-right"></i>'
                        }),
                        onclick: () => set_day(+1)
                    })
                ]
            }),

            $br(),

            calendar_table = $table({
                className: "calendar-table"
            })
        ]
    });

    set_day(0);

    return elem;
}

function qibla() {
    let qibla_text;
    let qibla_direction;
    let north_direction;

    let elem = $div({
        className: "card qibla",
        content: [
            qibla_text = $span({className: "qibla-text"}),

            qibla_direction = $div({
                className: "qibla-direction",
                content: '<i class="arrow-large"></i>'
            }),

            north_direction = $div({
                className: "north-direction",
                content: $div({
                    className: "north-text-container",
                    content: '<span class="north-text">N</span>'
                })
            }),

            $div({
                className: "big-circle",
                content: `<span class="big-circle-text">${strings.qibla}</span>`
            })
        ]
    });

    function qibla_from_north(lat2, lng2) {
        const lng1 = 21.4225, lat1 = 39.826181; // Kabba location
        const lngDelta = (lng2 - lng1);
        const y = Math.sin(lngDelta) * Math.cos(lat2);
        const x = Math.cos(lat1) * Math.sin(lat2) - Math.sin(lat1) * Math.cos(lat2) * Math.cos(lngDelta);
        let res = (Math.atan2(y, x) * 180) / Math.PI;
        if (res < 0) {
            res += 360;
        }
        return res;
    }

    let from_north = qibla_from_north(config.lat, config.lng);

    function set_north(dir) {
        north_direction.style.transform = `rotate(${dir}deg)`;
        qibla_direction.style.transform = `rotate(${dir + from_north}deg)`;
    }

    globalThis.set_north = set_north; // for droid
    set_north(0);
    qibla_text.innerText = strings.qibla_north.replace("%s", Math.round(from_north));

    return elem;
}

function modal() {
    // styles in css
    return $div({
        id: "modal",
        className: "modal",
        onclick: e => {
            if (e.target == e.currentTarget) {
                modal_hide();
            }
        }
    }, [
        $div({
            id: "modal-content",
            className: "card modal-content"
        }),

        /*$button({
          className: "button-card",
          style: {float: "right"},
          onclick: modal_hide
          }, strings.close)*/
    ]);
}

function modal_show(content, onhide) {
    const m = $get("#modal");
    const mcon = $get("#modal-content");

    mcon.innerHTML = "";
    mcon.appendChild($div(content));
    Object.assign(mcon, {
        dir: config.lang == "ar" ? "rtl" : "ltr",
        style: {
            textAlign: config.lang == "ar" ? "right" : "left"
        }
    });

    m.onhide = onhide;
    m.style.display = "block";
    document.body.style.overflow = "hidden";
}

function modal_hide() {
    const m = $get("#modal");
    const mcon = $get("#modal-content");

    if (m.onhide) m.onhide();
    m.onhide = undefined;
    m.style.display = "none";
    mcon.innerHTML = "";
    document.body.style.overflow = "auto";
}

function place_button() {
    return $div({
        className: "button-card ripple",
        content: strings.find_place,
        onclick: e => place_search_show()
    });
}

function settings_button() {
    return $div({
        className: "button-card ripple",
        content: strings.settings,
        onclick: e => settings_show()
    });
}

function settings_show() {
    modal_show(settings(), set_ui);
}

function settings() {

    function cfg_list(key, a_val, a, callback) {
        if (!Array.isArray(a_val)) {
            a_val = Object.keys(a_val);
        }
        if (!a) {
            a = a_val;
        }

        let selected = a_val.indexOf(config[key]);
        return $td($select({
            onchange: e => {
                config[key] = e.target.value;
                save_config();
                if (callback) callback(e);
            }
        }, (() => {
            let res = [];
            for (let i in a) {
                if (i == selected) {
                    res.push($option({value: a_val[i], selected: "selected"}, a[i]));
                } else {
                    res.push($option({value: a_val[i]}, a[i]));
                }
            }
            return res;
        })()));
    }

    function cfg_input(key) {
        return $td($input({
            type: "number",
            value: config[key],
            onchange: e => {
                config[key] = Number(e.target.value);
                save_config();
            }
        }));
    }

    const $line = () => $tr($td({colSpan: 2}, $hr()));

    return [
        $table({
            style: {margin: "auto"}
        }, [
            $tr([$td(strings.language), cfg_list("lang",
                                                 ["ar", "en"],
                                                 ["عربي", "English"],
                                                 () => {
                                                     set_ui();
                                                     modal_hide();
                                                 })]),

            $tr([$td(strings.theme), cfg_list("theme",
                                              ["auto", "dark", "light"],
                                              strings.list_themes,
                                              update_theme)]),

            $line(),

            $tr([$td(strings.find_place), $td($button({
                style: {width: "100%"},
                onclick: e => modal_show(place_search())
            }, strings.search))]),
            $tr([$td(strings.lat),      cfg_input("lat")]),
            $tr([$td(strings.lng),      cfg_input("lng")]),
            $tr([$td(strings.timezone), cfg_input("time_zone")]),

            $line(),

            $tr([$td(strings.calc_method), cfg_list("calc_method",
                                                    calc_methods,
                                                    strings.list_calc_methods,
                                                    set_ui)]),

            $tr([$td(strings.asr_method), cfg_list("asr_juristic",
                                                   asr_methods,
                                                   strings.list_asr_methods,
                                                   set_ui)]),

            $tr([$td(strings.high_lat_method), cfg_list("adjust_high_lats",
                                                        high_lat_methods,
                                                        strings.list_high_lat_methods,
                                                        set_ui)])
        ])
    ];
}

function set_place_obj(p) {
    config.time_zone = p[2];
    config.lat       = p[3];
    config.lng       = p[4];

    switch (p[0]) {
    case "Cairo":   config.calc_method = "egypt";          break;
    case "Amman":   config.calc_method = "jordan";         break;
    case "Karachi": config.calc_method = "karachi";        break;
    case "Doha":    config.calc_method = "qatar";          break;
    case "Kuwait":  config.calc_method = "kuwait";         break;
    case "London":  config.calc_method = "england_london"; break;
    case "Mecca":   config.calc_method = "makkah";         break;
    default:        config.calc_method = "makkah";         break;
    }

    let hist = config.history;
    hist = hist ? hist.split("\n") : [];
    hist.unshift(p[0]);
    hist = hist.slice(0, 5);
    hist = hist.filter((v, i, a) => a.indexOf(v) === i);
    config.history = hist.join("\n");

    config.first_welcome = false;
    save_config();
    set_ui();
}

function place_name_to_obj(place) {
    for (let p of places) {
        if (place == p[0]) {
            return p;
        }
    }
    return undefined;
}

function set_place(place) {
    set_place_obj(place_name_to_obj(place));
}

function place_search_show() {
    modal_show(place_search());
    sleep(500).then(() =>
        $get("#place-searchbar").focus());
}

function place_search() {
    return $div([
        $input({
            id: "place-searchbar",
            type: "text",
            placeholder: strings.find_place,
            style: {width: "97%"},
            autofocus: true,
            tabIndex: 0,

            oninput: e => {
                const results_div = $get("#place-results");
                let results = [];
                const input = e.target.value.toLowerCase();
                results_div.innerHTML = "";
                if (input.length > 0) {
                    const results = [];
                    for (let p of places) {
                        if (input == p[0].slice(0, input.length)
                            .toLowerCase() ||
                            input == p[1].slice(0, input.length)) {
                            results.push(p);
                        }
                    }
                    for (let p of places) {
                        if ((p[0].toLowerCase().includes(input) ||
                             p[1].includes(input)) &&
                            !results.includes(p)) {
                            results.push(p);
                        }
                    }
                    const lang_index = config.lang == "ar" ? 1 : 0;
                    for (let i = 0; i < results.length && i < 5; i++) {
                        results_div.appendChild($div({
                            onclick: e => {
                                set_place(results[i][0]);
                                modal_hide();
                            },
                            style: {width: "100%"}
                        }, results[i][lang_index]));
                        results_div.appendChild($hr());
                    }
                }
            },

            onkeyup: e => {
                if (e.code == "Enter") {
                    $get("#place-results").children[0].click();
                }
            }
        }),

        $div({
            id: "place-results",
            style: {marginTop: "10px"}
        })
    ]);
}

function history() {
    let res = [];
    let hist = config.history;

    if (hist) {
        hist = hist.split("\n");
        for (let place of hist) {
            let p = place_name_to_obj(place);
            res.push($div({
                className: "button-card ripple",
                content: p[config.lang == "ar" ? 1 : 0],
                onclick: () => set_place(p[0])
            }));
        }
    }
    return $div(res);
}

// ui utils =====

function update_theme() {
    let light = false;

    if (config.theme == "auto") {
        if (isdroid) {
            light = droid.getTheme() == "light";
        } else {
            light = window.matchMedia &&
                window.matchMedia('(prefers-color-scheme: light)').matches;
        }
    } else {
        light = config.theme == "light";
    }

    if (light) {
        document.body.classList.add("light");
    } else {
        document.body.classList.remove("light");
    }

    theme_droid();
}

function theme_droid() {
    if (isdroid) {
        let s = getComputedStyle(document.body);
        let bg = s.getPropertyValue("--window-background").trim();
        let fg = s.getPropertyValue("--base-foreground").trim();
        let light = document.body.classList.contains("light");
        droid.theme(light, bg, fg);
    }
}

function dialog(title, str) {
    if (!str) str = "";
    if (isdroid) {
        droid.dialog(title, str);
    } else {
        alert(title  + "\n\n" + str);
    }
}

// utils =====

async function sleep(time) {
    return new Promise(resolve => setTimeout(resolve, time));
}

function run_at(hh, mm, callback) {
    // https://stackoverflow.com/a/25492756/3825872
    var interval = 0;
    var today = new Date();
    var today_hh = today.getHours();
    var today_mm = today.getMinutes();
    if ((today_hh > hh) || (today_hh == hh && today_mm > mm)) {
        var midnight = new Date();
        midnight.setHours(24,0,0,0);
        interval = midnight.getTime() - today.getTime() +
            (hh * 60 * 60 * 1000) + (mm * 60 * 1000);
    } else {
        interval = (hh - today_hh) * 60 * 60 * 1000 + (mm - today_mm) * 60 * 1000;
    }
    return setTimeout(() => {
        if (interval != 0) {
            callback();
        }
    }, interval);
}

// main =====

function set_strings() {
    if (config.lang in strings_map) {
        strings = strings_map[config.lang];
    } else {
        strings = strings_map.en;
    }

    if (isdroid) {
        droid.setStrings(JSON.stringify(strings));
    }
}

function set_ui() {
    set_strings();

    const views = [
        prayer_cards(),
        calendar(),
        qibla(),
        history(),
        place_button(),
        settings_button()
    ];

    const cont = $get("#main-view-items");
    cont.innerHTML = "";
    for (let e of views)
        cont.appendChild(e);
    update_theme();
    try { ripple_apply(); } catch (e) {}
}

async function main() {
    load_config();

    await pt_init();

    document.body.innerHTML = "";
    document.body.appendChild($div({className: "main-container"}, [
        $div({id: "main-view-items"}),
        modal()
    ]));
    modal_hide();
    set_ui();

    if (window.matchMedia) {
        window.matchMedia('(prefers-color-scheme: light)')
            .addEventListener('change', update_theme);
    }

    // update at 12:00 am
    run_at(0, 0, set_ui);

    window.addEventListener("keyup", e => {
        if ($get("#modal").style.display != "none") {
            switch (e.code) {
            case "Escape": modal_hide(); break;
            }
        } else {
            switch (e.code) {
            case "KeyF": place_search_show(); break;
            case "KeyS": settings_show();     break;
            }
        }
    });

    if (config.first_welcome) {
        place_search_show();
        config.first_welcome = false;
    }

    // for droid
    Object.assign(globalThis, {
        place_search_show, settings_show, config
    });
}

main();
