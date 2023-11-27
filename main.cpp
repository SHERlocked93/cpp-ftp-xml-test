#include <iostream>

#include "FtpCtrl.h"
std::string configPath("../FTPConfig.xml");

void getFtpTasksThread()
{
    std::unique_ptr<FtpCtrl> m_pFtpCtrl = std::make_unique<FtpCtrl>(configPath);
    m_pFtpCtrl->init();

    while (true) {
        for (const auto& task : m_pFtpCtrl->getFtpTasks) {
            auto iRet = m_pFtpCtrl->getFtpFile(task);
            std::cout << "getFtpFile : " << (iRet == 0 ? "succ" : "err") << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

std::thread putFtpTasksThread()
{
    std::unique_ptr<FtpCtrl> m_pFtpCtrl = std::make_unique<FtpCtrl>(configPath);
    m_pFtpCtrl->init();

    while (true) {
        for (const auto& task : m_pFtpCtrl->putFtpTasks) {
            auto iRet = m_pFtpCtrl->putFtpFile(task);
            std::cout << "putFtpFile : " << (iRet == 0 ? "succ" : "err") << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

int main()
{
    std::thread GetFtpTasksThread(getFtpTasksThread);
    std::thread PutFtpTasksThread(putFtpTasksThread);

    GetFtpTasksThread.join();
    PutFtpTasksThread.join();

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
