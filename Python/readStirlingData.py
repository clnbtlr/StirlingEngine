##########################################################################
# Matlab script to read in pressure and volume data from Arduino and 
# calculate ideal and actual Stirling cycle P-V and T-s curves, work, heat
# and efficiencies.
# 
# 28/04/2024.
#
# MIT License
# Copyright (c) 2024 clnbtlr
##########################################################################

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

## Read in Arduino data (using MPXV7002 pressure sensor)
df = pd.read_csv('data.csv')

fig, ax1 = plt.subplots()
#plt.rcParams["font.family"] = 'Times New Roman'
#plt.rcParams.update({'font.size': 8})
plt.title('Arduino data')
color = 'tab:red'
ax1.plot(df.t,df.Pmeas,'-',color=color)
ax1.set_ylabel('Pressure, $P$ [kPa]', color=color)
ax1.tick_params(axis='y', labelcolor=color)
ax2 = ax1.twinx()
color = 'tab:blue'
ax2.plot(df.t,df.Vmeas,'-',color=color)
ax2.set_ylabel('Volume change, $ΔV$ [cm3]', color=color)
ax2.tick_params(axis='y', labelcolor=color)
ax1.set_xlabel('time [s]')
fig.tight_layout()

## Temperature conditions and engine geometry
# Temperature conditions
Th = 90 + 273.15 # heat source temperature at hot water temperature [K]
Tc = 18 + 273.15 # heat sink temperature at ambient [K]
R = 287 # Gas constant for air [J/kg.K]
cp = 1004 # specific heat capacity at constant pressure [J/kg.K]
cv = cp - R # specific heat capacity at constant volume [J/kg.K]
Patm = 101325 # atmospheric pressure [Pa]

# Engine geometry
D = 90e-3 # diameter of large cylinder [m]
H = 20e-3 # height of large cylinder [m]
D_disp = D - 2.5e-3 # estm. diameter of displacer [m]
H_disp = 8e-3 # estm. height of displacer [m]
porosity = 0.9 # porosity of polyurethane foam
d = 16e-3 # diameter of piston [m]; included in "A" in Arduino sketch
h = 2*4e-3 # total stroke length of piston [m]; corresponds to "r" in Arduino sketch
v1 = np.pi/4*D**2*H - (1-porosity)*np.pi/4*D_disp**2*H_disp # minium internal volume [m3]
v2 = v1 + np.pi/4*d**2*h # maximum internal volume [m3]

## Mass of air
# With connection to pressure transducer open, and piston in middle
# position, pressure and temperature will be at atmospheric. Can then use
# the reference specfic volume at that point to caluclate mass of air
# inside when resealed at these conditions.
m = Patm*(v1)/R/Tc
print(m)
V1 = v1/m # specific volume at V1 [m3/kg]
V2 = v2/m # specific volume at V2 [m3/kg]
Vmeas = ((df.Vmeas*1e-6) + v1 + 0.5*np.pi/4*d**2*h)/m # volume values from Arduino [m3/kg]
Pmeas = df.Pmeas*1000 + Patm # pressure values from Arduino to abs. pressure [Pa]

plt.figure()
plt.title('P-V diagram of Arduino data')
plt.plot(Vmeas,Pmeas/1000,'.')
plt.xlabel('Volume,$ν$ [m3/kg]')
plt.ylabel('Pressure, $P$ [kPa]')

## Calculate ideal Stirling Cycle
N = 100 # number of points

# Process 1-2: Isothermal Heat Addition
n = 1 # polytropic index
P1 = R*Th/V1
P2 = R*Th/V2
V = np.linspace(V1,V2,N)
Ph = (P1*V1**n)/V**n # Pressure values for Th isotherm [Pa]
S12 = np.zeros(N) # specific entropy from 1-2
S12[0] = 4.0186e3 # reference specific entropy value at Th and P1 [J/kg.K]
for i in range(1,len(Ph)):
    S12[i] = R*np.log(Ph[i-1]/Ph[i])
S12 = np.cumsum(S12) # cumulative sum of entropy

# Process 2-3: Constant Volume Heat Removal
V3 = V2
P3 = R*Tc/V3
T = np.linspace(Th,Tc,N)
S23 = np.zeros(N) # specific entropy from 2-3
S23[0] = S12[-1]
for i in range(1,len(T)):
    S23[i] = np.real(cv*np.log(T[i]/T[i-1]))
S23 = np.cumsum(S23)

# Process 3-4: Isothermal Heat Removal
V4 = V1
P4 = R*Tc/V4
Pc = (P4*V4**n)/V**n # Pressure values for Tc isotherm [Pa]
S34 = np.zeros(N) # specific entropy from 3-4
S34[0] = S23[-1]
for i in range(len(Pc)-1,0,-1):
    S34[i] = R*np.log(Pc[i]/Pc[i-1])
S34 = np.cumsum(S34)

# Process 4-1: Constant Volume Heat Addition
S41 = np.zeros(N) # specific entropy from 4-1
S41[0] = S12[0]
for i in range(1,len(T)):
    S41[i] = np.real(cv*np.log(T[i]/T[i-1]))
S41 = np.cumsum(S41)

