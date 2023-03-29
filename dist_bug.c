#include "eyebot.h"
#include "math.h"
#include <stdio.h>

#define GOAL_X 5500
#define GOAL_Y 5500
#define STEP 1000

double getRelativeGoalLocationAng(int curX, int curY, int x, int y){
    double rot = atan2(y-curY, x-curX) * 180/M_PI;
    return rot;
}

double getRelativeGoalLocationDis(int curX, int curY, int x, int y){
    int nx = x - curX;
    int ny = y - curY;
    double distance = sqrt(nx*nx+ny*ny);
    return distance;
}

bool checkAtPoint(double x, double y, int goalX, int goalY){
    float nx = goalX-x;
    float ny = goalY-y;
    if ((nx >=-50 && nx <= 50)&&(ny >=-50 && ny <= 50)){
        return true;
    } else{
        return false;
    }
}

bool dist_bug(x, y){
    while (true){
       

        int currentXPosition, currentYPosition, currentAngle;
        VWGetPosition(&currentXPosition, &currentYPosition, &currentAngle);
        

        if (checkAtPoint(currentXPosition, currentYPosition, x, y)){
            return true;
        }

        int angle = getRelativeGoalLocationAng(currentXPosition, currentYPosition, x, y);
        printf("angle = %i, currentAngle = %i\n", angle, currentAngle);

        int angDif = angle - currentAngle;
        if (5<angDif || -5>angDif){
             VWTurn(angle, 60);
             VWWait();
             VWGetPosition(&currentXPosition, &currentYPosition, &currentAngle);

        } 
        
        VWSetSpeed(100, 0);

        bool obstacleDetected = false;
        if (PSDGet(PSD_FRONT)<100){
            obstacleDetected = true;
        }

        if (obstacleDetected){
            VWSetSpeed(0, 0);

            
            VWGetPosition(&currentXPosition, &currentYPosition, &currentAngle);

            double hitPoint[2] = {currentXPosition, currentYPosition};

            double minimumDistance = INFINITY;
            // bool leftHitPoint = False;

            VWTurn(90, 60);
            VWWait();
            double distToRight = PSDGet(PSD_RIGHT);

            while (true){

                // boundary following
                // TODO: follow the boundary adjusting direction each time loop executes
                // remember to make this non blocking
                
                double distRight = PSDGet(PSD_RIGHT);
                double dist = distRight- distToRight;
                VWSetSpeed(100, 0);
                if (-25>dist){
                    VWSetSpeed(0, 0);
                    VWTurn(-10, 60);
                    VWWait();
                    VWSetSpeed(100, 0);
                }
                if (25<dist){
                    VWSetSpeed(0, 0);
                    VWTurn(10, 60);
                    VWWait();
                    VWSetSpeed(100, 0);
                }

                VWGetPosition(&currentXPosition, &currentYPosition, &currentAngle);
                

                if (checkAtPoint(currentXPosition, currentYPosition, hitPoint[0], hitPoint[1])){
                    return false;
                }

                double distance = getRelativeGoalLocationDis(currentXPosition, currentYPosition, x, y);
                double angle = getRelativeGoalLocationAng(currentXPosition, currentYPosition, x, y);

                if (distance < minimumDistance){
                    minimumDistance = distance;
                }

                int angleToGoal = angle - currentAngle;

                int lidar_values[360];
                LIDARGet(lidar_values);

                double freeSpaceToGoal = lidar_values[angleToGoal];

                if ((distance = freeSpaceToGoal) <= minimumDistance - STEP){
                    break;
                }




            }

        }
    }
}

int main() {
    SIMSetRobot(0, 500, 500, 0, 0);
    LCDMenu("Play", "", "", "Break");
    while (1){
        int key = KEYRead();
        if (key == KEY1){
            LCDPrintf("dist_bug(%i, %i);", GOAL_X, GOAL_Y);
            dist_bug(GOAL_X, GOAL_Y);
            break;
        } 


        
        if (key == KEY4){
            break;
        }

    }
    return EXIT_SUCCESS;
}