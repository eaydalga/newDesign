DROP TABLE `mesajek`;
CREATE TABLE `mesajek` (
  `msgNo` CHAR(20) UNICODE NOT NULL COMMENT 'mesaj numarasi',
  `ekNo` CHAR(20) UNICODE NOT NULL COMMENT 'mesaj ek numarasi',
  `ekBilgi` CHAR(250) UNICODE NOT NULL COMMENT 'mesaj ek bilgisi',
  PRIMARY KEY (`msgNo`,`ekNo`)
)
ENGINE = MyISAM
COMMENT = 'mesaj ekleri';
