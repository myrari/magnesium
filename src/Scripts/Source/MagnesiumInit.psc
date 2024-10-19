; Copyright (c) 2024 Myra Roberts
; This file is part of Magnesium.
;
; Magnesium is free software: you can redistribute it and/or modify it under
; the terms of the GNU General Public License as published by the Free Software
; Foundation, either version 2 of the License, or (at your option) any later
; version.
;
; Magnesium is distributed in the hope that it will be useful, but WITHOUT ANY
; WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
; A PARTICULAR PURPOSE. See the GNU General Public License for more details.

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
    string messageFromQueue = CustomPapyrusFunctions.GetMessage()
    while messageFromQueue
        ProcessMessage(messageFromQueue)
        messageFromQueue = CustomPapyrusFunctions.GetMessage()
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
    endif
EndFunction
