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
        bgON:"white",      // background color for the items;
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
    {pos:[740,92], itemoff:[0,69], leveloff:[21,0], style:STYLE, size:[22,70]},
    {code:"Item 1", url:"item1.html",
        sub:[
            {"itemoff":[21,0]},
            {code:"SubItem 1", "url":"subitem1_1.html"},
            {code:"SubItem 2", "url":"subitem1_2.html"},
            {code:"SubItem 3", "url":"subitem1_3.html"},
            {code:"SubItem 4", "url":"subitem1_4.html"}
        ]
    },
    {code:"Item 2", url:"item2.html",
        sub:[
            {itemoff:[21,0]},
            {code:"SubItem 1", "url":"subitem2_1.html"},
            {code:"SubItem 2", "url":"subitem2_2.html"},
            {code:"SubItem 3", "url":"subitem2_3.html"},
            {code:"SubItem 4", "url":"subitem2_4.html"}
        ]
    },
    {code:"Çık", url:"item3.html" }
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
    <br />
    <!-- haber basliklari buraya yazilir.... -->
<?php
    include "inc/haberbaslik.inc"
?>
    <script>goster('sec1','d1')</script>
    <!-- haber basliklari sonu .... -->


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
<?php
       include "inc/kullanici.inc"
?>
      <div id="altsatir">&nbsp;</div>
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