## P-V and T-s plots
plt.figure()
plt.title('P-V diagram')
plt.plot(V,Ph/1000,'r')
plt.plot([V2,V3],np.array([P2,P3])/1000,'-ok',markerfacecolor='k')
plt.plot(V,Pc/1000,'-b')
plt.plot([V4,V1],np.array([P4,P1])/1000,'-ok',markerfacecolor='k')
# arrows
plt.annotate('',xy=(V[int(N/2)],Ph[int(N/2)]/1000),xytext=(V[int(N/2-1)],Ph[int(N/2-1)]/1000),arrowprops=dict(arrowstyle='simple',color='r'))
plt.annotate('',xy=(V2,(P2+P3)/2/1000),xytext=(V2,(P2+P3)/2/1000*1.01),arrowprops=dict(arrowstyle='simple',color='k'))
plt.annotate('',xy=(V[int(N/2)],Pc[int(N/2)]/1000),xytext=(V[int(N/2+1)],Pc[int(N/2+1)]/1000),arrowprops=dict(arrowstyle='simple',color='b'))
plt.annotate('',xy=(V1,(P1+P4)/2/1000),xytext=(V1,(P1+P4)/2/1000*0.99),arrowprops=dict(arrowstyle='simple',color='k'))
# isotherms
Vref = np.linspace(V1*0.99,V2*1.01,N)
Phref = (R*Th/(Vref[0]))*((Vref[0])**n)/Vref**n
Pcref = (R*Tc/(Vref[0]))*((Vref[0])**n)/Vref**n
plt.plot(Vref,Phref/1000,'r--')
plt.plot(Vref,Pcref/1000,'b--')
# text labels
plt.annotate('1 ',[V1,P1/1000],ha='right',va='bottom')
plt.annotate(' 2',[V2,P2/1000],ha='left',va='bottom')
plt.annotate(' 3',[V3,P3/1000],ha='left',va='top')
plt.annotate('4 ',[V4,P4/1000],ha='right',va='top')
plt.annotate('$T_h$',[V[int(N/2)],Ph[int(N/2)]/1000],ha='center',va='bottom',color='r')
plt.annotate('$T_c$',[V[int(N/2)],Pc[int(N/2)]/1000],ha='center',va='bottom',color='b')
plt.xlabel('Volume,$ν$ [m3/kg]')
plt.ylabel('Pressure, $P$ [kPa]')
# Arduino data
plt.plot(Vmeas,Pmeas/1000,'-g')

plt.figure()
plt.title('T-s diagram')
plt.plot(np.array([S12[0],S12[-1]])/1000,np.array([Th,Th])-273.15,'r-o',markerfacecolor='k',markeredgecolor='k')#,zorder=100)
plt.plot(S23/1000,T-273.15,'k-')
plt.plot(np.array([S34[0],S34[-1]])/1000,np.array([Tc,Tc])-273.15,'b-o',markerfacecolor='k',markeredgecolor='k')#zorder=0)
plt.plot(S41/1000,T-273.15,'k-')
# arrows
plt.annotate('',xy=(S12[int(N/2)]/1000,Th-273.15),xytext=(S12[int(N/2-1)]/1000,Th-273.15),arrowprops=dict(arrowstyle='simple',color='r'))
plt.annotate('',xy=(S23[int(N/2)]/1000,T[int(N/2)]-273.15),xytext=(S23[int(N/2-1)]/1000,T[int(N/2-1)]-273.15),arrowprops=dict(arrowstyle='simple',color='k'))
plt.annotate('',xy=(S34[int(N/2)]/1000,Tc-273.15),xytext=(S34[int(N/2-1)]/1000,Tc-273.15),arrowprops=dict(arrowstyle='simple',color='b'))
plt.annotate('',xy=(S41[int(N/2)]/1000,T[int(N/2)]-273.15),xytext=(S41[int(N/2+1)]/1000,T[int(N/2+1)]-273.15),arrowprops=dict(arrowstyle='simple',color='k'))
# text labels
plt.annotate('1 ',[S12[0]/1000,Th-273.15],ha='right',va='bottom')
plt.annotate(' 2',[S23[0]/1000,Th-273.15],ha='left',va='bottom')
plt.annotate(' 3',[S34[0]/1000,Tc-273.15],ha='left',va='top')
plt.annotate('4 ',[S41[-1]/1000,Tc-273.15],ha='right',va='top')
plt.annotate('$T_h$',[S12[int(N/2)]/1000,Th-273.15],ha='center',va='bottom',color='r')
plt.annotate('$T_c$',[S34[int(N/2)]/1000,Tc-273.15],ha='center',va='bottom',color='b')
plt.xlabel('Entropy, $s$ [kJ/kg.K]')
plt.ylabel('Temperature, $T$ [°C]')
# Arduino data
Tmeas = np.zeros_like(Vmeas) # initiate Tmeas
Smeas = np.zeros_like(Vmeas) # initiate Smeas
for i in range(0,len(Vmeas)):
    Tmeas[i] = Pmeas[i]*Vmeas[i]/R # Experimental gas temperature [K]
    # For s, uses relation for polytropic process between Th and Tmeas[i]
    Smeas[i] = R*np.log(Vmeas[i]/V1) - cv*np.log(Th/Tmeas[i]) + S12[0]
plt.plot(Smeas/1000,Tmeas-273.15,'-g')

plt.show()