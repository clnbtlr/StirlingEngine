%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Matlab script to read in pressure and volume data from Arduino and 
% calculate ideal and actual Stirling cycle P-V and T-s curves, work, heat
% and efficiencies.
% 
% 28/04/2024.
%
% MIT License
% Copyright (c) 2024 clnbtlr
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear;
close all

%% Read serial data from Arduino Micro
% The data can be either read directly from the arduino or imported from
% mat files. 
% Uncomment the corresponding lines to do either.

% https://uk.mathworks.com/help/instrument/read-streaming-data-from-arduino.html

% arduinoObj = serialport("COM3",19200);
% configureTerminator(arduinoObj,"CR/LF");
% flush(arduinoObj);
% arduinoObj.UserData = struct("P",[],"V",[],"Count",1);
% configureCallback(arduinoObj,"terminator",@readData);
% % save arduinoData1.mat Pmeas Vmeas

load arduinoData.mat % data from MPXV7002
figure; hold on; box on;
title('Arduino data')
t = (1:length(Pmeas))*12e-3; % Arduino "loop" takes ~ 12 millisecond
yyaxis left
plot(t,Pmeas,'-')
ylabel('Gauge Pressure [kPa]')
yyaxis right
plot(t,Vmeas,'-')
ylabel('\DeltaV [cm^3]')
xlabel('time [s]')
legend('Pressure','Volume')
% load arduinoData1.mat % data from PSE533
Pmeas = Pmeas*1000 + 101000; % pressure values from arduino [Pa]

%% Temperature conditions and engine geometry

% Temperature conditions
Th = 90 + 273.15; % heat source temperature at hot water temperature [K]
Tc = 18 + 273.15; % heat sink temperature at ambient [K]
R = 287; % Gas constant for air [J/kg.K]
cp = 1004; % specific heat capacity at constant pressure [J/kg.K]
cv = cp - R; % specific heat capacity at constant volume [J/kg.K]
Patm = 101325; % atmospheric pressure [Pa]

% Engine geometry
D = 90e-3; % diameter of large cylinder [m]
H = 20e-3; % height of large cylinder [m]
D_disp = D - 2*5e-3; % estm. diameter of displacer [m]
H_disp = 8e-3; % estm. height of displacer [m]
porosity = 0.9; % porosity of polyurethane foam
d = 16e-3; % diameter of piston [m]; included in "A" in Arduino sketch
h = 2*4e-3; % total stroke length of piston [m]; corresponds to "r" in Arduino sketch
v1 = pi/4*D^2*H - (1-porosity)*pi/4*D_disp^2*H_disp; % [m3]
v2 = v1 + pi/4*d^2*h; % [m3]

% Mass of air
% With connection to pressure transducer open, and piston in middle
% position, pressure and temperature will be at atmospheric. Can then use
% the reference specfic volume at that point to caluclate mass of air
% inside when resealed at these conditions.
m = 101000*(v1)/R/(20+273.15);
V1 = v1/m; % specific volume at V1 [m3/kg]
V2 = v2/m; % specific volume at V2 [m3/kg]
Vmeas = ((Vmeas*1e-6) + v1 + 0.5*pi/4*d^2*h)/m; % volume values from arduino [m3/kg]

figure; hold on; box on;
title('P-V diagram of Arduino data');
plot(Vmeas,Pmeas/1000,'og')
xlabel('Volume [m^3/kg]')
ylabel('Pressure [kPa]')
axis('padded');

%% calculate ideal Stirling cycle
N = 100; % number of points 

% Process 1-2: Isothermal Heat Addition
n = 1; % polytropic index
P1 = R*Th/V1;
P2 = R*Th/V2;
V = linspace(V1,V2,N);
Ph = (P1*V1^n)./V.^n;
S12 = 4.0186e+03; %refpropm('S','T',Th,'P',P1/1000,'air.ppf'); % [J/kg.K]
for i = 2:length(Ph)
    S12(i) = R*log(Ph(i-1)/Ph(i));
end
S12 = cumsum(S12);

% Process 2-3: Constant Volume Heat Removal
V3 = V2;
P3 = R*Tc/V3;
T = linspace(Th,Tc,N);
S23 = S12(end);
for i = 2:length(T)
    S23(i) = real(cv*log(T(i)/T(i-1)));
end
S23 = cumsum(S23);

% Process 3-4: Isothermal Heat Removal
V4 = V1;
P4 = R*Tc/V4;
Pc = (P4*V4^n)./V.^n;
S34 = S23(end);
for i = length(Pc):-1:2
    S34(i) = R*log(Pc(i)/Pc(i-1));
end
S34 = cumsum(S34);

% Process 4-1: Constant Volume Heat Addition
S41 = S12(1);
for i = 2:length(T)
    S41(i) = real(cv*log(T(i)/T(i-1)));
end
S41 = cumsum(S41);

