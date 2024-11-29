import dotenv from 'dotenv'
dotenv.config()
import {TreatJson} from './dofusDbConverter.js'
import {MakeInputTemplate, MakeInputHeader, RunOptimisationAsync} from "./optimizer.js";
import {CachedRequest, CachedRequestArray} from "./cachedRequest.js";

function TreatOptimisationSucceeded(result){
    global.Success = true;
    global.Result = "test succeed";
}

function TreatOptimisationFailed(err) {
    global.Success = false;
    global.Error = "test error";
}

import { Client, GatewayIntentBits, Partials, SlashCommandBuilder, REST, Routes, ModalBuilder, TextInputBuilder, TextInputStyle, ActionRowBuilder} from 'discord.js';
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
    new SlashCommandBuilder()
        .setName('clear_template')
        .setDescription('Reset your text template to default')
];


if (process.env.DISCORD_TOKEN != null){
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


    client.on('interactionCreate', async (interaction) => {
        if (interaction.isCommand() && interaction.commandName === 'clear_template'){
            CachedRequests.SetRequest(interaction.user.id, TEMPLATE_TEXT)
            await interaction.deferReply();
            await interaction.followUp({
                content: `Cleared your cache !`,
                ephemeral: false,
            });
        }
        if (interaction.isCommand() && interaction.commandName === 'tititemplate') {

            var user = CachedRequests.GetRequest(interaction.user.id)
            var baseText = TEMPLATE_TEXT;
            if (user){
                baseText = user.request;
            }
            const modal = new ModalBuilder()
                .setCustomId('tititemplate_modal')
                .setTitle('Edit Template');

            const textInput = new TextInputBuilder()
                .setCustomId('template_text_input')
                .setLabel('Template')
                .setStyle(TextInputStyle.Paragraph)
                .setPlaceholder('Edit the template text here...')
                .setValue(baseText);

            const actionRow = new ActionRowBuilder().addComponents(textInput);
            modal.addComponents(actionRow);

            try {
                await interaction.showModal(modal);
            } catch (error) {
                console.error('Error showing modal:', error);
                return
            }
        }

        if (interaction.isModalSubmit() && interaction.customId === 'tititemplate_modal') {
            console.log('Modal submitted!');

            const editedTemplate = interaction.fields.getTextInputValue('template_text_input');
            console.log('Edited template:', editedTemplate);

            try {
                await interaction.deferReply(); // Acknowledge interaction immediately
                await interaction.followUp({
                    content: `Input received :\n\`\`\`${editedTemplate}\`\`\``,
                    ephemeral: false,
                });
                CachedRequests.SetRequest(interaction.user.id, editedTemplate)
            } catch (error) {
                console.error('Error while handling modal submission:', error);
                return
            }
            await writeFileSync('./inputfiles/discord.in', editedTemplate);

            await RunOptimisationAsync(async () => await OptiSuccessful(interaction), async(err) => await interaction.followUp(err));
        }
    });
    
    async function OptiSuccessful(interaction) {
        await TreatJson(async (link, result) => {
                const random = Math.floor(Math.random() * 6);
                var toPrint;

                if (random < 1) {
                    toPrint = "C’est du lourd, tu crames les mobs ! 🔥💥"
                } else if (random < 2) {
                    toPrint = "Des dégâts qui font pleurer les boss ! 😭👊"
                } else if (random < 2) {
                    toPrint = "T’as sorti l’artillerie lourde, wow ! 🛡️💣"
                } else if (random < 3) {
                    toPrint = "Tu vas rouler sur eux comme un bulldozer ! 🚜💥"
                } else if (random < 4) {
                    toPrint = "C’est plus du DPS, c’est un génocide ! ☠️🔥"
                } else {
                    toPrint = "Un DPS de légende, on devrait écrire une saga là-dessus ! 📖⚡"
                }

                toPrint = '**[' + result + '💥]** ' + toPrint;
                await interaction.followUp(toPrint)
                await interaction.followUp(link)
            },
            async (err) => {
                console.log(err)
            });
    }
    
    const CachedRequests = new CachedRequestArray();
    client.login(process.env.DISCORD_TOKEN);
}

