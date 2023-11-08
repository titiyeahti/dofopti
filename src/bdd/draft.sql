select max(length(name)) from items;


select max(length(name)) from panos;


SELECT substr(name, 0, 31) from items where length(name) > 31;

drop table if EXISTS moche;
create table if not EXISTS moche as select * from items where  ROWID < 0;
insert into moche select * from items as itema where 
(select 1 from stats where (id_item = itema.id and stat = "strength" and maxval > 79)) is not null or 
(itema.id_pano <> 0 and (SELECT id_pano from stats join (select * from items where items.id_pano = itema.id_pano) as curpano 
on stats.id_item = curpano.id 
where stat = "strength" and maxval > 79) is not null) order by slot_code;

alter table moche add tempid INTEGER;

UPDATE  moche set tempid = rowid + 18;

/* usefull */

select count( DISTINCT id_pano) from moche where id_pano is not null;
select panos.name, id_pano, count(*) from moche join panos on moche.id_pano = panos.id group by panos.name;

select distinct stat from consts;

/* for each pano */
select nb_items, stat, stat_code, value from bonuses where id_pano = 17 order by nb_items;
select tempid, stat_code, sign, value from consts join moche on id = id_item;
select * from moche order by id_pano;
