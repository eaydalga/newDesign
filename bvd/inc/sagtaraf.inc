       <div class="borderright">
<b>Aybim Üyelik Sistemi</b> yalnız üyeler için sağlanan yazılım ve tasarım olanakları paylaşım ortamıdır. Üyeler kendi kaynak kodlarını paylaşıma açabildikleri gibi var olan tasarım ve kodları indirip kullanabilirler. Ayrıca üyeler kendi yazılımlarının pazarlanmasını da isteyebilirler. Ayrıntılar için <a href="uyelik.html">Üyelik Sistemi</a> sayfasına gidebilirsiniz.<br>
         <br>
         <form id="uye" name="uye" action="apps/sifreli.php">
           <div id="sifre" class="yazi">
            <table class="yazi">
               <tr><th colspan="2"><center>ÜYE GİRİŞİ</center></th></tr>
              <tr><td valign="top">Kullanıcı Kodu</td>
                  <td valign="top"><input type="text" name="user" size="8" class="required"></td></tr>
              <tr><td valign="top">Şifre</td>
                      <td valign="top"><input type="password" name="passwd" size="8" class="required">
                    <input type="submit" onclick="ikontrol('uye')" value="Git"></td></tr>
            </table>
           </div>
         </form>
         <br>
<?php
     if(isset($_SESSION['sifrehata'])) {
         // basvuru icin hata mesaji
         echo("<b>HATA:</b> ".$_SESSION['sifrehata']);
         session_unset();
     }
?>
         <br>
         Eğer üye değilseniz ve üye olmak isterseniz
<a href="apps/index.php">YENİ ÜYE GİRİŞİ</a>
ile üyemiz olun. Üyelerimizle bilgilerinizi paylaşın.<br>
<b>Bilgi paylaşıldıkça değerlidir</b>.<br>
       </div>
