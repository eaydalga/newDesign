<?php
define(CHUNK_WAIT_STRUCT,0); // bekleyen yapı (yığın tutarsız 11.1'e de bakın)
define(CHUNK_STRUCT,1); // yapı
define(CHUNK_BINARY,2); // ikili dizi
define(CHUNK_SAYI,3); // sayısal
define(CHUNK_KARAK,4); // karakter
define(CHUNK_KAYAN,5); // kayan noktalu (ANSI/IEEE 754-1985)
define(CHUNK_UTF-8,6); // UTF-8
define(CHUNK_SAKLI,7); // saklı

class chunk {
     var $chunkID;   // INTEGER (1..65535),
     var $flags;     // BIT STRING,
     var $length;    // OCTET STRING SIZE 3, -- or: INTEGER (0..16777215)
     var $data;      // OCTET STRING
}
-------------------------------------------------------------------------
   typedef short int      ChunkID;
   typedef unsigned char  Byte;

   typedef struct Chunk
   {
     ChunkID    chunkID;
     Byte       flags;
     char       length [3];
     Byte       data;
   } Chunk;

   typedef struct
   {
     ChunkID  chunkID;       // yığın adı (ID)
     Byte    *container;     // tüm yığına gösterge
     long     bufferSize;    // tampon alanın boyutu
     Chunk   *currChunk;     // gerçek yığına gösterge
     long     dataLength;    // Yığın içindeki verinin boyu
     long     maxLength;     // SDX_extract için yığının max. boyu
     long     remainingSize; // SDX_create işlevinden sonra kalan boy
     long     value;         // sayısal veri türü için
     double   fvalue;        // kayan noktalı veri için
     char    *function;      // çalıştırılan SDXF işlevinin adı
     Byte    *data;          // Veriye gösterge
     Byte    *cryptkey;      // Kriptolama anahtarına gösterge
     short    count;         // dizideki (max.) öğe sayısı
     short    dataType;      // Yığın veri türü / ilkleme açma tür
     short    ec;            // gelişmiş dönüş kodu

     short    rc;            // dönüş-kodu
     short    level;         // sıradüzen düzeyi
     char     filler;        // SDX_extract için doldurma karakteri
     Byte     encrypt;       // verinin kriptolanması kodu (0 / 1)
     Byte     compression;   // sıkıştırma yöntemi
                             //   (00=yok, 01=RL1, 02=zip/deflate)
   } SDX_obj, *SDX_handle;
-------------------------------------------------------------------------

   class C_SDXF
   {
     public:

     // yapıcı ve yıkıcı
     C_SDXF  ();                          // yalancı
     C_SDXF  (Byte *cont);                // eski tampon
     C_SDXF  (Byte *cont, long size);     // yeni tampon
     C_SDXF  (long size);                 // yeni tampon
     ~C_SDXF ();
     // yöntemler:

     void init  (void);                   // eski tampon
     void init  (Byte *cont);             // eski tampon
     void init  (Byte *cont, long size);  // yeni tampon
     void init  (long size);              // yeni tampon

     void enter   (void);
     void leave   (void);
     void next    (void);
     long extract (Byte *data, long length);    // chars, bits
     long extract (void);                       // sayısal veri
     void create  (ChunkID);                    // yapısal
     void create  (ChunkID, long value);        // sayısal
     void create  (ChunkID, double fvalue);     // kayan noktalı
     void create  (ChunkID, Byte *data, long length);// ikili
     void create  (ChunkID, char *data);             // karakter
     void set_compression (Byte compression_method);
     void set_encryption  (Byte *encryption_key);

     // arayüz:

     ChunkID  id;        // 8.4.1'e bakın
     short    dataType;  // 8.4.2'e bakın
     long     length;    // verinin boyu ya da yığın

     long     value;
     double   fvalue;
     short    rc;  // ham dönüş değeri 8.4.3'e bak
     short    ec;  // gelişmiş dönüş değeri 8.4.4'e bak

     protected:
     // uygulama bağımlı...

   };
-------------------------------------------------------------------------
   typedef struct
   {
    Byte            *toHost;        // Trans tab net -> host
    Byte            *toNet;         // Trans tab host -> net
    int              maxlevel;      // highest possible level
    int              translation;   // translation net <-> host
                                    // is in effect=1 or not=0
    TEncryptProc    *encryptProc;   // alternate encryption routine
    TGetTablesProc  *getTablesProc; // alternate routine defining
                                    // translation Tables
    TcvtUTF8Proc    *convertUTF8;   // routine to convert to/from UTF-8
   }  SDX_TOptions;

   typedef long TencryptProc (
     int   mode,   // 1= kripyolamak, 2= kripto açma, 3= kriptolu uzunluk
     Byte *buffer, // kriptolanack/açılacak veri
     long  len,    // len: atmpon boyu
     char *passw); // Şifre

    // kriptolanmış/açılmış veri uzunluğunu döndürür
    // (tampon parametre ve şifre mode=3 için dikkate alınmaz)
    // mode=3 ve len=0 için öbekboyunu döndürür
    // öbeksiz algoritmalar için öbekboyu sıfırdır

   typedef int TGetTablesProc (Byte **toNet, Byte **toHost);
    // toNet, toHost: çıkış parametrelerine gösterge
    // her ikisi de 256 baytlık çevirim tanlosuna göstergedir
    // başarıyı döndürür: 1 = ok, 0 = hata.

   typedef int TcvtUTF8Proc
   ( int   mode,     // 1 = UTF-8'e, 2 = UTF-8'den
     Byte *target, int *targetlength,  // çıkış
     Byte *source, int sourcelength);  // giriş
   // hedefboyu giriş olarak en büyük boyu içerir
   // başarı döndürür: 1 = ok, 0 = çevirim yok
?>
