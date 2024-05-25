#include <iostream>
#include <vector>
#include<random>
#include<fstream>
#include<string>
#include<sstream>

using namespace std;
class Task {
private:
    int execTime;
    int period;
    int powerUsed;

public:
    Task() {
        execTime = 0;
        period = 0;
        powerUsed = 0;

    }
    Task(int inpExecTime, int inpPeriod, int inpPowerUsed) {
        if (inpExecTime <= 0 || inpPeriod <= 0 || inpPowerUsed <= 0) {
            std::cout << "There is an invalid value in the input.";
        }
        else {
            execTime = inpExecTime;
            period = inpPeriod;
            powerUsed = inpPowerUsed;
        }
    }
    void setExecTime(int e) {
        execTime = e;
    }
    int getExecTime() {
        return execTime;
    }
    void setPeriod(int p) {
        period = p;
    }
    int getPeriod() {
        return period;
    }
    void setPowerUsed(int power) {
        powerUsed = power;
    }
    int getPowerUsed() {
        return powerUsed;
    }
};
class TaskSet {
private:
    double utilization;
    int taskSetSize;
    std::vector<Task>taskS;
public:
    TaskSet(double inpUtilization, int inpTaskSetSize) {
        utilization = inpUtilization;
        taskSetSize = inpTaskSetSize;
        std::vector<double>vectorUsetTask = uUniFast(inpUtilization, inpTaskSetSize);
        std::vector<int>vectorPeriodTask = periodTask(inpTaskSetSize);
        bool flag = 0;
        while (!flag) {
            flag = 1;
            for (int count = 0; count < inpTaskSetSize; count++) {
                int exct = vectorPeriodTask[count] * vectorUsetTask[count];
                if (exct == 0) {
                    flag = 0;
                    vectorUsetTask = uUniFast(inpUtilization, inpTaskSetSize);
                    //cout << "task set not valid!" << endl;
                    break;
                }

            }

        }
        std::vector<int>vectorPowerTask = powerTask(inpTaskSetSize);
        Task temPTask;
        for (int k = 0; k < inpTaskSetSize; k++) {
            temPTask.setPeriod(vectorPeriodTask[k]);
            temPTask.setExecTime(vectorPeriodTask[k] * vectorUsetTask[k]);
            temPTask.setPowerUsed(vectorPowerTask[k]);
            taskS.push_back(temPTask);
        }
        print(taskS);
    }
    void print(std::vector<Task>myTaskset) {
        for (auto& myTask : myTaskset) {
            std::cout << "ExecTime:" << myTask.getExecTime() << " Period:" << myTask.getPeriod() << " PowerUsed:" << myTask.getPowerUsed() << std::endl;
        }
    }
    std::vector<double>uUniFast(double inputUtilization, int inputTaskSetSize) {
        double sumU = inputUtilization;
        double nextSumU;
        std::vector<double> vectorU;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double>dis(0.0, 1.0);
        for (int i = 1; i < inputTaskSetSize; i++) {
            double numberRandom = dis(gen);
            nextSumU = sumU * pow(numberRandom, 1.0 / (inputTaskSetSize - i));
            vectorU.push_back(sumU - nextSumU);
            sumU = nextSumU;
        }
        vectorU.push_back(sumU);
        return vectorU;
    }
    std::vector<int>periodTask(int inTaskSetSize) {
        std::vector<int>p = {6,8,15,10,12,15,20,30,60 };
        std::vector<int>per;
        int j = 0;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int>dis(1, 60);
        for (int i = 0; i < inTaskSetSize; i++) {
            j = dis(gen) % 6;
            per.push_back(p[j]);
        }
        return per;
    }
    std::vector<int>powerTask(int inpTasksSize) {
        std::vector<int>vecTP;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int>dis(4,6);
        for (int z = 0; z < inpTasksSize; z++) {
            vecTP.push_back(dis(gen));
        }
        return vecTP;
    }
    int getTaskSetSize() {
        return  taskSetSize;
    }
    Task getTask(int inpIndex) {

        return taskS[inpIndex];
    }
};
class TimingSimulation {
public:
    TimingSimulation() {}
    std::vector<int> batteryChargingRate() {
        std::ifstream inputFile("Data.txt");
        if (inputFile) {
            std::vector<string>vectorStringPower;
            std::vector<int>vectorIntPower;
            std::string line, words, ss;
            while (std::getline(inputFile, line)) {
                std::istringstream iss(line);
                while (std::getline(iss, words, ',')) {
                    vectorStringPower.push_back(words);
                }
                ss = vectorStringPower[vectorStringPower.size() - 1];
                if (std::stoi(ss) < 0) {
                    vectorIntPower.push_back(0);
                }
                else { vectorIntPower.push_back((std::stoi(ss))); }

            }
            if (vectorIntPower.size() == 0) {
                std::cout << "file is empty";
            }
            else {
                //for (auto& value1 : vectorIntPower) {
                   // cout << value1;
                //}
                return vectorIntPower;
            }

        }
        else {
            std::cout << "Unable to open file Data.txt" << endl;

        }
    }

