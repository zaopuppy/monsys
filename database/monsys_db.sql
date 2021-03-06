
create user 'monsys'@'127.0.0.1' identified by 'monsys';
create user 'monsys'@'localhost' identified by 'monsys';
create user 'monsys'@'%' identified by 'monsys';
GRANT ALL PRIVILEGES ON * . * TO 'monsys'@'%' identified by 'monsys';
GRANT ALL PRIVILEGES ON * . * TO 'monsys'@'127.0.0.1' identified by 'monsys';
GRANT ALL PRIVILEGES ON * . * TO 'monsys'@'localhost' identified by 'monsys';
flush privileges;

-- select host, user from mysql.user;

create database if not exists monsys_db character set = utf8;

use monsys_db;

-- FGW_LIST
create table if not exists fgw_list (
  account varchar(30) not null,
  device varchar(30) not null,
  pubkey char(30) not null
);

-- alter table account_info add fgw_list varchar(100);
-- alter table account_info add nickname varchar(30) not null default 'User';
-- ACCOUNT_INFO
create table if not exists account_info (
  account varchar(30) not null,
  password varchar(30) not null,
  salt char(5) not null,
  status integer not null,
  nickname varchar(30) not null,
  fgw_list varchar(1024)
);

-- DEVICE_INFO
create table if not exists device_info (
  device_id varchar(30) not null primary key
);

commit;

