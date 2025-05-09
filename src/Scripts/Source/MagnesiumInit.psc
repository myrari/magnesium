; Copyright (c) 2024 Myra Roberts
; This file is part of Magnesium.

Scriptname MagnesiumInit extends Quest
{Initialization script for Magnesium, runs as soon as quests load}

; interveral to query for commands in queue
float interval = 1.0

Event OnInit()
    Debug.Notification("Magnesium initialized")
    RegisterForSingleUpdate(interval)
EndEvent

; called every interval, execute command queue and register for next update
Event OnUpdate()
    ProcessMessageQueue()
    RegisterForSingleUpdate(interval)
EndEvent

; get the first command in the queue, keep executing until queue is empty
Function ProcessMessageQueue()
    string messageFromQueue = Magnesium.PopMessage()
    while messageFromQueue
        ProcessMessage(messageFromQueue)
        messageFromQueue = Magnesium.PopMessage()
    endwhile
EndFunction

; process a given command
Function ProcessMessage(string messageString) 
    ; MESSAGE SERIALIZATION SCHEMA:
    ; ${messageType};{data}
    ; ; is delimiter
    string delimiter = ";"
    string[] components = StringUtil.Split(messageString, delimiter)
    string messageType = components[0]
    string messageData = components[1]

    ; determine what to do based on messageType
    if messageType == "DisplayMessage"
        Debug.Notification(messageData)
    elseif messageType == "ConsoleCommand"
        ConsoleUtil.ExecuteCommand(messageData)
    elseif messageType == "SetInterval"
        float newInterval = messageData as float
        interval = newInterval
        Debug.Notification("Set interval to " + messageData)
    else
        Debug.Notification("Received message of unkown type: " + messageType)
    endif
EndFunction
