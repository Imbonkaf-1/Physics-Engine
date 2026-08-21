#include<iostream>
#include<vector>
#include<filesystem>
#include<fstream>

#include<math.h>

using namespace std;

struct Vector
{   
    double x = 0;
    double y = 0;

};

class Body{
    public: 
        double mass = 0;
        double radius = 0;
        double inertia = 0;
        double dense = 0;
        Vector position = {0,0};
        Vector velocity = {0,0};
        Vector acceleration = {0,0};
        Vector force = {0,0};

        Vector initialVelocity = {0,0};


    Body(
        double m, double r, double i, 
        double d, Vector p, Vector v,
        Vector a, Vector f, Vector iv
    ) {
        mass = m;
        radius = r;
        inertia = i;
        dense = d;
        position = p;
        velocity = v;
        acceleration = a;
        force = f;
        initialVelocity = iv;
    }
};

const Vector gravity = {0,-9.8}; 

void motion(vector<Body>& Balls, double& time, ofstream& outFile);
double CalculateDragCoefficient(vector<Body>& Balls, int num);
void Collision(vector<Body>& Balls, int i, int j, Vector& Normal, Vector& Tangent, double distance);
void makeCSV(vector<Body> Balls, double time, ofstream& outFile);

int main(){

    try{
        std::filesystem::remove("simulation.csv");
    } catch (const std::filesystem::filesystem_error& e){
        
    }


    vector<Body> Balls;

    Body ball_1(
        0.5, 0.1, 5,
        1, {0,0}, {0,0},
        {0,0},{0,0}, {0,0}
    );

    Body ball_2(
        0.5, 0.1, 5,
        1, {0,0}, {0,0},
        {0,0},{0,0}, {0,0}
    );

    Balls.push_back(ball_1);
    Balls.push_back(ball_2);

    ofstream outFile("simulation.csv");
    outFile << "time,Ball1X,Ball1Y,Ball1VX,Ball1VY,Ball2X,Ball2Y,Ball2VX,Ball2VY\n";


    double time = 0;;
    motion(Balls, time, outFile);

    outFile.close();

    return 0;
}

void motion(vector<Body>& Balls, double& time, ofstream& outFile){

    double StartingHeight = 100;
    double acceleration = 20;
    
    double increment = 0.01;


    //Ball1 Config
    Balls.at(0).position.y = 0;
    Balls.at(0).position.x = 0;
    Balls.at(0).acceleration.x = 20;
    Balls.at(0).velocity.x = Balls.at(0).acceleration.x;



    Balls.at(1).position.y = 0;
    Balls.at(1).position.x = 60;
    Balls.at(1).acceleration.x = -20;
    Balls.at(1).velocity.x = Balls.at(1).acceleration.x;

    for(double time = 0; time < 30; time+=increment){

        Vector Normal;
        Vector Tangent;
        
        for(int i = 0; i < Balls.size(); i++){
            for(int j = i+1; j < Balls.size(); j++){
            
                double distance = sqrt(pow(Balls.at(j).position.x - Balls.at(i).position.x, 2) + pow(Balls.at(j).position.y - Balls.at(i).position.y, 2));

                if(Balls.at(i).radius+Balls.at(j).radius >= distance){

                    Normal.x = (Balls.at(j).position.x - Balls.at(i).position.x)/distance;
                    Normal.y = (Balls.at(j).position.y - Balls.at(i).position.y)/distance; 
                    Tangent.x = -Normal.y;
                    Tangent.y = Normal.x;

                    Collision(Balls, i, j, Normal, Tangent, distance);
                }
            }
        }
            
        for(int num = 0; num < Balls.size(); num++){

            double DragForceX = 0;
            double DragForceY = 0;
            double DragCoefficient = CalculateDragCoefficient(Balls, num); //Calls the function to calculate DragCoefficient.

            //Uses the euler integration method to constantly apply negative acceleration (gravity) on the ball
            //Funnily enough since its being applied all the time it also handles the bouncing of the ball...
            //Since gravity is always pulling down. Neat!
            Balls.at(num).velocity.y += gravity.y * increment;
            Balls.at(num).position.y += Balls.at(num).velocity.y * increment;
            Balls.at(num).position.x += Balls.at(num).velocity.x * increment;

            //Calculates DragForce for vertical AND Horizontal motion
            DragForceX += -Balls.at(num).velocity.x*abs(Balls.at(num).velocity.x)*DragCoefficient;
            DragForceY += -Balls.at(num).velocity.y*abs(Balls.at(num).velocity.y)*DragCoefficient;

            //Applies a negative force equal to the dragforce again using the euler integration method
            Balls.at(num).velocity.y += (DragForceY/Balls.at(num).mass)*increment;
            Balls.at(num).velocity.x += (DragForceX/Balls.at(num).mass)*increment;

            if(Balls.at(num).position.y <= 0){ //Condition for bouncing back when from the "ground", to be replaced with an actual object
                Balls.at(num).position.y = 0;
                Balls.at(num).velocity.y = Balls.at(num).velocity.y*-0.85;
            }

            makeCSV(Balls, time, outFile); //Function call to make the .csv file

            //Displays height and vertical velocity of ball (acceleration due to gravity)
            std::cout << "Height For Ball 1: " << Balls.at(0).position.y << "\n";        
            std::cout << "Y velocity For Ball 1: " << Balls.at(0).velocity.y << "\n\n";

            //Displays horizontal position and velocity of ball (given initial nudge aka acceleration)
            std::cout << "X velocity For Ball 1: " << Balls.at(0).velocity.x << "\n";
            std::cout << "Position For Ball 1: " << Balls.at(0).position.x << "\n\n";

            std::cout << "Height For Ball 2: " << Balls.at(1).position.y << "\n";        
            std::cout << "Y velocity For B  all 2: " << Balls.at(1).velocity.y << "\n\n";

            //Displays horizontal position and velocity of ball (given initial nudge aka acceleration)
            std::cout << "X velocity For Ball 2: " << Balls.at(1).velocity.x << "\n";
            std::cout << "Position For Ball 2: " << Balls.at(1).position.x << "\n\n";
        }   
    }
}

