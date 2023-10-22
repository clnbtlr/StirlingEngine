clear;
% close all

%% Read serial data from Arduino Micro
% https://uk.mathworks.com/help/instrument/read-streaming-data-from-arduino.html

% arduinoObj = serialport("COM3",19200);
% configureTerminator(arduinoObj,"CR/LF");
% flush(arduinoObj);
% arduinoObj.UserData = struct("P",[],"V",[],"Count",1);
% configureCallback(arduinoObj,"terminator",@readData);

load arduinoData.mat
figure; hold on;
plot(Pmeas,'-m')
plot(Vmeas,'-b')
Pmeas = Pmeas*1000 + 101325; % pressure values from arduino [Pa]

%% calculate ideal Stirling cycle

% Temperature conditions
Th = 90 + 273.15; % heat source temperature [K]
Tc = 20 + 273.15; % heat sink temperature [K]
R = 287; % Gas constant for air [J/kg.K]

% Engine geometry
D = 90e-3; % diameter of large cylinder [m]
H = 20e-3; % height of large cylinder [m]
D_disp = D - 2*5e-3; % estm. diameter of displacer
H_disp = 8e-3; % estm. height of displacer
porosity = 0.9; % porosity of polyurethane foam
d = 16e-3; % diameter of piston; included in "A" in Arduino sketch
h = 2*4e-3; % total stroke length of piston; corresponds to "r" in Arduino sketch
v1 = pi/4*D^2*H - (1-porosity)*pi/4*D_disp^2*H_disp; % [m3]
v2 = v1 + pi/4*d^2*h; % [m3]

% Mass of air
% With connection to pressure transducer open, and piston in middle
% position, pressure and temperature will be at atmospheric. Can then use
% the reference specfic volume at that point to caluclate mass of air
% inside when resealed at these conditions.
m = 101325*(v1)/R/(20+273.15);
V1 = v1/m; % specific volume at V1 [m3/kg]
V2 = v2/m; % specific volume at V2 [m3/kg]
Vmeas = ((Vmeas*1e-6) + v1 + 0.5*pi/4*d^2*h)/m; % pressure values from arduino [m3/kg]

N = 100; % number of points 
% Process 1-2: Isothermal Heat Addition
n = 1; % polytropic index
P1 = R*Th/V1;
P2 = R*Th/V2;
V = linspace(V1,V2,N);
Ph = (P1*V1^n)./V.^n;

% Process 2-3: Constant Volume Heat Removal
V3 = V2;
P3 = R*Tc/V3;

% Process 3-4: Isothermal Heat Removal
V4 = V1;
P4 = R*Tc/V4;
Pc = (P4*V4^n)./V.^n;

% Process 4-1: Constant Volume Heat Addition
% nothing to calculate as know all values already.

figure; hold on;
plot(Vmeas,Pmeas,'-g')

figure; hold on; box on;
plot(V,Ph,'r')
plot([V2,V3],[P2,P3],'-ok','MarkerFaceColor','k')
plot(V,Pc,'-b')
plot([V4,V1],[P4,P1],'-ok','MarkerFaceColor','k')
% arrows
plot(V(N/2),Ph(N/2),'>r','MarkerFaceColor','r')
plot(V2,(P2+P3)/2,'vk','MarkerFaceColor','k')
plot(V(N/2),Pc(N/2),'<b','MarkerFaceColor','b')
plot(V1,(P1+P4)/2,'^k','MarkerFaceColor','k')
% isotherms
Vref = linspace(V1-0.01,V2+0.01,N);
Phref = (R*Th/(V1-0.01))*((V1-0.01)^n)./Vref.^n;
Pcref = (R*Tc/(V1-0.01))*((V1-0.01)^n)./Vref.^n;
plot(Vref,Phref,'r--')
plot(Vref,Pcref,'b--')
% text labels
text(V1,P1,'1  ','HorizontalAlignment','right')
text(V2,P2,'  2','HorizontalAlignment','left')
text(V3,P3,'  3','HorizontalAlignment','left')
text(V4,P4,'4  ','HorizontalAlignment','right')
text(V(N/2),Ph(N/2),{'T_h',' ',' '},'HorizontalAlignment','center','Color','r')
text(V(N/2),Pc(N/2),{' ',' ','T_c'},'HorizontalAlignment','center','Color','b')
% Arduino data
plot(Vmeas,Pmeas,'-g')
xlabel('Volume [m^3/kg]')
ylabel('Pressure [Pa]')
axis('padded');

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