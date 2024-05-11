<?php
define('SDX_OLD',1);
define('SDX_NEW',2);

define('SDX_DT_inconsistent', 0);  // (yığın tutarsız 11.1'e de bakın)
define('SDX_DT_structured', 1);    // yapı
define('SDX_DT_binary', 2);        // ikili dizi
define('SDX_DT_numeric', 3);       // sayısal
define('SDX_DT_char', 4);          // karakter
define('SDX_DT_float', 5);         // kayan noktalu (ANSI/IEEE 754-1985)
define('SDX_DT_UTF8', 6);          // UTF-8


define('SDX_RC_ok', 0);
define('SDX_RC_failed', 1);
define('SDX_RC_warning', 1);
define('SDX_RC_illegalOperation', 2);
define('SDX_RC_dataError', 3);
define('SDX_RC_parameterError', 4);
define('SDX_RC_programError', 5);
define('SDX_RC_noMemory', 6);


define('SDX_EC_ok',  0);
define('SDX_EC_eoc',  1); // yığın sonu
define('SDX_EC_notFound',  2);
define('SDX_EC_dataCutted',  3);
define('SDX_EC_overflow',  4);
define('SDX_EC_wrongInitType',  5);
define('SDX_EC_comprerr',  6); // sıkıştırma hatası
define('SDX_EC_forbidden',  7);
define('SDX_EC_unknown',  8);
define('SDX_EC_levelOvflw',  9);
define('SDX_EC_paramMissing', 10);
define('SDX_EC_magicError', 11);
define('SDX_EC_not_consistent', 12);
define('SDX_EC_wrongDataType', 13);
define('SDX_EC_noMemory', 14);
define('SDX_EC_error', 99); // rc yeterli


class chunk {
     var $chunkID;   // INTEGER (1..65535),
     var $flags;     // BIT STRING,
     var $length;    // OCTET STRING SIZE 3, -- or: INTEGER (0..16777215)
     var $cdata;      // OCTET STRING
};
class T_stack        /* cursor stack   (private)               */
    {
      var $chunk;
      var $length;
      var $rest;
    };
class SDXF_obj {
     var $chunkID;       // yığın adı (ID)
     var $container;     // tüm yığına gösterge
     var $bufferSize;    // tampon alanın boyutu
     var $currChunk;     // gerçek yığına gösterge
     var $dataLength;    // Yığın içindeki verinin boyu
     var $maxLength;     // SDX_extract için yığının max. boyu
     var $remainingSize; // SDX_create işlevinden sonra kalan boy
     var $value;         // sayısal veri türü için
     var $fvalue;        // kayan noktalı veri için
     var $function;      // çalıştırılan SDXF işlevinin adı
     var $sdata;          // Veriye gösterge
     var $cryptkey;      // Kriptolama anahtarına gösterge
     var $count;         // dizideki (max.) öğe sayısı
     var $dataType;      // Yığın veri türü / ilkleme açma tür
     var $ec;            // gelişmiş dönüş kodu
     var $rc;            // dönüş-kodu
     var $level;         // sıradüzen düzeyi
     var $filler;        // SDX_extract için doldurma karakteri
     var $encrypt;       // verinin kriptolanması kodu (0 / 1)
     var $compression;   // sıkıştırma yöntemi
                         //   (00=yok, 01=RL1, 02=zip/deflate)
     var $newChunk;      /* (private)                              */
     var $oldChunkID;    /* (private)                              */

     var $iflags;        /* internal flags (private)               */
     var $stack;

     // yöntemler:

// php pack ve pack unpack burada kullanılır.
/*
** array unpack($format, $data)
** string pack($format, arg1, arg2, ... 1rg3)
**    $format a - NULL padded
**            A - space padded
**            H - big endian
**            c - signed char
**            C - unsigned char
**            n - short big endian
**            N - 32 bir big endian
**            f - float
**            d - double
**            x - null byte
**     count (integer or *)
** e.g. binarydata = pack("nvc*", 0x1234, 0x5678, 65, 66);
**      0x12,0x34,0x56,0x78,0x41,0x42
*/

