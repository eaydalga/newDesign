DROP TABLE `personel`;
CREATE TABLE `personel` (
  `usrNo` CHAR(20) UNICODE NOT NULL COMMENT 'usr no',
  `bilgiKodu` INT UNSIGNED NOT NULL COMMENT 'tablo dizini 0-9 arasi deger',
  `bilgiAlani` CHAR(200) NOT NULL COMMENT 'tablo dizini degeri',
  `bkod` CHAR(10) NOT NULL COMMENT 'bilgi kodu '
)
ENGINE = MyISAM
COMMENT = 'personel bilgiler';
