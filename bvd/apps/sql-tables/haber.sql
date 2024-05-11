DROP TABLE `haber`;
CREATE TABLE `haber` (
  `haberNo` CHAR(20) UNICODE NOT NULL COMMENT 'haber numarasi XXX-NNNNNNNN',
  `baslangic` CHAR(8) UNICODE NOT NULL COMMENT 'baslangic tarihi YYYYAAGG',
  `bitis` CHAR(8) UNICODE NOT NULL COMMENT 'bitis tarihi YYYYAAGG',
  `dosyaAdi` CHAR(250) UNICODE NOT NULL COMMENT 'Dosya yolu',
  `hkod` CHAR(10) NOT NULL COMMENT 'haber kodu',
  PRIMARY KEY (`haberNo`)
)
ENGINE = MyISAM
COMMENT = 'haberler';