     function SDX_init($con, $size, $dType) { // $dataType = SDX_NEW -> $size
        $chunk = array();
        $this->container = $con;
        if($dType == SDX_NEW)
            $this->bufferSize = $size;
     }
     function SDX_enter() {
        $chunk = unpack("nID/Nflen",$this->container);
        $this->chunkID = $chunk['ID'];
        $this->currChunk = $chunk['ID'];
        $flag = ($chunk['flen'] >> 24) & 0xff;
        $this->dataLength=$chunk['len'] & 0xffffff;
        $this->sdata = substr($this->container,7,$this->dataLength);
        $stack->length = 7 +  $chunk[2];
        $stack->rest = substr($this->contianer, $stack->length);
        if($this->encrypt) {
             // at the end check encryption and decrypt
             $this->SDX_set_encryption($this->cryptkey);
        }
        if($this->compression != "00") {
            //   (00=yok, 01=RL1, 02=zip/deflate)
            // at the end check compression and decompress
            $this->SDX_set_compression($this->compression);
        }
     }
     function SDX_next() {
        $chunk = unpack("nID/Nflen",$this->container);
        if(count($chunk) > 0) {
            $this->currChunk = $chunk['ID'];
            $flag = ($chunk['flen'] >> 24) & 0xff;
            $this->dataLength=$chunk['flen'] & 0xffffff;
            $this->sdata = substr($stack->rest,7,$this->dataLength);
            $stack->length = 7 +  $chunk['len'];
            $this->rest = substr($stack->rest, $stack->length);
        }
     }
     function SDX_extract($data, $length) { // current filler from above
        /*
             SDX_EC_dataCut       SDX_EC_malformed
             SDX_EC_crypterror    SDX_EC_comprerr
             SDX_EC_unknown       SDX_EC_wrongLength
             SDX_EC_end_of_data
        */
        $dd = unpack("nID/Nflen",$this->sdata);
        if($this->dataType == SDX_DT_structured) {
            $this->stack->chunk = $dd['ID'];
            $flag = ($dd['flen'] >> 24) & 0xff;
            $this->stack->length = $length - 7;
            $this->stack->rest = substr($data,7,$this->stack->length);
        }
        if($this->dataType == SDX_DT_float) {
            $this->stack->chunk = $dd['ID'];
            $flag = ($dd['flen'] >> 24);
            $this->fvalue = substr($data,7,$length - 7);
        }
        if($this->dataType == SDX_DT_numeric) {
            $this->stack->chunk = $dd['ID'];
            $flag = ($dd['flen'] >> 24);
            $this->value = substr($data,7,$length - 7);
        }
        if($this->encrypt) {
             // at the end check encryption and decrypt
             $this->SDX_set_decryption($this->cryptkey);
        }
        if($this->compression != "00") {
            //   (00=yok, 01=RL1, 02=zip/deflate)
            // at the end check compression and decompress
            $this->SDX_set_decompression($this->compression);
        }
        return(SDX_EC_ok);
     }
     function SDX_select($ChunkID) {
        // data from above
        // value -> dataType : kayan noktali
        //     SDX_EC_notfound 
        if($this->dataType == SDX_DT_float) {
            $this->fvalue = $data;
        }
        if($this->dataType == SDX_DT_numeric) {
            $this->value = $data;
        }
        return SDX_EC_ok;
     }

     function SDX_create($ChunkID, $dataLength, $value, $flag, $dataType) {
        $this->sdata = $value;
        if($this->compression != "00") {
            //   (00=yok, 01=RL1, 02=zip/deflate)
            // at the end check compression
            $this->SDX_set_compression($this->compression);
        }
        if($this->encrypt) {
             // at the end check encryption
             $this->SDX_set_encryption($this->cryptkey);
        }
        $this->dataType = $dataType;
        $this->container = pack("nN",$ChunkID,($flag << 24) | ($dataLength & 0xffffff));
        $this->chunkID = $ChunkID;
        $this->currChunk = $ChunkID;
        $stack->length = 7 +  $dataLength;
        $stack->rest = $this->sdata;
        $this->dataLength=$dataLength;
        $this->container = $this->container . $value;
     }

