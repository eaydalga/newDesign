<?
# Test
# ------------------------------------------------------------
$pos = new Pos();
$pos->kart_num = '';      // Kart Numarasi
$pos->kart_kod = '';      // CVV2 Numarasi
$pos->kart_son = '';      // Son Kullanma Tarihi (YYYYAA)
$pos->tutar    = '0,00';  // Tutar (kurus , ile ayrilacak)
$pos->Gonder();

# Pos
# ------------------------------------------------------------
class Pos{
   var $kart_num;
   var $kart_kod;
   var $kart_son;
   var $tutar;

   # Kurucu
   # ---------------------------------------------------------
   function Pos(){
      $this->url      = 'https://www.e-tahsildar.com.tr/NetProvWS/NetProvWS.asmx/GetAuthorization';
      $this->org      = ''; // Organizasyon Numarasi
      $this->terminal = ''; // Terminal Numarasi
      $this->firma    = ''; // Firma Numarasi
   }

   # Gonder
   # ---------------------------------------------------------
   function Gonder(){
      if($this->test === true) return true;

      // Veri
      $veri =
         "pOrgNo=$this->org&".
         "pFirmNo=$this->firma&".
         "pTermNo=$this->terminal&".
         "pCardNo=$this->kart_num&".
         "pCvv2No=$this->kart_kod&".
         "pExpiry=$this->kart_son&".
         "pAmount=$this->tutar";

      // Gonder
      $ch = curl_init();
      $rs = curl_setopt($ch,CURLOPT_URL, $this->url);
            curl_setopt($ch,CURLOPT_POST,1);
            curl_setopt($ch,CURLOPT_POSTFIELDS,$veri);
            curl_setopt($ch,CURLOPT_SSL_VERIFYHOST,  2);
            curl_setopt($ch,CURLOPT_USERAGENT,'Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)');
            curl_setopt($ch,CURLOPT_RETURNTRANSFER,1);
            curl_setopt($ch,CURLOPT_SSL_VERIFYPEER, false);
      $sn = curl_exec  ($ch); curl_close($ch);
      $fp = fopen('kontrol.htm','a+'); fputs($fp,"<b>".date('Y-m-d H:i')."</b>"."<hr/>".htmlspecialchars($sn)."<br><br>"); fclose($fp);

      // Sonuc
      if($sn == NULL){ return -1; }
      else{
         preg_match("'<string.*>(\d{2}).*</string>'is",$sn,$kod);
         $kod = $kod[1];
      }

      // Basarili
      if($kod == '00') return true;
      else return -2;
   }
}
?>

