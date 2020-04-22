var __pt_opts = {
    calc_method: pt.QATAR,
    fajr_angle: 18,
    magrib_is_minuets: true,
    magrib_val: 0,
    isha_is_minuets: true,
    isha_val: 90,
    asr_juristic: pt.SHAFII,
    dhuhr_minutes: 0,
    adjust_high_lats: pt.ANGLE_BASED,
    lat: 25.2899589,
    lng: 51.4974742,
    time_zone: 3,
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

var __old_pt_set_opts = pt.set_opts;

pt.set_opts = function(opts) {
    if (opts.calc_method) __pt_opts.calc_method = opts.calc_method;
    if (opts.fajr_angle) __pt_opts.fajr_angle = opts.fajr_angle;
    if (opts.magrib_is_minuets) __pt_opts.magrib_is_minuets = opts.magrib_is_minuets;
    if (opts.magrib_val) __pt_opts.magrib_val = opts.magrib_val;
    if (opts.isha_is_minuets) __pt_opts.isha_is_minuets = opts.isha_is_minuets;
    if (opts.isha_val) __pt_opts.isha_val = opts.isha_val;
    if (opts.asr_juristic) __pt_opts.asr_juristic = opts.asr_juristic;
    if (opts.dhuhr_minutes) __pt_opts.dhuhr_minutes = opts.dhuhr_minutes;
    if (opts.adjust_high_lats) __pt_opts.adjust_high_lats = opts.adjust_high_lats;
    if (opts.lat) __pt_opts.lat = opts.lat;
    if (opts.lng) __pt_opts.lng = opts.lng;
    if (opts.time_zone) __pt_opts.time_zone = opts.time_zone;
    if (opts.offset_fajr) __pt_opts.offset_fajr = opts.offset_fajr;
    if (opts.offset_sunrise) __pt_opts.offset_sunrise = opts.offset_sunrise;
    if (opts.offset_duhr) __pt_opts.offset_duhr = opts.offset_duhr;
    if (opts.offset_asr) __pt_opts.offset_asr = opts.offset_asr;
    if (opts.offset_sunset) __pt_opts.offset_sunset = opts.offset_sunset;
    if (opts.offset_magrib) __pt_opts.offset_magrib = opts.offset_magrib;
    if (opts.offset_isha) __pt_opts.offset_isha = opts.offset_isha;
    if (opts.iqama_fajr) __pt_opts.iqama_fajr = opts.iqama_fajr;
    if (opts.iqama_duhr) __pt_opts.iqama_duhr = opts.iqama_duhr;
    if (opts.iqama_asr) __pt_opts.iqama_asr = opts.iqama_asr;
    if (opts.iqama_magrib) __pt_opts.iqama_magrib = opts.iqama_magrib;
    if (opts.iqama_isha) __pt_opts.iqama_isha = opts.iqama_isha;
    __old_pt_set_opts(
        __pt_opts.calc_method,
        __pt_opts.fajr_angle,
        __pt_opts.magrib_is_minuets,
        __pt_opts.magrib_val,
        __pt_opts.isha_is_minuets,
        __pt_opts.isha_val,
        __pt_opts.asr_juristic,
        __pt_opts.dhuhr_minutes,
        __pt_opts.adjust_high_lats,
        __pt_opts.lat,
        __pt_opts.lng,
        __pt_opts.time_zone,
        __pt_opts.offset_fajr,
        __pt_opts.offset_sunrise,
        __pt_opts.offset_duhr,
        __pt_opts.offset_asr,
        __pt_opts.offset_sunset,
        __pt_opts.offset_magrib,
        __pt_opts.offset_isha,
        __pt_opts.iqama_fajr,
        __pt_opts.iqama_duhr,
        __pt_opts.iqama_asr,
        __pt_opts.iqama_magrib,
        __pt_opts.iqama_isha
    );
};

var TIMES_NAMES = [
    "FAJR",
    "FAJR_IQAMA",
    "SUNRISE",
    "DUHR",
    "DUHR_IQAMA",
    "ASR",
    "ASR_IQAMA",
    "SUNSET",
    "MAGRIB",
    "MAGRIB_IQAMA",
    "ISHA",
    "ISHA_IQAMA"
];

var p = pt.prayer_time_new();
pt.calculate_for(p, 2019, 7, 30);

for (var pray of [pt.FAJR, pt.SUNRISE, pt.DUHR, pt.ASR, pt.MAGRIB, pt.ISHA]) {
    console.log(TIMES_NAMES[pray] + "\t" + pt.get_time(p, pray));
}

