DROP TABLE `ulog`;
CREATE TABLE `ulog` (
  `userNo` CHAR(20) UNICODE NOT NULL COMMENT 'uye no',
  `logNo` CHAR(20) UNICODE NOT NULL COMMENT 'log numarasi',
  `logTar` CHAR(8) UNICODE NOT NULL COMMENT 'log tarihi',
  `logSaat` CHAR(5) UNICODE NOT NULL COMMENT 'log saati',
  `logKod` CHAR(30) NOT NULL COMMENT 'log kodu',
  `logMsg` CHAR(250) UNICODE NOT NULL COMMENT 'log mesaji',
  PRIMARY KEY (`userNo`,`logNo`)
)
ENGINE = MyISAM
COMMENT = 'istekler';
