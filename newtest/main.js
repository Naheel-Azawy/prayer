
var
yellow   = "#FFC107",
dark     = "#111111",
light    = "#eeeeee",
black    = "#000000",
white    = "#ffffff",
bg_dark  = "#000000",
bg_light = "#ffffff";

var card_color, bg_color, font_color;

update_main(true);
var pray_times = get_times();
document.title = app_name;

var node = document.getElementById("my_node");

var pray_cards = [];
for (var i = 0; i<6; i++)
	  pray_cards.push(new PrayCard(i, false));

for (var i = 0; i<pray_cards.length; i++)
	  node.appendChild(pray_cards[i].div);

repeat_update();

// functions --------------------------------------

function update_main(b) {

	  P.init ();
	  initStrings ();

	  if (P.isLight) {
		    card_color = light;
		    bg_color = bg_light;
		    font_color = black;
	  } else {
		    card_color = dark;
		    bg_color = bg_dark;
		    font_color = white;
	  }

	  document.body.style.background = bg_color;

	  if (!b) {
		    send_notif();
		    pray_cards [get_next_position_pray_only ()].activateCard ();
  		  for (var i = 0; i<pray_cards.length; i++) pray_cards[i].updateCard ();
	  }
	  var dt = new Date();
	  if (dt.getHours() == 0 && dt.getMinutes() == 0) {
		    get_times ();
	  }

}

function repeat_update() {
	  update_main(false);
	  var d = new Date();
	  //document.write("before sleep");
	  sleep(60000 - (d.getMilliseconds() + d.getSeconds()*1000)).then(function() {
		    //document.write("after sleep");
		    setInterval(function() {
			      //document.write("iiiii");
			      update_main(false);
		    }, 10000);
	  });
}

function update_all_cards() {
	  for (var i = 0; i<pray_cards.length; i++) pray_cards[i].updateCard ();
}

function PrayCard(i, active) {
	  this.i = i;
	  this.active = active;

	  var timeStr = twelveHourMode(pray_times[i]);
	  var textStr = pray_names[i];

	  var div = document.createElement('div');
	  div.id = "card" + this.i + "_div";
	  //div.className = "my-card mdl-card mdl-shadow--2dp mdl-button mdl-js-button mdl-button--raised mdl-js-ripple-effect";
	  div.className = "my-card mdl-card mdl-shadow--2dp mdl-js-button mdl-button--raised mdl-js-ripple-effect";

	  var div1 = document.createElement('div');
	  div1.style.textAlign = "center";
	  var time = document.createElement('div');
	  time.style.float = "left";
	  time.style.width = "50%";
	  time.id = "time" + this.i + "_div";
	  time.innerHTML = timeStr;
	  var text = document.createElement('div');
	  text.style.float = "left";
	  text.style.width = "50%";
	  text.id = "text" + this.i + "_div";
	  text.innerHTML = textStr;
	  div1.appendChild(time);
	  div1.appendChild(text);
	  div.appendChild(div1);

	  var rem=document.createElement('div');
	  rem.id = "rem"+this.i+"_div";
	  rem.style.textAlign = "center";
	  rem.style.color = black;
	  rem.innerHTML = get_remaining_time (i);
	  div.appendChild(rem);

	  this.div = div;

  	div.onclick = function(){
		    pray_cards[i].activateCard();
	  };

	  this.updateCard = function() {
		    var
		    time = document.getElementById("time"+this.i+"_div"),
		    text = document.getElementById("text"+this.i+"_div"),
		    rem = document.getElementById("rem"+this.i+"_div");

		    time.innerHTML = twelveHourMode(pray_times[i]);
		    text.innerHTML = pray_names[i];
		    pray_cards[this.i].colorize();
		    if (this.active) rem.innerHTML = get_remaining_time(i);
	  };

	  this.activateCard = function() {
		    for (var j = 0; j<pray_cards.length; j++) {
			      var
			      card = document.getElementById(
					      "card"+j+"_div"),
			      rem = document.getElementById(
					      "rem"+j+"_div");
			      if (this === pray_cards[j]) {
				        pray_cards[j].active = true;
				        rem.style.display = 'inline';
			      } else {
				        pray_cards[j].active = false;
				        rem.style.display = 'none';
			      }
			      pray_cards[j].colorize();
		    }
		    update_main(true);
	  };

	  this.colorize = function() {
		    var
		    card = document.getElementById("card"+this.i+"_div"),
		    time = document.getElementById("time"+this.i+"_div"),
		    text = document.getElementById("text"+this.i+"_div");
		    if (this.active) {
			      card.style.background = yellow;
			      time.style.color = black;
			      text.style.color = black;
		    } else {
			      card.style.background = card_color;
			      time.style.color = font_color;
			      text.style.color = font_color;
		    }
	  };


}

//send_notif("test");
