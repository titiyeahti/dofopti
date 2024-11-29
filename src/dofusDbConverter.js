import { encode, decode } from "@msgpack/msgpack";
import {readFile} from 'fs';

function SaveStuff(e) {
    var t = {};
    t[0] = {};
    for (var a = 0; a < e.BaseCaracs.length; a += 1)
        0 != e.BaseCaracs[a] && (t[0][a] = e.BaseCaracs[a]);
    
    t[0][0] = '' + (parseInt(t[0][0]) - 100)
    t[1] = e.AdditionalPoints,
        t[2] = e.Level,
        t[3] = 0,
        t[3] |= e.Poney << 5,
        t[3] |= e.PuissanceCarac << 4,
        t[3] |= e.AllowDamage << 3,
        t[3] |= e.ExoPA << 2,
        t[3] |= e.ExoPM << 1,
        t[3] |= e.ExoPO << 0,
        t[4] = {};
    for (var a = 0; a < e.NumPicks.length; a += 1)
        1 != e.NumPicks[a] && (t[4][a] = e.NumPicks[a]);
    t[5] = [];
    for (var a = 0; a < e.ItemIds.length; a += 1)
        for (var n = 0; n < e.ItemIds[a].length; n += 1)
            t[5].push(e.ItemIds[a][n]);
    return encode(t)
}

export async function TreatJson(Success, Fail){
    await readFile('discord.json', 'utf8', async function(err, data) {
        if (err) {
            await Fail(err)
            return;
        }
        const jdata = JSON.parse(data)
        const byteArray = SaveStuff(jdata);
        var c = ""
        for (var o = 0; o < byteArray.length; o += 1){
            c += String.fromCharCode(byteArray[o]);
        }
        await Success("https://www.dofusbook.net/fr/equipement/dofus-stuffer/objets?stuff=" + btoa(c), jdata.Result);
    });
}
