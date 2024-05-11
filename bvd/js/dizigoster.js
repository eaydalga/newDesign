function dizigostersil(fname)
{
   if(document.getElementById) {
      n = fname.length;
      j=1;
      ff = fname.substr(0,n-1);
      nm = ff + j;
      while(document.getElementById(nm)) {
          target=document.getElementById(nm);
          if(target) {
              target.style.display = "none";
          }
          j++;
          nm = ff + j;
      }
   }
}
function dizigoster(fname)
{
   if(document.getElementById) {
      dizigostersil(fname);
      target=document.getElementById(fname);
      if(target) {
          target.style.display = "inline";
      }
   }
}
