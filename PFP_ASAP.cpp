#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>     // for pow
#include <limits>    // for numeric_limits

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
            execTime = 0;
            period = 0;
            powerUsed = 0;
        } else {
            execTime = inpExecTime;
            period = inpPeriod;
            powerUsed = inpPowerUsed;
        }
    }

    void setExecTime(int e) { execTime = e; }
    int getExecTime() { return execTime; }

    void setPeriod(int p) { period = p; }
    int getPeriod() { return period; }

    void setPowerUsed(int power) { powerUsed = power; }
    int getPowerUsed() { return powerUsed; }
};

class TaskSet {
private:
    double utilization;
    int taskSetSize;
    std::vector<Task> taskS;

public:
    TaskSet(double inpUtilization, int inpTaskSetSize) {
        utilization = inpUtilization;
        taskSetSize = inpTaskSetSize;

        std::vector<double> vectorUsetTask = uUniFast(inpUtilization, inpTaskSetSize);
        std::vector<int> vectorPeriodTask = periodTask(inpTaskSetSize);

        bool flag = 0;
        while (!flag) {
            flag = 1;
            for (int count = 0; count < inpTaskSetSize; count++) {
                double exctDouble = vectorPeriodTask[count] * vectorUsetTask[count];
                int exct = (int)std::round(exctDouble);
                if (exct <= 0) {
                    flag = 0;
                    vectorUsetTask = uUniFast(inpUtilization, inpTaskSetSize);
                    break;
                }
            }
        }

        std::vector<int> vectorPowerTask = powerTask(inpTaskSetSize);

        Task temPTask;
        for (int k = 0; k < inpTaskSetSize; k++) {
            int exct = (int)std::round(vectorPeriodTask[k] * vectorUsetTask[k]);
            if (exct <= 0) exct = 1;

            temPTask.setPeriod(vectorPeriodTask[k]);
            temPTask.setExecTime(exct);
            temPTask.setPowerUsed(vectorPowerTask[k]);
            taskS.push_back(temPTask);
        }

        // print(taskS); // اگر خواستی برای دیباگ فعالش کن
    }

    void print(std::vector<Task> myTaskset) {
        for (auto& myTask : myTaskset) {
            std::cout << "ExecTime:" << myTask.getExecTime()
                      << " Period:" << myTask.getPeriod()
                      << " PowerUsed:" << myTask.getPowerUsed() << std::endl;
        }
    }

    std::vector<double> uUniFast(double inputUtilization, int inputTaskSetSize) {
        double sumU = inputUtilization;
        double nextSumU;
        std::vector<double> vectorU;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dis(0.0, 1.0);

        for (int i = 1; i < inputTaskSetSize; i++) {
            double numberRandom = dis(gen);
            nextSumU = sumU * std::pow(numberRandom, 1.0 / (inputTaskSetSize - i));
            vectorU.push_back(sumU - nextSumU);
            sumU = nextSumU;
        }
        vectorU.push_back(sumU);
        return vectorU;
    }

    std::vector<int> periodTask(int inTaskSetSize) {
        std::vector<int> p = {6, 8, 10, 12, 15, 20, 30, 60};
        std::vector<int> per;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(0, (int)p.size() - 1);

        for (int i = 0; i < inTaskSetSize; i++) {
            int j = dis(gen);
            per.push_back(p[j]);
        }
        return per;
    }

    std::vector<int> powerTask(int inpTasksSize) {
        std::vector<int> vecTP;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(4, 6);

        for (int z = 0; z < inpTasksSize; z++) {
            vecTP.push_back(dis(gen));
        }
        return vecTP;
    }

    int getTaskSetSize() { return taskSetSize; }

    Task getTask(int inpIndex) {
        return taskS[inpIndex];
    }
};

class TimingSimulation {
public:
    TimingSimulation() {}

    std::vector<int> batteryChargingRate() {
        std::ifstream inputFile("Data.txt");
        std::vector<int> vectorIntPower;

        if (!inputFile) {
            std::cout << "Unable to open file Data.txt" << endl;
            return vectorIntPower; // خالی برمی‌گردانیم
        }

        std::string line;
        while (std::getline(inputFile, line)) {
            // انتظار: time,rate
            std::istringstream iss(line);
            std::string left, right;
            if (!std::getline(iss, left, ',')) continue;
            if (!std::getline(iss, right)) continue;

            try {
                int rate = std::stoi(right);
                if (rate < 0) rate = 0;
                vectorIntPower.push_back(rate);
            } catch (...) {
                // خط خراب/غیرعددی
            }
        }

        if (vectorIntPower.size() == 0) {
            std::cout << "file is empty or invalid format" << endl;
        }
        return vectorIntPower;
    }

