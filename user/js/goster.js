function gostersil(fname,nn)
{
   if(document.getElementById) {
      j=1;
      nm = fname + j;
      nd = nn + j;
      while(document.getElementById(nm)) {
          target=document.getElementById(nm);
          if(target) {
              target.style.display = "none";
              document.getElementById(nd).style.backgroundColor = "#ffffff";
          }
          j++;
          nm = fname + j;
          nd = nn + j;
      }
   }
}
function goster(fname,nd)
{
   if(document.getElementById) {
      gostersil('sec','d');
      target=document.getElementById(fname);
      if(target) {
          target.style.display = "inline";
          document.getElementById(nd).style.backgroundColor = "#d4d4f4";
      }
   }
}
