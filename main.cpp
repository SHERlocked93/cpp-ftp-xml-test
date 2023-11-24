#include <iostream>

#include "FtpCtrl.h"

int main()
{
    std::string configPath("../FTPConfig.xml");
    FtpCtrl::getInstance()->init(configPath);

    std::thread getFtpTasksThread(FtpCtrl::getFtpTasksThread);
    std::thread putFtpTasksThread(FtpCtrl::putFtpTasksThread);

    getFtpTasksThread.join();
    putFtpTasksThread.join();

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
