# include <iostream>
# include <vector>

std::vector<double> room(int Lx, int Ly, double dx, double dy, double fo, double ff, double dt, double vx, double vy){

std::vector <double> Malla ((Lx+1)*(Ly+1),0);

double hx=double(Lx)/dx;
double hy=double(Ly)/dy;

for(int i=0; i<Lx+1; i++){
    for (int j=0; j<Ly+1; j++){
        Malla[i*(Ly+1)+j]=0;
    }
}
malla[0] = fo;
malla[Lx*(Ly+1)+Ly] = ff;



double C = (vx * dt / hx) + (vy * dt / hy);

    if (C <= 1)
    {
        cout << "Se cumple la condicion CFL" << endl;
    }
    else
    {
        dt = dt * (1.0 / C);
        cout << "Nuevo dt = " << dt << endl;
    }

return Malla;
}

int main(){





    return 0
}