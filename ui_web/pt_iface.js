import {pt_asm} from "../build/prayertimes.wasm.js";

let pt_asm_exports = undefined;

/* Times Indices --- */
const FAJR         =  0;
const FAJR_IQAMA   =  1;
const SUNRISE      =  2;
const DUHR         =  3;
const DUHR_IQAMA   =  4;
const ASR          =  5;
const ASR_IQAMA    =  6;
const SUNSET       =  7;
const MAGRIB       =  8;
const MAGRIB_IQAMA =  9;
const ISHA         = 10;
const ISHA_IQAMA   = 11;
const TIMES_LEN    = 12;

const TIMES = [FAJR, SUNRISE, DUHR, ASR, MAGRIB, ISHA];
export const TIMES_NAMES = ["fajr", "sunrise", "duhr", "asr", "magrib", "isha"];

/* Calculation Methods --- */
const CUSTOM             =  0; /* Custom Setting */
const MAKKAH             =  1; /* Umm al-Qura, Makkah */
const EGYPT              =  2; /* Egyptian General Authority of Survey */
const QATAR              =  3; /* Qatar Calendar House */
const KARACHI            =  4; /* University of Islamic Sciences, Karachi */
const ISNA               =  5; /* Islamic Society of North America (ISNA) */
const MWL                =  6; /* Muslim World League (MWL) */
const ALGERIA            =  7; /* Algeria, Ministry of Religious Affairs and Endowments */
const JORDAN             =  8; /* Jordan, Ministry of Awqaf, Islamic Affairs and Holy Places */
const KUWAIT             =  9; /* Kuwait, Ministry of Awqaf and Islamic Affairs */
const ENGLAND_BIRMINGHAM = 10; /* United Kingdom, Birmingham Central Mosque */
const ENGLAND_LONDON     = 11; /* United Kingdom, London Central Mosque */
const GERMANY_MUNCHEN    = 12; /* Germany, Islamic Center of Munich */
const GERMANY_AACHEN     = 13; /* Germany, Islamic Center of Aachen */

/* Juristic Methods --- */
const SHAFII = 0; /* Shafii (standard) */
const HANAFI = 1; /* Hanafi */

/* Adjusting Methods for Higher Latitudes --- */
const NONE        = 0; /* No adjustment */
const MIDNIGHT    = 1; /* middle of night */
const ONE_SEVENTH = 2; /* 1/7th of night */
const ANGLE_BASED = 3; /* angle/60th of night */

/* Exported consts --- */
// these are not defaults as it seems to have a noticeable effect
// on performance

export const names = {
    fajr:         FAJR,
    fajr_iqama:   FAJR_IQAMA,
    sunrise:      SUNRISE,
    duhr:         DUHR,
    duhr_iqama:   DUHR_IQAMA,
    asr:          ASR,
    asr_iqama:    ASR_IQAMA,
    sunset:       SUNSET,
    magrib:       MAGRIB,
    magrib_iqama: MAGRIB_IQAMA,
    isha:         ISHA,
    isha_iqama:   ISHA_IQAMA,
};

export const calc_methods = {
    custom:             CUSTOM,
    makkah:             MAKKAH,
    egypt:              EGYPT,
    qatar:              QATAR,
    karachi:            KARACHI,
    isna:               ISNA,
    mwl:                MWL,
    algeria:            ALGERIA,
    jordan:             JORDAN,
    kuwait:             KUWAIT,
    england_birmingham: ENGLAND_BIRMINGHAM,
    england_london:     ENGLAND_LONDON,
    germany_munchen:    GERMANY_MUNCHEN,
    germany_aachen:     GERMANY_AACHEN
};

export const asr_methods = {
    shafii: SHAFII,
    hanafi: HANAFI
};

export const high_lat_methods = {
    none:        NONE,       
    midnight:    MIDNIGHT,   
    one_seventh: ONE_SEVENTH,
    angle_based: ANGLE_BASED
};

export const default_config = {
    calc_method: "qatar",
    fajr_angle: -1,
    magrib_is_minuets: false,
    magrib_val: -1,
    isha_is_minuets: false,
    isha_val: -1,

    asr_juristic: "shafii",
    dhuhr_minutes: 0,
    adjust_high_lats: "angle_based",

    lat: 25.411720,
    lng: 51.503878,
    time_zone: 3.0,

    offset_fajr: 0,
    offset_sunrise: 0,
    offset_duhr: 0,
    offset_asr: 0,
    offset_sunset: 0,
    offset_magrib: 0,
    offset_isha: 0,

    iqama_fajr: 25,
    iqama_duhr: 20,
    iqama_asr: 25,
    iqama_magrib: 10,
    iqama_isha: 20
};

/* Helpers --- */

