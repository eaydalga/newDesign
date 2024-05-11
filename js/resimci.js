var tempX = 0;
var tempY = 0;

function getMouseXY(e) {
   var IE = document.all?true:false;
   if (IE) { // grab the x-y pos.s if browser is IE
      tempX = event.clientX + document.body.scrollLeft;
      tempY = event.clientY + document.body.scrollTop;
   } else { // grab the x-y pos.s if browser is NS
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

function yy_bak(tempy,h1)
{
    if(tempy > h1) {
       yy = Math.abs(tempy - h1);
    } else {
       yy = Math.abs(tempy - h1 / 2);
    }
return yy;
}
function xx_bak(tempx,w1)
{
   if(tempx+w1 > 1000) {
       xx = Math.abs(tempx - w1);
   } else {
       xx = Math.abs(tempx - w1 / 2);
   }
return xx;
}

function resim(e)
{
   var s = 'sayfaac';
   if(document.getElementById) {
      clearall();
      target=document.getElementById(s);
      target.style.display = "inline";

      getMouseXY(e);
      yy = yy_bak(tempY,420) + "px";
      xx = xx_bak(tempX,600) + "px";
      
      target1=document.getElementById('contains1');
      if(target1) {
           target1.style.position = "absolute";
           target1.style.top = yy;
           target1.style.left = xx;
      } else {
          target1=document.getElementById('seccontains1');
          if(target1) {
               target1.style.position = "absolute";
               target1.style.top = yy;
               target1.style.left = xx;
          }
      }

   }
}

function myDrawPic(e, pic, w, h)
{
   var body="<img src=\""+pic+"\" alt=\"\" width=\"" + w + "\" heigh=\"" + h + "\" onmouseout=\"resimsil();\" onclick=\"resimsil();\"/>";

   getMouseXY(e);
   yy = yy_bak(tempY,h) + "px";
   xx = xx_bak(tempX,w) + "px";

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