    void pfpAsap(TaskSet myTaskSet) {
        int hyperPeriod = 60;

        // انرژی را double نگه می‌داریم تا تقسیم صحیح خرابش نکند
        double power = 0.0;

        std::vector<int> powerRate = batteryChargingRate();
        if (powerRate.empty()) {
            // اگر فایل نبود/خالی بود، برای اینکه شبیه‌سازی اجرا شود:
            powerRate.assign(10, 0); // 10 دقیقه نرخ شارژ صفر
        }

        // remaining execution time for current job of each task
        std::vector<int> vectEct;
        for (int i = 0; i < myTaskSet.getTaskSetSize(); i++) {
            vectEct.push_back(myTaskSet.getTask(i).getExecTime());
        }

        // release time for each task/job
        std::vector<int> vecReleas(myTaskSet.getTaskSetSize(), 0);

        // deadline time for each current job (release + period)
        std::vector<int> vecDeadline(myTaskSet.getTaskSetSize(), 0);
        for (int i = 0; i < myTaskSet.getTaskSetSize(); i++) {
            vecDeadline[i] = vecReleas[i] + myTaskSet.getTask(i).getPeriod();
        }

        std::ofstream outputFile("powerInS.txt");
        std::ofstream filePr("pr.txt");

        // ساخت یک ترتیب اولویت ثابت: period کمتر => اولویت بالاتر (RM)
        // اگر period برابر بود، اندیس کمتر
        std::vector<int> priorityOrder;
        for (int i = 0; i < myTaskSet.getTaskSetSize(); i++) priorityOrder.push_back(i);

        // sort ساده بدون <algorithm> (برای اینکه سبک خیلی عوض نشه)
        for (int i = 0; i < (int)priorityOrder.size(); i++) {
            for (int j = i + 1; j < (int)priorityOrder.size(); j++) {
                int a = priorityOrder[i];
                int b = priorityOrder[j];
                int pa = myTaskSet.getTask(a).getPeriod();
                int pb = myTaskSet.getTask(b).getPeriod();
                if (pb < pa || (pb == pa && b < a)) {
                    priorityOrder[i] = b;
                    priorityOrder[j] = a;
                }
            }
        }

        int totalTime = (int)powerRate.size() * hyperPeriod;

        for (int time = 0; time < totalTime; time++) {

            // هر 60 واحد زمان، شارژ باتری و ثبت داده
            if (time % hyperPeriod == 0) {
                int minuteIndex = time / hyperPeriod;

                filePr << powerRate[minuteIndex] << std::endl;
                outputFile << (int)std::round(power) << std::endl;

                if (power < 100.0) {
                    power += (double)powerRate[minuteIndex];
                    if (power > 100.0) power = 100.0;
                }
            }

            // چک deadline miss برای همه تسک‌ها (در لحظه deadline)
            for (int i = 0; i < myTaskSet.getTaskSetSize(); i++) {
                if (time == vecDeadline[i]) {
                    if (vectEct[i] > 0) {
                        cout << "deadline miss task " << i << endl;
                    }
                    // Job بعدی release می‌شود (مدل periodic)
                    vecReleas[i] += myTaskSet.getTask(i).getPeriod();
                    vecDeadline[i] = vecReleas[i] + myTaskSet.getTask(i).getPeriod();
                    vectEct[i] = myTaskSet.getTask(i).getExecTime();
                }
            }

            // انتخاب تسک با اولویت ثابت (اولین ready در priorityOrder)
            int hiTask = -1;
            for (int idx = 0; idx < (int)priorityOrder.size(); idx++) {
                int t = priorityOrder[idx];
                if (vecReleas[t] <= time && vectEct[t] > 0) {
                    hiTask = t;
                    break;
                }
            }

            if (hiTask == -1) {
                // هیچ تسک آماده‌ای نداریم
                continue;
            }

            // انرژی مصرفی در هر tick: powerUsed / execTime (double)
            double perTickEnergy = 0.0;
            int execT = myTaskSet.getTask(hiTask).getExecTime();
            if (execT > 0) {
                perTickEnergy = (double)myTaskSet.getTask(hiTask).getPowerUsed() / (double)execT;
            }

            // اجرای 1 واحد زمان اگر انرژی کافی است
            if (power >= perTickEnergy && vectEct[hiTask] > 0 && vecReleas[hiTask] <= time) {
                power -= perTickEnergy;
                if (power < 0.0) power = 0.0;
                vectEct[hiTask] -= 1;
            }
        }
    }
};

int main() {
    TaskSet t1(0.8, 4);
    TimingSimulation ti;
    ti.pfpAsap(t1);
    return 0;
}
