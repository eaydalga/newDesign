    <div id="kisi2" style="display: none">
     <div id="acikmavi">
       <table class="yazi">
       <tr><td valign="top">
       <center><font color="#8e9ccb"><h3>Yakın Arkadaş Arama</h3></font></center>
       <form id="form1" method="POST">
<?php
       /* yakin arkadas arama ya da kod uzerinden davet cikartma islemi */
       echo("<table class=\"yazi\" width=\"300\">");
       echo("<tr><td>Üye No</td><td>");
       echo("    <input type=\"hidden\" name=\"usrno\" value=\"$usr\">");
       echo("    <input type=\"hidden\" name=\"arkadas\" value=\"0\">");
       echo("    <input type=\"hidden\" name=\"basla\" value=\"0\">");
       echo("    <input type=\"hidden\" name=\"sayisi\" value=\"10\">");
       echo("    <input type=\"text\" name=\"kod\" class=\"required\" size=\"9\">");
       echo("    </td>");
       echo("    <td><input type=\"button\" value=\"Üye Ara\" class=\"gyazi\" onclick=\"javascript:uyeler('form1','sec3','sec3');\">");
       echo("    </td></tr>");
       echo("<tr><td>&nbsp;</td>");
       echo("    <td></td>");
       echo("    <td><input type=\"button\" value=\"Arkadaş Ol\" class=\"gyazi\" onclick=\"javascript:ikontrol('form1');\"></td></tr>");
       echo("<tr><td>&nbsp;</td><td></td><td><input type=\"button\" value=\"Davet Çıkart\" class=\"gyazi\" onclick=\"javascript:ozelac('kisi6');\"></td></tr>");
       echo("</table>");
?>
       </form>
       </td><td valign="top">
           <br />
           <div id="sec3" style="display: none">
           </div>
       </td></tr></table>
       <br />
       <br />
       <div id="kisi6" style="display: none">
         <center><font color="#8e9ccb"><h3>Yakın Arkadaşı Davet Etme</h3></font></center>
         <form id="form2" method="POST" action="email.php">
<?php
         echo("<table class=\"yazi\">");
         echo("<tr><td>E-posta Adresi</td><td>");
         echo("    <input type=\"hidden\" name=\"usrno\" value=\"$usr\">");
         echo("    <input type=\"text\" name=\"email\" class=\"validate-email\" size=\"17\">");
         echo("    <input type=\"button\" value=\"Davet Et\" class=\"gyazi\" onclick=\"javascript:ikontrol('form2');\">");
         echo("    </td></tr>");
         echo("</table>");
?>
         </form>
<?php
         echo("<span class=\"rpic\"><input type=\"button\" value=\"Kapat\" class=\"nav\" onclick=\"javascript:usrclear13('kisi6');\"></span>");
?>
         <br />
         <br />
       </div>
     </div>
    </div>
