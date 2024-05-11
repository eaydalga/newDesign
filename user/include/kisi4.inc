  <div id="kisi4" style="display: none">
     <div id="header">
        <div class="ul">
              <table cellpadding="0" cellspacing="0" class="yyazi">
               <tr>
                   <td><div class="li"><a href="javascript:clear13('edit2');clear13('edit3');clear13('edit4');ozelac('edit1');">Şifre</a></div></td>
                   <td><div class="li"><a href="javascript:clear13('edit1');clear13('edit3');clear13('edit4');ozelac('edit2');">ePosta</a></div></td>
                   <td><div class="li"><a href="javascript:clear13('edit2');clear13('edit1');clear13('edit4');ozelac('edit3');">Ad Soyad</a></td>
                   <td><div class="li"><a href="javascript:clear13('edit2');clear13('edit3');clear13('edit1');ozelac('edit4');">Resim Yükle</a></td>
               </td></tr>
              </table>
        </div>
     </div>
     <div id="acikmavi">
       <div class="yyazi">
<?php
       $sorgu = "SELECT email,fullName from user where usrNo='$usr' and usrkod='1'";
       list($em,$fna) = read_row($sorgu,$usr);
       echo("<table class=\"yyazi\">");
       echo("<tr><td><font color=\"#8e9ccb\"><b>$usr için</b></font></td><td width=\"70%\">&nbsp;</td></tr>");
       echo("<tr><td><b>E-posta</b></td><td width=\"70%\">$em</td></tr>");
       echo("<tr><td><b>Adı Soyadı</b></td><td width=\"70%\">$fna</td></tr>");
       echo("</table>");
?>
       </div>
       <hr>
       <div id="edit1" style="display: inline">
           <font color="#8e9ccb"><h3>Üye Şifre Girişi</h3></font>
           <form id="gir8" action="sifreli.php" method="post">
<?php
           echo("<input type=\"hidden\" name=\"kullanici\" value=\"$usr\">");
           echo("<input type=\"hidden\" name=\"islem\" value=\"sf\">");
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
           <iframe id="sec8" name="sec8" src="" style="width:496px;border:1px solid #fff;background-color:#ffffff;display:none;"></iframe>
       </div>
       <div id="edit2" style="display: none">
           <font color="#8e9ccb"><h3>ePosta adresi Düzenleme</h3></font>
           <form id="gir9" action="gunle.php" method="post">
<?php
           echo("<input type=\"hidden\" name=\"kullanici\" value=\"$usr\">");
           echo("<input type=\"hidden\" name=\"islem\" value=\"em\">");
?>
           <table cellpadding="1" cellspacing="3" class="yyazi">
           <tr><td>ePosta Adresi</td><td width="70%"><nobr><input type="text" name="email" size="10">
           <input type="submit" value="Git" class="gyazi"></nobr>
           </td></tr>
           </table>
           </form>
           <iframe id="sec9" name="sec9" src="" style="width:496px;border:1px solid #fff;background-color:#ffffff;display:none;"></iframe>
       </div>
       <div id="edit3" style="display: none">
           <font color="#8e9ccb"><h3>Ad ve Soyad Düzenleme</h3></font>
           <form id="gir10" action="gunle.php" method="post">
<?php
           echo("<input type=\"hidden\" name=\"kullanici\" value=\"$usr\">");
           echo("<input type=\"hidden\" name=\"islem\" value=\"ad\">");
?>
           <table cellpadding="1" cellspacing="3" class="yyazi">
           <tr><td>Ad Soyad</td><td width="70%"><nobr><input type="text" name="fullname" size="10">
           <input type="submit" value="Git" class="gyazi"></nobr>
           </td></tr>
           </table>
           </form>
           <iframe id="sec10" name="sec10" src="" style="width:496px;border:1px solid #fff;background-color:#ffffff;display:none;"></iframe>
       </div>
       <div id="edit4" style="display: none">
           <font color="#8e9ccb"><h3>Resim Yükleme</h3></font>
<?php $_SESSION['user']=$usr; ?>
           <form id="fresim" method="POST" enctype="multipart/form-data" action="resim.php">
           <table cellpadding="1" cellspacing="3" class="yyazi">
           <tr><td>Resim Dosyası</td>
               <td><input type="file" name="resim">
               &nbsp;&nbsp;<input type="submit" value="Yükle" class="gyazi">
               <input type="reset" value="Sil" class="gyazi">
               </td></tr>
           </table>
           </form>
           <iframe id="sec6" name="sec6" src="" style="width:496px;border:1px solid #fff;background-color:#ffffff;display:none;"></iframe>
       </div>
       <br />
     </div>
    </div>

