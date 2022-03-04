import HijrahDate from "hijrah-date";

import {
    pt_asm_init, prayertimes, next_time,
    remaining_to, time_str_nearest, TIMES_NAMES
} from "./pt_iface";

import {
    $div, $span, $table, $tr, $td, $br, $hr
} from "elemobj";

import {ripple_apply} from "./ripple";

import "./style.css";
import "./ripple.css";

// setup =====
// TODO: build real setup

const config = {
    lat: 25.411720,
    lng: 51.503878,
    timezone: 3.0,
    lang: "en",
    time12h: true
};

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

    if (config.lang == "ar") {
        str_rem = passed ? "مضى " : "المتبقي ";
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
    } else {
        str_rem = passed ? "Passed " : "Remaining ";
        str_hr = str_hr_2 = str_hr_many = "hr";
        str_min = str_min_2 = str_min_many = "min";
        str_and = " and ";

        if (minutes == 0) {
            str_min_many = minutes_str = str_and = "";
        }
        if (hours == 0) {
            str_hr_many = hours_str = str_and = "";
        }
    }

    let res = str_rem +
        hours_str +
        (hours_str == "" ? "" : " ") +
        str_hr_many + str_and + minutes_str +
        (minutes_str == "" ? "" : " ") +
        str_min_many;

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
            this.elem_time.innerText = time_human(times[this.i]);
            if (this.active) {
                let i = this.i == next ? real_next : this.i;
                this.elem_rema.innerText = remaining_time_human(
                    remaining_to(times, new Date(), i), i);
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
                        // TODO: fix this
                        let h = new HijrahDate(hdate.getFullYear(), hdate.getMonth(),
                                               event.target.innerText);
                        let g = h.toGregorian();
                        let str = h.format("dd MMMM yyyy", config.lang) + "\n" +
                            g.getDate() + " " +
                            g.toLocaleString(config.lang,
                                             { month: 'long' }) + " " +
                            g.getFullYear() + "\n\n";
                        let times = prayertimes(g);
                        for (let i of TIMES_NAMES) {
                            str += `${time_name(i)}: ${time_human(times[i])}\n`;
                        }
                        toast(str);
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

function deadbeef() {
    return $div({
        className: "button-card ripple",
        content: "CLICK ME",
        onclick: () => toast("deadbeef")
    });
}

function theme_btn() {
    return $div({
        className: "button-card ripple",
        content: "TOGGLE THEME",
        onclick: () => {
            document.body.classList.toggle("light");
            theme_droid();
        }
    });
}

function lang_btn() {
    return $div({
        className: "button-card ripple",
        content: "TOGGLE LANG",
        onclick: () => {
            config.lang = config.lang == "ar" ? "en" : "ar";
            main();
        }
    });
}

function reload_btn() {
    return $div({
        className: "button-card ripple",
        content: "RELOAD",
        onclick: main
    });
}

function loc_btn() {
    // TODO
    /*let whatever = document.getElementById("whatever");
    return $div({
        className: "button-card ripple",
        content: "LOCATION TEST",
        onclick: () => {
            if (navigator.geolocation) {
                navigator.geolocation.watchPosition(data => {
                    console.log(data);
                    whatever.innerText = JSON.stringify({
                        accuracy: data.coords.accuracy,
                        accuracy: data.coords.accuracy,
                        altitude: data.coords.altitude,
                        altitudeAccuracy: data.coords.altitudeAccuracy,
                        heading: data.coords.heading,
                        latitude: data.coords.latitude,
                        longitude: data.coords.longitude,
                        speed: data.coords.speed
                    }, null, 2);
                }, err => {
                    if (err.code == 1) {
                        alert("Error: Access is denied!");
                    } else if (err.code == 2) {
                        alert("Error: Position is unavailable!");
                    }
                }, {
                    timeout: 60000
                });
            } else {
                alert("Sorry, browser does not support geolocation!");
            }
        }
    });*/
}

// ui utils =====

function theme_droid() {
    if (typeof droid != "undefined") {
        let s = getComputedStyle(document.body);
        let bg = s.getPropertyValue("--window-background").trim();
        let fg = s.getPropertyValue("--base-foreground").trim();
        let light = document.body.classList.contains("light");
        droid.theme(light, bg, fg);
    }
}

function toast(str) {
    if (typeof droid != "undefined") {
        droid.toast(str);
    } else {
        alert(str);
    }
}

// main =====

async function main() {
    await pt_asm_init();
    const times = prayertimes(new Date(), undefined); // TODO: add configs

    document.body.innerHTML = "";
    document.body.appendChild($div({className: "main-container"}, [
        prayer_cards(times),
        calendar(),
        qibla(),
        $hr(),
        deadbeef(),
        theme_btn(),
        lang_btn(),
        reload_btn()
    ]));
    theme_droid();

    try { ripple_apply(); } catch (e) {}
}

main();