     function SDX_append($ChunkID, $datalength, $value, $flag, $dataType) {
        // append after currChunkID
        $this->sdata = $value;
        if($this->compression != "00") {
            //   (00=yok, 01=RL1, 02=zip/deflate)
            // at the end check compression
            $this->SDX_set_compression($this->compression);
        }
        if($this->encrypt) {
             // at the end check encryption
             $this->SDX_set_encryption($this->cryptkey);
        }
        $this->dataType = $dataType;
        $cc = pack("nCN",$ChunkID,$flag,$datalength);
        $cc = pack("nN",$ChunkID,($flag << 24) | ($dataLength & 0xffffff));
        $this->chunkID = $ChunkID;
        $this->currChunk = $ChunkID;
        $stack->length = 7 +  $datalength;
        $stack->rest = $this->sdata;
        $this->dataLength=$datalength;
        $this->container = $this->container . $cc . $value;
     }

     function SDX_set_compression($compression_method) {
        // compress data
        // deflate method
        $size = strlen($this->sdata);
        $contents = gzcompress($this->sdata, 9);
        $compheader = ($compression_method << 24) | ($size & 0xffffff);
        $this->sdata = $compheader . $contents;
     }
     function SDX_set_decompression($compression_method) {
        // compress data
        // deflate method
        $dd = substr($this->sdata,4);
        $ds = substr($this->sdata,0,4);
        $dsize = $ds & 0xffffff;
        $uncompressed = gzuncompress($dd);
        $size = strlen($uncompressed);
        if($size == $dsize)
            $this->sdata = $uncompressed;
     }
     function SDX_set_encryption($encryption_key) {
        $crypt = aes_encrypt($encryption_key,$this->sdata);
        if($crypt != -1) {
            $this->sdata = $crypt;
        }
     }
     function SDX_set_decryption($encryption_key) {
        $plaintxt = aes_decrypt($encryption_key,$this->sdata);
        if($crypt != -1) {
            $this->sdata = $plaintxt;
        }
     }
     function SDX_leave() {
     }
   };

function sdxf_comp($comp,$enc,$enckey)
   {
   // compression method setup is needed
   if($comp != '') {
           $sdxf->compression = "02";  // 01 -> eskidi artik
       } else {
           $sdxf->compression = "00";
       }
   // requesting encryption?
   $sdxf->encrypt = 0;
   if($comp != '') {
       $sdxf->encrypt = 0;
       // if yes enter crypo key
       $sdxf->cryptkey = $enckey;
       }
   };

function sdxf_control($params)
   {
   /* ************************************************************
   ** params[0] infile
   ** params[1] comp
   ** params[2] enc
   ** params[3] enckey
   ** ***********************************************************/
   $sdxf = new SDXF_obj;
   $sdxf->$stack = new T_stack;
   sdxf_comp($comp,$enc,$enckey);
   };
function sdxf_create($params)
   {
   /* ************************************************************
   ** params[0] chunkID
   ** params[1] comp       ** params[2] enc   ** params[3] enckey
   ** params[4] datalength ** params[5] $data ** params[6] $datatype
   ** params[10] chunk ** params[11] ekleme
   ** ***********************************************************/
   $sdxf = new SDXF_obj;
   $sdxf->stack = new T_stack;
   $comp = $params[1];
   $enc = $params[2];
   $enckey = $params[3];
   $access = $params[9];
   sdxf_comp($comp,$enc,$enckey);
   $sdxf->SDX_init($params[10], $params[4], $params[11]);
   $sdxf-> SDX_create($params[0], $params[4], $params[5], $params[6], $params[6]);
   $sdxf->SDX_leave();
   return $sdxf->container;
   };
