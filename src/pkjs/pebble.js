Pebble.addEventListener("ready",
  function(e) {
    console.log("PebbleKit JS ready!");
  }
);

Pebble.addEventListener("showConfiguration",
  function(e) {
    //Load the remote config page
    Pebble.openURL("http://51015977.de.strato-hosting.eu/pebble/countdown/count_config_v1.html");
  }
);

Pebble.addEventListener("webviewclosed",
  function(e) {
    //Get JSON dictionary
    var configuration = JSON.parse(decodeURIComponent(e.response));

		configuration.text1 = configuration.text1.replace(/\$/g,"%");
		configuration.text2 = configuration.text2.replace(/\$/g,"%");
		
		console.log("Configuration window returned: " + JSON.stringify(configuration));
		
    //Send to Pebble, persist there
    Pebble.sendAppMessage(
      {
				"KEY_TEXT1"			: configuration.text1,
				"KEY_TEXT2"			: configuration.text2,
				"KEY_MODE"			: configuration.mode,
        "KEY_DAY_START"	: configuration.start,
				"KEY_DAY_END"		: configuration.end,
				"KEY_VIBE"			: configuration.vibe,

				"KEY_YEAR"			: configuration.year,
				"KEY_MON"				: configuration.mon,
				"KEY_DAY"				: configuration.day,
				"KEY_HOUR"			: configuration.hour,
				"KEY_MIN"				: configuration.min
				//"KEY_SEC"				: configuration.sec,
				

      },
      function(e) {
        console.log("Sending settings data...");
      },
      function(e) {
        console.log("Settings feedback failed!");
      }
    );
  }
);
