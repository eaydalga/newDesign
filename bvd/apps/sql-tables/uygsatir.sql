DROP TABLE `uygsatir`;
CREATE TABLE `uygsatir` (
  `uygSecNo` INT UNSIGNED NOT NULL,
  `uygSayi` INT UNSIGNED NOT NULL,
  `uygHtml` CHAR(200) UNICODE NOT NULL,
  `uygInField` CHAR(10) NOT NULL,
  `uygOutField` CHAR(10) NOT NULL,
  `uygTers` BOOLEAN  NOT NULL,
  `uygDevam` BOOLEAN  NOT NULL,
  `uygIptal` BOOLEAN  NOT NULL,
  `uygAtla` BOOLEAN  NOT NULL,
  PRIMARY KEY (`uygSecNo`, `uygSayi`)
)
ENGINE = MyISAM;
