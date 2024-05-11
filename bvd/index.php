<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<?php
session_start();
?>
<!-- baslik.inc basi -->
<?php
    include "inc/baslik.inc"
?>
</head>
<body topmargin=0 leftmargin=0 marginheight=0 marginwidth=0>
<div id="headertop1">
<div id="headertop">
<!-- ust banner bolumu -->
<?php
  include "inc/ustbolum.inc"
?>
  <table cellpadding="0" cellspacing="0" border="0">
  <tr>
  <td valign="top" width="140">
  <!-- sol tarafin basi -->
<?php
  include "inc/soltaraf.inc"
?>
  </td>
  <td valign="top" width="800">
  <div id="header">
    <br />


    <!-- haber kutulari buraya yazilir -->
    <div id="baslik">
       <!-- sag taraftakiler buraya yazilir -->
<?php
       include "inc/sagtaraf.inc"
?>
       <!-- kutular -->
<?php
       include "inc/kutu.inc"
?>
       <br />
       <!--  kullanici bilgileri baslangici  -->
      <!-- include "inc/kullanici.inc" -->
    </div>
    <!-- haber basliklari buraya yazilir.... -->
<?php
    include "inc/haberbaslik.inc"
?>
    <script>goster('sec1','d1')</script>
    <!-- haber basliklari sonu .... -->
    <div id="altsatir">
        <br />
        <span class="rside">Tasarım: Aybim Bilgisayar Tic Ltd Şti&nbsp;&nbsp;</span>
    </div>
  </div>
  </td>
  </tr></table>
</div>
</div>
<br>
</body></html>