function sdxf_create_array($params)
   {
   /* ************************************************************
   ** params[0] chunkID
   ** params[1] comp       ** params[2] enc   ** params[3] enckey
   ** params[4] datalength ** params[5] $data ** params[6] $datatype
   ** params[10] chunk ** params[11] ekleme
   ** ***********************************************************/
   $sdxf = new SDXF_obj;
   $sdxf->stack = new T_stack;
   $comp = $params[1];
   $enc = $params[2];
   $enckey = $params[3];
   $access = $params[9];
   sdxf_comp($comp,$enc,$enckey);
   $sdxf->SDX_init($params[10], $params[4], $params[11]);
   $sdxf-> SDX_create($params[0], $params[4], $params[5], $params[6], $params[6]);
   $sdxf->SDX_leave();
   return $sdxf->container;
   };
function sdxf_create_short($params)
   {
   /* ************************************************************
   ** params[0] chunkID
   ** params[1] comp       ** params[2] enc   ** params[3] enckey
   ** params[4] datalength ** params[5] $data ** params[6] $datatype
   ** params[10] chunk ** params[11] ekleme
   ** ***********************************************************/
   $sdxf = new SDXF_obj;
   $sdxf->stack = new T_stack;
   $comp = $params[1];
   $enc = $params[2];
   $enckey = $params[3];
   $access = $params[9];
   sdxf_comp($comp,$enc,$enckey);
   $sdxf->SDX_init($params[10], $params[4], $params[11]);
   $sdxf-> SDX_create($params[0], $params[4], $params[5], $params[6], $params[6]);
   $sdxf->SDX_leave();
   return $sdxf->container;
   }

function sdxf_create_struct($params)
   {
   /* ************************************************************
   ** params[0] chunkID
   ** params[1] comp       ** params[2] enc   ** params[3] enckey
   ** params[4] datalength ** params[5] $data ** params[6] $datatype
   ** params[10] chunk ** params[11] ekleme
   ** ***********************************************************/
   $sdxf = new SDXF_obj;
   $sdxf->stack = new T_stack;
   $comp = $params[1];
   $enc = $params[2];
   $enckey = $params[3];
   $access = $params[9];
   sdxf_comp($comp,$enc,$enckey);
   $sdxf->SDX_init($params[10], $params[4], $params[11]);
   $sdxf-> SDX_create($params[0], $params[4], $params[5], $params[6], $params[6]);
   $sdxf->SDX_leave();
   return $sdxf->container;
   };
function sdxf_append($params)
   {
   /* ************************************************************
   ** params[0] chunkID
   ** params[1] comp       ** params[2] enc   ** params[3] enckey
   ** params[4] datalength ** params[5] $data ** params[6] $datatype
   ** params[10] chunk ** params[11] ekleme
   ** ***********************************************************/
   $sdxf = new SDXF_obj;
   $sdxf->stack = new T_stack;
   $comp = $params[1];
   $enc = $params[2];
   $enckey = $params[3];
   $access = $params[9];
   sdxf_comp($comp,$enc,$enckey);
   $sdxf->SDX_init($params[10], $params[4], $params[11]);
   $sdxf-> SDX_append($params[0], $params[4], $params[5], $params[6], $params[6]);
   $sdxf->SDX_leave();
   return $sdxf->container;
   };
function sdxf_appendstruct($params)
   {
   /* ************************************************************
   ** params[0] chunkID
   ** params[1] comp       ** params[2] enc   ** params[3] enckey
   ** params[4] datalength ** params[5] $data ** params[6] $datatype
   ** params[10] chunk ** params[11] ekleme
   ** ***********************************************************/
   $sdxf = new SDXF_obj;
   $sdxf->stack = new T_stack;
   $comp = $params[1];
   $enc = $params[2];
   $enckey = $params[3];
   $access = $params[9];
   sdxf_comp($comp,$enc,$enckey);
   $sdxf->SDX_init($params[10], $params[4], $params[11]);
   $sdxf-> SDX_append($params[0], $params[4], $params[5], $params[6], $params[6]);
   $sdxf->SDX_leave();
   return $sdxf->container;
   };
?>
