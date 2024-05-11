DROP TABLE `uygalan`;
CREATE TABLE `uygalan` (
  `uygField` INT UNSIGNED NOT NULL,
  `uygFNo` INT UNSIGNED NOT NULL,
  `uygSz` INT UNSIGNED NOT NULL,
  `uygTip` CHAR(3) NOT NULL,
  `uygDesc` CHAR(200) UNICODE NOT NULL,
  `uygkod` CHAR(10) NOT NULL,
  PRIMARY KEY (`uygField`, `uygFNo`)
)
ENGINE = MyISAM;
