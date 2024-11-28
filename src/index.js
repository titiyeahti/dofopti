import dotenv from 'dotenv'
dotenv.config()
import {TreatJson} from './dofusDbConverter.js'
import {RunOptimisation, MakeInputTemplate, MakeInputHeader, RunOptimisationAsync} from "./optimizer.js";

//RunOptimisation(TreatOptimisationSucceeded, TreatOptimisationFailed)

function TreatOptimisationSucceeded(result){
    global.Success = true;
    global.Result = "test succeed";
}

function TreatOptimisationFailed(err) {
    global.Success = false;
    global.Error = "test error";
}

import { Client, GatewayIntentBits, Partials, SlashCommandBuilder, REST, Routes } from 'discord.js';
import {writeFileSync} from "fs";

const client = new Client({
    intents: [
        GatewayIntentBits.Guilds,
        GatewayIntentBits.GuildMessages,
        GatewayIntentBits.MessageContent
    ],
    partials: [Partials.Channel],
});

// ------------- Slash Command Registration -------------
const commands = [
    new SlashCommandBuilder()
        .setName('tititemplate')
        .setDescription('Get a text template to modify!'),
];

const rest = new REST({ version: '10' }).setToken(process.env.DISCORD_TOKEN);

(async () => {
    try {
        console.log('Refreshing application commands...');
        await rest.put(
            Routes.applicationCommands(process.env.CLIENT_ID),
            { body: commands },
        );
        console.log('Commands registered successfully!');
    } catch (error) {
        console.error('Error registering commands:', error);
    }
})();

// ------------- Bot Event Handlers -------------
client.once('ready', () => {
    console.log(`Logged in as ${client.user.tag}!`);
});

// Template text to send when the command is called
const TEMPLATE_TEXT = MakeInputTemplate()
const HEADER_TEXT = MakeInputHeader()


client.on('interactionCreate', async (interaction) => {
    if (!interaction.isCommand()) return;

    if (interaction.commandName === 'tititemplate') {
        await interaction.reply({
            content: `Here's your template:\n\`\`\`${TEMPLATE_TEXT}\`\`\`\nReply with the modified version to see the magic!`,
            ephemeral: false, // Visible to everyone in the channel.
        });
    }
});

client.on('messageCreate', async (message) => {
    if (message.author.bot) return; // Ignore bot messages.

    // If the user replies with the modified template:
    if (message.content && message.content.includes(HEADER_TEXT)) {
        var lines = message.content.split('\n');
        if (lines.length <= 1){
            return;
        }
        lines.splice(0,1);
        var newtext = lines.join('\n');
        
        /*
        try {
            const content = 'Some content!';
            await writeFile('./inputfiles/discord.in', content);
        } catch (err) {writeFileSync
            await message.reply('```' + err + '```');
            return;
        }*/
        
        await writeFileSync('./inputfiles/discord.in', newtext);
        
        var result = await RunOptimisationAsync();

        await message.reply(result)
        
    }
});

// Login the bot
client.login(process.env.DISCORD_TOKEN);