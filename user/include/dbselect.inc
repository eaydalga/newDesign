<?php
require("include/conf.inc");

$baglan = mysql_connect($dbhost, $dbuser, $dbpasswd) or
                    die("Bağlantı kurulamadı" . mysql_error());

function db_sorgula($query, $handle) {
    $bsonuc = mysql_db_query($dbhost, $query, $handle) or
                         die('Sorgu hatalı: ' . mysql_error());;
    echo "<table>\n";
    while ($line = mysql_fetch_array($result, MYSQL_ASSOC)) {
        echo "\t<tr>\n";
        foreach ($line as $col_value) {
            echo "\t\t<td>$col_value</td>\n";
        }
        echo "\t</tr>\n";
    }
    echo "</table>\n";
    @mysql_freeresult($bsonuc);
}

function db_bitti($bsonuc)
    mysql_close($baglan) die("kapatılamadı", mysql_error());
}
