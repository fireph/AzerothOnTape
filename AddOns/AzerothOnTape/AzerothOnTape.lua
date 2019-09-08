
AzerothOnTape = {}
AzerothOnTape.lines = {}
AzerothOnTape.str = nil

BINDING_HEADER_AzerothOnTape = "Azeroth On Tape"
BINDING_HEADER_AzerothOnTape = "Azeroth On Tape"
BINDING_NAME_readquesttext = "Read Quest Text"
BINDING_NAME_readquestobjectives = "Read Quest Objectives"

SLASH_AOT_INFO1 = "/azerothontape"
SLASH_AOT_INFO2 = "/aot"

SlashCmdList["AOT_INFO"] = function(msg) AzerothOnTape:ShowWowQuestTextReaderInfo() end

-- Initializing...
--print("Initializing AOT 1.0");

local aotGossipDialogOpen = false;

-- Dummy frame to get events
local aotEventFrame = CreateFrame("FRAME", "AotEventFrame")
aotEventFrame:RegisterEvent("GOSSIP_SHOW")
aotEventFrame:RegisterEvent("GOSSIP_CLOSED")

-- Event handler
local function aotEventHandler(self, event, ...)
    if (event == "GOSSIP_CLOSED") then
        aotGossipDialogOpen = false
        --print("closed")
    elseif (event == "GOSSIP_SHOW") then
        aotGossipDialogOpen = true
        --print("shown")
    end
end

aotEventFrame:SetScript("OnEvent", aotEventHandler);

--print("...Initializing AOT 1.0 DONE");

function AzerothOnTape:ShowWowQuestTextReaderInfo()
    DEFAULT_CHAT_FRAME:AddMessage("Azeroth On Tape Addon 1.0 Installed!")
    DEFAULT_CHAT_FRAME:AddMessage("Make sure the Azeroth On Tape Application is running.")
    DEFAULT_CHAT_FRAME:AddMessage("Important! Open the WoW Keybindings menu, find the section for Azeroth On Tape.")
    DEFAULT_CHAT_FRAME:AddMessage("Bind Read Quest Objectives to Shift-F11.")
    DEFAULT_CHAT_FRAME:AddMessage("Bind Read Quest Text to Shift-F12.")
    DEFAULT_CHAT_FRAME:AddMessage("Usage: Select a quest from your quest log, press one of the application hotkeys (set in the external application, NOT the WoW hotkeys Shift-F11 and Shift F12). Application must be running for text-to-speech to work.")
end

function AzerothOnTape:GetSelectedQuestInfo()

    local questSelected = GetQuestLogSelection()
    local questText, questObjectives = GetQuestLogQuestText()
    local questTitle, level, questTag, suggestedGroup, isHeader, isCollapsed, isComplete, questID = GetQuestLogTitle(questSelected)
    
    -- This requires the NPC dialog to still be open
    local npcQuestRewardText = GetRewardText()

    -- This requires the NPC dialog to still be open
    local npcQuestTitle = GetTitleText()
    local npcQuestText = GetQuestText()
    local npcQuestObjectives = GetObjectiveText()
    local npcProgressText = GetProgressText()

    -- This requires the NPC dialog to still be open
    local npcGossipText = ""

    -- Because you can grab this text even when not talking to the NPC we only set it when such a dialog
    -- is open. We rather have it read the regular quest texts when not engaging with a NPC.
    if (aotGossipDialogOpen) then
        npcGossipText = GetGossipText()
    end

    if (questTitle == nil) then
        questTitle = ""
    end

    if (questText == nil) then
        questText = ""
    end

    if (questObjectives == nil) then
        questObjectives = ""
    end

    if (npcGossipText == nil) then
        npcGossipText = ""
    end

    if (npcQuestText == nil) then
        npcQuestText = ""
    end

    if (npcQuestObjectives == nil) then
        npcQuestObjectives = ""
    end

    if (npcProgressText == nil) then
        npcProgressText = ""
    end

    if (npcQuestRewardText == nil) then
        npcQuestRewardText = ""
    end

    -- Did we have a quest?
    --if (questText ~= nil) then

    local questXml = '{"questTitle":"' .. questTitle:gsub('"','\\"') .. '","questDescription":"' .. questText:gsub('"','\\"') .. '","questObjectives":"' .. questObjectives:gsub('"','\\"') .. '","questId":"' .. questID .. '","npcQuestTitle":"' .. npcQuestTitle:gsub('"','\\"') .. '","npcQuestRewardText":"' .. npcQuestRewardText:gsub('"','\\"') .. '","npcQuestText":"' .. npcQuestText:gsub('"','\\"') .. '","npcQuestObjectives":"' .. npcQuestObjectives:gsub('"','\\"') .. '","npcGossipText":"' .. npcGossipText:gsub('"','\\"') .. '","npcProgressText":"' .. npcProgressText:gsub('"','\\"') .. '"}'

    AzerothOnTapeFrame:Show()
    --AzerothOnTapeFrame:EnableKeyboard(false)
    AzerothOnTapeText:SetText("")
    AzerothOnTapeFrameScrollText:Disable();
    AzerothOnTapeFrameScrollText:IsKeyboardEnabled(false)

    if (AzerothOnTapeFrameScrollText:GetText()  ~= questXml) then
        AzerothOnTapeFrameScrollText:SetText(questXml);

        AzerothOnTapeFrameScrollText:HighlightText()
    end

    AzerothOnTapeFrameScrollText:Enable();
    --end
end

function AzerothOnTape:OnTextChanged(this)
end

function AzerothOnTape:Hide()
    AzerothOnTapeFrame:Hide()
end
