
 <div id="kisi5" style="display: none">
     <div id="mbaslik">
         <div class="ul">
              <table cellpadding="2" cellspacing="0" class="yazi">
               <tr>
                   <td><div class="li"><a href="javascript:usrclear13('msg2');usrclear13('msg3');ozelac('msg1');" class="gyazi">&nbsp;Gelen&nbsp;</a></div></td>
                   <td><div class="li"><a href="javascript:usrclear13('msg1');usrclear13('msg3');ozelac('msg2');" class="gyazi">&nbsp;Giden&nbsp;</a></div></td>
                   <td><div class="li"><a href="javascript:usrclear13('msg2');usrclear13('msg1');ozelac('msg3');" class="gyazi">&nbsp;Yeni&nbsp;</a></td>
               </td></tr>
              </table>
         </div>
     </div>
     <div id="acikmavi">
       <div id="msg1" style="display: inline">
<?php
       /* ***********************************************************
        * mesaj toNo fromNo msgNo uygMsg uygEkler
        *       fromNo --> user fullName
        *       uygEkler != '' mesajeklerine bak
        *                      msgNo  ekNo ekBilgi
        * *********************************************************** */
       $sorgu="SELECT fromNo,uygMsg,uygEkler from mesaj where toNo='$usr' and rmsgkod='1'";
       $result = mysql_query($sorgu);
       if($result) {
           $fullname = usr_adi($usr);
           $i=0;
           while ($line = mysql_fetch_array($result, MYSQL_NUM)) {
               $fromno = $line[0];
               $fromname = usr_adi($fromno);
               $uygmsg = $line[1];
               $uygekler = $line[2];
               /* fullname fromname uygmsg uygekler */
               mesaj_yaz($fullname, $fromname, $uygmsg, $uygekler);
               $i++;
           }
           @mysql_free_result($result);
           if($i == 0) {
               echo("<br><br><span class=\"yazi\">&nbsp;&nbsp;&nbsp;$fullname için <b>Gelen kutusu</b> boş...</span>");
               echo("<br><br>");
               echo("<br><br>");
               echo("<br><br>");
               echo("<br><br>");
           }
       } else {
           echo("Gelen kutusu sorunlu...");
       }
?>
       </div>

       <div id="msg2" style="display: none">
<?php
       $sorgu="SELECT toNo,uygMsg,uygEkler from mesaj where fromNo='$usr' and rmsgkod='1'";
       $result = mysql_query($sorgu);
       if($result) {
           $fullname = usr_adi($usr);
           $i=0;
           while ($line = mysql_fetch_array($result, MYSQL_NUM)) {
               $tono = $line[0];
               $toname = usr_adi($fromno);
               $uygmsg = $line[1];
               $uygekler = $line[2];
               /* fullname fromname uygmsg uygekler */
               mesaj_yaz($toname, $fullname, $uygmsg, $uygekler);
               $i++;
           }
           @mysql_free_result($result);
           if($i == 0) {
               echo("<br><br><span class=\"yazi\">&nbsp;&nbsp;&nbsp;$fullname için <b>Giden kutusu</b> boş...</span>");
               echo("<br><br>");
               echo("<br><br>");
               echo("<br><br>");
               echo("<br><br>");
           }
       } else {
           echo("Giden kutusu sorunlu...");
       }
?>
       </div>

       <div id="msg3" style="display: none">
         <br>
         <br>
         <table class="yazi">
         <form id="fek" method="POST" enctype="multipart/form-data" action="upload.php">
         <tr><td valign="top">Ekler</td><td>
                 <input name="uploaded" type="file" />
                 <input type="submit" value="Yükle" onclick="javascript:tasi('formx','attach','fek','uploaded');">
             </td><tr>
         </table>
         </form>
         <br />
         <table class="yazi">
         <tr><td valign="top">
         <form id="formx" method="POST" action="mesaj.php">
<?php
         echo("<input type=\"hidden\" name=\"usrno\" value=\"$usr\">");
         echo("<input type=\"hidden\" name=\"basla\" value=\"0\">");
         echo("<input type=\"hidden\" name=\"sayisi\" value=\"10\">");
         echo("<input type=\"hidden\" name=\"arkadas\" value=\"\">");
?>
         <table class="yazi">
         <tr><td>Kime</td><td> <input type="text" name="tono" size="17" class="required">
                 <input type="button" value="Gözat" onclick="javascript:uyeler2('formx','sec4','sec4','tono')">
             </td><tr>
         <tr><td>Cc</td><td> <input type="text" name="tocc" size="17">
                 <input type="button" value="Gözat" onclick="javascript:uyeler2('formx','sec4','sec4','tocc')">
             </td><tr>
         <tr><td>Ekler</td><td> <input type="text" name="attach" size="17" class="">
         <tr><td>Konu</td><td> <input type="text" name="subject" size="17" class="required">
              </td></tr>
         <tr><td>&nbsp;</td><td>
                 <iframe id="sec5" name="sec5" src="" style="width:230;height:auto;border:1px solid #fff;background-color:#ffffff;display:none;"></iframe>
              </td></tr>
         <tr><td valign="top">İçerik</td><td><textarea name="body" rows="7" cols="20" class="required"></textarea>
                 <input type="button" value="Gönder" onclick="javascript:ikontrol('formx')">
             </td><tr>
         </table>
         </form>
         </td><td valign="top">
             <br />
             <div id="sec4" style="display: none">
             </div>
         </td></tr></table>
       </div>
       <br />
     </div>
    </div>
   </div>
