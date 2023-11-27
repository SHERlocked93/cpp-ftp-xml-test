//
// Created by qianyu on 2023/11/21.
//

#ifndef FTP_XML_TEST_FTPCTRL_H
#define FTP_XML_TEST_FTPCTRL_H

#include <filesystem>
#include <iostream>
#include <memory>
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
        std::cout << "TaskId: " << taskId << "\tPort: " << port
                  << "\tIPAddress: " << ipAddress << "\tUserName: " << userName
                  << "\tPassword: " << password << "\tRemotePath: " << remotePath
                  << "\tLocalPath: " << localPath << "\tBackPath: " << backPath
                  << "\tPutEnable: " << (int)putEnable
                  << "\tGetEnable: " << (int)getEnable
                  << "\tBackEnable: " << (int)backEnable
                  << "\tFileType: " << fileType << std::endl;
    }
};

class FtpCtrl {
public:
    FtpCtrl(std::string configPath);
    ~FtpCtrl();

    // 读取ftp配置文件并启动上传/下载任务线程
    int init();

    int getFtpFile(const FtpCtrlTask&);
    int putFtpFile(const FtpCtrlTask&);

    std::vector<FtpCtrlTask> putFtpTasks;
    std::vector<FtpCtrlTask> getFtpTasks;

private:
    std::unique_ptr<embeddedmz::CFTPClient> m_ftpClient;

    std::string ftpConfigFilePath;
};

#endif // FTP_XML_TEST_FTPCTRL_H
