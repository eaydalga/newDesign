DROP TABLE `istek`;
CREATE TABLE `istek` (
  `toNo` CHAR(20) UNICODE NOT NULL COMMENT 'isteği alan numarasi',
  `istekNo` CHAR(20) UNICODE NOT NULL COMMENT 'istek sira numarasi',
  `fromNo` CHAR(20) UNICODE NOT NULL COMMENT 'isteği gönderen numarasi',
  `uygKodu` CHAR(30) NOT NULL COMMENT 'Uygulama kodu',
  `uygMsg` CHAR(250) UNICODE NOT NULL COMMENT 'Uygulama mesaji',
  `ikod` CHAR(10) NOT NULL COMMENT 'istek kodu',
  PRIMARY KEY (`toNo`,`istekNo`)
)
ENGINE = MyISAM
COMMENT = 'istekler';
