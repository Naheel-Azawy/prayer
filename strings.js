//using Prefs;

var
pray_names,
app_name,
remainingTimeHintString,
remainingTimePasssedHintString,
remainingTimeHintToPrayString,
remainingTimeHintToString,
remainingTimeHintToIqamaString,
remainingTimeHintToImsakString,
prayTimeEntered,
prayTimeEnteredFull,
prayTimeEnteredFullIq,
iqamaString,
fajrTxt,
sunriseTxt,
duhrTxt,
jumuahTxt,
asrTxt,
magribTxt,
ishaTxt;

function initStrings() {
		if (P.isAr) {
			  app_name = "الصلاة";
			  pray_names = ["الفجر", "الشروق", "الظهر", "العصر", "المغرب", "العشاء"];
			  remainingTimeHintString = "المتبقي";
			  remainingTimePasssedHintString = "مرت";
			  remainingTimeHintToPrayString = "لصلاة";
			  remainingTimeHintToString = "إلى";
			  remainingTimeHintToIqamaString = "للإقامة";
			  remainingTimeHintToImsakString = "للإمساك";
			  prayTimeEntered = "دخل وقت";
			  prayTimeEnteredFull = "دخل وقت الصلاة";
			  prayTimeEnteredFullIq = "دخل وقت الإقامة";
			  iqamaString = "إقامة";
			  fajrTxt = "الفجر";
			  sunriseTxt = "الشروق";
			  duhrTxt = "الظهر";
			  jumuahTxt = "الجمعة";
			  asrTxt = "العصر";
			  magribTxt = "المغرب";
			  ishaTxt = "العشاء";
		} else {
			  app_name = "The Prayer";
			  pray_names = ["Fajr", "Sunrise", "Duhr", "Asr", "Maghrib", "Isha"];
			  remainingTimeHintString = "remaining";
			  remainingTimePasssedHintString = "passed";
			  remainingTimeHintToPrayString = "to";
			  remainingTimeHintToString = "to";
			  remainingTimeHintToIqamaString = "to iqama";
			  remainingTimeHintToImsakString = "to imsak";
			  prayTimeEntered = "It's time for";
			  prayTimeEnteredFull = "It's praying time";
			  prayTimeEnteredFullIq = "It's iqama time";
			  iqamaString = "iqama";
			  fajrTxt = "Fajr";
			  sunriseTxt = "Sunrise";
			  duhrTxt = "Duhr";
			  jumuahTxt = "Jumuah";
			  asrTxt = "Asr";
			  magribTxt = "Magrib";
			  ishaTxt = "Isha";
		}

		if (new Date().getDay() === 5) {
			  duhrTxt = jumuahTxt;
			  pray_names [2] = jumuahTxt;
		}
}


