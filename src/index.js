import dotenv from 'dotenv'
dotenv.config()
import { Client, Events, GatewayIntentBits, ModalBuilder, TextInputBuilder, TextInputStyle, ActionRowBuilder, InteractionType } from 'discord.js';
import { exec } from 'child_process'
import { encode, decode } from "@msgpack/msgpack";
import {readFile} from 'fs';

function SaveStuff(e) {
    var t = {};
    t[0] = {};
    for (var a = 0; a < e.BaseCaracs.length; a += 1)
        0 != e.BaseCaracs[a] && (t[0][a] = e.BaseCaracs[a]);
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

function TreatJson(){
    readFile('json3.json', 'utf8', function(err, data) {
        if (err) {
            return console.log(err);
        }
        const jdata = JSON.parse(data)
        const byteArray = SaveStuff(jdata);
        console.log(byteArray)
        var c = ""
        for (var o = 0; o < byteArray.length; o += 1){
            c += String.fromCharCode(byteArray[o]);
        }
        console.log(c)
        console.log(btoa(c));
        return btoa(c);
    });
    
}

TreatJson();
/*


const client = new Client({
    intents: [
        GatewayIntentBits.Guilds,
        GatewayIntentBits.GuildMessages,
        GatewayIntentBits.MessageContent
    ],
});


client.login(process.env.DISCORD_TOKEN);

console.log("login success")

client.on('messageCreate', (message) => {
    
    // Ignore message when busy
    if (global.Busy) return;
    
    // Ignore messages from the bot itself
    if (message.author.bot) return;
    
    
    global.Busy = true;
    global.Channel = message.channel;
    //RunOptimisation()
});

client.on(Events.InteractionCreate, async interaction => {
    console.log(interaction)
    if (!interaction.isChatInputCommand()) return;

    if (interaction.commandName === 'ping') {
        const modal = new ModalBuilder()
            .setCustomId('myModal')
            .setTitle('My Modal');

        // Add components to modal

        // Create the text input components
        const favoriteColorInput = new TextInputBuilder()
            .setCustomId('favoriteColorInput')
            // The label is the prompt the user sees for this input
            .setLabel("What's your favorite color?")
            // Short means only a single line of text
            .setStyle(TextInputStyle.Short);

        const hobbiesInput = new TextInputBuilder()
            .setCustomId('hobbiesInput')
            .setLabel("What's some of your favorite hobbies?")
            // Paragraph means multiple lines of text.
            .setStyle(TextInputStyle.Paragraph);

        // An action row only holds one text input,
        // so you need one action row per text input.
        const firstActionRow = new ActionRowBuilder().addComponents(favoriteColorInput);
        const secondActionRow = new ActionRowBuilder().addComponents(hobbiesInput);

        // Add inputs to the modal
        modal.addComponents(firstActionRow, secondActionRow);

        // Show the modal to the user
        await interaction.showModal(modal);
    }
});


function RunOptimisation(){
    exec("./dofopti.out inputfiles/ramtest.in", (err, stdout, stderr)=>{
        if (err){
            TreatEndOptimisation("", err)
        }

        TreatEndOptimisation(stdout, "")
    });
}

function TreatEndOptimisation(optimisation, err){
    global.Busy = false;
    if (err.length > 0){
        global.Channel.send("Something went wrong");
        return;
    }
    
    global.Channel.send(optimisation.substring(0, 1900));
}*/