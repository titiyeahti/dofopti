select bonuses.id_pano, nb_items, stat_code, value from bonuses join moche on bonuses.id_pano = moche.id_pano order by bonuses.id_pano;

select bonuses.id_pano, count(*) from bonuses join moche on bonuses.id_pano = moche.id_pano group by bonuses.id_pano order by bonuses.id_pano;

select * from panos where id = 499;

select * from bonuses where id_pano = 277;

select pid, count(*) from bonuses join (SELECT DISTINCT id_pano as pid from moche) as rig on pid = id_pano GROUP by pid order by pid;

select 