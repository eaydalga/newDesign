DROP TABLE `user`;
CREATE TABLE `user` (
  `usrNo` CHAR(20) UNICODE NOT NULL COMMENT 'uluslar arasi usr numarasi',
  `email` CHAR(30) NOT NULL COMMENT 'email adresi',
  `passwd` CHAR(20) UNICODE NOT NULL COMMENT 'kullanici sifresi',
  `fullName` CHAR(70) UNICODE NOT NULL COMMENT 'Kullanici adi',
  `usrkod` CHAR(10) NOT NULL COMMENT 'gecerli kullanici kod',
  PRIMARY KEY (`email`)
)
ENGINE = MyISAM
COMMENT = 'rotary kullanicilari';
