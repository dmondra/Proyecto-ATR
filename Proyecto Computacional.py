import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm

def Dimension(Lx, Ly, dx, dy, fo, ff): #dx y dy son la cantidad de pasos en cada eje
  malla=np.zeros((Lx+1,Ly+1), dtype=float)
  #salto x & y 
  hx= Lx/dx
  hy= Ly/dy
  malla[0,0]=fo
  malla[Lx,Ly]= ff

  return malla, hx, hy
 

def tiempo (dt, vx,vy, hx, hy):
    #Condición CFL
  C=(vx*dt/hx)+(vy*dt/hy)
  if C<=1:
    print("Se cumple la condición CFL")
  else: #refinar salto del tiempo
    dt=dt*1/C 

  return dt

#main

