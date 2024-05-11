DROP TABLE `useruyg`;
CREATE TABLE `useruyg` (
  `usrNo` CHAR(20) UNICODE NOT NULL,
  `uygKodu` INT UNSIGNED NOT NULL,
  `uygSecNo` INT UNSIGNED NOT NULL,
  `uukod` CHAR(10) NOT NULL,
  PRIMARY KEY (`usrNo`,`uygKodu`)
)
ENGINE = MyISAM;
