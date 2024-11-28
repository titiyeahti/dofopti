import {exec} from "child_process";

export async function RunOptimisation(SuccessCallback, ErrorCallback){
    exec("./dofopti.out inputfiles/discord.in", async (err, stdout, stderr) => {
        if (err) {
            await ErrorCallback(err)
            return;
        }

        await SuccessCallback(stdout)
    });
}


export function MakeInputHeader(){
    return "%%%TITI PLEASE!%%%\n";
}
export function MakeInputTemplate(){
    const input = MakeInputHeader() +
        "#LEVEL\n" +
        "200\n" +
        "#TARGETED_SLOTS\n" +
        "amulet 1\n" +
        "hat 1\n" +
        "ring 2\n" +
        "weapon 1\n" +
        "shield 1\n" +
        "belt 1\n" +
        "back 1\n" +
        "boots 1\n" +
        "dofus 6\n" +
        "prysmaradite 1\n" +
        "pet 1\n" +
        "#BASE_STATS\n" +
        "cha 100\n" +
        "str 100\n" +
        "int 100\n" +
        "agi 100\n" +
        "pa 1\n" +
        "pm 1\n" +
        "#OBJECTIVE\n" +
        "str 1.\n" +
        "pui 1.\n" +
        "#CONSTRAINTS\n" +
        "vit >= 2999\n" +
        "#LOCK_ITEMS\n" +
        "-shaker\n"
    
    return input;
}