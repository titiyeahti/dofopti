select * from 
	(select panos.id, panos.name, max(bonuses.nb_items) as size, count(distinct items.id) as item_count from 
		panos join items on items.id_pano = panos.id JOIN bonuses on panos.id = bonuses.id_pano GROUP by panos.id)
	where size != item_count;

drop VIEW if exists item_selection ;	
create TEMPORARY VIEW item_selection as select * from items where min_lvl > 199;

select id from item_selection;

drop VIEW if exists pano_selection;
create TEMPORARY view pano_selection as select panos.id as id, panos.name as name from panos inner join item_selection on panos.id = id_pano group by panos.id;

select name, id_item, maxval from stats join item_selection on stats.id_item = item_selection.id where stat = "strength";

select id, nb_items, stat_code, value from pano_selection JOIN bonuses on pano_selection.id = bonuses.id_pano where stat = "strength" order by id;