DROP TABLE `friends`;
CREATE TABLE `friends` (
  `usrNo` CHAR(20) UNICODE NOT NULL,
  `friendNo` CHAR(20) UNICODE NOT NULL,
  `ukod` CHAR(10) UNICODE NOT NULL,
  PRIMARY KEY (`usrNo`,`friendNo`)
)
ENGINE = MyISAM;
