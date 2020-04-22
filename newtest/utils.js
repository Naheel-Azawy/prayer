var _times, _times_iq;

function get_times () {

	  _times = new Array(6);
	  _times_iq = new Array(11);

	  var _p = new PrayTimes(P.calc_method);
	  _p.adjust({
		    imsak    : '10 min',
		    dhuhr    : '0 min',
		    asr      : P.asr_juristic == 'shafii' ? 'Standard' : 'Hanafi',
		    highLats : P.high_lats_method
	  });
	  var _t = _p.getTimes(new Date(), [P.latitude, P.longitude], P.timezone, null, '24h');

	  var prayerTimes = [_t.fajr, _t.sunrise, _t.dhuhr, _t.asr, _t.maghrib, _t.isha];

	  _times[0] = prayerTimes[0];
	  _times[1] = prayerTimes[1];
	  _times[2] = prayerTimes[2];
	  _times[3] = prayerTimes[3];
	  _times[4] = prayerTimes[4];
	  _times[5] = prayerTimes[5];

	  _times_iq[0] = prayerTimes[0];
	  _times_iq[1] = sec2Time (time2Sec (prayerTimes[0]) + P.iqamaFajr *60);
	  _times_iq[2] = prayerTimes[1];
	  _times_iq[3] = prayerTimes[2];
	  _times_iq[4] = sec2Time (time2Sec (prayerTimes[2]) + P.iqamaDuhr *60);
	  _times_iq[5] = prayerTimes[3];
	  _times_iq[6] = sec2Time (time2Sec (prayerTimes[3]) + P.iqamaAsr *60);
	  _times_iq[7] = prayerTimes[4];
	  _times_iq[8] = sec2Time (time2Sec (prayerTimes[4]) + P.iqamaMagrib *60);
	  _times_iq[9] = prayerTimes[5];
	  _times_iq[10] = sec2Time (time2Sec (prayerTimes[5]) + P.iqamaIsha *60);

	  return _times;
}

function get_remaining_time (position) {

	  if (_times == null || _times_iq == null) get_times ();

	  var rem_word = remainingTimeHintString;

	  position = position_no_to_iq (position);

	  var now = now_in_sec ();
	  var next_position = get_next_position_iq ();

	  var next = time2Sec (_times_iq [next_position]);
	  var wanted = time2Sec (_times_iq [position]);
	  var wanted_is_next = position_iq_to_no (next_position) == position_iq_to_no (position);
	  var time = wanted_is_next ? next : wanted;
	  var diff = time - now;
	  if (diff == 0) {
		    return is_iqama_time (next_position) ? prayTimeEnteredFullIq : prayTimeEnteredFull;
	  } else if (diff < 0) {
		    diff = -diff;
		    rem_word = remainingTimePasssedHintString;
		    if (next_position == 0 && position == 0) {
			      diff = time + 24*3600 - now;
			      rem_word = remainingTimeHintString;
		    }
	  } else if (is_iqama_time (next_position) && wanted_is_next) {
		    rem_word = remainingTimeHintString + " " + remainingTimeHintToIqamaString;
	  }

	  return P.isAr ? rem_word + " " + secondsToTimeInWords(diff) :
			  secondsToTimeInWords(diff) + " " + rem_word;
}

function is_iqama_time (next_position) {
	  return next_position == 1 || next_position == 4 || next_position == 6 ||
				next_position == 8 || next_position == 10;
}

function get_next_position_pray_only () {
	  return position_iq_to_no (get_next_position_iq ());
}

function get_next_position_iq () {
	  if (_times == null || _times_iq == null) get_times ();

	  var time_sec = now_in_sec ();
	  var next = 0;
	  var least = -1;
	  for (var i = 0; i<_times_iq.length; i++) {
		    var t = time2Sec (_times_iq [i]);
		    if (t - time_sec < 0) continue;
		    if (time2Sec (_times_iq [i]) < least || least <0) {
			      least = time2Sec (_times_iq [i]);
			      next = i;
		    }
	  }
	  return next;
}

function is_now (position) {
	  var time_sec = now_in_sec ();
	  var next = time2Sec (_times [position]);
	  return (next - time_sec) == 0;
}

function position_iq_to_no (i) {
	  switch (i) {
		case 0: return 0;  // fajr
		case 1: return 0;  // fajr iq
		case 2: return 1;  // sunrise
		case 3: return 2;  // duhr
		case 4: return 2;  // duhr iq
		case 5: return 3;  // asr
		case 6: return 3;  // asr iq
		case 7: return 4;  // magrib
		case 8: return 4;  // magrib iq
		case 9: return 5;  // isha
		case 10: return 5; // isha iq
		default: return 0;
	  }
}

function position_no_to_iq (i) {
	  switch (i) {
		case 0: return 0; // fajr
		case 1: return 2; // sunrise
		case 2: return 3; // duhr
		case 3: return 5; // asr
		case 4: return 7; // magrib
		case 5: return 9; // isha
		default: return 0;
	  }
}

//const TEST_TIME = "05:10";

function now_in_sec () {
	  var now = new Date();
	  return now.getHours() * 3600 + now.getMinutes() * 60;
	  //return time2Sec (TEST_TIME);
}

function send_notif (s) {

	  s = typeof s !== 'undefined' ? s : 'no';

	  if (s == "no") {
        for (var i =0; i<6; i++) {
			      if (is_now (i)) {
				        s = pray_names[i];
				        break;
			      }
		    }
	  }

	  if (s == "no") return;

	  var
	  name  = app_name,
	  title = s,
	  msg   = prayTimeEntered + " " + s;

	  alert(name + '\n' + title + '\n' + msg);

}

function calc_method_str_to_int (s) {
	  switch (s.toLowerCase()) {
		case "karachi":	return 0;
		case "isna":	return 1;
		case "mwl":	return 2;
		case "makkah":	return 3;
		case "egypt":	return 4;
		case "qatar":	return 6;
		default:	return 3;
	  }
}

function asr_juristic_str_to_int (s) {
	  switch (s.toLowerCase()) {
		case "shafii":	return 0;
		case "hanafi":	return 1;
		default:	return 0;
	  }
}

function high_lats_str_to_int (s) {
	  switch (s.toLowerCase()) {
		case "none":		return 0;
		case "midnight":	return 1;
		case "oneseventh":	return 2;
		case "anglebased":	return 3;
		default:		return 3;
	  }
}

// sleep time expects milliseconds
function sleep (time) {
	  return new Promise(function(resolve) {setTimeout(resolve, time);});
}
