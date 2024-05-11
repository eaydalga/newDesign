function clearall()
{
   if(document.getElementById) {
        target2=document.getElementById('gads');
        if(target2) target2.style.display = "none";
        target2=document.getElementById('sayfaac');
        if(target2) target2.style.display = "none";
   }
}

function clearbase()
{
   if(document.getElementById) {
        for(j=1;j<12;j++) {
            target2=document.getElementById('sec' + j);
            if(target2) target2.style.display = "none";
        }
        for(j=20;j<22;j++) {
            target2=document.getElementById('sec' + j);
            if(target2) target2.style.display = "none";
        }
        for(j=70;j<80;j++) {
            target2=document.getElementById('sec' + j);
            if(target2) target2.style.display = "none";
        }
   }
}
function okuresim()
{
   var s = 'sayfaac';
   if(document.getElementById) {
      clearall();
      target=document.getElementById(s);
      target.style.display = "inline";
   }
}
function baslik(j)
{
   if(document.getElementById) {
      clearbase();
      target2=document.getElementById('sec' + j);
      if(target2) { target2.style.display = "inline";
          document.getElementById("content").style.top = "0px";
      }
   }
}
function oku(b)
{
   if(document.getElementById) {
      ackapa(0);
      target=document.getElementById('sayfaac');
      if(target.style.display == "none") {
          okuresim();
      }
      baslik(b);
   }
}
function kapat()
{
   clearbase();
   clearall();
   if(document.getElementById) {
      target=document.getElementById('gads');
      if(target) target.style.display = "inline";
   }
}

function isAlphanum(str){
  var re = /[^a-zA-Z0-9]/g
  if (re.test(str)) return false;
  return true;
}

function isAlpha(str){
  var re = /[^a-zA-Z]/g
  if (re.test(str)) return false;
  return true;
}

function mycheck(veri, sinif)
{
 var psinif = new Array();
 var errstr = "";
 var i=0;
 var psx = new Array();
 psinif = sinif.split(" ");
 n = psinif.length;
 while(i < n) {
     // alert("("+veri+") "+psinif[i]);
     if(psinif[i] == "validate-alpha") {
          if (!isAlpha(veri)) {
              errstr += "alfabetik değil";
          }
     }
     if(psinif[i] == "validate-alphanum") {
          psx = veri.split(" ");
          psn = psx.length;
          for (j=0; j<psn; j++) {
             if (!isAlphanum(psx[j])) {
                 errstr += "alfa numerik değil";
                 break;
             }
          }
     }
     if(psinif[i] == "validate-date") {
          // u - mm/dd/yy, w - dd/mm/yy
          // p - previous, f - forward, a - any
          if(!validateDate(veri,'W','a')) {
              errstr += "tarih hatali";
          }
     }
     if(psinif[i] == "validate-time") {
          if(!checktime(veri)) {
              errstr += "saat hatalı";
          }
     }
     if(psinif[i] == "validate-email") {
          if(!Validate_Email_Address(veri)) {
              errstr += "e-posta adresi hatali";
          }
     }
     if(psinif[i] == "validate-integer") {
          if (!isNum(veri)) {
              errstr += "sayısal değil";
          }
     }
     if(psinif[i] == "validate-float") {
          if (!validateFloat(veri)) {
              errstr += "kayan noktalı sayı değil";
          }
     }
     if(psinif[i] == "required") {
          if (!(veri.length > 0)) {
              errstr += "zorunlu bilgi girişi";
          }
     }
     if(errstr.length > 0) errstr += ",";
     i++;
 }
 return errstr;
}