figure; hold on; box on;
title('P-V diagram');
plot(V,Ph/1000,'r')
plot([V2,V3],[P2,P3]/1000,'-ok','MarkerFaceColor','k')
plot(V,Pc/1000,'-b')
plot([V4,V1],[P4,P1]/1000,'-ok','MarkerFaceColor','k')
% arrows
plot(V(N/2),Ph(N/2)/1000,'>r','MarkerFaceColor','r')
plot(V2,(P2+P3)/2/1000,'vk','MarkerFaceColor','k')
plot(V(N/2),Pc(N/2)/1000,'<b','MarkerFaceColor','b')
plot(V1,(P1+P4)/2/1000,'^k','MarkerFaceColor','k')
% isotherms
Vref = linspace(V1*0.99,V2*1.01,N);
Phref = (R*Th/(Vref(1)))*((Vref(1))^n)./Vref.^n;
Pcref = (R*Tc/(Vref(1)))*((Vref(1))^n)./Vref.^n;
plot(Vref,Phref/1000,'r--')
plot(Vref,Pcref/1000,'b--')
% text labels
text(V1,P1/1000,'1  ','HorizontalAlignment','right')
text(V2,P2/1000,'  2','HorizontalAlignment','left')
text(V3,P3/1000,'  3','HorizontalAlignment','left')
text(V4,P4/1000,'4  ','HorizontalAlignment','right')
text(V(N/2),Ph(N/2)/1000,{'T_h',' ',' '},'HorizontalAlignment','center','Color','r')
text(V(N/2),Pc(N/2)/1000,{' ',' ','T_c'},'HorizontalAlignment','center','Color','b')
% Arduino data
plot(Vmeas,Pmeas/1000,'-g')
xlabel('Volume [m^3/kg]')
ylabel('Pressure [kPa]')
axis('padded');

figure; hold on; box on;
title('T-s diagram');
plot([S12(1)/1000,S12(end)/1000],[Th,Th]-273.15,'r-o','MarkerFaceColor','k','MarkerEdgeColor','k')
plot(S23/1000,T-273.15,'k-')
plot([S34(1)/1000,S34(end)/1000],[Tc,Tc]-273.15,'b-o','MarkerFaceColor','k','MarkerEdgeColor','k')
plot(S41/1000,T-273.15,'k-')
% arrows
plot(S12(N/2)/1000,Th-273.15,'>r','MarkerFaceColor','r')
plot(S23(N/2)/1000,(Th+Tc)/2-273.15,'vk','MarkerFaceColor','k')
plot(S34(N/2)/1000,Tc-273.15,'<b','MarkerFaceColor','b')
plot(S41(N/2)/1000,(Th+Tc)/2-273.15,'^k','MarkerFaceColor','k')
% text labels
text(S12(1)/1000,Th-273.15,'1  ','HorizontalAlignment','right')
text(S23(1)/1000,Th-273.15,'  2','HorizontalAlignment','left')
text(S34(1)/1000,Tc-273.15,'  3','HorizontalAlignment','left')
text(S41(end)/1000,Tc-273.15,'4  ','HorizontalAlignment','right')
text(S12(N/2)/1000,Th-273.15,{'T_h',' ',' '},'HorizontalAlignment','center','Color','r')
text(S34(N/2)/1000,Tc-273.15,{' ',' ','T_c'},'HorizontalAlignment','center','Color','b')
% Arduino data
for i = 1:length(Vmeas)
    Tmeas(i) = Pmeas(i)*Vmeas(i)/R; % ideal gas law
    % For s, uses relation for polytopic process between Th and Tmeas(i). 
    Smeas(i) = R*log(Vmeas(i)/V1)-cv*log(Th/Tmeas(i)) + S12(1);
end
plot(Smeas/1000,Tmeas-273.15,'-g')
xlabel('Entropy [kJ/kg.K]')
ylabel('Temperature [^\circC]')
axis('padded');

%% calculate Work in 1 cycle
% find minimum boundary of arduino data
k = boundary(Vmeas',Pmeas');
Work = polyarea(Pmeas(k),Vmeas(k)); % [J/kg] area inside boundary
Work = Work*m % [J]

% find cycle period
[~,idx]=findpeaks(Pmeas); 
for i = 1:length(idx)-1
    tp(i)=t(idx(i+1))-t(idx(i));
end
Freq = 1/mean(tp); % cycle frequency [Hz]
Power = Work*Freq % [W]

%% calculate Heat input in 1 cycle
% find minimum boundary of arduino data
k = boundary(Smeas',Tmeas');
Qnet = polyarea(Smeas(k),Tmeas(k)); % [J/kg] area inside boundary
Qnet = Qnet*m; % [J]

% To calculate Qh, form a convex hull around data points and two points at
% Smeas[min,max] and T = 0 K. 
Smeas2 = [Smeas';min(Smeas);max(Smeas)];
Tmeas2 = [Tmeas';0;0];
k2 = boundary(Smeas2,Tmeas2,0); % convex hull.
Qh = polyarea(Smeas2(k2),Tmeas2(k2)); % [J/kg] area inside boundary
Qh = Qh*m; % [J]

figure; hold on; box on;
title('T-s diagram of Arduino data');
plot(Smeas/1000,Tmeas-273.15,'og')
xlabel('Entropy [kJ/kg.K]')
ylabel('Temperature [^\circC]')
axis('padded');

%% Calculate efficiencies
% Carnot efficiency
eta_c = (1-(Tc/Th))*100 % [%]
% Actual efficiency
eta_a = Work/Qh*100 % [%]

%%
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