function time_str_to_index(time_str) {
    switch (time_str) {
    case "fajr":         return FAJR;
    case "fajr_iqama":   return FAJR_IQAMA;
    case "sunrise":      return SUNRISE;
    case "duhr":         return DUHR;
    case "duhr_iqama":   return DUHR_IQAMA;
    case "asr":          return ASR;
    case "asr_iqama":    return ASR_IQAMA;
    case "sunset":       return SUNSET;
    case "magrib":       return MAGRIB;
    case "magrib_iqama": return MAGRIB_IQAMA;
    case "isha":         return ISHA;
    case "isha_iqama":   return ISHA_IQAMA;
    default: throw new Error(`Unknown time ${time_str}`);
    }
}

function index_to_time_str(index) {
    switch (index) {
    case FAJR:         return "fajr";
    case FAJR_IQAMA:   return "fajr_iqama";
    case SUNRISE:      return "sunrise";
    case DUHR:         return "duhr";
    case DUHR_IQAMA:   return "duhr_iqama";
    case ASR:          return "asr";
    case ASR_IQAMA:    return "asr_iqama";
    case SUNSET:       return "sunset";
    case MAGRIB:       return "magrib";
    case MAGRIB_IQAMA: return "magrib_iqama";
    case ISHA:         return "isha";
    case ISHA_IQAMA:   return "isha_iqama";
    default: throw new Error(`Unknown index ${index}`);
    }
}

export function time_str_nearest(time_str) {
    switch (time_str) {
    case "fajr":         return "fajr";
    case "fajr_iqama":   return "fajr";
    case "sunrise":      return "sunrise";
    case "duhr":         return "duhr";
    case "duhr_iqama":   return "duhr";
    case "asr":          return "asr";
    case "asr_iqama":    return "asr";
    case "sunset":       return "magrib";
    case "magrib":       return "magrib";
    case "magrib_iqama": return "magrib";
    case "isha":         return "isha";
    case "isha_iqama":   return "isha";
    default: throw new Error(`Unknown time ${time_str}`);
    }
}

export async function pt_init() {
    // pt_asm is auto generated
    const pt_codearray = new Uint8Array(pt_asm.length);
    for (let i in pt_asm) pt_codearray[i] = pt_asm.charCodeAt(i);
    const wasm = await WebAssembly.instantiate(pt_codearray);
    pt_asm_exports = wasm.instance.exports;
    // TODO: add android handler
}

function short_time_to_obj(stime) {
    return {
        h: (stime / 60) | 0,
        m: stime % 60
    };
}

function short_time_to_date(stime, from_date) {
    let d = from_date ? new Date(from_date) : new Date();
    d.setHours((stime / 60) | 0);
    d.setMinutes(stime % 60);
    d.setSeconds(0);
    return d;
}

function date_to_short_time(date) {
    return date.getHours() * 60 +
        date.getMinutes();
}

/* Main interface --- */

export function prayertimes(date, opts) {
    date = date || new Date();
    opts = opts || {};

    let {memory, pt_full} = pt_asm_exports;

    let times_buf = new Uint16Array(memory.buffer, 0, TIMES_LEN);
    let year      = date.getFullYear();
    let month     = date.getMonth() + 1;
    let day       = date.getDate();

    let args = {};
    Object.assign(args, default_config);
    Object.assign(args, opts);

    if (typeof(args.calc_method) == "string")
        args.calc_method = calc_methods[args.calc_method];
    if (typeof(args.asr_juristic) == "string")
        args.asr_juristic = asr_methods[args.asr_juristic];
    if (typeof(args.adjust_high_lats) == "string")
        args.adjust_high_lats = high_lat_methods[args.adjust_high_lats];

    pt_full(times_buf, year, month, day,
            args.calc_method,
            args.fajr_angle,
            args.magrib_is_minuets,
            args.magrib_val,
            args.isha_is_minuets,
            args.isha_val,
            args.asr_juristic,
            args.dhuhr_minutes,
            args.adjust_high_lats,
            args.lat,
            args.lng,
            args.time_zone,
            args.offset_fajr,
            args.offset_sunrise,
            args.offset_duhr,
            args.offset_asr,
            args.offset_sunset,
            args.offset_magrib,
            args.offset_isha,
            args.iqama_fajr,
            args.iqama_duhr,
            args.iqama_asr,
            args.iqama_magrib,
            args.iqama_isha);

    let res = {times_buf};
    for (let i = 0; i < TIMES_LEN; ++i) {
        res[index_to_time_str(i)] =
            short_time_to_date(times_buf[i], date);
    }
    return res;
}

export function next_time(times, now) {
    let {next_time} = pt_asm_exports;
    now = date_to_short_time(now || new Date());
    let next_index = next_time(times.times_buf, now);
    return index_to_time_str(next_index);
}

export function remaining_to(times, now, target_time) {
    let {remaining_to} = pt_asm_exports;
    now = now || new Date();
    target_time = target_time || next_time(times, now);
    target_time = time_str_to_index(target_time);
    now = date_to_short_time(now);
    let rem = remaining_to(times.times_buf, now, target_time);
    return rem;
}
