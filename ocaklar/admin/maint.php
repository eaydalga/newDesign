<?php
   require("../include/conf.inc");
   require("../include/muser.inc");
   require("../include/maint.inc");
   // db baglantısı gerekli
   $baglan = mysql_connect($dbhost, $dbuser, $dbpasswd)
             or die("Bağlantı kurulamadı" . mysql_error());
   mysql_select_db($dbname, $baglan);
   function user_tbl($islem) {
       global $userno1,$email,$passwd,$fullname,$usrkod;
       global $pp;
       global $result;
       global $mystatus;

       $mystatus = "";
       if($islem == "R") {
           $sorgu = "SELECT email,passwd,fullName FROM user where usrNo='$userno1' and usrkod='$usrkod'";
           list($email,$pp,$fullname) = read_row($sorgu,$userno1);
       }
       if($islem == "U") {
           if(($passwd != "bos") && (strlen($passwd) >= 4))
                $pp = crypt($passwd,"aA");
           else $pp = "bos";
           $sorgu = "UPDATE user set email='$email', passwd='$pp', fullName='$fullname', usrkod='$usrkod' where usrNo='$userno1'";
           $mystatus = update_row($sorgu);
       }
       if($islem == "D") {
           $sorgu = "DELETE from user where usrNo='$userno1'";
           $bsonuc = mysql_query($sorgu);
           @mysql_freeresult($bsonuc);
       }
       if($islem == "I") {
           if(($passwd != "bos") && (strlen($passwd) >= 4))
                $pp = crypt($passwd,"aA");
           else $pp = "bos";
           $sorgu = "INSERT INTO user (usrNo, email, passwd, fullName, usrkod) values('$userno1','$email','$pp','$fullname','$usrkod')";
           $mystatus = insert_row($sorgu);
       }
       if($islem == "N" || $islem == "P") {
           if($islem == "N")
                $sorgu = "SELECT usrNo,email,passwd,fullName FROM user where usrNo>'$userno1' and usrkod='1'";
           else $sorgu = "SELECT usrNo,email,passwd,fullName FROM user where usrNo<'$userno1' and usrkod='1'";
           list($userno1,$email,$passwd,$fullname) = multi_row($islem,$sorgu);
       }
       if($islem == "L") {
           $sorgu = "SELECT * FROM user";
           $result = mysql_query($sorgu);
           $mystatus = mysql_error();
       }
       @mysql_close($baglan);
       }
   function personel_tbl($islem) {
       global $userno2,$bilgikodu,$bilgialani,$bkod;
       global $result;
       global $mystatus;
       if($islem == "R") {
           $sorgu = "SELECT bilgikodu,bilgialani FROM personel where usrNo='$userno2' and bkod='$bkod'";
           list($bilgikodu,$bilgialani) = read_row($sorgu,$userno2);
       }
       if($islem == "U") {
           $sorgu = "UPDATE personel set bilgiKodu='$bilgikodu', bilgiAlani='$bilgialani' where usrNo='$userno2'";
           $mystatus = update_row($sorgu);
       }
       if($islem == "D") {
           $sorgu = "DELETE from personel where usrNo='$userno2'";
           $bsonuc = mysql_query($sorgu);
           @mysql_freeresult($bsonuc);
       }
       if($islem == "I") {
           $sorgu = "INSERT INTO personel (usrNo, bilgiKodu, bilgiAlani, bkod) values('$userno2','$bilgikodu','$bilgialani','$bkod')";
           $mystatus = insert_row($sorgu);
       }
       if($islem == "N" || $islem == "P") {
           if($islem == "N")
                $sorgu = "SELECT usrNo,bilgiKodu,bilgiAlani FROM personel where usrNo>'$userno2' and bkod='$bkod'";
           else $sorgu = "SELECT usrNo,bilgiKodu,bilgiAlani FROM personel where usrNo<'$userno2' and bkod='$bkod'";
           list($userno2,$bilgikodu,$bilgialani) = multi_row($islem,$sorgu);
       }
       if($islem == "L") {
           $sorgu = "SELECT * FROM personel";
           $result = mysql_query($sorgu);
           $mystatus = mysql_error();
       }
       @mysql_close($baglan);
       }
   function friend_tbl($islem) {
       global $userno3,$arkadasno,$ukod;
       global $result;
       global $mystatus;
       if($islem == "R") {
           $sorgu = "SELECT friendNo FROM friends where usrNo='$userno3' and ukod='1'";
           list($arkadasno) = read_row($sorgu,$userno3);
       }
       if($islem == "U") {
           $sorgu = "UPDATE friends set friendNo='$arkadasno' where usrNo='$userno3', ukod='$ukod'";
           $mystatus = update_row($sorgu);
       }
       if($islem == "D") {
           $sorgu = "DELETE from friends where usrNo='$userno3'";
           $bsonuc = mysql_query($sorgu);
           @mysql_freeresult($bsonuc);
       }
       if($islem == "I") {
           $sorgu = "INSERT INTO friends (usrNo, friendNo, ukod) values('$userno3','$arkadasno','$ukod')";
           $mystatus = insert_row($sorgu);
       }
       if($islem == "N" || $islem == "P") {
           if($islem == "N")
                $sorgu = "SELECT usrNo,friendNo FROM friends where usrNo>'$userno3' and ukod='1'";
           else $sorgu = "SELECT usrNo,friendNo FROM friends where usrNo<'$userno3' and ukod='1'";
           list($userno3,$arkadasno) = multi_row($islem,$sorgu);
       }
       if($islem == "L") {
           $sorgu = "SELECT * FROM friends";
           $result = mysql_query($sorgu);
           $mystatus = mysql_error();
       }
       @mysql_close($baglan);
       }
   function uygbaslik_tbl($islem) {
       global $uygkodu4,$uygsecno4,$uygadi4;
       global $result;
       global $mystatus;
       if($islem == "R") {
           $sorgu = "SELECT uygKodu,uygSecNo,uygAdi FROM uygbaslik where uygKodu='$uygkodu'";
           list($uygkodu4,$uygsecno4,$uygadi4) = read_row($sorgu,$uygkodu);
       }
       if($islem == "U") {
           $sorgu = "UPDATE uygbaslik set uygSecNo='$uygsecno4', uygAdi='$uygadi4' where usrNo='$uygkodu4'";
           $mystatus = update_row($sorgu);
       }
       if($islem == "D") {
           $sorgu = "DELETE from uygbaslik where uygKodu='$uygkodu4'";
           $bsonuc = mysql_query($sorgu);
           @mysql_freeresult($bsonuc);
       }
       if($islem == "I") {
           $sorgu = "INSERT INTO uygbaslik (uygKodu, uygSecNo, uygAdi) values('$uygkodu4','$uygsecno4','$uygadi4')";
           $mystatus = insert_row($sorgu);
       }
       if($islem == "N" || $islem == "P") {
           if($islem == "N")
                $sorgu = "SELECT uygKodu,uygSecNo,uygAdi FROM uygbaslik where uygKodu>'$uygkodu4'";
           else $sorgu = "SELECT uygKodu,uygSecNo,uygAdi FROM uygbaslik where uygKodu<'$uygkodu4'";
           list($uygkodu4,$uygsecno,$uygadi) = multi_row($islem,$sorgu);
       }
       if($islem == "L") {
           $sorgu = "SELECT * FROM uygbaslik";
           $result = mysql_query($sorgu);
           $mystatus = mysql_error();
       }
       @mysql_close($baglan);
       }
   function uygsatir_tbl($islem) {
       global $uygsecno5,$uygsayi5,$uyghtml5, $uyginf5,$uygoutf5,$uygters,$uygiptal,$uygatla,$uygdevam;
       global $result;
       global $mystatus;
       if($islem == "R") {
           $sorgu = "SELECT uygSecNo,uygSayi,uygHtml,uygInf,uygOutf,uygTers,uygDevam,uygIptal,uygAtla FROM uygsatir where uygSecNo='$uygsecno5' and uygSatir='$uygsayi5'";
           list($uygsecno5, $uygsayi5, $uyghtml5, $uyginf5, $uygoutf5, $uygters, $uygdevam, $uygiptal, $uygatla) = read_row($sorgu,"$uygsecno5 $uygsayi5");
       }
       if($islem == "U") {
           $sorgu = "UPDATE uygsatir set uygHtml='$uyghtml5', uygInf='$uyginf5', uygOutf='$uygoutf5', uygTers='$uygters', uygDevam='$uygdevam', uygiptal='$uygiptal', uygAtla='$uygatla' where uygSecNo='$uygsecno5' and uygSayi='$uygsayi5'";
           $mystatus = update_row($sorgu);
       }
       if($islem == "D") {
           $sorgu = "DELETE from uygsatir where uygSecNo='$uygsecno5' and uygSayi='$uygsayi5'";
           $bsonuc = mysql_query($sorgu);
           @mysql_freeresult($bsonuc);
       }
       if($islem == "I") {
           $sorgu = "INSERT INTO uygsatir (uygSecNo,uygSayi,uygHtml,uygInf,uygOutf,uygTers,uygDevam,uygIptal,uygAtla) values('$uygsecno5', '$uygsayi5', '$uyghtml5', '$uyginf5', '$uygoutf5', '$uygters', '$uygdevam', '$uygiptal', '$uygatla')";
           $mystatus = insert_row($sorgu);
       }
       if($islem == "N" || $islem == "P") {
           if($islem == "N")
                $sorgu = "SELECT uygSecNo,uygSayi,uygHtml,uygInf,uygOutf,uygTers,uygDevam,uygIptal,uygAtla FROM uygsatir where uygSecNo>'$uygsecno5'";
           else $sorgu = "SELECT uygSecNo,uygSayi,uygHtml,uygInf,uygOutf,uygTers,uygDevam,uygIptal,uygAtla FROM uygsatir where uygSecNo<'$uygsecno5'";
           list($uygsecno5, $uygsayi5, $uyghtml5, $uyginf5, $uygoutf5, $uygters, $uygdevam, $uygiptal, $uygatla) = multi_row($islem,$sorgu);
       }
       if($islem == "L") {
           $sorgu = "SELECT * FROM uygsatir";
           $result = mysql_query($sorgu);
           $mystatus = mysql_error();
       }
       @mysql_close($baglan);
       }
   function uygulama_tbl($islem) {
       global $uygsecno6,$uygsecadi6,$uygbilgi;
       global $result;
       global $mystatus;
       if($islem == "R") {
           $sorgu = "SELECT uygSecNo,uygSecAdi,uygBilgi FROM uygulama where uygSecNo='$uygsecno6'";
           list($uygsecno6,$uygsecadi6,$uygbilgi) = read_row($sorgu,$uygsecno6);
       }
       if($islem == "U") {
           @mysql_freeresult($bsonuc);
           $sorgu = "UPDATE uygulama set uygSecAdi='$uygsecadi6', uygBilgi='$uygbilgi' where uygSecNo='$uygsecno6'";
           $mystatus = update_row($sorgu);
       }
       if($islem == "D") {
           $sorgu = "DELETE from uygulama where uygSecNo='$uygsecno6'";
           $bsonuc = mysql_query($sorgu);
           @mysql_freeresult($bsonuc);
       }
       if($islem == "I") {
           $sorgu = "INSERT INTO uygulama (uygSecNo, uygSecAdi, uygBilgi) values('$uygsecno6','$uygsecadi6','$uygbilgi6')";
           $mystatus = insert_row($sorgu);
       }
       if($islem == "N" || $islem == "P") {
           if($islem == "N")
                $sorgu = "SELECT uygSecNo,uygSecAdi,uygBilgi FROM uygulama where uygSecNo>'$uygsecno6'";
           else $sorgu = "SELECT uygSecNo,uygSecAdi,uygBilgi FROM uygulama where uygSecNo<'$uygsecno6'";
           list($uygsecno6,$uygsecadi,$uygbilgi) = multi_row($islem,$sorgu);
       }
       if($islem == "L") {
           $sorgu = "SELECT * FROM uygulama";
           $result = mysql_query($sorgu);
           $mystatus = mysql_error();
       }
       @mysql_close($baglan);
       }
   function uygalan_tbl($islem) {
       global $uygfield7,$uygfno7,$uygsz7,$uygtip7,$uygdesc7,$uygkod;
       global $result;
       global $mystatus;
       if($islem == "R") {
           $sorgu = "SELECT uygField,uygFNo,uygSz,uygTip,uygDesc FROM uygalan where uygField='$uygfield7' and uygkod='$uygkod'";
           list($uygfield7,$uygfno7,$uygsz7,$uygtip7,$uygdesc7) = read_row($sorgu,$uygfield7);
       }
       if($islem == "U") {
           $sorgu = "UPDATE uygalan set uygSz='$uygsz7', uygTip='$uygtip7', uygDesc='$uygdesc7', uygkod='$uygkod' where uygField='$uygfield7' and uygFNo='$uygfno7'";
           $mystatus = update_row($sorgu);
       }
       if($islem == "D") {
           $sorgu = "DELETE from uygalan where uygField='$uygfield7' and uygFNo='$uygfno7'";
           $bsonuc = mysql_query($sorgu);
           @mysql_freeresult($bsonuc);
       }
       if($islem == "I") {
           $sorgu = "INSERT INTO uygalan (uygField, uygFNo, uygSz, uygTip, uygDesc, uygkod) values('$uygfield7','$uygfno7','$uygsz7','$uygtip7','$uygdesc7','$uygkod')";
           $mystatus = insert_row($sorgu);
       }
       if($islem == "N" || $islem == "P") {
           if($islem == "N")
                $sorgu = "SELECT uygField,uygFNo,uygSz,uygTip,uygDesc FROM uygalan where uygField>'$uygfield7' and uygkod='$uygkod'";
           else $sorgu = "SELECT uygField,uygFNo,uygSz,uygTip,uygDesc FROM uygalan where uygField<'$uygfield7' and uygkod='$uygkod'";
           list($uygfield7,$uygfno7,$uygsz7,$uygtip7,$uygdesc7) = multi_row($islem,$sorgu);
       }
       if($islem == "L") {
           $sorgu = "SELECT * FROM uygalan";
           $result = mysql_query($sorgu);
           $mystatus = mysql_error();
       }
       @mysql_close($baglan);
       }
   function istek_tbl($islem) {
       global $tono,$istekno,$fromno,$uygkodu,$uygmsg,$ikod;
       global $result;
       global $mystatus;
       if($islem == "R") {
           $sorgu = "SELECT fromNo,uygKodu,uygMsg FROM istek where toNo='$tono' and istekNo='$istekno' and ikod='$ikod'";
           list($fromno,$uygkodu,$uygmsg) = read_row($sorgu,$tono);
       }
       if($islem == "U") {
           $sorgu = "UPDATE istek set fromNo='$fromno', uygKodu='$uygkodu', uygMsg='$uygmsg' and ikod='$ikod' where toNo='$tono' and istekNo='$istekno'";
           $mystatus = update_row($sorgu);
       }
       if($islem == "D") {
           $sorgu = "DELETE from istek where toNo='$tono' and istekNo='$istekno'";
           $bsonuc = mysql_query($sorgu);
           @mysql_freeresult($bsonuc);
       }
       if($islem == "I") {
           $sorgu = "INSERT INTO istek (toNo, istekNo, fromNo, uygKodu, uygMsg, ikod) values('$tono','$istekno','$fromno','$uygkodu','$uygmesg','$ikod')";
           $mystatus = insert_row($sorgu);
       }
       if($islem == "N" || $islem == "P") {
           if($islem == "N")
                $sorgu = "SELECT toNo,istekNo,fromNo,uygKodu,uygMsg FROM istek where toNo>'$tono' and istekNo>'$istekno' and ikod='$ikod'";
           else $sorgu = "SELECT toNo,istekNo,fromNo,uygKodu,uygMsg FROM istek where toNo<'$tono' and istekNo<'$istekno' and ikod='$ikod'";
           list($tono,$istekno,$fromno,$uygkodu,$uygmsg) = multi_row($islem,$sorgu);
       }
       if($islem == "L") {
           $sorgu = "SELECT * FROM istek";
           $result = mysql_query($sorgu);
           $mystatus = mysql_error();
       }
       @mysql_close($baglan);
       }
   function mesaj_tbl($islem) {
       global $tono,$fromno,$msgno,$uygmsg,$uygekler;
       global $result;
       global $mystatus;
       if($islem == "R") {
           $sorgu = "SELECT fromNo,uygMsg,uygEkler FROM mesaj where toNo='$toNo' and msgNo='$msgno' and rmsgkod='$rmsgkod'";
           list($msgno,$fromNo,$uygmsg,$uygekler) = read_row($sorgu,$tono . " " . $msgno);
       }
       if($islem == "U") {
           $sorgu = "UPDATE mesaj set fromNo='$fromno', uygMsg='$uygmsg', uygEWkler='$uygekler', rmsgkod='$rmsgkod' where toNo='$tono' and msgNo='$msgno'";
           $mystatus = update_row($sorgu);
       }
       if($islem == "D") {
           $sorgu = "DELETE from mesaj where toNo='$tono' and msgNo='$istekno'";
           $bsonuc = mysql_query($sorgu);
           @mysql_freeresult($bsonuc);
       }
       if($islem == "I") {
           $sorgu = "INSERT INTO mesaj (toNo, msgNo, fromNo, uygMsg, uygEkler, rmsgkod) values('$tono','$msgno','$fromno','$uygmsg','$uygekler','$rmsgkod')";
           $mystatus = insert_row($sorgu);
       }
       if($islem == "N" || $islem == "P") {
           if($islem == "N")
                $sorgu = "SELECT toNo,msgNo,fromNo,uygMsg,uygEkler FROM mesaj where toNo>'$tono' and msgNo>'$msgno' and rmsgkod='$rmsgkod'";
           else $sorgu = "SELECT toNo,msgNo,fromNo,uygMsg,uygEkler FROM mesaj where toNo<'$tono' and msgNo<'$msgno' and rmsgkod='$rmsgkod'";
           list($tono,$msgno,$fromno,$uygmsg,$uygekler) = multi_row($islem,$sorgu);
       }
       if($islem == "L") {
           $sorgu = "SELECT * FROM mesaj";
           $result = mysql_query($sorgu);
           $mystatus = mysql_error();
       }
       @mysql_close($baglan);
       }
   function mesajek_tbl($islem) {
       global $msgno,$ekno,$ekbilgi;
       global $result;
       global $mystatus;
       if($islem == "R") {
           $sorgu = "SELECT ekBilgi FROM mesajek where msgNo='$msgno' and ekNo='$ekno'";
           list($ekbilgi) = read_row($sorgu,$ekno);
       }
       if($islem == "U") {
           $sorgu = "UPDATE mesajek set ekBilgi='$ekbilgi' where msgNo='$msgno' and ekNo='$ekno'";
           $mystatus = update_row($sorgu);
       }
       if($islem == "D") {
           $sorgu = "DELETE from mesajek where masNo='$msgno' and ekNo='$ekno'";
           $bsonuc = mysql_query($sorgu);
           @mysql_freeresult($bsonuc);
       }
       if($islem == "I") {
           $sorgu = "INSERT INTO mesajek (msgNo, ekNo, ekBilgi) values('$msgno','$ekno','$ekbilgi')";
           $mystatus = insert_row($sorgu);
       }
       if($islem == "N" || $islem == "P") {
           if($islem == "N")
                $sorgu = "SELECT msgNo,ekNo,ekBilgi FROM mesajek where msgNo>'$msgno' and ekNo>'$ekno'";
           else $sorgu = "SELECT msgNo,ekNo,ekBilgi FROM mesajek where msgNo<'$msgno' and ekNo<'$ekno'";
           list($msgno,$ekno,$ekbilgi) = multi_row($islem,$sorgu);
       }
       if($islem == "L") {
           $sorgu = "SELECT * FROM mesajek";
           $result = mysql_query($sorgu);
           $mystatus = mysql_error();
       }
       @mysql_close($baglan);
       }
   function useruyg_tbl($islem) {
       global $usrno,$uygkodu,$uygsecno,$uukod;
       global $result;
       global $mystatus;
       if($islem == "R") {
           $sorgu = "SELECT uygsecno FROM useruyg where usrNo='$usrno' and uygKodu='$uygkodu' and uukod='$uukod'";
           list($uygsecno) = read_row($sorgu,$usrno . " " . $uygkodu);
       }
       if($islem == "U") {
           $sorgu = "UPDATE useruyg set uygSecNo='$uygsecno' where usrNo='$usrno' and uygKodu='$uygkodu'";
           $mystatus = update_row($sorgu);
       }
       if($islem == "D") {
           $sorgu = "DELETE from useruyg where usrNo='$usrno' and uygKodu='$uygkodu'";
           $bsonuc = mysql_query($sorgu);
           @mysql_freeresult($bsonuc);
       }
       if($islem == "I") {
           $sorgu = "INSERT INTO useruyg (usrNo, uygKodu, uygSecNo,uukod) values('$usrNo','$uygKodu','$uygSecNo','$uukod')";
           $mystatus = insert_row($sorgu);
       }
       if($islem == "N" || $islem == "P") {
           if($islem == "N")
                $sorgu = "SELECT usrNo,uygKodu,uygSecNo FROM useruyg where uygNo>'$usrno' and uygSecNo>'$uygsecno' and uukod='$uukod'";
           else $sorgu = "SELECT usrNo,uygKodu,uygSecNo FROM useruyg where uygNo<'$usrno' and uygSecNo<'$uygsecno' and uukod='$uukod'";
           list($usrno,$uygkodu,$uygsecno) = multi_row($islem,$sorgu);
       }
       if($islem == "L") {
           $sorgu = "SELECT * FROM useruyg";
           $result = mysql_query($sorgu);
           $mystatus = mysql_error();
       }
       @mysql_close($baglan);
       }
   function ulog_tbl($islem) {
       global $userno, $logno, $logtar, $logsaat, $logkod, $logmsg;
       global $result;
       global $mystatus;
       if($islem == "R") {
           $sorgu = "SELECT logNo, logTar, logSaat, logKod, logMsg FROM ulog where userNo='$userno'";
           list($logno, $logtar, $logsaat, $logkod, $logmsh) = read_row($sorgu,$userno);
       }
       if($islem == "U") {
           $sorgu = "UPDATE ulog set logNo='$logno', logTar='$logtar', logSaat='$logsaat', $logkod='$logkod', $logMsg='$logmsg' where userNo='$userno'";
           $mystatus = update_row($sorgu);
       }
       if($islem == "D") {
           $sorgu = "DELETE from ulog where usrNo='$userno'";
           $bsonuc = mysql_query($sorgu);
           @mysql_freeresult($bsonuc);
       }
       if($islem == "I") {
           $sorgu = "INSERT INTO ulog (userNo, logNo, logTar, logSaat, logKod, logMsg) values('$userno','$logno','$logtar','$logsaat','$logkod','$logmsg')";
           $mystatus = insert_row($sorgu);
       }
       if($islem == "N" || $islem == "P") {
           $sorgu = "SELECT logNo, logTar, logSaat, logKod, logMsg FROM ulog where userNo='$userno'";
           if($islem == "N")
                $sorgu = "SELECT userNo,logNo,logTar,logSaat,logKod,logMsg FROM useruyg where userNo>'$userno' and logNo>'$logno'";
           else $sorgu = "SELECT userNo,logNo,logTar,logSaat,logKod,logMsg FROM useruyg where userNo<'$userno' and logNo<'$logno'";
           list($userno,$logno,$logsaat,$logkod,$logmsg) = multi_row($islem,$sorgu);
       }
       if($islem == "L") {
           $sorgu = "SELECT * FROM useruyg";
           $result = mysql_query($sorgu);
           $mystatus = mysql_error();
       }
       @mysql_close($baglan);
       }
   function haber_tbl($islem) {
       global $haberno, $baslangic, $bitis, $dosyaadi, $hkod;
       global $result;
       global $mystatus;
       if($islem == "R") {
           $sorgu = "SELECT baslangic,bitis,dosyaAdi FROM haber where haberNo='$haberno' and hkod='$hkod'";
           list($baslangic,$bitis,$dosyaadi) = read_row($sorgu,$haberno . " " . $hkod);
       }
       if($islem == "U") {
           $sorgu = "UPDATE haber set baslangic='$baslangic', bitis='$bitis', dosyaAdi='$dosyaadi', hkod='$hkod' where haberNo='$haberno'";
           $mystatus = update_row($sorgu);
       }
       if($islem == "D") {
           $sorgu = "DELETE from haber where haberNo='$haberno'";
           $bsonuc = mysql_query($sorgu);
           @mysql_freeresult($bsonuc);
       }
       if($islem == "I") {
           $sorgu = "INSERT INTO haber (haberNo, baslangic, bitis,dosyaAdi, hkod) values('$haberno','$baslangic','$bitis','$dosyaadi','$hkod')";
           $mystatus = insert_row($sorgu);
       }
       if($islem == "N" || $islem == "P") {
           if($islem == "N")
                $sorgu = "SELECT haberNo,baslangic,bitis,dosyaAdi,hkod FROM haber where haberNo>'$haberno'";
           else $sorgu = "SELECT haberNo,baslangic,bitis,dosyaAdi,hkod FROM haber where haberNo>'$haberno'";
           list($haberno,$baslangic,$bitis,$dosyaadi,$hkod) = multi_row($islem,$sorgu);
       }
       if($islem == "L") {
           $sorgu = "SELECT * FROM haber";
           $result = mysql_query($sorgu);
           $mystatus = mysql_error();
       }
       @mysql_close($baglan);
       }
   function reklam_tbl($islem) {
       global $reklamno, $rkod, $bastar, $bittar, $dosyaadi;
       global $result;
       global $mystatus;
       if($islem == "R") {
           $sorgu = "SELECT basTar,bitTar,dosyaAdi,hkod FROM reklam where reklamNo='$reklamno' and rkod='$rkod'";
           list($bastar,$bittar,$dosyaadi,$hkod) = read_row($sorgu,$reklamno . " " . $rkod);
       }
       if($islem == "U") {
           $sorgu = "UPDATE reklam set basTar='$bastar', bitTar='$bittar', dosyaAdi='$dosyaadi' where reklamNo='$reklamno' and rkod='$rkod'";
           $mystatus = update_row($sorgu);
       }
       if($islem == "D") {
           $sorgu = "DELETE from reklam where reklamNo='$reklamno' and rkod='$rkod'";
           $bsonuc = mysql_query($sorgu);
           @mysql_freeresult($bsonuc);
       }
       if($islem == "I") {
           $sorgu = "INSERT INTO reklam (reklamNo, basTar, bitTar,dosyaAdi,hkod) values('$reklamno','$bastar','$bittar','$dosyaadi','$hkod')";
           $mystatus = insert_row($sorgu);
       }
       if($islem == "N" || $islem == "P") {
           if($islem == "N")
                $sorgu = "SELECT reklamNo,rkod,basTar,bitTar,dosyaAdi FROM reklam where reklanNo>'$reklamno' and rkod='$rkod'";
           else $sorgu = "SELECT reklamNo,rkod,basTar,bitTar,dosyaAdi FROM reklam where reklanNo<'$reklamno' and rkod='$rkod'";
           list($reklamno,$rkod,$bastar,$bittar,$dosyaadi) = multi_row($islem,$sorgu);
       }
       if($islem == "L") {
           $sorgu = "SELECT * FROM reklam";
           $result = mysql_query($sorgu);
           $mystatus = mysql_error();
       }
       @mysql_close($baglan);
       }
