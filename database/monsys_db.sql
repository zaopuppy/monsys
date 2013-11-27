
create database if not exists monsys_db character set = utf8;

create user 'monsys'@'%' identified by 'monsys';
GRANT ALL PRIVILEGES ON * . * TO 'monsys'@'%';
flush privileges;

use monsys_db;

create table if not exists fgw_list (
  account varchar(30) not null,
  device varchar(30) not null,
  pubkey char(30) not null
);

-- alter table account_info add fgw_list varchar(100);
-- alter table account_info add nickname varchar(30) not null default 'User';
create table if not exists account_info (
  account varchar(30) not null,
  password varchar(30) not null,
  salt char(5) not null,
  nickname varchar(30) not null,
  fgw_list varchar(1024)
);


