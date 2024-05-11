DROP TABLE `mesaj`;
CREATE TABLE `mesaj` (
  `toNo` CHAR(20) UNICODE NOT NULL COMMENT 'isteği alan numarasi',
  `msgNo` CHAR(30) UNICODE NOT NULL COMMENT 'mesaj No',
  `fromNo` CHAR(20) UNICODE NOT NULL COMMENT 'isteği gönderen numarasi',
  `uygMsg` CHAR(250) UNICODE NOT NULL COMMENT 'Uygulama mesaji',
  `uygEkler` CHAR(200) UNICODE NOT NULL COMMENT 'Uygulama ekleri',
  `rmsgkod` CHAR(10) NOT NULL COMMENT 'mesaj kodu',
  PRIMARY KEY (`toNo`,`msgNo`)
)
ENGINE = MyISAM
COMMENT = 'mesaj islemleri';
