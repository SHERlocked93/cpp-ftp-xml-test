//
// Created by qianyu on 2023/11/21.
//

#ifndef FTP_XML_TEST_FTPCTRL_H
#define FTP_XML_TEST_FTPCTRL_H

#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "ftp/FTPClient.h"
#include "xml/pugixml.hpp"

namespace fs = std::filesystem;

struct FtpCtrlTask {
    int taskId;
    int port;
    std::string ipAddress;
    std::string userName;
    std::string password;
    std::string remotePath; // 远程路径
    std::string localPath;  // 本地路径
    std::string backPath;
    int putEnable; // 0-关闭 1-开启
    int getEnable;
    int backEnable;
    std::string fileType;

    void print() const
    {
        std::cout << "TaskId: " << taskId << "\tPort: " << port << "\tIPAddress: " << ipAddress << "\tUserName: " << userName << "\tPassword: " << password << "\tRemotePath: " << remotePath << "\tLocalPath: " << localPath << "\tBackPath: " << backPath << "\tPutEnable: " << (int)putEnable << "\tGetEnable: " << (int)getEnable << "\tBackEnable: " << (int)backEnable << "\tFileType: " << fileType << std::endl;
    }
};

class FtpCtrl {
public:
    ~FtpCtrl();
    static FtpCtrl* getInstance()
    {
        static FtpCtrl ftpCtrl;
        return &ftpCtrl;
    }

    // 读取ftp配置文件并启动上传/下载任务线程
    int init(std::string& configPath);

    static int getFtpFile(const FtpCtrlTask&);
    static int putFtpFile(const FtpCtrlTask&);

    std::vector<FtpCtrlTask> putFtpTasks;
    std::vector<FtpCtrlTask> getFtpTasks;

    static void getFtpTasksThread()
    {
        while (true) {
            for (const auto& task : FtpCtrl::getInstance()->getFtpTasks) {
                auto iRet = FtpCtrl::getFtpFile(task);
                std::cout << "getFtpFile : " << iRet << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    static std::thread putFtpTasksThread()
    {
        while (true) {
            for (const auto& task : FtpCtrl::getInstance()->putFtpTasks) {
                auto iRet = FtpCtrl::putFtpFile(task);
                std::cout << "putFtpFile : " << iRet << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

private:
    embeddedmz::CFTPClient* m_ftpClient;

    std::string ftpConfigFilePath;
};

#endif // FTP_XML_TEST_FTPCTRL_H