/* ********************************** main program *********************** */
   if(isset($_REQUEST['fname'])) {
      // assign variables
      $fname = $_REQUEST['fname'];
      $islem = $_REQUEST['islem'];
      if(!isset($dvar))
          $dvar=1;
      switch($fname) {
              case 'form1':
                        $fuser = 'user1';
                        $userno1 = $_REQUEST['userno1'];
                        $email = $_REQUEST['email'];
                        $passwd = $_REQUEST['passwd'];
                        $fullname = $_REQUEST['fullname'];
                        $usrkod = $_REQUEST['usrkod'];
                        // user tablosu
                        user_tbl($islem);
                        break;
              case 'form2':
                        $fuser = 'user2';
                        $userno2    = $_REQUEST['userno2'];
                        $bilgikodu  = $_REQUEST['bilgikodu'];
                        $bilgialani = $_REQUEST['bilgialani'];
                        $bkod = $_REQUEST['bkod'];
                        // personel tablosu
                        personel_tbl($islem);
                        break;
              case 'form3':
                        $fuser = 'user3';
                        $userno3   = $_REQUEST['userno3'];
                        $arkadasno = $_REQUEST['arkadasno'];
                        $ukod = $_REQUEST['ukod'];
                        // friend tablosu
                        friend_tbl($islem);
                        break;
              case 'form4':
                        $fuser = 'user4';
                        $uygkodu4 = $_REQUEST['uygkodu4'];
                        $uygsecno4 = $_REQUEST['uygsecno4'];
                        $uygadi4 = $_REQUEST['uygadi4'];
                        // uygbaslik tablosu
                        uygbaslik_tbl($islem);
                        break;
              case 'form5':
                        $fuser = 'user5';
                        $uygsecno5 = $_REQUEST['uygsecno5'];
                        $uygsayi5 = $_REQUEST['uygsayi5'];
                        $uyghtml5 = $_REQUEST['uyghtml5'];
                        $uyginf5 = $_REQUEST['uyginf5'];
                        $uygoutf5 = $_REQUEST['uygoutf5'];
                        $uygters = $_REQUEST['uygters'];
                        $uygdevam = $_REQUEST['uygdevam'];
                        $uygiptal = $_REQUEST['uygiptal'];
                        $uygatla = $_REQUEST['uygatla'];
                        // uygsatir tablosu
                        uygsatir_tbl($islem);
                        break;
              case 'form6':
                        $fuser = 'user6';
                        $uygsecno6 = $_REQUEST['uygsecno6'];
                        $uygsecadi6 = $_REQUEST['uygsecadi6'];
                        $uygsecbilgi = $_REQUEST['uygsecbilgi'];
                        // uygulama tablosu
                        uygulama_tbl($islem);
                        break;
              case 'form7':
                        $fuser = 'user7';
                        $uygfield7 = $_REQUEST['uygfield7'];
                        $uygfno7 = $_REQUEST['uygfno7'];
                        $uygsz7 = $_REQUEST['uygsz7'];
                        $uygtip7 = $_REQUEST['uygtip7'];
                        $uygdesc7 = $_REQUEST['uygdesc7'];
                        $uygkod = $_REQUEST['uygkod'];
                        // uygalan tablosu
                        uygalan_tbl($islem);
                        break;
              case 'form8':
                        $fuser = 'user8';
                        $tono = $_REQUEST['tono'];
                        $istekno = $_REQUEST['istekno'];
                        $fromno = $_REQUEST['fromno'];
                        $uygkodu = $_REQUEST['uygkodu'];
                        $uygmsg = $_REQUEST['uygmsg'];
                        $ikod = $_REQUEST['ikod'];
                        // istek tablosu
                        istek_tbl($islem);
                        break;
              case 'form9':
                        $fuser = 'user9';
                        $tobo = $_REQUEST['tono'];
                        $fromno = $_REQUEST['fromno'];
                        $msgno = $_REQUEST['msgno'];
                        $uygmsg = $_REQUEST['uygmsg'];
                        $rmsgkod = $_REQUEST['rmsgkod'];
                        // mesaj tablosu
                        mesaj_tbl($islem);
                        break;
              case 'form10':
                        $fuser = 'user10';
                        $msgno = $_REQUEST['msgno'];
                        $ekno = $_REQUEST['ekno'];
                        $ekbilgi = $_REQUEST['ekbilgi'];
                        // mesaj ekleri tablosu
                        mesajek_tbl($islem);
                        break;
              case 'form11':
                        $fuser = 'user11';
                        $msgno = $_REQUEST['usrno'];
                        $ekno = $_REQUEST['uygkodu'];
                        $ekbilgi = $_REQUEST['uygsecno'];
                        $uukod = $_REQUEST['uukod'];
                        // mesaj ekleri tablosu
                        useruyg_tbl($islem);
                        break;
              case 'form12':
                        $fuser = 'user12';
                        $userno=$_REQUEST['userNo'];
                        $logno=$_REQUEST['logNo'];
                        $logtar=$_REQUEST['logTar'];
                        $logsaat=$_REQUEST['logSaat'];
                        $logkod=$_REQUEST['logKod'];
                        $logmsg=$_REQUEST['logMsg'];
                        log_tbl($islem);
                        break;
              case 'form13':
                        $fuser = 'user13';
                        $haberno= $_REQUEST['haberNo'];
                        $baslangic= $_REQUEST['baslangic'];
                        $bitis= $_REQUEST['bitis'];
                        $dosyaadi= $_REQUEST['dosyaAdi'];
                        $hkod= $_REQUEST['hkod'];
                        haber_tbl($islem);
                        break;
              case 'form14':
                        $fuser = 'user14';
                        $reklamno = $_REQUEST['reklamNo'];
                        $rkod = $_REQUEST['rkod'];
                        $bastar = $_REQUEST['basTar'];
                        $bittar = $_REQUEST['bitTar'];
                        $dosyaadi = $_REQUEST['dosyaAdi'];
                        reklam_tbl($islem);
                        break;
      }
   } else {
      $dvar = 0;
      $mystatus = "";
      $pp = "bos";
   }
