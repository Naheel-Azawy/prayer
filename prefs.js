function Prefs () {

	  this.init = function () {
		    this.isAr = false;
		    this.isLight = false;
		    this.is24HourFormat = false;

		    this.latitude = 25.285447;
		    this.longitude = 51.531040;
		    this.timezone = 3;
		    this.calc_method = "qatar";
		    this.asr_juristic = "shafii";
		    this.high_lats_method = "anglebased";

		    this.iqamaFajr = 25;
		    this.iqamaDuhr = 20;
		    this.iqamaAsr = 25;
		    this.iqamaMagrib = 10;
		    this.iqamaIsha = 20;
	  };

}

var P = new Prefs();
