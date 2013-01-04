-- phpMyAdmin SQL Dump
-- version 3.5.2.1
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Nov 05, 2012 at 05:38 AM
-- Server version: 5.5.24-0ubuntu0.12.04.1
-- PHP Version: 5.3.10-1ubuntu3.4

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `picosafe_key`
--

-- --------------------------------------------------------

--
-- Table structure for table `picosafekey`
--

CREATE TABLE IF NOT EXISTS `picosafelogin` (
  `user` varchar(255) NOT NULL,
  `aes` varchar(255) NOT NULL,
  `datablock` varchar(255) NOT NULL,
  `counter` int(11) NOT NULL,
  `hw` int(11) NOT NULL,
  `locked` int(1) NOT NULL,
  PRIMARY KEY (`user`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
