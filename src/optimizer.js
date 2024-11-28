import {exec} from "child_process";
import util from 'util';

const execPromise = util.promisify(exec);

export async function RunOptimisationAsync() {
    try {
        const { stdout, stderr } = await execPromise("./dofopti.out inputfiles/discord.in discord.json");
        if (stderr) {
            throw new Error(stderr); // Handle errors from stderr
        }
        return stdout; // Return stdout as the success result
    } catch (error) {
        throw error; // Throw an error to be handled by the caller
    }
}

export function RunOptimisation(SuccessCallback, ErrorCallback){
    exec("./dofopti.out inputfiles/discord.in", (err, stdout, stderr) => {
        if (err) {
            ErrorCallback(stderr)
            return;
        }
        SuccessCallback(stdout)
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