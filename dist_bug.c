#include "eyebot.h"
#include "math.h"
#include <stdio.h>

#define GOAL_X 5500
#define GOAL_Y 5500
#define STEP 1500

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
    if ((nx >=-75 && nx <= 75)&&(ny >=-75 && ny <= 75)){
        return true;
    } else{
        return false;
    }
}

bool dist_bug(x, y){
    while (true){


        int key = KEYRead();
        if (key == KEY4){
            VWSetSpeed(0, 0);
            return false;
        }



       

        int currentXPosition, currentYPosition, currentAngle;
        VWGetPosition(&currentXPosition, &currentYPosition, &currentAngle);
        printf("curX: %i, curY: %i, curAngle: %i\n", currentXPosition, currentYPosition, currentAngle);
        

        if (checkAtPoint(currentXPosition, currentYPosition, x-500, y-500)){
            return true;
        }

        int angle = getRelativeGoalLocationAng(currentXPosition, currentYPosition, x, y);
        int angDif = angle - currentAngle;
        if (5<angDif || -5>angDif){
             VWTurn(angDif, 60);
             VWWait();
             VWGetPosition(&currentXPosition, &currentYPosition, &currentAngle);

        } 
        
        VWSetSpeed(100, 0);

        bool obstacleDetected = false;
        if (PSDGet(PSD_FRONT)<150){
            obstacleDetected = true;
        }

        if (obstacleDetected){
            VWSetSpeed(0, 0);

            
            VWGetPosition(&currentXPosition, &currentYPosition, &currentAngle);

            double hitPoint[2] = {currentXPosition, currentYPosition};

            double minimumDistance = 999999999;
            

            VWTurn(90, 60);
            VWWait();



            int count = 0;


            while (true){
                count++;
                int key = KEYRead();
                if (key == KEY4){
                    VWSetSpeed(0, 0);
                    return false;;
                }

                // boundary following
                // TODO: follow the boundary adjusting direction each time loop executes
                // remember to make this non blocking
                
                double tooFar = PSDGet(PSD_RIGHT) > 150;
                double tooClose = PSDGet(PSD_RIGHT) < 100;
                if (tooFar){
                    VWTurn(-15, 45);
                    VWWait();
                } else if(tooClose){
                    VWTurn(15, 45);
                    VWWait();
                }
                else{
                    VWStraight(50, 50);
                    VWWait();
                }
                
                int lidar_values[360];
                LIDARGet(lidar_values);
                VWGetPosition(&currentXPosition, &currentYPosition, &currentAngle);
                

                if (checkAtPoint(currentXPosition, currentYPosition, hitPoint[0], hitPoint[1]) && count > 10){
                     return false;
                }

                double distance = getRelativeGoalLocationDis(currentXPosition, currentYPosition, x, y);
                double angle = getRelativeGoalLocationAng(currentXPosition, currentYPosition, x, y);
                
                if (distance < minimumDistance){
                    minimumDistance = distance;
                }
                //angle = 180 + phi - req_angle
                //  if (angle <0){
                    //angle += 360;
                //}

                //freespace = dists[angle]

                VWGetPosition(&currentXPosition, &currentYPosition, &currentAngle);
                int angleToGoal = 180 + currentAngle - angle;
                if (angleToGoal < 0){
                    angleToGoal = 360;
                }
                
                
                

                

                
                //printf("angleToGoal = %i, LIDARVALUE = %i\n", angleToGoal, lidar_values[angleToGoal]);
                double freeSpaceToGoal = lidar_values[angleToGoal];



                //printf("distance(%f)-freeSpaceToGoal(%f)<=minimumDistance(%f)-STEP(%i)\n", round(distance), round(freeSpaceToGoal), round(minimumDistance), STEP);

                if ((distance - freeSpaceToGoal) <= (minimumDistance - STEP)){
                    VWStraight(200, 100);
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
            VWSetSpeed(0, 0);
            break;
        } 


        
        if (key == KEY4){
            VWSetSpeed(0, 0);
            break;
        }

    }
    return EXIT_SUCCESS;
}