var sonraki=0;
var t;
function sif_clearbase()
{
   if(document.getElementById) {
        for(j=1;j<14;j++) {
            target2=document.getElementById('kisi' + j);
            if(target2) target2.style.display = "none";
        }
   }
}

function sayfa(j)
{
   if(document.getElementById) {
      clearbase();
      target2=document.getElementById('kisi' + j);
      if(target2) target2.style.display = "inline";
   }
}

function ikontrol(ff)
{
    var ik_errstr = "";
    var err;
    for(i=0; i < document.forms[ff].elements.length; i++) {
        var aa = document.forms[ff].elements[i].name;
        if ( aa != "undefined" ) {
            wff=document.forms[ff].elements[i].value;
            err = mycheck(wff,document.forms[ff].elements[i].className);
            if (err.length > 0) {
                ik_errstr += document.forms[ff].elements[i].name+ ":" + err + "\n";
            }
        }
    }
    if(ik_errstr.length > 0) {
        alert(ik_errstr);
        return false;
    }
    if(ff == 'form1') {
        // burada mail gonderme çağrılacak
        var body="s="+ff+"&";
        var usr = document.forms[ff].elements['usrno'].value;
        var kod = document.forms[ff].elements['kod'].value;
        body=body + "usrno="+usr+"&kod="+kod+"&win=sec3&";
        baglanti("email.php", body, 'sec3','sec3');
    } else {
        document.forms[ff].submit();
    }
}

function uyeler(s,t,w)
{
var body="s="+s+"&";

usr=document.forms[s].elements['usrno'].value;
kod=document.forms[s].elements['kod'].value;
frnd=document.forms[s].elements['arkadas'].value;
basla=document.forms[s].elements['basla'].value;
sayisi=document.forms[s].elements['sayisi'].value;
body = body + "usrno=" + usr + "&" + "kod=" + kod + "&arkadas=" + frnd +"&basla=" + basla + "&sayisi=" + sayisi + "&";
baglanti("uyebak.php",body,t,w);
}

function uyeler2(s,t,w,b)
{
var body="s="+s+"&";

usr=document.forms[s].elements['usrno'].value;
kod=document.forms[s].elements[b].value;
frnd=document.forms[s].elements['arkadas'].value;
basla=document.forms[s].elements['basla'].value;
sayisi=document.forms[s].elements['sayisi'].value;
body = body + "usrno=" + usr + "&" + b + "=" + kod + "&arkadas=" + frnd +"&basla=" + basla + "&sayisi=" + sayisi + "&";
baglanti("uyebak.php",body,t,w);
}

function ekler(s,t,w)
{
baglanti("resimbak.php","",t,w);
}

function baglanti(towhere,body,t,w)
{
var myConn2 = new XHConn();
var fnBitti = function (oXML) {
   if(document.getElementById) {
      target1=document.getElementById(t);
      if(target1) {
            target1.style.display = "none";
      }
      target=document.getElementById(w);
      if(target) {
            target.innerHTML=oXML.responseText;
            target.style.display = "inline";
      }
   }
 };
myConn2.connect(towhere, "POST", body, fnBitti);
}

function sif_clear13(nn)
{
   target1=document.getElementById(nn);
   if(target1) {
         target1.style.display = "none";
   }
}

function ozelac(nn)
{
   target1=document.getElementById(nn);
   if(target1) {
         target1.style.display = "inline";
   }
}

function uyeekle(ff,deger,uad)
{
   target1=document.getElementById(ff);
   if(target1) {
       if(uad == "kod") {
            target1.elements[uad].value = deger;
            clear13('sec3');
       } else {
            target1.elements[uad].value += " " + deger;
       }
   }
}

function resimekle(ff,deger,uad)
{
   target1=document.getElementById(ff);
   if(target1) {
       target1.elements[uad].value = deger;
       clear13('sec31');
   }
}


function dosyalist(deger)
{
   ff = "sec5";
   target1=document.getElementById(ff);
   if(target1) {
        target1.innerHTML += deger+"<br>";
   }
}
function sif_gostersil()
{
   if(document.getElementById) {
      i=1;
      nn = "haber"+ i;
      while( document.getElementById(nn)) {
         target1= document.getElementById(nn);
         target1.style.display = "none";
         i++;
         nn = "haber"+ i;
      }
   }
}

function sif_goster(nn,tx)
{
  if(document.getElementById) {
      sif_gostersil();
      nm = "haber"+ nn;
      target1=document.getElementById(nm);
      if(target1) {
          target1.style.display = "inline";
          sonraki=nn+1;
          if(tx) {
              nm = "haber"+ sonraki;
              if(document.getElementById(nm)) {
                   clearTimeout(t);
                   t=setTimeout("sif_goster(sonraki,1)",5000);
              } else {
                   clearTimeout(t);
                   t=setTimeout("sif_goster(1,1)",5000);
              }
          } else {
              clearTimeout(t);
              document.getElementById(nm).style.display="inline";
          }
      } else {
          if(tx) {
               t=setTimeout("sif_goster(1,1)",5000);
          } else {
               clearTimeout(t);
               nm = "haber1";
               document.getElementById(nm).style.display="inline";
          }
      }
  }
}

function timedMsg()
{
sonraki++;
t=setTimeout("sif_goster(sonraki,1)",5000);
}

function tasi(toform,tofield,fform,ffield)
{
target = document.getElementById(toform)
target1 = document.getElementById(fform)
a = target.elements[tofield].value
b = target1.elements[ffield].value;
fname = b.substring(b.lastIndexOf('/')+1)
target.elements[tofield].value = a + "," + fname;
}

function logout()
{
location.href="kapa.php";
}
