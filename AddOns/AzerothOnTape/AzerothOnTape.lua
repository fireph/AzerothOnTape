
AzerothOnTape = {}
AzerothOnTape.lines = {}
AzerothOnTape.str = nil

BINDING_HEADER_AzerothOnTape = "Azeroth On Tape"
BINDING_HEADER_AzerothOnTape = "Azeroth On Tape"
BINDING_NAME_readquesttext = "Read Quest Text"
BINDING_NAME_readquestobjectives = "Read Quest Objectives"

SLASH_WQTR_INFO1 = "/wowquesttextreader"
SLASH_WQTR_INFO2 = "/wqtr"
SLASH_WQTR_INFO3 = "/questtextreader"

SlashCmdList["WQTR_INFO"] = function(msg) AzerothOnTape:ShowWowQuestTextReaderInfo() end

-- Initializing...
--print("Initializing WQTR 2.0");

local wqtrGossipDialogOpen = false;

-- Dummy frame to get events
local wqtrEventFrame = CreateFrame("FRAME", "WqtrEventFrame")
wqtrEventFrame:RegisterEvent("GOSSIP_SHOW")
wqtrEventFrame:RegisterEvent("GOSSIP_CLOSED")

-- Event handler
local function wqtrEventHandler(self, event, ...)
    if (event == "GOSSIP_CLOSED") then
        wqtrGossipDialogOpen = false
        --print("closed")
    elseif (event == "GOSSIP_SHOW") then
        wqtrGossipDialogOpen = true
        --print("shown")
    end
end

wqtrEventFrame:SetScript("OnEvent", wqtrEventHandler);

--print("...Initializing WQTR 2.0 DONE");

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
    if (wqtrGossipDialogOpen) then
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

    local questXml = '{"questTitle":"' .. questTitle .. '","questDescription":"' .. questText .. '","questObjectives":"' .. questObjectives .. '","questId":"' .. questID .. '","npcQuestTitle":"' .. npcQuestTitle .. '","npcQuestRewardText":"' .. npcQuestRewardText .. '","npcQuestText":"' .. npcQuestText .. '","npcQuestObjectives":"' .. npcQuestObjectives .. '","npcGossipText":"' .. npcGossipText .. '","npcProgressText":"' .. npcProgressText .. '"}'

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
