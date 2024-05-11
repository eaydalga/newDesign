<?php
function dec2string($veri,$base) {
   $tablo = array(0=>'0',1=>'1',2=>'2',3=>'3',4=>'4',5=>'5',6=>'6',7=>'7',
               8=>'8',9=>'9',10=>'A',11=>'B',12=>'C',13=>'D',14=>'E',15=>'F');
   $sayi = "";
   $kalan = $veri;
   while($kalan) {
       $dd = $kalan % $base;
       $sayi = $tablo[$dd] . $sayi;
       $kalan = (int)($kalan / $base);
   }
   if((strlen($sayi) % 4) != 0)
       $sayi = "0" . $sayi;
       
   return $sayi;
}

function string2dec($veri,$base) {
   $tablo = array(0=>'0',1=>'1',2=>'2',3=>'3',4=>'4',5=>'5',6=>'6',7=>'7',
               8=>'8',9=>'9',10=>'A',11=>'B',12=>'C',13=>'D',14=>'E',15=>'F');
   $kalan=$veri;
   $len = strlen($kalan);
   $sum = 0;
   for($i=0;$i<$len;$i++) {
       $c = substr($kalan,0,1);
       $k = array_search($c,$tablo);
       $sum = $sum * $base + $k;
       $kalan = substr($kalan,1);
   }
   return $sum;
}

$in = "0BC4";
$base = 16;
echo(string2dec($in,$base));
?>
