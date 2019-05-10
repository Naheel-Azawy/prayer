//using Prefs;

// Time Helper

function sec2TimeArr(timeInSec) {
    var s = timeInSec % 60;
    var totalMinutes = Math.floor(timeInSec / 60);
    var m = totalMinutes % 60;
    var h = Math.floor(totalMinutes / 60);
    return new Array(s, m, h);
}

function time2Sec(time) {
    var arr = time.split(":");
    return parseInt(arr[0]) * 3600 + parseInt(arr[1]) * 60;
}

function sec2Time(sec) {
    var arr = sec2TimeArr(sec);
    return ((arr[2] < 10) ? "0" + arr[2] : arr[2]) +
        ":" + ((arr[1] < 10) ? "0" + arr[1] : arr[1]);
}

function timeStr2Arr(time) {
    return sec2TimeArr(time2Sec(time));
}

function secondsToTimeInWords(time) {

    var stringHour = "ساعة";
    var stringHours = "ساعات";
    var string2Hours = "ساعتان";

    var stringHourEn = "hr";

    var stringMinute = "دقيقة";
    var stringMinutes = "دقائق";
    var string2Minutes = "دقيقتان";

    var stringMinuteEn = "min";

    var and = " و ";
    var andEn = " and ";

    var hours = Math.floor(time / 3600);
    time = time - (hours * 3600);
    var minutes = Math.floor(time / 60);

    var hoursstring = hours;
    var minutesstring = minutes;

    if (P.isAr) {
        if (minutes > 10) {
            stringMinutes = stringMinute;
        }
        if (hours > 10) {
            stringHours = stringHour;
        }
        if (minutes == 1) {
            stringMinutes = stringMinute;
            minutesstring = "";
        }
        if (hours == 1) {
            stringHours = stringHour;
            hoursstring = "";
        }
        if (minutes == 2) {
            stringMinutes = string2Minutes;
            minutesstring = "";
        }
        if (hours == 2) {
            stringHours = string2Hours;
            hoursstring = "";
        }
        if (minutes == 0) {
            stringMinutes = "";
            minutesstring = "";
            and = "";
        }
        if (hours == 0) {
            stringHours = "";
            hoursstring = "";
            and = "";
        }
    } else {

        stringHours = stringHourEn;
        stringMinutes = stringMinuteEn;
        and = andEn;

        if (minutes == 1) {
            stringMinutes = stringMinuteEn;
        }
        if (hours == 1) {
            stringHours = stringHourEn;
        }
        if (minutes == 0) {
            stringMinutes = "";
            minutesstring = "";
            and = "";
        }
        if (hours == 0) {
            stringHours = "";
            hoursstring = "";
            and = "";
        }
    }

    var remTime = hoursstring + (hoursstring == "" ? "" : " ") +
        stringHours + and + minutesstring + (minutesstring == "" ? "" : " ") + stringMinutes;

    return remTime;
}

function twelveHourMode(time) {
    if (P.is24HourFormat)
        return time;
    else {
        var t = time.split(":");
        var amPm = "AM";
        var hours = parseInt(t[0]);
        if (hours > 12) {
            hours = hours - 12;
            amPm = "PM";
        }
        var hoursstring = hours;
        if (hours < 10)
            hoursstring = "0" + hours;

        var beforeAmPmLang = hoursstring + ":" + t[1] + " " + amPm;

        var result;
        if (P.isAr)
            result = beforeAmPmLang.replace("AM", "ص").replace("PM", " م ");
        else
            result = beforeAmPmLang;

        return result;
    }
}

