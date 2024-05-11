function addLinksToTrees() {
	if( !document.getElementsByTagName || !document.createElement || !document.childNodes ) { return; }
	var oULs = document.getElementsByTagName('ul');
	for( var i = 0; i < oULs.length; i++ ) {
		if( oULs[i].className == 'treeview' ) {
			var oLinks = oULs[i].getElementsByTagName('a');
			for( var j = 0; j < oLinks.length; j++ ) {
				oLinks[j].onclick = function () {
					alert(this.parentNode.getElementsByTagName('span')[0].firstChild.nodeValue.replace(/ - - /g,'\n\n').replace(/ - /g,'\n'));
					return false;
				};
			}
		}
	}
}
if( document.getElementsByTagName && document.createElement && document.childNodes ) {
	var oPgHead = document.getElementsByTagName('head')[0];
	if(oPgHead) {
		var oNwStl = document.createElement('link');
		oNwStl.setAttribute('type','text/css');
		oNwStl.setAttribute('rel','stylesheet');
		oNwStl.setAttribute('href','/styles/hidedfns.css');
		oNwStl.setAttribute('media','screen,projection,tv,handheld');
		oPgHead.appendChild(oNwStl);
		var oIndXel = document.getElementById('index');
		var oH3inIndx = document.createElement('h3');
		oH3inIndx.appendChild(document.createTextNode('Options'));
		var oULinindex = document.createElement('ul');
		var oLIinindex = document.createElement('li');
		var oAinindex = document.createElement('a');
		oAinindex.appendChild(document.createTextNode('Show tree-view details'));
		oAinindex.setAttribute('href','javascript:alert(\'Please click the link normally.\');');
		oAinindex.onclick = function () {
			if( oNwStl.parentNode == oPgHead ) {
				oAinindex.firstChild.nodeValue = 'Hide tree-view details';
				oPgHead.removeChild(oNwStl);
			} else {
				oAinindex.firstChild.nodeValue = 'Show tree-view details';
				oPgHead.appendChild(oNwStl);
			}
			return false;
		};
		oLIinindex.appendChild(oAinindex);
		oULinindex.appendChild(oLIinindex);
		oIndXel.appendChild(oH3inIndx);
		oIndXel.appendChild(oULinindex);
	}
}