void Collision(vector<Body>& Balls, int i, int j, Vector& Normal, Vector& Tangent, double distance){

    double Coefficient_Of_Restituion = 0.88;

    double Tangential_Velocity_1 = Balls.at(i).velocity.x*Tangent.x + Balls.at(i).velocity.y*Tangent.y;
    double Tangential_Velocity_2 = Balls.at(j).velocity.x*Tangent.x + Balls.at(j).velocity.y*Tangent.y;

    double Normal_Velocity_1 = Balls.at(i).velocity.x*Normal.x + Balls.at(i).velocity.y*Normal.y;
    double Normal_Velocity_2 = Balls.at(j).velocity.x*Normal.x + Balls.at(j).velocity.y*Normal.y;

    double Final_Normal_Velocity_1 = (Balls.at(i).mass*Normal_Velocity_1 + Balls.at(j).mass*Normal_Velocity_2 + Balls.at(j).mass*Coefficient_Of_Restituion*(Normal_Velocity_2 - Normal_Velocity_1))/(Balls.at(i).mass + Balls.at(j).mass);
    double Final_Normal_Velocity_2 = (Balls.at(i).mass*Normal_Velocity_1 + Balls.at(j).mass*Normal_Velocity_2 + Balls.at(i).mass*Coefficient_Of_Restituion*(Normal_Velocity_1 - Normal_Velocity_2))/(Balls.at(i).mass + Balls.at(j).mass);

    Balls.at(i).velocity.x = Final_Normal_Velocity_1*Normal.x + Tangential_Velocity_1*Tangent.x;
    Balls.at(i).velocity.y = Final_Normal_Velocity_1*Normal.y + Tangential_Velocity_1*Tangent.y;

    double overlap = (Balls.at(i).radius + Balls.at(j).radius) - distance;

    Balls.at(i).position.x -= overlap;
    Balls.at(j).position.x += overlap;

    Balls.at(j).velocity.x = Final_Normal_Velocity_2*Normal.x + Tangential_Velocity_2*Tangent.x;
    Balls.at(j).velocity.y = Final_Normal_Velocity_2*Normal.y + Tangential_Velocity_2*Tangent.y;

}

double CalculateDragCoefficient(vector<Body>& Balls, int num){
    double mass_density_of_fluid = 1.255; //for air
    double Dimensionless_Drag_Coefficient = 0.47; //for sphere
    double Area_Facing_Drag = 3.14*pow(Balls.at(num).radius, 2);

    double Coef = 0.5*mass_density_of_fluid*Dimensionless_Drag_Coefficient*Area_Facing_Drag;
    return Coef;
}

void SpaceMotion(){

}

void makeCSV(vector<Body> Balls, double time, ofstream& outFile){
    outFile << time << "," << Balls.at(0).position.x << "," << Balls.at(0).position.y 
        << "," << Balls.at(0).velocity.x << "," << Balls.at(0).velocity.y << ','
        << Balls.at(1).position.x << "," << Balls.at(1).position.y << "," << Balls.at(1).velocity.x
        << "," << Balls.at(1).velocity.y << "\n";
}