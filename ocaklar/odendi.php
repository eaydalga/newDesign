<?php
session_start();
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<!-- baslik.inc basi -->
<?php
    include "inc/baslik.inc"
?>
<body topmargin=0 leftmargin=0 marginheight=0 marginwidth=0>
<script type="text/javascript">
// Part 1 - Styles
var STYLE = {
    border:1,              // item's border width, pixels; zero means "none";
    shadow:2,              // item's shadow size, pixels; zero means "none"
    color:{
        border:"#666666",  // color of the item border, if any;
        shadow:"#DBD8D1",  // color of the item shadow, if any;
        bgON:"#d4d4f4",      // background color for the items;
        bgOVER:"#B6BDD2"   // background color for the item
                           // which is under mouse right now;
    },
    css:{
        ON:null,           // CSS class for items;
        OVER:null          // CSS class  for item which is under mouse;
    }
};

// Part 2 - Menu structure
var MENU_ITEMS = [
    {pos:[465,92], itemoff:[0,69], leveloff:[21,0], style:STYLE, size:[22,70]},
    {code:"&nbsp;Ana Sayfa", url:"index.php"},
    {code:"&nbsp;Yazılım", url:"",
        sub:[
            {itemoff:[21,0]},
            {code:"&nbsp;Genel", "url":"progs/index.html"},
            {code:"&nbsp;Css", "url":"progs/css.html"},
            {code:"&nbsp;HTML", "url":"progs/html.html"},
            {code:"&nbsp;dHTML", "url":"progs/dhtml.html"},
            {code:"&nbsp;js", "url":"progs/js.html"},
            {code:"&nbsp;Php", "url":"progs/php.html"}
        ]
    },
    {code:"&nbsp;Belgeler", url:"",
        sub:[
            {itemoff:[21,0],size:[22,90]},
            {code:"&nbsp;Genel", "url":"docs/index.html"},
            {code:"&nbsp;Kullanıcılar", "url":"docs/kullanici.html"},
            {code:"&nbsp;Blog", "url":"docs/blog.html"},
            {code:"&nbsp;Filmler", "url":"docs/filmler.html"},
            {code:"&nbsp;Internet TV", "url":"docs/webtv.html"},
            {code:"&nbsp;Veri Değişimi", "url":"docs/bvd.html"},
            {code:"&nbsp;Sürekli Yayım", "url":"docs/swy.html"},
            {code:"&nbsp;Panorama", "url":"docs/pg.html"},
        ]
    },
    {code:"&nbsp;Hakkımızda", url:"hakkimizda.html" },
    {code:"&nbsp;Adres", url:"adres.html"},
    {code:"&nbsp;Iletişim", url:"iletisim.html"},
    {code:"&nbsp;Arşiv", url:"arşiv/index.html"}
];

// Part 3 - Initialization code
var m1 = new COOLjsMenu("menu1", MENU_ITEMS);
</script>
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
<br /><br />
<?php
if(!empty($_SESSION['urun'])) {
   $dirfile = "urunler/" . $_SESSION['urun'];
   echo("<br>Indireceğiniz ürün için burayı <a href=\"$dirfile\">TIKLAYIN</a>");
} else {
   echo("<br>Yaptığınız bağış için TEŞEKKÜR ederiz");
}
session_unset();
?>
    <br />
    <div class="yazi">
        <div id="altsatir">
            <br />
            <span class="rside">Tasarım: Aybim Bilgisayar Tic Ltd Şti&nbsp;&nbsp;</span>
        </div>
    </div>
  </div>
  </td>
  </tr></table>
</div>
</div>
<br>
<script type="text/javascript">
var myMenu = new COOLjsMenu("menu1", MENU_ITEMS);
</script>
</body></html>