    void pfpAsap(TaskSet myTaskSet) {
        int hyperPeriod = 60;
        int power =0;
        int counterTimer;
        std::vector<int>powerRate = batteryChargingRate();
        std::vector<int>vectEct;
        std::vector<int>vectopewerplot;
        int timeMiss = 0;
        int hiTask = 0;
        for (int i = 0; i < myTaskSet.getTaskSetSize(); i++) {
            vectEct.push_back(myTaskSet.getTask(i).getExecTime());
        }
        std::vector<int>vecReleas;
        for (int k = 0; k < myTaskSet.getTaskSetSize(); k++) {
            vecReleas.push_back(0);
        }
        std::ofstream outputFile("powerInS.txt");
        std::ofstream filePr("pr.txt");
        std::vector<int>vectEctCopy = vectEct;
        for (int time = 0; time < powerRate.size() * 60; time++) {
            if (time % 60 == 0) {
                filePr << powerRate[time / 60] << std::endl;
                outputFile << power << std::endl;
                if (power < 100) {
                    power = power + powerRate[time / 60];
                    if (power > 100) { power = 100; }    
                }
            }
            for (int x = 0; x < myTaskSet.getTaskSetSize(); x++) {
                if (vectEct[x] != 0 && vecReleas[x] <= time) {
                    hiTask = x;
                    break;

                }
            }
            //cout << "time=" << time << endl;
            //cout << "power" << power << endl;
            //cout << "number of task:" << hiTask << endl;
           // std::cout << "execTime:" << vectEct[hiTask] << endl;
            if (vecReleas[hiTask] <= time && vectEct[hiTask] > 0 && power >= myTaskSet.getTask(hiTask).getPowerUsed() / (myTaskSet.getTask(hiTask).getExecTime())) {
                power = power - (myTaskSet.getTask(hiTask).getPowerUsed() / myTaskSet.getTask(hiTask).getExecTime());
                vectEct[hiTask] = vectEct[hiTask] - 1;

                //std::cout << "power after run=" << power << endl;
            }
            if (time % myTaskSet.getTask(hiTask).getPeriod() == 0 && vectEct[hiTask] != 0) {
                cout << "deadline miss task " << hiTask<<endl;
            }
            if (time % myTaskSet.getTask(hiTask).getPeriod() == 0 && vectEct[hiTask]== 0) {
                cout << "be deadline resed task " << hiTask<<endl;
            }
            if (vecReleas[hiTask] <= time && vectEct[hiTask] > 0 && power < myTaskSet.getTask(hiTask).getPowerUsed() / (myTaskSet.getTask(hiTask).getExecTime())) {
               // cout << "power low" << endl;
            }
            if (vectEct[hiTask] == 0) {
                vecReleas[hiTask] = vecReleas[hiTask] + myTaskSet.getTask(hiTask).getPeriod();
                vectEct[hiTask] = myTaskSet.getTask(hiTask).getExecTime();

            }
        }

 
    }


};

int main()
{
    TaskSet t1(0.8,4);
    TimingSimulation ti;
    ti.pfpAsap(t1);

    return 0;
}


