DROP TABLE `reklam`;
CREATE TABLE `reklam` (
  `reklamNo` CHAR(20) UNICODE NOT NULL COMMENT 'reklam numarasi XXX-NNNNNNNN',
  `basTar` CHAR(8) UNICODE NOT NULL COMMENT 'baslangic tarihi YYYYAAGG',
  `bitTar` CHAR(8) UNICODE NOT NULL COMMENT 'bitis tarihi YYYYAAGG',
  `dosyaAdi` CHAR(250) UNICODE NOT NULL COMMENT 'Dosya yolu',
  `rkod` CHAR(10) NOT NULL COMMENT 'reklam kodu',
  PRIMARY KEY (`reklamNo`)
)
ENGINE = MyISAM
COMMENT = 'reklamlar';
