    <div id="kisi3" style="display: none">
     <div id="acikmavi">
       <table width="100%" class="yyazi">
       <tr><td valign="top">
       <center><font color="#8e9ccb"><h3>Kişisel Bilgiler</h3></font></center>
<?php
       /*   personel tablosundan usr ile                    *
        *     bilgikodu bilgialani alinir                   *
        *     bilgi kodundan adi secilir                    *
        *     tabloda bilgi adi ve bilgi alani yazdirilir.  */
       $sorgu = "SELECT bilgiKodu, bilgiAlani from personel where usrNo='$usr' order by bilgiKodu";
       $result = mysql_query($sorgu);
       echo("<form id=\"fkisi\" method=\"post\" action=\"kisisel.php\">");
       echo "<table class=\"yyazi\" border=\"0\" width=\"90%\">\n";
       $i = 0;
       $adlen = count($bilgiAdi);
       echo("<input type=\"hidden\" name=\"usrkod\" value=\"$usr\">");
       while ($line = mysql_fetch_array($result, MYSQL_NUM)) {
            $bkod = $line[0];
            $balan = $line[1];
            $ad = $bilgiAdi[$bkod];
            /* burada uygulama hakkinda ikon varsa kullanmak gerekir */
            /* echo "\t\t<td>$balan</td>\n"; */
            $bilgi_kod[$i] = $bkod;
            $bilgi_ad[$i] = $ad;
            $bilgi_len[$i] = $bilgiLen[$bkod];
            $bilgi_alan[$i] = $balan;
            $i++;
       }
       for($j=1;$j<=$adlen;$j++) {
           $yoksa=-1;
           for($k=0;$k<$i;$k++) {
              if($bilgi_kod[$k] == $j)
                   $yoksa=$k;
           } 
           if($yoksa == -1) {
              $ix = $j - 1;
              echo("<input type=\"hidden\" name=\"kod$j\" value=\"$ix\">");
              echo("<tr><td valign=\"top\">$j</td>");
              echo("    <td valign=\"top\">$bilgiAdi[$j]</td>");
              $aa = split(":",$bilgiLen[$j]);
              if($aa[0] > 1) {
                   echo("<td valign=\"top\"><textarea name=\"ad$j\" col=\"$aa[1]\" rows=\"$aa[0]\"></textarea>");
              } else {
                   echo("<td valign=\"top\"><input type=\"text\" name=\"ad$j\" size=\"$aa[1]\">");
                   if($j == $resimno)
                       echo("<input type=\"button\" name=\"bak$j\" value=\"Gözat\" onclick=\"javascript:ekler('fkisi','sec31','sec31')\">");
              }
           } else {
              /* her bilgi alani icin giris alani olurstur */
              $ix = $yoksa;
              echo("<input type=\"hidden\" name=\"kod$j\" value=\"$bilgi_kod[$ix]\">");
              echo("<tr><td valign=\"top\">$bilgi_kod[$ix]</td>");
              echo("    <td valign=\"top\">$bilgi_ad[$ix]</td>");
              $aa = split(":",$bilgi_len[$ix]);
              if($aa[0] > 1) {
                   echo("<td valign=\"top\"><textarea name=\"ad$j\" col=\"$aa[1]\" rows=\"$aa[0]\">$bilgi_alan[$ix]</textarea>");
              } else {
                   echo("<td valign=\"top\"><input type=\"text\" name=\"ad$j\" size=\"$aa[1]\" value=\"$bilgi_alan[$ix]\">");
                   if($j == $resimno)
                       echo("<input type=\"button\" name=\"bak$j\" value=\"Gözat\" onclick=\"javascript:ekler('fkisi','sec31','sec31')\">");
              }
           }
           echo("</td></tr>");
       }
       echo("<tr><td colspan=\"3\"> <span class=\"rpic\"><input type=\"submit\" value=\"Günle\" class=\"gyazi\"></span> Yukarıdaki bilgilerde değişiklik yapılırsa düğmeyi tıklayın </td></tr>");
       echo "</table>\n";
       echo("</form>");

       // Free resultset
       mysql_free_result($result);
?>
       </td><td valign="top">
           <div id="sec31" style="display: none"></div>
       </td></tr>
       </table>
           <iframe id="sec7" name="sec7" src="" style="width:496px;border:1px solid #fff;background-color:#ffffff;display:none;"></iframe>
       </div>
     </div>
