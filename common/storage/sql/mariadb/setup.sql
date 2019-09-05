CREATE USER 'ciri'@'localhost' IDENTIFIED BY '';
CREATE DATABASE `tangle` CHARACTER SET utf8;
GRANT ALL PRIVILEGES ON `tangle` . * TO 'ciri'@'localhost';
CREATE DATABASE `spent-addresses` CHARACTER SET utf8;
GRANT ALL PRIVILEGES ON `spent-addresses` . * TO 'ciri'@'localhost';
FLUSH PRIVILEGES;
