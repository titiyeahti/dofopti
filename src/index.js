import dotenv from 'dotenv'
dotenv.config()
import { Client, GatewayIntentBits } from 'discord.js';

const client = new Client({
    intents: [
        GatewayIntentBits.Guilds,
        GatewayIntentBits.GuildMessages,
        GatewayIntentBits.MessageContent
    ],
});

/*
client.login(process.env.DISCORD_TOKEN);

console.log("login success")

client.on('messageCreate', (message) => {
    // Ignore messages from the bot itself
    if (message.author.bot) return;

    global.Busy = true;
    
});*/


import { exec } from 'child_process'

exec("./dofopti.out inputfiles/ramtest.in", (err, stdout, stderr)=>{
    if (err){
        console.log(err);
        return;
    }

    console.log(stdout);
});