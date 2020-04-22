function getParameters() {
    var prmstr = window.location.search.substr(1);
    return prmstr != null && prmstr != "" ? transformToAssocArray(prmstr) : {};
}

function transformToAssocArray( prmstr ) {
    var params = {};
    var prmarr = prmstr.split("&");
    for ( var i = 0; i < prmarr.length; i++) {
        var tmparr = prmarr[i].split("=");
        params[tmparr[0]] = tmparr[1];
    }
    return params;
}

function Prefs () {

	  this.init = function () {

        var p = getParameters();

		    this.isAr = p.isAr === 'true' || false;
		    this.isLight = p.isLight === 'true' || false;
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