?>
<html>
<head>
<link rel="stylesheet" type="text/css" href="../main.css" media="screen" />
<script language="JavaScript" src="../js/browser.js"> </script>
<script language="JavaScript" src="../js/email.js"> </script>
<script language="JavaScript" src="../js/date.js"> </script>
<script language="JavaScript" src="../js/float.js"> </script>
<script language="JavaScript" src="../js/baslik.js"> </script>
<script>
function secim(m)
{
    if(document.getElementById) {
       clearbase();
       target=document.getElementById(m);
       if(target) {
             target.style.display = "inline";
       }
    }
}
function ikontrol(ff)
{
    var ik_errstr = "";
    var err;
    tt = document.forms[ff];
    ttin = "";
    for (i=0;i<tt.islem.length;i++) {
	if (tt.islem[i].checked) {
		ttin = tt.islem[i].value;
	}
    }
    if(ttin == "") {
        alert("işlem secimi yapılmamış");
        return false;
    }
    if(ttin == "I" || ttin == "U") {
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
    }
    document.forms[ff].submit();
}
</script>
</head>
<body>
<span class="yyazi"><center><h1>Yönetim İşlemleri</h1></center></span>
<table class="yyazi">
<tr><td valign="top" width="200">
<div id="soltaraf">
    <div id="ymenublock">
         <font color="#8e9ccb"><h3>Kullanıcılar</h3></font>
         <a href="javascript:secim('user1')" class="nav">Kullanıcı Bakımı</a>
         <a href="javascript:secim('user2')" class="nav">Kişisel Bilgi Bakımı</a>
         <a href="javascript:secim('user3')" class="nav">Arkadaş Bakımı</a>
         <font color="#8e9ccb"><h3>Uygulamalar</h3></font>
         <a href="javascript:secim('user4')" class="nav">Başlık Bakımı</a>
         <a href="javascript:secim('user5')" class="nav">Satır Bakımı</a>
         <a href="javascript:secim('user6')" class="nav">Uygulama Bakımı</a>
         <a href="javascript:secim('user7')" class="nav">Alan Bakımı</a>
         <font color="#8e9ccb"><h3>Kullanıcı Uygulamaları</h3></font>
         <a href="javascript:secim('user11')" class="nav">KulUyg Bakımı </a>
         <font color="#8e9ccb"><h3>Istek ve Mesaj</h3></font>
         <a href="javascript:secim('user8')" class="nav">Istek Bakımı</a>
         <a href="javascript:secim('user9')" class="nav">Mesaj Bakımı</a>
         <a href="javascript:secim('user10')" class="nav">Mesaj Eklentisi</a>
         <font color="#8e9ccb"><h3>Diğer Dosyalar</h3></font>
         <a href="javascript:secim('user12')" class="nav">Islem Kaydı</a>
         <a href="javascript:secim('user13')" class="nav">Haberle</a>
         <a href="javascript:secim('user14')" class="nav">Reklamlar</a>
         <a href="index.php" class="nav"><h3>Ana Sayfa</h3></a>
    </div>
