function myvoid()
{
}
function iekapa(target)
{
if (BrowserDetect.browser == "Explorer")
     target1.style.display = "";
else target1.style.display = "none";
}
function iletisim()
{
var errstr = "";
var err;
var selectBox = document.forms['id2676084'].wf_How;

name1=document.forms['id2676084'].wf_Name1.value;
err = mycheck(name1,document.forms['id2676084'].wf_Name1.className);
if (err.length > 0) errstr += "Ad:" + err + "\n";
email=document.forms['id2676084'].wf_Email.value;
err = mycheck(email,document.forms['id2676084'].wf_Email.className);
if (err.length > 0) errstr += "E-posta:" + err + "\n";
tel=document.forms['id2676084'].wf_Telephone.value;
err = mycheck(tel,document.forms['id2676084'].wf_Telephone.className);
if (err.length > 0) errstr += "Telefon:" + err + "\n";
yourq=document.forms['id2676084'].wf_YourQuery3.value;
err = mycheck(yourq,document.forms['id2676084'].wf_YourQuery3.className);
if (err.length > 0) errstr += "Dilek:" + err + "\n";
howto = selectBox.options[selectBox.selectedIndex].value
secq=document.forms['id2676084'].security_code.value;
err = mycheck(secq,document.forms['id2676084'].security_code.className);
if (err.length > 0) errstr += "Guvenlik Kodu:" + err + "\n";
if(errstr.length > 0) {
    alert(errstr);
    return false;
}
body = "wf_Name1="+name1+"&"+ "wf_Email="+email+"&"+ "wf_Telephone="+tel+"&"+ "wf_YourQuery3="+yourq+"&"+ "wf_How="+howto+"&";
body += "security_code="+secq+"&";
var myConn = new XHConn();
if (!myConn)
     alert("XMLHTTP desteği yok. Daha yeni/iyi gezgin/tarayıcı kullanın.");
var fnWhenDone = function (oXML) {
   if(document.getElementById) {
      target1=document.getElementById('sec6');
      if(target1) {
            iekapa(target1);
      }
      target=document.getElementById('sec7');
      if(target) {
            target.innerHTML=oXML.responseText;
            target.style.display = "inline";
      }
   }
 };
myConn.connect("mail.php", "POST", body, fnWhenDone);
document.forms['id2676084'].reset();
return true;
}

function ksecim(s,t)
{
var myConn2 = new XHConn();
var body="s="+s+"&"+"t="+t;

var fnBitti = function(oXML) {
   if(document.getElementById) {
      target1=document.getElementById('sec2');
      if(target1) {
            iekapa(target1);
      }
      target=document.getElementById('sec8');
      if(target) {
            target.innerHTML=oXML.responseText;
            target.style.display = "inline";
      }
   }
 };
myConn2.connect("secim.php", "POST", body, fnBitti);
}

function ksecim2(sayfa,m,n)
{
var errstr = "";
var err;
var body="sayfa="+sayfa+"&";
wfs=document.forms['id12333'].wf_s.value;
wft=document.forms['id12333'].wf_t.value;
for(i=0; i < document.forms['id12333'].elements.length; i++) {
    var aa = document.forms['id12333'].elements[i].name;
    if ( aa != "undefined" ) {
        wff=document.forms['id12333'].elements[i].value;
        err = mycheck(wff,document.forms['id12333'].elements[i].className);
        if (err.length > 0) {
            errstr += document.forms['id12333'].elements[i].name+ ":" + err + "\n";
        }
        else {
            if (!(aa == "sayfa" || aa == "submit" || aa == "undefined")) {
               body += aa + "=" + wff + "&";
            }
        }
    }
}
if(errstr.length > 0) {
    alert(errstr);
    return false;
}

var myConn3 = new XHConn();
var fnSon = function(oXML) {
   if(document.getElementById) {
      target1=document.getElementById('sec'+m);
      if(target1) {
            iekapa(target1);
      }
      target=document.getElementById('sec'+n);
      if(target) {
            target.innerHTML=oXML.responseText;
            target.style.display = "inline";
      }
   }
 };
myConn3.connect("secim.php", "POST", body, fnSon);
}

function bitsin(n)
{
   if(document.getElementById) {
      xx = 'sec'+n;
      target1=document.getElementById(xx);
      if(target1) {
            iekapa(target1);
      }
   }
   kapat();
}

function isteme(t)
{
var myConn5 = new XHConn();
var body="bweb="+t;

var fnIsteme = function (oXML) {
   if(document.getElementById) {
      target1=document.getElementById('sec5');
      if(target1) {
            iekapa(target1);
      }
      target=document.getElementById('sec11');
      if(target) {
            target.innerHTML=oXML.responseText;
            target.style.display = "inline";
      }
   }
 };
myConn5.connect("isteme.php", "GET", body, fnIsteme);
}

function isteme2()
{
var myConn6 = new XHConn();
var errstr = "";
var body = "bweb=0&";
var err;

for(i=0; i < document.forms['wf_Enquiry'].elements.length; i++) {
    var aa = document.forms['wf_Enquiry'].elements[i].name;
    if ( aa != "undefined" ) {
        wff=document.forms['wf_Enquiry'].elements[i].value;
        err = mycheck(wff,document.forms['wf_Enquiry'].elements[i].className);
        if (err.length > 0) {
            errstr += document.forms['wf_Enquiry'].elements[i].name+ ":" + err + "\n";
        }
        else {
            if (!(aa == "sayfa" || aa == "submit" || aa == "undefined")) {
               body += aa + "=" + wff + "&";
            }
        }
    }
}
if(errstr.length > 0) {
    alert(errstr);
    return false;
}
var fnIsteme2 = function (oXML) {
   if(document.getElementById) {
      target1=document.getElementById('sec11');
      if(target1) {
            iekapa(target1);
      }
      target=document.getElementById('sec12');
      if(target) {
            target.innerHTML=oXML.responseText;
            target.style.display = "inline";
      }
   }
 };
myConn6.connect("isteme.php", "GET", body, fnIsteme2);
}
