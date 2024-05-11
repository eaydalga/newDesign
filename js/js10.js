
function doNuhn() {}

function getRefToDiv(divID,oDoc) {
	if( document.getElementById ) { return document.getElementById(divID); }
	if( document.all ) { return document.all[divID]; }
	if( !oDoc ) { oDoc = document; }
	if( document.layers ) {
		if( oDoc.layers[divID] ) {
			return oDoc.layers[divID];
		} else {
			for( var x = 0, y; !y && x < oDoc.layers.length; x++ ) {
				y = getRefToDiv(divID,oDoc.layers[x].document);
			}
			return y;
		}
	}
	return false;
}

function showDiv(divID_as_a_string) {
	var myReference = getRefToDiv(divID_as_a_string);
	if( !myReference ) { window.alert('Nothing works in this browser'); return; }
	if( myReference.style ) { myReference.style.visibility = 'visible'; } else {
		if( myReference.visibility ) { myReference.visibility = 'show'; } else {
			window.alert('Nothing works in this browser'); return; } }
}

function hideDiv(divID_as_a_string) {
	var myReference = getRefToDiv(divID_as_a_string);
	if( !myReference ) { window.alert('Nothing works in this browser'); return; }
	if( myReference.style ) { myReference.style.visibility = 'hidden'; } else {
		if( myReference.visibility ) { myReference.visibility = 'hide'; } else {
			window.alert('Nothing works in this browser'); return; } }
}

function mvDiv(divID_as_a_string) {
	var myReference = getRefToDiv(divID_as_a_string), noPx = document.childNodes ? 'px' : 0;
	if( !myReference ) { window.alert('Nothing works in this browser'); return; }
	if( myReference.style ) { myReference = myReference.style; }
	if( parseInt(myReference.left) ) {
		window.alert('Sorry, I will not allow you to move this element too far,\ninstead I will reset it.');
		myReference.left = 0 + noPx; myReference.top = 0 + noPx; } else {
		myReference.left = 20 + noPx; myReference.top = 10 + noPx; }
}

var clodiv = true;

function clDiv(divID_as_a_string) {
	var dC = clodiv ? '#00ff00' : '#dddddd'; clodiv = clodiv ? false : true;
	var myReference = getRefToDiv(divID_as_a_string);
	if( !myReference ) { window.alert('Nothing works in this browser'); return; }
	if( myReference.style ) { myReference = myReference.style; }
	myReference.background = dC;
	myReference.backgroundColor = dC;
	myReference.bgColor = dC;
}

var rewr = 0;

function rwDiv(divID_as_a_string) {
	var myReference = getRefToDiv(divID_as_a_string);
	if( !myReference ) { window.alert('Nothing works in this browser'); return; }
	if( myReference.innerHTML ) { myReference.innerHTML = '<p>The contents of this element have been rewritten ' + ( ++rewr ) + ' time' + ( ( rewr - 1 ) ? 's' : '' ) + '.<\/p>'; } else {
		if( myReference.document && myReference.document != window.document ) {
			myReference.document.open();
			myReference.document.write( '<p>The contents of this element have been rewritten ' + ( ++rewr ) + ' time' + ( ( rewr - 1 ) ? 's' : '' ) + '.<\/p>' );
			myReference.document.close();
		} else {
			window.alert('Your browser does not allow the contents to be re-written.\nI have not implemented an iframe here.\nSee my generic re-writable elements example for how this should be done.'); return; }
    }
}

function clpDiv(divID_as_a_string,ot,ob) {
	var myReference = getRefToDiv(divID_as_a_string);
	if( !myReference ) { window.alert('Nothing works in this browser'); return; }
	if( myReference.clip ) { myReference.clip.top = ot; myReference.clip.bottom = ob; } else {
		if( myReference.style ) { myReference.style.clip = 'rect(' + ot + 'px,250px,' + ob + 'px,0px)'; } else {
			window.alert('Nothing works in this browser'); return; } }
}

var zInd = 300;

function upEl(theDiv) {
	if( document.layers ) {
		theDiv = document.layers['myDiv6C'].document.layers[theDiv];
	} else {
		theDiv = getRefToDiv(theDiv);
	}
	if( !theDiv ) { window.alert( 'Nothing works in this browser.' ); return; }
	if( theDiv.style ) { theDiv = theDiv.style; }
	theDiv.zIndex = zInd; zInd++;
}

function changeDisplay( elementId, setTo ) {
	if( document.getElementById ) { var theElement = document.getElementById( elementId ); } else {
		if( document.all ) { var theElement = document.all[ elementId ]; } else { var theElement = new Object(); } }
	if( !theElement ) { return; }
	if( theElement.style ) { theElement = theElement.style; }
	if( typeof( theElement.display ) == 'undefined' && !( window.ScriptEngine && ScriptEngine().indexOf( 'InScript' ) + 1 ) ) { window.alert( 'Nothing works in this browser.' ); return; }
	theElement.display = setTo;
}

function chBG( oColour ) {
	if( document.documentElement && document.documentElement.style ) {
		document.documentElement.style.backgroundColor = oColour; }
	if( document.body && document.body.style ) { document.body.style.backgroundColor = oColour; }
	document.bgColor = oColour;
}

function rSDiv(oName,newWidth,newHeight) {
	var myReference = getRefToDiv(oName), noPx = document.childNodes ? 'px' : 0;
	if( myReference.style ) { myReference = myReference.style; }
	if( myReference.resizeTo ) { myReference.resizeTo( newWidth, newHeight ); }
	myReference.width = newWidth + noPx; myReference.pixelWidth = newWidth;
	myReference.height = newHeight + noPx; myReference.pixelHeight = newHeight;
}

function createPosEl() {
	window.oPsnx = window.oPsnx ? ( window.oPsnx + 1 ) : 1;
	if( document.layers && window.Layer ) {
		var a = new Layer( 350, document.layers['elNew'] );
		a.document.open();
		a.document.write('This positioned element (number ' + window.oPsnx + ') has been created inside the existing positioned element');
		a.document.close();
		a.bgColor = ( window.oPsnx % 2 ) ? '#ff6666' : '#66ff66'; a.left = window.oPsnx * 2; a.top = window.oPsnx * 2; a.visibility = 'show';
	} else {
		var a = getRefToDiv('elNew');
		var theString = '<div style="position:absolute;left:'+(window.oPsnx*2)+'px;top:'+(window.oPsnx*2)+'px;width:350px;background-color:#'+((window.oPsnx%2)?'ff6666':'66ff66')+';">This positioned element (number ' + window.oPsnx + ') has been created inside the existing positioned element<\/div>';
		if( a.iinsertAdjacentHTML ) { a.insertAdjacentHTML('beforeEnd',theString);
		} else if( typeof( a.innerHTML ) != 'undefined' ) { a.innerHTML += theString;
		} else { window.alert('Nothing works in this browser'); }
	}
}

