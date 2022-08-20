import {
    default_config,
    calc_methods, asr_methods, high_lat_methods
} from "./pt_iface";

const KEY = "prayer-config";

export let config = {
    first_time: true,
    first_welcome: true,
    lang: "en",
    time12h: true,
    theme: "auto",
    history: "",

    calc_method: "qatar",
    asr_juristic: "shafii",
    adjust_high_lats: "angle_based",

    lat: 25.411720,
    lng: 51.503878,
    time_zone: 3.0
};

export function auto_config() {
    return;
    // TODO
    config.time_zone = new Date().getTimezoneOffset() / -60;
    if (navigator.geolocation) {
        navigator.geolocation.getCurrentPosition(position => {
            config.lat = position.coords.latitude;
            config.lng = position.coords.longitude;
        });
    }
}

export function load_config() {
    if (typeof droid != "undefined") {
        Object.assign(config, JSON.parse(droid.prefsLoad()));
    } else if (window.localStorage) {
        Object.assign(config, JSON.parse(localStorage.getItem(KEY)));
    } else {
        console.error("Can't save config");
    }

    if (config.first_time) {
        auto_config();
        config.first_time = false;
        save_config();
    }
}

export function save_config() {
    if (typeof droid != "undefined") {
        droid.prefsSave(JSON.stringify(config));
    } else if (window.localStorage) {
        localStorage.setItem(KEY, JSON.stringify(config));
    } else {
        console.error("Can't save config");
    }
}
