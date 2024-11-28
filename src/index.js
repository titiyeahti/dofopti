import dotenv from 'dotenv'
dotenv.config()
import {TreatJson} from './dofusDbConverter.js'
import {MakeInputTemplate, MakeInputHeader, RunOptimisationAsync} from "./optimizer.js";

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

var editedTemplate = ""

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
    const HEADER_TEXT = MakeInputHeader()


    client.on('interactionCreate', async (interaction) => {
        if (interaction.isCommand() && interaction.commandName === 'tititemplate') {

            const modal = new ModalBuilder()
                .setCustomId('tititemplate_modal')
                .setTitle('Edit Template');

            const textInput = new TextInputBuilder()
                .setCustomId('template_text_input')
                .setLabel('Template')
                .setStyle(TextInputStyle.Paragraph)
                .setPlaceholder('Edit the template text here...')
                .setValue(TEMPLATE_TEXT);

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

            editedTemplate = interaction.fields.getTextInputValue('template_text_input');
            console.log('Edited template:', editedTemplate);

            try {
                await interaction.deferReply(); // Acknowledge interaction immediately
                await interaction.followUp({
                    content: `Input received :\n\`\`\`${editedTemplate}\`\`\``,
                    ephemeral: false,
                });
            } catch (error) {
                console.error('Error while handling modal submission:', error);
                return
            }
            await writeFileSync('./inputfiles/discord.in', editedTemplate);

            await RunOptimisationAsync();

            await TreatJson(async (link) => {
                    await interaction.followUp(link)
                },
                async (err) => {
                    console.log(err)
                });
        }

    });
// Login the bot
    client.login(process.env.DISCORD_TOKEN);
}

