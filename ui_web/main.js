import HijrahDate from "hijrah-date";

import {
    pt_init, prayertimes, next_time,
    remaining_to, time_str_nearest, TIMES_NAMES,
    calc_methods, asr_methods, high_lat_methods
} from "./pt_iface";

import {
    config, load_config, save_config
} from "./configs";

import {
    $div, $span, $table, $tr, $td, $br, $hr,
    $input, $select, $option, $get
} from "elemobj";

import {ripple_apply} from "./ripple";

import "./style.css";
import "./ripple.css";

const time_name = i =>
      (config.lang == "ar" ?
       {
           fajr: "الفجر",
           sunrise: "الشروق",
           duhr: "الظهر",
           asr: "العصر",
           magrib: "المغرب",
           isha: "العشاء"
       } : {
           fajr: "Fajr",
           sunrise: "Sunrise",
           duhr: "Duhr",
           asr: "Asr",
           magrib: "Magrib",
           isha: "Isha"
       })[i];

function remaining_time_human(time, time_name) {
    if (time == 0) {
        if (config.lang == "ar") {
            return "حان وقت " + time_name;
        } else {
            return "it's time for " + time_name;
        }
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
        if (config.lang == "ar") {
            res += "  للإقامة";
        } else {
            res += " to iqama";
        }
    }

    return res;
}

function time_human(date) {
    let h = date.getHours();
    let m = date.getMinutes();

    let suffix;
    if (config.time12h) {
        suffix = config.lang == "ar" ? "ص" : "AM";
        if (h > 12) {
            suffix = config.lang == "ar" ? "م" : "PM";
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
            this.elem_name.innerText = time_name(this.i);
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
                        let title = h.format("dd MMMM yyyy", config.lang) + "\n" +
                            g.getDate() + " " +
                            g.toLocaleString(config.lang,
                                             { month: 'long' }) + " " +
                            g.getFullYear();
                        let times = prayertimes(g);
                        let msg = "";
                        for (let i of TIMES_NAMES) {
                            msg += `${time_name(i)}: ${time_human(times[i])}\n`;
                        }
                        dialog(title, msg);
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
    let str_qibla, str_from_north;
    if (config.lang == "ar") {
        str_qibla = "القبلة";
        str_from_north = "القبلة على زاوية %s° من الشمال";
    } else {
        str_qibla = "QIBLA";
        str_from_north = "Qibla is %s° from north";
    }

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
                content: `<span class="big-circle-text">${str_qibla}</span>`
            })
        ]
    });

    function qibla_from_north(lat2, lng2) {
        let lng1 = 21.4225, lat1 = 39.826181; // Kabba location
        let lngDelta = (lng2 - lng1);
        let y = Math.sin(lngDelta) * Math.cos(lat2);
        let x = Math.cos(lat1) * Math.sin(lat2) - Math.sin(lat1) * Math.cos(lat2) * Math.cos(lngDelta);
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

    globalThis.set_north = set_north;
    set_north(0);
    qibla_text.innerText = str_from_north.replace("%s", Math.round(from_north));

    return elem;
}

function config_ui() {

    function cfg_list(key, a, callback) {
        if (!Array.isArray(a)) {
            a = Object.keys(a);
        }
        let selected = a.indexOf(config[key]);
        return $td($select({
            onchange: e => {
                config[key] = e.target.value;
                save_config();
                if (callback) callback(e);
                main();
            }
        }, (() => {
            let res = [];
            for (let i in a) {
                if (i == selected) {
                    res.push($option({selected: "selected"}, a[i]));
                } else {
                    res.push($option(a[i]));
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
        $div({
            className: "button-card ripple",
            content: config.lang == "ar" ? "الإعدادات" : "Settings",
            onclick: e => {
                let elem = $get("#config");
                if (elem.style.display == "none") {
                    elem.style.display = "block";
                    window.scrollTo(0, document.body.scrollHeight);
                } else {
                    elem.style.display = "none";
                }
            }
        }),

        $table({
            id: "config",
            style: {display: "none", width: "100%"}
        }, [
            $line(),

            $tr([$td("Language"), cfg_list("lang", ["ar", "en"])]),
            $tr([$td("Theme"), cfg_list("theme", ["auto", "dark", "light"], update_theme)]),

            $line(),

            $tr([$td("Latitude"),  cfg_input("lat")]),
            $tr([$td("Longitude"), cfg_input("lng")]),
            $tr([$td("Timezone"),  cfg_input("time_zone")]),

            $line(),

            $tr([$td("Calculation method"),   cfg_list("calc_method",      calc_methods)]),
            $tr([$td("Asr method"),           cfg_list("asr_juristic",     asr_methods)]),
            $tr([$td("High latitude method"), cfg_list("adjust_high_lats", high_lat_methods)])
        ])
    ];
}

function deadbeef() {
    return $div({
        className: "button-card ripple",
        content: "CLICK ME",
        onclick: () => dialog("deadbeef")
    });
}

function special_case() {
    function $thing(txt, cfg) {
        return $div({
            className: "button-card ripple",
            content: txt,
            onclick: () => {
                Object.assign(config, cfg);
                save_config();
                main();
            }
        });
    }

    return [
        $thing(config.lang == "ar" ? "الدوحة" : "Doha", {
            calc_method:  "qatar",
            asr_juristic: "shafii",
            lat:           25.411720,
            lng:           51.503878,
            time_zone:     3.0
        }),
        $thing(config.lang == "ar" ? "بغداد" : "Baghdad", {
            calc_method:  "makkah",
            asr_juristic: "shafii",
            lat:           33.3118944,
            lng:           44.2158181,
            time_zone:     3.0
        })
    ];
}

// ui utils =====

function update_theme() {
    let light = false;

    if (config.theme == "auto") {
        if (typeof droid != "undefined") {
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
    if (typeof droid != "undefined") {
        let s = getComputedStyle(document.body);
        let bg = s.getPropertyValue("--window-background").trim();
        let fg = s.getPropertyValue("--base-foreground").trim();
        let light = document.body.classList.contains("light");
        droid.theme(light, bg, fg);
    }
}

function dialog(title, str) {
    if (!str) str = "";
    if (typeof droid != "undefined") {
        droid.dialog(title, str);
    } else {
        alert(title  + "\n\n" + str);
    }
}

// utils =====

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

async function main() {
    load_config();
    await pt_init();
    const times = prayertimes(new Date(), config);

    document.body.innerHTML = "";
    document.body.appendChild($div({className: "main-container"}, [
        prayer_cards(times),
        calendar(),
        qibla(),
        ...special_case(),
        ...config_ui()
    ]));

    update_theme();
    if (window.matchMedia) {
        window.matchMedia('(prefers-color-scheme: light)')
            .addEventListener('change', update_theme);
    }

    // re-run at 12:00 am
    run_at(0, 0, main);

    try { ripple_apply(); } catch (e) {}
}

main();