</div>
</td><td valign="top">
<div id="ortataraf">
    <div id="user1" style="display: none;">
        <h3>Kullanıcı Bakım</h3>
        <form id="form1" action="maint.php" method="post">
        <input type="hidden" name="fname" value="form1">
        <table class="yyazi">
        <tr><td>&nbsp;</td><td>
        Oku<input type="radio" name="islem" value="R">
        Yaz<input type="radio" name="islem" value="U">
        Ekle<input type="radio" name="islem" value="I">
        Sonra<input type="radio" name="islem" value="N">
        Önce<input type="radio" name="islem" value="P">
        Liste<input type="radio" name="islem" value="L">
        </td></tr>
        <tr><td>userNo</td><td>
<?php
    if(isset($userno1)) {
        echo("<input type=\"text\" name=\"userno1\" class=\"required\" size=\"17\" value=\"$userno1\"></td></tr>");
        echo("<tr><td>email</td><td>");
        echo("<input type=\"text\" name=\"email\" size=\"17\" value=\"$email\"></td></tr>");
        echo("<tr><td>passwd</td><td>");
        echo("<input type=\"text\" name=\"passwd\" size=\"17\" value=\"$pp\"></td></tr>");
        echo("<tr><td>fullName</td><td>");
        echo("<input type=\"text\" name=\"fullname\" size=\"17\" value=\"$fullname\">");
        echo("</td></tr>");
        echo("<tr><td>usrkod</td><td>");
        echo("<input type=\"text\" name=\"usrkod\" class=\"validate-alphanum\" size=\"17\">");
      } else {
?>
        <input type="text" name="userno1" class="required" size="17">
        </td></tr>
        <tr><td>email</td><td>
        <input type="text" name="email" class="validate-email" size="17">
        </td></tr>
        <tr><td>passwd</td><td>
        <input type="text" name="passwd" class="required" size="17">
        </td></tr>
        <tr><td>fullName</td><td>
        <input type="text" name="fullname" class="validate-alphanum" size="17">
        </td></tr>
        <tr><td>usrkod</td><td>
        <input type="text" name="usrkod" class="validate-alphanum" size="17">
<?php
      }
