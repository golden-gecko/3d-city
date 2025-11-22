-- phpMyAdmin SQL Dump
-- version 2.11.4
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Czas wygenerowania: 22 Lut 2009, 18:07
-- Wersja serwera: 5.0.45
-- Wersja PHP: 5.2.5

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";

--
-- Baza danych: `city`
--

-- --------------------------------------------------------

--
-- Struktura tabeli dla  `buildings`
--

DROP TABLE IF EXISTS `buildings`;
CREATE TABLE IF NOT EXISTS `buildings` (
  `map_id` int(11) NOT NULL,
  `building_id` int(11) NOT NULL,
  `street_id` int(11) default NULL,
  `name` varchar(500) collate utf8_polish_ci NOT NULL,
  `description` varchar(5000) collate utf8_polish_ci default NULL,
  `number` int(11) NOT NULL default '0',
  `height` float NOT NULL default '30',
  `texture` varchar(500) collate utf8_polish_ci default NULL,
  PRIMARY KEY  (`building_id`),
  UNIQUE KEY `map_id` (`map_id`,`building_id`),
  KEY `street_id` (`street_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_polish_ci;

-- --------------------------------------------------------

--
-- Struktura tabeli dla  `buildings_points`
--

DROP TABLE IF EXISTS `buildings_points`;
CREATE TABLE IF NOT EXISTS `buildings_points` (
  `building_id` int(11) NOT NULL,
  `number` int(11) NOT NULL,
  `x` float NOT NULL,
  `y` float default NULL,
  `z` float NOT NULL,
  UNIQUE KEY `building_id` (`building_id`,`number`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_polish_ci;

-- --------------------------------------------------------

--
-- Struktura tabeli dla  `junctions`
--

DROP TABLE IF EXISTS `junctions`;
CREATE TABLE IF NOT EXISTS `junctions` (
  `map_id` int(11) NOT NULL,
  `junction_id` int(11) NOT NULL,
  `name` varchar(500) collate utf8_polish_ci NOT NULL,
  `description` varchar(5000) collate utf8_polish_ci NOT NULL,
  `has_traffic_lights` tinyint(1) NOT NULL default '0',
  `center_x` float NOT NULL,
  `center_y` float default NULL,
  `center_z` float NOT NULL,
  PRIMARY KEY  (`junction_id`),
  UNIQUE KEY `map_id` (`map_id`,`junction_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_polish_ci;

-- --------------------------------------------------------

--
-- Struktura tabeli dla  `junctions_routes`
--

DROP TABLE IF EXISTS `junctions_routes`;
CREATE TABLE IF NOT EXISTS `junctions_routes` (
  `junction_id` int(11) NOT NULL,
  `route_id` int(11) NOT NULL,
  UNIQUE KEY `junction_id` (`junction_id`,`route_id`),
  KEY `route_id` (`route_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_polish_ci;

-- --------------------------------------------------------

--
-- Struktura tabeli dla  `maps`
--

DROP TABLE IF EXISTS `maps`;
CREATE TABLE IF NOT EXISTS `maps` (
  `map_id` int(11) NOT NULL,
  `name` varchar(500) collate utf8_polish_ci NOT NULL,
  `description` varchar(5000) collate utf8_polish_ci default NULL,
  `size_x` float NOT NULL,
  `size_z` float NOT NULL,
  `terrain_size_x` int(11) NOT NULL,
  `terrain_size_z` int(11) NOT NULL,
  `center_latitude` double NOT NULL,
  `center_longitude` double NOT NULL,
  `zoom` float NOT NULL,
  `texture` varchar(500) collate utf8_polish_ci default NULL,
  PRIMARY KEY  (`map_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_polish_ci;

-- --------------------------------------------------------

--
-- Struktura tabeli dla  `maps_terrain`
--

DROP TABLE IF EXISTS `maps_terrain`;
CREATE TABLE IF NOT EXISTS `maps_terrain` (
  `map_id` int(11) NOT NULL,
  `offset` int(11) NOT NULL,
  `value` float NOT NULL,
  UNIQUE KEY `map_id` (`map_id`,`offset`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_polish_ci;

-- --------------------------------------------------------

--
-- Struktura tabeli dla  `plants`
--

DROP TABLE IF EXISTS `plants`;
CREATE TABLE IF NOT EXISTS `plants` (
  `map_id` int(11) NOT NULL,
  `plant_id` int(11) NOT NULL,
  `name` varchar(500) collate utf8_polish_ci NOT NULL,
  `description` varchar(5000) collate utf8_polish_ci default NULL,
  `height` float NOT NULL default '20',
  PRIMARY KEY  (`plant_id`),
  UNIQUE KEY `map_id` (`map_id`,`plant_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_polish_ci;

-- --------------------------------------------------------

--
-- Struktura tabeli dla  `plants_points`
--

DROP TABLE IF EXISTS `plants_points`;
CREATE TABLE IF NOT EXISTS `plants_points` (
  `plant_id` int(11) NOT NULL,
  `number` int(11) NOT NULL,
  `x` float NOT NULL,
  `y` float default NULL,
  `z` float NOT NULL,
  UNIQUE KEY `plant_id` (`plant_id`,`number`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_polish_ci;

-- --------------------------------------------------------

--
-- Struktura tabeli dla  `routes`
--

DROP TABLE IF EXISTS `routes`;
CREATE TABLE IF NOT EXISTS `routes` (
  `map_id` int(11) NOT NULL,
  `route_id` int(11) NOT NULL,
  `street_id` int(11) default NULL,
  `name` varchar(500) collate utf8_polish_ci NOT NULL,
  `description` varchar(5000) collate utf8_polish_ci default NULL,
  `avarage_speed` float NOT NULL default '45',
  `max_speed` smallint(6) NOT NULL default '50',
  `width` float NOT NULL default '10',
  PRIMARY KEY  (`route_id`),
  UNIQUE KEY `map_id` (`map_id`,`route_id`),
  KEY `street_id` (`street_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_polish_ci;

-- --------------------------------------------------------

--
-- Struktura tabeli dla  `routes_points`
--

DROP TABLE IF EXISTS `routes_points`;
CREATE TABLE IF NOT EXISTS `routes_points` (
  `route_id` int(11) NOT NULL,
  `number` int(11) NOT NULL,
  `x` float NOT NULL,
  `y` float default NULL,
  `z` float NOT NULL,
  UNIQUE KEY `route_id` (`route_id`,`number`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_polish_ci;

-- --------------------------------------------------------

--
-- Struktura tabeli dla  `streets`
--

DROP TABLE IF EXISTS `streets`;
CREATE TABLE IF NOT EXISTS `streets` (
  `map_id` int(11) NOT NULL,
  `street_id` int(11) NOT NULL,
  `name` varchar(500) collate utf8_polish_ci NOT NULL,
  `description` varchar(5000) collate utf8_polish_ci default NULL,
  PRIMARY KEY  (`street_id`),
  UNIQUE KEY `map_id` (`map_id`,`street_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_polish_ci;

-- --------------------------------------------------------

--
-- Stand-in structure for view `v_buildings`
--
CREATE TABLE IF NOT EXISTS `v_buildings` (
`building_id` int(11)
,`map_id` int(11)
,`name` varchar(500)
,`description` varchar(5000)
,`number` int(11)
,`height` float
,`texture` varchar(500)
);
-- --------------------------------------------------------

--
-- Stand-in structure for view `v_buildings_points`
--
CREATE TABLE IF NOT EXISTS `v_buildings_points` (
`map_id` int(11)
,`building_id` int(11)
,`number` int(11)
,`x` float
,`y` float
,`z` float
);
-- --------------------------------------------------------

--
-- Stand-in structure for view `v_junction`
--
CREATE TABLE IF NOT EXISTS `v_junction` (
`map_id` int(11)
,`junction_id` int(11)
,`name` varchar(500)
,`description` varchar(5000)
,`has_traffic_lights` tinyint(1)
,`center_x` float
,`center_y` float
,`center_z` float
);
-- --------------------------------------------------------

--
-- Stand-in structure for view `v_junctions_routes`
--
CREATE TABLE IF NOT EXISTS `v_junctions_routes` (
`map_id` int(11)
,`junction_id` int(11)
,`route_id` int(11)
);
-- --------------------------------------------------------

--
-- Stand-in structure for view `v_maps`
--
CREATE TABLE IF NOT EXISTS `v_maps` (
`map_id` int(11)
,`name` varchar(500)
,`description` varchar(5000)
,`size_x` float
,`size_z` float
,`terrain_size_x` int(11)
,`terrain_size_z` int(11)
,`center_latitude` double
,`center_longitude` double
,`zoom` float
,`texture` varchar(500)
);
-- --------------------------------------------------------

--
-- Stand-in structure for view `v_maps_terrain`
--
CREATE TABLE IF NOT EXISTS `v_maps_terrain` (
`map_id` int(11)
,`offset` int(11)
,`value` float
);
-- --------------------------------------------------------

--
-- Stand-in structure for view `v_plants`
--
CREATE TABLE IF NOT EXISTS `v_plants` (
`map_id` int(11)
,`plant_id` int(11)
,`name` varchar(500)
,`description` varchar(5000)
,`height` float
);
-- --------------------------------------------------------

--
-- Stand-in structure for view `v_plants_points`
--
CREATE TABLE IF NOT EXISTS `v_plants_points` (
`map_id` int(11)
,`plant_id` int(11)
,`number` int(11)
,`x` float
,`y` float
,`z` float
);
-- --------------------------------------------------------

--
-- Stand-in structure for view `v_routes`
--
CREATE TABLE IF NOT EXISTS `v_routes` (
`map_id` int(11)
,`route_id` int(11)
,`name` varchar(500)
,`description` varchar(5000)
,`avarage_speed` float
,`max_speed` smallint(6)
,`width` float
);
-- --------------------------------------------------------

--
-- Stand-in structure for view `v_routes_points`
--
CREATE TABLE IF NOT EXISTS `v_routes_points` (
`map_id` int(11)
,`route_id` int(11)
,`number` int(11)
,`x` float
,`y` float
,`z` float
);
-- --------------------------------------------------------

--
-- Stand-in structure for view `v_water_objects`
--
CREATE TABLE IF NOT EXISTS `v_water_objects` (
`map_id` int(11)
,`water_object_id` int(11)
,`name` varchar(500)
,`description` varchar(5000)
,`is_river` tinyint(1)
,`width` float
);
-- --------------------------------------------------------

--
-- Stand-in structure for view `v_water_objects_points`
--
CREATE TABLE IF NOT EXISTS `v_water_objects_points` (
`map_id` int(11)
,`water_object_id` int(11)
,`number` int(11)
,`x` float
,`y` float
,`z` float
);
-- --------------------------------------------------------

--
-- Struktura tabeli dla  `water_objects`
--

DROP TABLE IF EXISTS `water_objects`;
CREATE TABLE IF NOT EXISTS `water_objects` (
  `map_id` int(11) NOT NULL,
  `water_object_id` int(11) NOT NULL,
  `name` varchar(500) collate utf8_polish_ci NOT NULL,
  `description` varchar(5000) collate utf8_polish_ci default NULL,
  `is_river` tinyint(1) NOT NULL default '0',
  `width` float NOT NULL default '20',
  PRIMARY KEY  (`water_object_id`),
  UNIQUE KEY `map_id` (`map_id`,`water_object_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_polish_ci;

-- --------------------------------------------------------

--
-- Struktura tabeli dla  `water_objects_points`
--

DROP TABLE IF EXISTS `water_objects_points`;
CREATE TABLE IF NOT EXISTS `water_objects_points` (
  `water_object_id` int(11) NOT NULL,
  `number` int(11) NOT NULL,
  `x` float NOT NULL,
  `y` float default NULL,
  `z` float NOT NULL,
  UNIQUE KEY `water_object_id` (`water_object_id`,`number`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_polish_ci;

-- --------------------------------------------------------

--
-- Structure for view `v_buildings`
--
DROP TABLE IF EXISTS `v_buildings`;

DROP VIEW IF EXISTS `v_buildings`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `city`.`v_buildings` AS select `city`.`buildings`.`building_id` AS `building_id`,`city`.`buildings`.`map_id` AS `map_id`,`city`.`buildings`.`name` AS `name`,`city`.`buildings`.`description` AS `description`,`city`.`buildings`.`number` AS `number`,`city`.`buildings`.`height` AS `height`,`city`.`buildings`.`texture` AS `texture` from `city`.`buildings`;

-- --------------------------------------------------------

--
-- Structure for view `v_buildings_points`
--
DROP TABLE IF EXISTS `v_buildings_points`;

DROP VIEW IF EXISTS `v_buildings_points`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `city`.`v_buildings_points` AS select `city`.`buildings`.`map_id` AS `map_id`,`city`.`buildings`.`building_id` AS `building_id`,`city`.`buildings_points`.`number` AS `number`,`city`.`buildings_points`.`x` AS `x`,`city`.`buildings_points`.`y` AS `y`,`city`.`buildings_points`.`z` AS `z` from (`city`.`buildings` join `city`.`buildings_points` on((`city`.`buildings`.`building_id` = `city`.`buildings_points`.`building_id`)));

-- --------------------------------------------------------

--
-- Structure for view `v_junction`
--
DROP TABLE IF EXISTS `v_junction`;

DROP VIEW IF EXISTS `v_junction`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `city`.`v_junction` AS select `city`.`junctions`.`map_id` AS `map_id`,`city`.`junctions`.`junction_id` AS `junction_id`,`city`.`junctions`.`name` AS `name`,`city`.`junctions`.`description` AS `description`,`city`.`junctions`.`has_traffic_lights` AS `has_traffic_lights`,`city`.`junctions`.`center_x` AS `center_x`,`city`.`junctions`.`center_y` AS `center_y`,`city`.`junctions`.`center_z` AS `center_z` from `city`.`junctions`;

-- --------------------------------------------------------

--
-- Structure for view `v_junctions_routes`
--
DROP TABLE IF EXISTS `v_junctions_routes`;

DROP VIEW IF EXISTS `v_junctions_routes`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `city`.`v_junctions_routes` AS select `city`.`junctions`.`map_id` AS `map_id`,`city`.`junctions`.`junction_id` AS `junction_id`,`city`.`junctions_routes`.`route_id` AS `route_id` from (`city`.`junctions` join `city`.`junctions_routes` on((`city`.`junctions`.`junction_id` = `city`.`junctions_routes`.`junction_id`)));

-- --------------------------------------------------------

--
-- Structure for view `v_maps`
--
DROP TABLE IF EXISTS `v_maps`;

DROP VIEW IF EXISTS `v_maps`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `city`.`v_maps` AS select `city`.`maps`.`map_id` AS `map_id`,`city`.`maps`.`name` AS `name`,`city`.`maps`.`description` AS `description`,`city`.`maps`.`size_x` AS `size_x`,`city`.`maps`.`size_z` AS `size_z`,`city`.`maps`.`terrain_size_x` AS `terrain_size_x`,`city`.`maps`.`terrain_size_z` AS `terrain_size_z`,`city`.`maps`.`center_latitude` AS `center_latitude`,`city`.`maps`.`center_longitude` AS `center_longitude`,`city`.`maps`.`zoom` AS `zoom`,`city`.`maps`.`texture` AS `texture` from `city`.`maps`;

-- --------------------------------------------------------

--
-- Structure for view `v_maps_terrain`
--
DROP TABLE IF EXISTS `v_maps_terrain`;

DROP VIEW IF EXISTS `v_maps_terrain`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `city`.`v_maps_terrain` AS select `city`.`maps_terrain`.`map_id` AS `map_id`,`city`.`maps_terrain`.`offset` AS `offset`,`city`.`maps_terrain`.`value` AS `value` from `city`.`maps_terrain`;

-- --------------------------------------------------------

--
-- Structure for view `v_plants`
--
DROP TABLE IF EXISTS `v_plants`;

DROP VIEW IF EXISTS `v_plants`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `city`.`v_plants` AS select `city`.`plants`.`map_id` AS `map_id`,`city`.`plants`.`plant_id` AS `plant_id`,`city`.`plants`.`name` AS `name`,`city`.`plants`.`description` AS `description`,`city`.`plants`.`height` AS `height` from `city`.`plants`;

-- --------------------------------------------------------

--
-- Structure for view `v_plants_points`
--
DROP TABLE IF EXISTS `v_plants_points`;

DROP VIEW IF EXISTS `v_plants_points`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `city`.`v_plants_points` AS select `city`.`plants`.`map_id` AS `map_id`,`city`.`plants`.`plant_id` AS `plant_id`,`city`.`plants_points`.`number` AS `number`,`city`.`plants_points`.`x` AS `x`,`city`.`plants_points`.`y` AS `y`,`city`.`plants_points`.`z` AS `z` from (`city`.`plants` join `city`.`plants_points` on((`city`.`plants`.`plant_id` = `city`.`plants_points`.`plant_id`)));

-- --------------------------------------------------------

--
-- Structure for view `v_routes`
--
DROP TABLE IF EXISTS `v_routes`;

DROP VIEW IF EXISTS `v_routes`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `city`.`v_routes` AS select `city`.`routes`.`map_id` AS `map_id`,`city`.`routes`.`route_id` AS `route_id`,`city`.`routes`.`name` AS `name`,`city`.`routes`.`description` AS `description`,`city`.`routes`.`avarage_speed` AS `avarage_speed`,`city`.`routes`.`max_speed` AS `max_speed`,`city`.`routes`.`width` AS `width` from `city`.`routes`;

-- --------------------------------------------------------

--
-- Structure for view `v_routes_points`
--
DROP TABLE IF EXISTS `v_routes_points`;

DROP VIEW IF EXISTS `v_routes_points`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `city`.`v_routes_points` AS select `city`.`routes`.`map_id` AS `map_id`,`city`.`routes`.`route_id` AS `route_id`,`city`.`routes_points`.`number` AS `number`,`city`.`routes_points`.`x` AS `x`,`city`.`routes_points`.`y` AS `y`,`city`.`routes_points`.`z` AS `z` from (`city`.`routes` join `city`.`routes_points` on((`city`.`routes`.`route_id` = `city`.`routes_points`.`route_id`)));

-- --------------------------------------------------------

--
-- Structure for view `v_water_objects`
--
DROP TABLE IF EXISTS `v_water_objects`;

DROP VIEW IF EXISTS `v_water_objects`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `city`.`v_water_objects` AS select `city`.`water_objects`.`map_id` AS `map_id`,`city`.`water_objects`.`water_object_id` AS `water_object_id`,`city`.`water_objects`.`name` AS `name`,`city`.`water_objects`.`description` AS `description`,`city`.`water_objects`.`is_river` AS `is_river`,`city`.`water_objects`.`width` AS `width` from `city`.`water_objects`;

-- --------------------------------------------------------

--
-- Structure for view `v_water_objects_points`
--
DROP TABLE IF EXISTS `v_water_objects_points`;

DROP VIEW IF EXISTS `v_water_objects_points`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`localhost` SQL SECURITY DEFINER VIEW `city`.`v_water_objects_points` AS select `city`.`water_objects`.`map_id` AS `map_id`,`city`.`water_objects`.`water_object_id` AS `water_object_id`,`city`.`water_objects_points`.`number` AS `number`,`city`.`water_objects_points`.`x` AS `x`,`city`.`water_objects_points`.`y` AS `y`,`city`.`water_objects_points`.`z` AS `z` from (`city`.`water_objects` join `city`.`water_objects_points` on((`city`.`water_objects`.`water_object_id` = `city`.`water_objects_points`.`water_object_id`)));

--
-- Ograniczenia dla zrzutów tabel
--

--
-- Ograniczenia dla tabeli `buildings`
--
ALTER TABLE `buildings`
  ADD CONSTRAINT `buildings_ibfk_1` FOREIGN KEY (`map_id`) REFERENCES `maps` (`map_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `buildings_ibfk_2` FOREIGN KEY (`street_id`) REFERENCES `streets` (`street_id`) ON DELETE SET NULL ON UPDATE CASCADE;

--
-- Ograniczenia dla tabeli `buildings_points`
--
ALTER TABLE `buildings_points`
  ADD CONSTRAINT `buildings_points_ibfk_1` FOREIGN KEY (`building_id`) REFERENCES `buildings` (`building_id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Ograniczenia dla tabeli `junctions`
--
ALTER TABLE `junctions`
  ADD CONSTRAINT `junctions_ibfk_1` FOREIGN KEY (`map_id`) REFERENCES `maps` (`map_id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Ograniczenia dla tabeli `junctions_routes`
--
ALTER TABLE `junctions_routes`
  ADD CONSTRAINT `junctions_routes_ibfk_1` FOREIGN KEY (`junction_id`) REFERENCES `junctions` (`junction_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `junctions_routes_ibfk_2` FOREIGN KEY (`route_id`) REFERENCES `routes` (`route_id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Ograniczenia dla tabeli `maps_terrain`
--
ALTER TABLE `maps_terrain`
  ADD CONSTRAINT `maps_terrain_ibfk_1` FOREIGN KEY (`map_id`) REFERENCES `maps` (`map_id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Ograniczenia dla tabeli `plants`
--
ALTER TABLE `plants`
  ADD CONSTRAINT `plants_ibfk_1` FOREIGN KEY (`map_id`) REFERENCES `maps` (`map_id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Ograniczenia dla tabeli `plants_points`
--
ALTER TABLE `plants_points`
  ADD CONSTRAINT `plants_points_ibfk_1` FOREIGN KEY (`plant_id`) REFERENCES `plants` (`plant_id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Ograniczenia dla tabeli `routes`
--
ALTER TABLE `routes`
  ADD CONSTRAINT `routes_ibfk_1` FOREIGN KEY (`map_id`) REFERENCES `maps` (`map_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `routes_ibfk_2` FOREIGN KEY (`street_id`) REFERENCES `streets` (`street_id`) ON DELETE SET NULL ON UPDATE CASCADE;

--
-- Ograniczenia dla tabeli `routes_points`
--
ALTER TABLE `routes_points`
  ADD CONSTRAINT `routes_points_ibfk_1` FOREIGN KEY (`route_id`) REFERENCES `routes` (`route_id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Ograniczenia dla tabeli `streets`
--
ALTER TABLE `streets`
  ADD CONSTRAINT `streets_ibfk_1` FOREIGN KEY (`map_id`) REFERENCES `maps` (`map_id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Ograniczenia dla tabeli `water_objects`
--
ALTER TABLE `water_objects`
  ADD CONSTRAINT `water_objects_ibfk_1` FOREIGN KEY (`map_id`) REFERENCES `maps` (`map_id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Ograniczenia dla tabeli `water_objects_points`
--
ALTER TABLE `water_objects_points`
  ADD CONSTRAINT `water_objects_points_ibfk_1` FOREIGN KEY (`water_object_id`) REFERENCES `water_objects` (`water_object_id`) ON DELETE CASCADE ON UPDATE CASCADE;

DELIMITER $$
--
-- Procedury
--
CREATE DEFINER=`root`@`localhost` PROCEDURE `begin_transaction`()
start transaction$$

DELIMITER ;
