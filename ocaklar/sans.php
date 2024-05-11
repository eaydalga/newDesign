<?php
$id = 49;
if(isset($_REQUEST['id']))
    $id=$_REQUEST['id'];
$a = Array();
$c = Array();
$d = Array(3, 4, 5, 6, 25, 33, 34, 38, 41, 52);
$k=0;
for ($i=0;$k<6;$i++) {
     $ukod=rand(1,$id);
     $varsa = 0;
     for($j=0;$j<$k;$j++) {
         if($a[$j] == $ukod) {
             $varsa = 1;
             break;
         }
     }
/*
     if($varsa == 0) {
         for($j=0;$j<10;$j++) {
             if($d[$j] == $ukod) {
                 $varsa = 1;
                 break;
             }
         }
     }
*/
     if($varsa == 0) {
        $a[$k] = $ukod;
        $k++;
     }
}
for ($j=0;$j<6;$j++) {
    $b = 99;
    for ($i=0;$i<6;$i++) {
        if($b > $a[$i]) {
            $k = $i;
            $b = $a[$i];
        }
    }
    $c[$j] = $b;
    $a[$k]=99;
}
for ($i=0;$i<6;$i++) {
     if(strlen($c[$i]) < 2)
        echo("0" . $c[$i] . " ");
     else 
        echo($c[$i] . " ");
}
?>