?>
        <input type="button" class="gyazi" value="git" onclick="javascript:ikontrol('form1');">
        </td></tr>
        </table>
        </form>
    </div>
    <div id="user2" style="display: none;">
        <h3>Kişisel Bilgi Bakımı</h3>
        <form id="form2" action="maint.php" method="post">
        <input type="hidden" name="fname" value="form2">
        <table class="yyazi">
        <tr><td>&nbsp;</td><td>
        Oku<input type="radio" name="islem" value="R">
        Yaz<input type="radio" name="islem" value="U">
        Ekle<input type="radio" name="islem" value="I">
        Sonra<input type="radio" name="islem" value="N">
        Önce<input type="radio" name="islem" value="P">
        Liste<input type="radio" name="islem" value="L">
        </td></tr>
        <tr><td>userNo</td><td>
<?php
    if(isset($userno2)) {
        echo("<input type=\"text\" name=\"userno2\" class=\"required\" size=\"17\" value=\"$userno2\"></td></tr>");
        echo("<tr><td>bilgiKodu</td><td>");
        echo("<input type=\"text\" name=\"bilgikodu\" class=\"validate-integer\" size=\"17\" value=\"$bilgikodu\"></td></tr>");
        echo("<tr><td>bilgiAlanı</td><td>");
        echo("<input type=\"text\" name=\"bilgialani\" class=\"validate-alphanum\" size=\"17\" value=\"$bilgialani\">");
        echo("</td></tr>");
        echo("<tr><td>bkod</td><td>");
        echo("<input type=\"text\" name=\"bkod\" class=\"validate-alphanum\" size=\"17\">");
      } else {
?>
        <input type="text" name="userno2" class="required" size="17"></td></tr>
        <tr><td>bilgiKodu</td><td>
        <input type="text" name="bilgikodu" class="validate-integer" size="17"></td></tr>
        <tr><td>bilgiAlanı</td><td>
        <input type="text" name="bilgialani" class="validate-alphanum" size="17">
        </td></tr>
        <tr><td>bkod</td><td>
        <input type="text" name="bkod" class="validate-alphanum" size="17">
<?php
      }
?>
        <input type="button" class="gyazi" value="git" onclick="javascript:ikontrol('form2');"></td></tr>
        </table>
        </form>
    </div>
    <div id="user3" style="display: none;">
        <h3>Arkadaş Bakımı</h3>
        <form id="form3" action="maint.php" method="post">
        <input type="hidden" name="fname" value="form3">
        <table class="yyazi">
        <tr><td>&nbsp;</td><td>
        Oku<input type="radio" name="islem" value="R">
        Yaz<input type="radio" name="islem" value="U">
        Ekle<input type="radio" name="islem" value="I">
        Sonra<input type="radio" name="islem" value="N">
        Önce<input type="radio" name="islem" value="P">
        Liste<input type="radio" name="islem" value="L">
        </td></tr>
        <tr><td>userNo</td><td>
<?php
    if(isset($userno3)) {
        echo("<input type=\"text\" name=\"userno3\" class=\"required\" size=\"17\" value=\"$userno3\"></td></tr>");
        echo("<tr><td>arkadasNo</td><td>");
        echo("<input type=\"text\" name=\"arkadasno\" class=\"required\" size=\"17\" value=\"$arkadasno\">");
        echo("</td></tr>");
        echo("<tr><td>ukod</td><td>");
        echo("<input type=\"text\" name=\"ukod\" class=\"vaildate-alphanum\" size=\"17\">");
      } else {
?>
        <input type="text" name="userno3" class="required" size="17"></td></tr>
        <tr><td>arkadasNo</td><td>
        <input type="text" name="arkadasno" class="required" size="17">
        </td></tr>
        <tr><td>ukod</td><td>
        <input type="text" name="ukod" class="vaildate-alphanum" size="17">
<?php
      }
