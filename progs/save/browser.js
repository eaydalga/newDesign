var BrowserDetect = {
	init: function () {
		this.browser = this.searchString(this.dataBrowser) || "An unknown browser";
		this.version = this.searchVersion(navigator.userAgent)
			|| this.searchVersion(navigator.appVersion)
			|| "an unknown version";
		this.OS = this.searchString(this.dataOS) || "an unknown OS";
	},
	searchString: function (data) {
		for (var i=0;i<data.length;i++)	{
			var dataString = data[i].string;
			var dataProp = data[i].prop;
			this.versionSearchString = data[i].versionSearch || data[i].identity;
			if (dataString) {
				if (dataString.indexOf(data[i].subString) != -1)
					return data[i].identity;
			}
			else if (dataProp)
				return data[i].identity;
		}
	},
	searchVersion: function (dataString) {
		var index = dataString.indexOf(this.versionSearchString);
		if (index == -1) return;
		return parseFloat(dataString.substring(index+this.versionSearchString.length+1));
	},
	dataBrowser: [
		{ 	string: navigator.userAgent,
			subString: "OmniWeb",
			versionSearch: "OmniWeb/",
			identity: "OmniWeb"
		},
		{
			string: navigator.vendor,
			subString: "Apple",
			identity: "Safari"
		},
		{
			prop: window.opera,
			identity: "Opera"
		},
		{
			string: navigator.vendor,
			subString: "iCab",
			identity: "iCab"
		},
		{
			string: navigator.vendor,
			subString: "KDE",
			identity: "Konqueror"
		},
		{
			string: navigator.userAgent,
			subString: "Firefox",
			identity: "Firefox"
		},
		{
			string: navigator.vendor,
			subString: "Camino",
			identity: "Camino"
		},
		{		// for newer Netscapes (6+)
			string: navigator.userAgent,
			subString: "Netscape",
			identity: "Netscape"
		},
		{
			string: navigator.userAgent,
			subString: "MSIE",
			identity: "Explorer",
			versionSearch: "MSIE"
		},
		{
			string: navigator.userAgent,
			subString: "Gecko",
			identity: "Mozilla",
			versionSearch: "rv"
		},
		{ 		// for older Netscapes (4-)
			string: navigator.userAgent,
			subString: "Mozilla",
			identity: "Netscape",
			versionSearch: "Mozilla"
		}
	],
	dataOS : [
		{
			string: navigator.platform,
			subString: "Win",
			identity: "Windows"
		},
		{
			string: navigator.platform,
			subString: "Mac",
			identity: "Mac"
		},
		{
			string: navigator.platform,
			subString: "Linux",
			identity: "Linux"
		}
	]

};
BrowserDetect.init();

/* nasil kullanilir :
 *    BrowserDetect.browser --- identity
 *    BrowserDetect.OS
 *    BrowserDetect.version              */

function ayarla()
{
  if (BrowserDetect.browser == "Explorer") {
    // if (document.getElementById('container')) {
    //     document.getElementById('syazi').style.fontWeight='bold';
    //     document.getElementById('byazi').style.fontWeight='bold';
    // }
    if (BrowserDetect.version<=6) {
        if (document.getElementById('contains1')) {
            document.getElementById('contains1').style.position="absolute";
            document.getElementById('banner1').style.position="absolute";
            document.getElementById('banner1').style.top="405px";
            document.getElementById('contains1').style.top="420px";
            // document.getElementById('syazi').style.fontWeight='bold';
            // document.getElementById('byazi').style.fontWeight='bold';
        }
    }
  } else {
    if (document.getElementById('container'))
        document.getElementById('container').style.width="530px";
    if (document.getElementById('contains1')) {
        document.getElementById('contains1').style.width="530px";
    }
    if (document.getElementById('syazi')) {
        document.getElementById('syazi').style.fontWeight='700';
    }
    if (document.getElementById('byazi')) {
        document.getElementById('byazi').style.fontWeight='700';
    }
    if (BrowserDetect.OS == "Linux") {
        if (document.getElementById('canvas')) {
            document.getElementById('canvas').style.fontSize='12';
        }
        if (document.getElementById('bdy')) {
            document.getElementById('bdy').style.fontSize='12';
        }
    }
  }
}
