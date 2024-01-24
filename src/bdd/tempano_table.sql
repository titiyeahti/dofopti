
drop table if exists mochep;

select * from slots order by code;

create table if not exists mochep as select id_pano, panos.name, count(*) as nb_items from moche join panos on moche.id_pano = panos.id group by id_pano order by id_pano;

select * from mochep;

alter table mochep add tempid integer;

alter table mochep add b_count integer;

update mochep set b_count = (select count(DISTINCT bonuses.nb_items) + 2 from bonuses where bonuses.id_pano = mochep.id_pano and bonuses.nb_items <= mochep.nb_items);

UPDATE mochep set tempid = rowid -1;

select sum(nb_items), sum(b_count) from mochep;
select * from mochep;