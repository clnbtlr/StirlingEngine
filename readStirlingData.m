% Read serial data from Arduino Micro
% https://uk.mathworks.com/help/instrument/read-streaming-data-from-arduino.html
clear; close all

arduinoObj = serialport("COM3",19200);
configureTerminator(arduinoObj,"CR/LF");
flush(arduinoObj);
arduinoObj.UserData = struct("P",[],"V",[],"Count",1);
configureCallback(arduinoObj,"terminator",@readData);

function readData(src,~)

    data = readline(src);
    data = strsplit(data,',');
    
    src.UserData.P(end+1) = str2double(data(1));
    src.UserData.V(end+1) = str2double(data(2));
    
    src.UserData.Count = src.UserData.Count + 1;
    
    if src.UserData.Count > 1000
        configureCallback(src,"off");
        plot(src.UserData.V(2:end),src.UserData.P(2:end),'o');
    end
end

