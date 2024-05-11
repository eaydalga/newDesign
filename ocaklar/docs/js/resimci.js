var tempX = 0;
var tempY = 0;

function getMouseXY(e) {
var IE = document.all?true:false;
if (IE) { // grab the x-y pos.s if browser is IE
tempX = event.clientX + document.body.scrollLeft;
tempY = event.clientY + document.body.scrollTop;
}
else { // grab the x-y pos.s if browser is NS
tempX = e.pageX;
tempY = e.pageY;
}
if (tempX < 0){tempX = 0;}
if (tempY < 0){tempY = 0;}

return true;
}

function resimsil()
{
   if(document.getElementById) {
       target1=document.getElementById('canvas');
       if(target1) {
           target1.innerHTML = "";
       }
   }
}

function myDrawPic(e, pic, w, h)
{
   var body="<img src=\""+pic+"\" alt=\"\" width=\"" + w + "\" heigh=\"" + h + "\" onmouseout=\"resimsil();\" onclick=\"resimsil();\"/>";

   xx = Math.abs(800 - w);
   yy = Math.abs(600 - h);

   if(document.getElementById) {
       target1=document.getElementById('canvas');
       if(target1) {
           target1.style.position = "absolute";
           target1.style.top = yy;
           target1.style.left = xx;
           target1.innerHTML = body;
       }
   }
}

function eclearall(i)
{
   if(document.getElementById) {
        for(j=1;j<25;j++) {
          target2=document.getElementById('acik-'+i+'-'+j);
          if(target2) target2.style.display = "none";
          target1=document.getElementById('racik-'+i+'-'+j);
          if(target1) target1.style.display = "none";
        }
   }
}

function yclearall()
{
   if(document.getElementById) {
        for(j=1;j<4;j++) {
          target2=document.getElementById('usec-'+j);
          if(target2) target2.style.display = "none";
        }
   }
}

function yresim(i)
{
   var s = 'usec-'+i;
   if(document.getElementById) {
      yclearall();
      target=document.getElementById(s);
      if(target) {
          target.style.display = "inline";
          document.getElementById("content").style.top = "0px";
      }

   }
}

function eresim(k,i)
{
   var s = 'acik-'+k+'-'+i;
   var ss = 'racik-'+k+'-'+i;
   if(document.getElementById) {
      eclearall(k);
      target=document.getElementById(s);
      if(target) {
          target.style.display = "inline";
          target2=document.getElementById(ss);
          target2.style.display = "inline";
          document.getElementById("content").style.top = "0px";
      }
   }
}