?>
        <input type="button" class="gyazi" value="git" onclick="javascript:ikontrol('form3');"></td></tr>
        </table>
        </form>
    </div>
    <div id="user4" style="display: none;">
        <h3>Başlık Bakımı</h3>
        <form id="form4" action="maint.php" method="post">
        <input type="hidden" name="fname" value="form4">
        <table class="yyazi">
        <tr><td>&nbsp;</td><td>
        Oku<input type="radio" name="islem" value="R">
        Yaz<input type="radio" name="islem" value="U">
        Ekle<input type="radio" name="islem" value="I">
        Sonra<input type="radio" name="islem" value="N">
        Önce<input type="radio" name="islem" value="P">
        Liste<input type="radio" name="islem" value="L">
        </td></tr>
        <tr><td>uygKodu</td><td>
<?php
    if(isset($uygkodu4)) {
        echo("<input type=\"text\" name=\"uygkodu4\" class=\"integer\" size=\"17\" value=\"$uygkodu4\"></td></tr>");
        echo("<tr><td>uygSecNo</td><td>");
        echo("<input type=\"text\" name=\"uygsecno4\" class=\"integer\" size=\"17\" value=\"$uygsecno4\"></td></tr>");
        echo("<tr><td>uygAdi</td><td>");
        echo("<input type=\"text\" name\"uygadi4\" class=\"required\" size=\"17\" value=\"$uygadi4\">");
      } else {
?>
        <input type="text" name="uygkodu4" class="integer" size="17"></td></tr>
        <tr><td>uygSecNo</td><td>
        <input type="text" name="uygsecno4" class="integer" size="17"></td></tr>
        <tr><td>uygAdi</td><td>
        <input type="text" name"uygadi4" class="required" size="17">
<?php
      }
?>
        <input type="button" class="gyazi" value="git" onclick="javascript:ikontrol('form4');"></td></tr>
        </table>
        </form>
    </div>
    <div id="user5" style="display: none;">
        <h3>Satır Bakımı</h3>
        <form id="form5" action="maint.php" method="post">
        <input type="hidden" name="fname" value="form5">
        <table class="yyazi">
        <tr><td>&nbsp;</td><td>
        Oku<input type="radio" name="islem" value="R">
        Yaz<input type="radio" name="islem" value="U">
        Ekle<input type="radio" name="islem" value="I">
        Sonra<input type="radio" name="islem" value="N">
        Önce<input type="radio" name="islem" value="P">
        Liste<input type="radio" name="islem" value="L">
        </td></tr>
        <tr><td>uygSecNo</td><td>
<?php
    if(isset($uygsecno5)) {
        echo("<input type=\"text\" name=\"uygsecno5\" class=\"integer\" size=\"17\" value=\"$uygsecno5\"></td></tr>");
        echo("<tr><td>uygSayi</td><td>");
        echo("<input type=\"text\" name=\"uygsayi5\" class=\"integer\" size=\"17\" value=\"$uygsayi5\"></td></tr>");
        echo("<tr><td>uygHtml</td><td>");
        echo("<input type=\"text\" name=\"uyghtml5\" class=\"validate-alphanum\" size=\"17\" value=\"$uyghtml5\"></td></tr>");
        echo("<tr><td>uygInf</td><td>");
        echo("<input type=\"text\" name=\"uyginf5\" class=\"validate-alphanum\" size=\"17\" value=\"$uyginf5\"></td></tr>");
        echo("<tr><td>uygOutf</td><td>");
        echo("<input type=\"text\" name=\"uygoutf5\" class=\"validate-alphanum\" size=\"17\" value=\"$uygoutf5\"></td></tr>");
        echo("<tr><td>uygTers</td><td>");
        echo("<input type=\"checkbox\" name=\"uygters\" value=\"T\"></td></tr>");
        echo("<tr><td>uygDevam</td><td>");
        echo("<input type=\"checkbox\" name=\"uygdevam\" value=\"D\"></td></tr>");
        echo("<tr><td>uygIptal</td><td>");
        echo("<input type=\"checkbox\" name=\"uygiptal\" value=\"I\"></td></tr>");
        echo("<tr><td>uygAtla</td><td>");
        echo("<input type=\"checkbox\" name=\"uygatla\" value=\"A\">");
      } else {
?>
        <input type="text" name="uygsecno5" class="integer" size="17"></td></tr>
        <tr><td>uygSayi</td><td>
        <input type="text" name="uygsayi5" class="integer" size="17"></td></tr>
        <tr><td>uygHtml</td><td>
        <input type="text" name="uyghtml5" class="validate-alphanum" size="17"></td></tr>
        <tr><td>uygInf</td><td>
        <input type="text" name="uyginf5" class="validate-alphanum" size="17"></td></tr>
        <tr><td>uygOutf</td><td>
        <input type="text" name="uygoutf5" class="validate-alphanum" size="17"></td></tr>
        <tr><td>uygTers</td><td>
        <input type="checkbox" name="uygters" value="T"></td></tr>
        <tr><td>uygDevam</td><td>
        <input type="checkbox" name="uygdevam" value="D"></td></tr>
        <tr><td>uygIptal</td><td>
        <input type="checkbox" name="uygiptal" value="I"></td></tr>
        <tr><td>uygAtla</td><td>
        <input type="checkbox" name="uygatla" value="A">
<?php
      }
?>
        <input type="button" class="gyazi" value="git" onclick="javascript:ikontrol('form5');"></td></tr>
        </table>
        </form>
    </div>
    <div id="user6" style="display: none;">
        <h3>Uygulama Bakımı</h3>
        <form id="form6" action="maint.php" method="post">
        <input type="hidden" name="fname" value="form6">
        <table class="yyazi">
        <tr><td>&nbsp;</td><td>
        Oku<input type="radio" name="islem" value="R">
        Yaz<input type="radio" name="islem" value="U">
        Ekle<input type="radio" name="islem" value="I">
        Sonra<input type="radio" name="islem" value="N">
        Önce<input type="radio" name="islem" value="P">
        Liste<input type="radio" name="islem" value="L">
        </td></tr>
        <tr><td>uygSecNo</td><td>
<?php
    if(isset($uygsecno6)) {
        echo("<input type=\"text\" name=\"uygsecno6\" class=\"integer\" size=\"17\" value=\"$uygsecno6\"></td></tr>");
        echo("<tr><td>uygSecAdi</td><td>");
        echo("<input type=\"text\" name=\"uygsecadi6\" class=\"validate-alphanum\" size=\"17\" value=\"$uygsecadi6\"></td></tr>");
        echo("<tr><td>uygSecBilgi</td><td>");
        echo("<textarea name=\"uygsecbilgi\" col=\"40\" row=\"5\">$uygsecbilgi</textarea>");
      } else {
?>
        <input type="text" name="uygsecno6" class="integer" size="17"></td></tr>
        <tr><td>uygSecAdi</td><td>
        <input type="text" name="uygsecadi6" class="validate-alphanum" size="17"></td></tr>
        <tr><td>uygSecBilgi</td><td>
        <textarea name="uygsecbilgi" col="40" row="5"></textarea>
<?php
      }
?>
        <input type="button" class="gyazi" value="git" onclick="javascript:ikontrol('form6');"></td></tr>
        </table>
        </form>
    </div>
    <div id="user7" style="display: none;">
        <h3>Alan Bakımı</h3>
        <form id="form7" action="maint.php">
        <input type="hidden" name="fname" value="form7">
        <table class="yyazi">
        <tr><td>&nbsp;</td><td>
        Oku<input type="radio" name="islem" value="R">
        Yaz<input type="radio" name="islem" value="U">
        Ekle<input type="radio" name="islem" value="I">
        Sonra<input type="radio" name="islem" value="N">
        Önce<input type="radio" name="islem" value="P">
        Liste<input type="radio" name="islem" value="L">
        </td></tr>
        <tr><td>uygField</td><td>
