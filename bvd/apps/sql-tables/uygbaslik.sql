DROP TABLE `uygbaslik`;
CREATE TABLE `uygbaslik` (
  `uygKodu` INT UNSIGNED NOT NULL,
  `uygSecNo` INT UNSIGNED NOT NULL,
  `uygAdi` CHAR(100) UNICODE NOT NULL,
  PRIMARY KEY (`uygKodu`)
)
ENGINE = MyISAM;
