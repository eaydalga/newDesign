  <div id="kisi1" style="display: none">
<?php
    if($pwd == "bos") {
?>
    <div id="acikmavi">
       <font color="#8e9ccb"><h3>Üye Şifre Girişi</h3></font>
       <form action="sifreli.php" method="post">
<?php
       echo("<input type=\"hidden\" size=\"10\" name=\"kullanici\" value=\"$usr\">");
?>
       <table cellpadding="1" cellspacing="3" class="yyazi">
          <tr><td>Yeni Şifre </td><td width="70%"><nobr><input type="password" name="sifre" size="10"></td></tr>
          <tr><td>Yeni Şifre Kontrol</td><td width="70%"><nobr><input type="password" name="sifre2" size="10">
              <input type="submit" value="Git" class="gyazi"></nobr>
          </td></tr>
       </table>
       <div class="yyazi">
<?php
       if(isset($_SESSION['sifrehata'])) {
           // sifre icin hata mesaji
           echo($_SESSION['sifrehata']);
           session_unset();
       }
?>
       </div>
       </form>
    </div>
<?php
   } else {
       /* burada neler olabilir:

       * log dosyasından son yapılan islemler goruntulenir.
       * Eger log kullanicisi friend icinde varsa ya da kullaniciysa
       * ozet bilgiyi görüntüle --- son 10 taneyi ---

       * haberleri görüntüle:
       * haber no, baslangic tarih bitis tarih
       * haber basligi, haber bilgisi
       */
?>
    <div id="header">
         <table cellpadding="0" cellspacing="0" class="yyazi">
         <tr>
             <td><div class="li"><a href="javascript:clear13('log1');ozelac('log2');">Haber Bak</a></div></td>
             <td><div class="li"><a href="javascript:clear13('log2');ozelac('log1');">Log Bak</a></div></td>
             </td></tr>
         </table>
    </div>
    <div id="acikmavi">
      <div id="log1" style="display: none">
<?php
         $bugun=date("Ymd",time());
         $lim=10;
         $sorgu="SELECT logTar, logSaat, logKod, logMsg from ulog where userNo='$usr' and logTar<='$bugun' order by logTar desc, logSaat desc limit 0, $lim";
         // $sorgu="SELECT logTar, logSaat, logKod, logMsg from ulog where userNo='$usr' and logTar<='$bugun'";
         $keyval = "İşlem kayıtları (ilk $lim adet)<br />";
         my_tabloyaz($sorgu,$keyval,"</tr><tr><td>Tarih</td><td>Saat</td><td>K</td><td>Mesaj</td></tr><tr><td colspan=\"4\"><hr></td>");
?>
      </div>
      <div id="log2" style="display: inline">
<?php
         echo("<center><font color=\"#8e9ccb\"><h3>Güncel Haberler</h3></font></center>");

         $sorgu="SELECT haberNo, dosyaAdi from haber where haberNo like 'USR%' and baslangic <= '$bugun' and hkod='1' order by baslangic desc";
         $i=my_haberyaz($sorgu,$haberdir);
         if($i < 1) {
             echo("Bugün yayımlanacak haber yok...<br />");
         }
?>
         <div id="ozel" style="display: inline">
         <script>timedMsg()</script>
         </div>
      </div>
    </div>
<?php
   }
   if($pwd == "bos") {
       echo("<script>sayfa('1')</script>");
   }
?>
  </div>