<?php
    if(isset($uygfield7)) {
        echo("<input type=\"text\" name=\"uygfield7\" class=\"integer\" size=\"17\" value=\"$uygfield7\"></td></tr>");
        echo("<tr><td>uygFNo</td><td>");
        echo("<input type=\"text\" name=\"uygfno7\" class=\"integer\" size=\"17\" value=\"$uygfno7\"></td></tr>");
        echo("<tr><td>uygSz</td><td>");
        echo("<input type=\"text\" name=\"uygsz7\" class=\"integer\" size=\"17\" value=\"$uygsz7\"></td></tr>");
        echo("<tr><td>uygTip</td><td>");
        echo("<input type=\"text\" name=\"uygtip7\" class=\"validate-alphanum\" size=\"17\" value=\"$uygtip7\"></td></tr>");
        echo("<tr><td>uygDesc</td><td>");
        echo("<textarea name=\"uygdesc7\" col=\"40\" row=\"5\">$uygdesc7</textarea>");
        echo("</td></tr>");
        echo("<tr><td>uygkod</td><td>");
        echo("<input type=\"text\" name=\"uygkod\" class=\"validate-alphanum\" size=\"17\">");
      } else {
?>
        <input type="text" name="uygfield7" class="integer" size="17"></td></tr>
        <tr><td>uygFNo</td><td>
        <input type="text" name="uygfno7" class="integer" size="17"></td></tr>
        <tr><td>uygSz</td><td>
        <input type="text" name="uygsz7" class="integer" size="17"></td></tr>
        <tr><td>uygTip</td><td>
        <input type="text" name="uygtip7" class="validate-alphanum" size="17"></td></tr>
        <tr><td>uygDesc</td><td>
        <textarea name="uygdesc7" col="40" row="5"></textarea>
        </td></tr>
        <tr><td>uygkod</td><td>
        <input type="text" name="uygkod" class="validate-alphanum" size="17">
<?php
      }
?>
        <input type="button" class="gyazi" value="git" onclick="javascript:ikontrol('form7');"></td></tr>
        </table>
        </form>
    </div>

    <div id="user8" style="display: none;">
        <h3>Istek Bakımı</h3>
        <form id="form8" action="maint.php">
        <input type="hidden" name="fname" value="form8">
        <table class="yyazi">
        <tr><td>&nbsp;</td><td>
        Oku<input type="radio" name="islem" value="R">
        Yaz<input type="radio" name="islem" value="U">
        Ekle<input type="radio" name="islem" value="I">
        Sonra<input type="radio" name="islem" value="N">
        Önce<input type="radio" name="islem" value="P">
        Liste<input type="radio" name="islem" value="L">
        </td></tr>
        <tr><td>to userNo</td><td>
<?php
    if(isset($tono)) {
        echo("<input type=\"text\" name=\"tono\" class=\"required\" size=\"17\"></td></tr>");
        echo("<tr><td>Istek No</td><td>");
        echo("<input type=\"text\" name=\"istekno\" class=\"integer\" size=\"17\"></td></tr>");
        echo("<tr><td>from userNo</td><td>");
        echo("<input type=\"text\" name=\"fromno\" class=\"required\" size=\"17\"></td></tr>");
        echo("<tr><td>uyg Kodu</td><td>");
        echo("<textarea name=\"uygkodu\" col=\"40\" row=\"5\"></textarea></td></tr>");
        echo("<tr><td>uyg Mesajı</td><td>");
        echo("<textarea name=\"uygmsg\" col=\"40\" row=\"5\"></textarea>");
        echo("</td></tr>");
        echo("<tr><td>ikod</td><td>");
        echo("<input type=\"text\" name=\"ikod\" class=\"required\" size=\"17\">");
    } else {
?>
        <input type="text" name="tono" class="required" size="17"></td></tr>
        <tr><td>Istek No</td><td>
        <input type="text" name="istekno" class="integer" size="17"></td></tr>
        <tr><td>from userNo</td><td>
        <input type="text" name="fromno" class="required" size="17"></td></tr>
        <tr><td>uyg Kodu</td><td>
        <textarea name="uygkodu" col="40" row="5"></textarea></td></tr>
        <tr><td>uyg Mesajı</td><td>
        <textarea name="uygmsg" col="40" row="5"></textarea>
        </td></tr>
        <tr><td>ikod</td><td>
        <input type="text" name="ikod" class="required" size="17">
<?php
      }
?>
        <input type="button" class="gyazi" value="git" onclick="javascript:ikontrol('form8');"></td></tr>
        </table>
        </form>
    </div>
    <div id="user9" style="display: none;">
        <h3>Mesaj Bakımı</h3>
        <form id="form9" action="maint.php">
        <input type="hidden" name="fname" value="form9">
        <table class="yyazi">
        <tr><td>&nbsp;</td><td>
        Oku<input type="radio" name="islem" value="R">
        Yaz<input type="radio" name="islem" value="U">
        Ekle<input type="radio" name="islem" value="I">
        Sonra<input type="radio" name="islem" value="N">
        Önce<input type="radio" name="islem" value="P">
        Liste<input type="radio" name="islem" value="L">
        </td></tr>
        <tr><td>to userNo</td><td>
<?php
    if(isset($tono)) {
        echo("<input type=\"text\" name=\"tono\" class=\"required\" size=\"17\"></td></tr>");
        echo("<tr><td>Mesaj No</td><td>");
        echo("<input type=\"text\" name=\"msgno\" class=\"required\" size=\"17\"></td></tr>");
        echo("<tr><td>from userNo</td><td>");
        echo("<input type=\"text\" name=\"fromno\" class=\"required\" size=\"17\"></td></tr>");
        echo("<tr><td>Mesaj</td><td>");
        echo("<textarea name=\"uygmsg\" col=\"40\" row=\"5\"></textarea></td></tr>");
        echo("<tr><td>Ekler</td><td>");
        echo("<textarea name=\"uygekler\" col=\"40\" row=\"5\"></textarea>");
        echo("</td></tr>");
        echo("<tr><td>rmsgkod</td><td>");
        echo("<input type=\"text\" name=\"rmsgkod\" class=\"required\" size=\"17\">");
    } else {
?>
        <input type="text" name="tono" class="required" size="17"></td></tr>
        <tr><td>Mesaj No</td><td>
        <input type="text" name="msgno" class="required" size="17"></td></tr>
        <tr><td>from userNo</td><td>
        <input type="text" name="fromno" class="required" size="17"></td></tr>
        <tr><td>Mesaj</td><td>
        <textarea name="uygmsg" col="40" row="5"></textarea></td></tr>
        <tr><td>Ekler</td><td>
        <textarea name="uygekler" col="40" row="5"></textarea>
        </td></tr>
        <tr><td>rmsgkod</td><td>
        <input type="text" name="rmsgkod" class="required" size="17">
<?php
      }
?>
        <input type="button" class="gyazi" value="git" onclick="javascript:ikontrol('form9');"></td></tr>
        </table>
        </form>
    </div>
    <div id="user10" style="display: none;">
        <h3>Mesaj Ekleri</h3>
        <form id="form10" action="maint.php">
        <input type="hidden" name="fname" value="form10">
        <table class="yyazi">
        <tr><td>&nbsp;</td><td>
        Oku<input type="radio" name="islem" value="R">
        Yaz<input type="radio" name="islem" value="U">
        Ekle<input type="radio" name="islem" value="I">
        Sonra<input type="radio" name="islem" value="N">
        Önce<input type="radio" name="islem" value="P">
        Liste<input type="radio" name="islem" value="L">
        </td></tr>
        <tr><td>Mesaj No</td><td>
<?php
    if(isset($msgno)) {
        echo("<input type=\"text\" name=\"msgno\" class=\"required\" size=\"17\"></td></tr>");
        echo("<tr><td>Ek No</td><td>");
        echo("<input type=\"text\" name=\"ekno\" class=\"integer\" size=\"17\"></td></tr>");
        echo("<tr><td>Ek Bilgi</td><td>");
        echo("<textarea name=\"ekbilgi\" col=\"40\" row=\"5\"></textarea>");
    } else {
?>
        <input type="text" name="msgno" class="required" size="17"></td></tr>
        <tr><td>Ek No</td><td>
        <input type="text" name="ekno" class="integer" size="17"></td></tr>
        <tr><td>Ek Bilgi</td><td>
        <textarea name="ekbilgi" col="40" row="5"></textarea>
<?php
      }
?>
        <input type="button" class="gyazi" value="git" onclick="javascript:ikontrol('form10');"></td></tr>
        </table>
        </form>
    </div>
    <div id="user11" style="display: none;">
        <h3>Kullanıcı Uygulamaları</h3>
        <form id="form11" action="maint.php">
        <input type="hidden" name="fname" value="form11">
        <table class="yyazi">
        <tr><td>&nbsp;</td><td>
        Oku<input type="radio" name="islem" value="R">
        Yaz<input type="radio" name="islem" value="U">
        Ekle<input type="radio" name="islem" value="I">
        Sonra<input type="radio" name="islem" value="N">
        Önce<input type="radio" name="islem" value="P">
        Liste<input type="radio" name="islem" value="L">
        </td></tr>
        <tr><td>Üye No</td><td>
<?php
    if(isset($usrno)) {
        echo("<input type=\"text\" name=\"usrno\" class=\"required\" size=\"17\"></td></tr>");
        echo("<tr><td>Uyg Kodu</td><td>");
        echo("<input type=\"text\" name=\"uygkodu\" class=\"integer\" size=\"17\"></td></tr>");
        echo("<tr><td>Ek Bilgi</td><td>");
        echo("<input type=\"text\" name=\"uygsecno\" class=\"integer\" size=\"17\">");
        echo("</td></tr>");
        echo("<tr><td>uukod</td><td>");
        echo("<input type=\"text\" name=\"uukod\" class=\"validate-alphanum\" size=\"17\">");
    } else {
?>
        <input type="text" name="usrno" class="required" size="17"></td></tr>
        <tr><td>Uyg Kodu</td><td>
        <input type="text" name="uygkodu" class="integer" size="17"></td></tr>
        <tr><td>Uyg SecNo</td><td>
        <input type="text" name="uygSecNo" class="integer" size="17">
        </td></tr>
        <tr><td>uukod</td><td>
        <input type="text" name="uukod" class="validate-alphanum" size="17">
<?php
      }
?>
        <input type="button" class="gyazi" value="git" onclick="javascript:ikontrol('form11');"></td></tr>
        </table>
        </form>
    </div>
    <div id="user12" style="display: none;">
        <h3>Kullanıcı İşlem Kaydı</h3>
        <form id="form12" action="maint.php">
        <input type="hidden" name="fname" value="form12">
        <table class="yyazi">
        <tr><td>&nbsp;</td><td>
        Oku<input type="radio" name="islem" value="R">
        Yaz<input type="radio" name="islem" value="U">
        Ekle<input type="radio" name="islem" value="I">
        Sonra<input type="radio" name="islem" value="N">
        Önce<input type="radio" name="islem" value="P">
        Liste<input type="radio" name="islem" value="L">
        </td></tr>
        <tr><td>Üye No</td><td>
<?php
    if(isset($userno)) {
        echo("<input type=\"text\" name=\"userno\" class=\"required\" size=\"17\"></td></tr>");
        echo("<tr><td>Log No</td><td>");
        echo("<input type=\"text\" name=\"logno\" class=\"required\" size=\"17\"></td></tr>");
        echo("<tr><td>Log Tar</td><td>");
        echo("<input type=\"text\" name=\"logtar\" class=\"required\" size=\"17\"></td></tr>");
        echo("<tr><td>Log Saat</td><td>");
        echo("<input type=\"text\" name=\"logsaat\" class=\"required\" size=\"17\"></td></tr>");
        echo("<tr><td>Log Kod</td><td>");
        echo("<input type=\"text\" name=\"logkod\" class=\"required\" size=\"17\"></td></tr>");
        echo("<tr><td>Log Msg</td><td>");
        echo("<input type=\"text\" name=\"logmsg\" class=\"required\" size=\"17\">");
    } else {
?>
        <input type="text" name="userno" class="required" size="17"></td></tr>
        <tr><td>Log No</td><td>
        <input type="text" name="logno" class="required" size="17"></td></tr>
        <tr><td>Log Tar</td><td>
        <input type="text" name="logtar" class="required" size="17"></td></tr>
        <tr><td>Log Saat</td><td>
        <input type="text" name="logsaat" class="required" size="17"></td></tr>
        <tr><td>Log Kod</td><td>
        <input type="text" name="logkod" class="required" size="17"></td></tr>
        <tr><td>Log Msg</td><td>
        <input type="text" name="logmsg" class="required" size="17">
<?php
      }
?>
        <input type="button" class="gyazi" value="git" onclick="javascript:ikontrol('form12');"></td></tr>
        </table>
        </form>
    </div>
    <div id="user13" style="display: none;">
        <h3>Haberler</h3>
        <form id="form11" action="maint.php">
        <input type="hidden" name="fname" value="form13">
        <table class="yyazi">
        <tr><td>&nbsp;</td><td>
        Oku<input type="radio" name="islem" value="R">
        Yaz<input type="radio" name="islem" value="U">
        Ekle<input type="radio" name="islem" value="I">
        Sonra<input type="radio" name="islem" value="N">
        Önce<input type="radio" name="islem" value="P">
        Liste<input type="radio" name="islem" value="L">
        </td></tr>
        <tr><td>Haber No</td><td>
<?php
    if(isset($haberno)) {
        echo("<input type=\"text\" name=\"haberno\" class=\"required\" size=\"17\"></td></tr>");
        echo("<tr><td>baslangic</td><td>");
        echo("<input type=\"text\" name=\"baslangic\" class=\"required\" size=\"17\"></td></tr>");
        echo("<tr><td>bitis</td><td>");
        echo("<input type=\"text\" name=\"bitis\" class=\"required\" size=\"17\"></td></tr>");
        echo("<tr><td>dosyaadi</td><td>");
        echo("<input type=\"text\" name=\"dosyaadi\" class=\"required\" size=\"17\"></td></tr>");
        echo("<tr><td>hkod</td><td>");
        echo("<input type=\"text\" name=\"hkod\" class=\"required\" size=\"17\">");
    } else {
?>
        <input type="text" name="haberno" class="required" size="17"></td></tr>
        <tr><td>baslangic</td><td>
        <input type="text" name="baslangic" class="required" size="17"></td></tr>
        <tr><td>bitis</td><td>
        <input type="text" name="bitis" class="required" size="17"></td></tr>
        <tr><td>dosyaadi</td><td>
        <input type="text" name="dosyaadi" class="required" size="17"></td></tr>
        <tr><td>hkod</td><td>
        <input type="text" name="hkod" class="required" size="17">
<?php
      }
?>
        <input type="button" class="gyazi" value="git" onclick="javascript:ikontrol('form13');"></td></tr>
        </table>
        </form>
    </div>
    <div id="user14" style="display: none;">
        <h3>Reklamlar</h3>
        <form id="form11" action="maint.php">
        <input type="hidden" name="fname" value="form14">
        <table class="yyazi">
        <tr><td>&nbsp;</td><td>
        Oku<input type="radio" name="islem" value="R">
        Yaz<input type="radio" name="islem" value="U">
        Ekle<input type="radio" name="islem" value="I">
        Sonra<input type="radio" name="islem" value="N">
        Önce<input type="radio" name="islem" value="P">
        Liste<input type="radio" name="islem" value="L">
        </td></tr>
        <tr><td>Reklam No</td><td>
<?php
    if(isset($haberno)) {
        echo("<input type=\"text\" name=\"reklamno\" class=\"required\" size=\"17\"></td></tr>");
        echo("<tr><td>rkod</td><td>");
        echo("<input type=\"text\" name=\"rkod\" size=\"17\"></td></tr>");
        echo("<tr><td>basTar</td><td>");
        echo("<input type=\"text\" name=\"bastar\" size=\"17\"></td></tr>");
        echo("<tr><td>bitTar</td><td>");
        echo("<input type=\"text\" name=\"bittar\" size=\"17\"></td></tr>");
        echo("<tr><td>dosyaAdi</td><td>");
        echo("<input type=\"text\" name=\"dosyaadi\" size=\"17\">");
    } else {
?>
        <input type="text" name="reklamno" class="required" size="17"></td></tr>
        <tr><td>rkod</td><td>
        <input type="text" name="rkod" size="17"></td></tr>
        <tr><td>basTar</td><td>
        <input type="text" name="bastar" size="17"></td></tr>
        <tr><td>bitTar</td><td>
        <input type="text" name="bittar" size="17"></td></tr>
        <tr><td>dosyaAdi</td><td>
        <input type="text" name="dosyaadi" size="17">
<?php
      }
?>
        <input type="button" class="gyazi" value="git" onclick="javascript:ikontrol('form14');"></td></tr>
        </table>
        </form>
    </div>
<?php
    if($dvar == "1") {
       echo("<b>$mystatus</b>");
       echo("<script>secim('$fuser');</script>");
       $dvar = 0;
    } else {
       echo("<script>secim('user1');</script>");
    }
?>
</div>
</td><td width="10">
</td><td valign="top">
<div id="ortataraf">
    <div id="liste1" style="display: inline;">
<?php
echo "<span class=\"yyazi\"><h3>Tablo listesi</h3></span>";
if(isset($result) && $result) {
    echo "<table class=\"yyazi\" border=\"1\">\n";
    while ($line = mysql_fetch_array($result, MYSQL_ASSOC)) {
        echo "\t<tr>\n";
        foreach ($line as $col_value) {
            echo "\t\t<td>$col_value</td>\n";
        }
        echo "\t</tr>\n";
    }
    echo "</table>\n";

    // Free resultset
    mysql_free_result($result);
    }
?>
    </div>
</div>
</td></tr></table>
</body>
</html>
