//
// Created by qianyu on 2023/11/21.
//

#include "FtpCtrl.h"

#include <cassert>
std::string addTrailingSlash(const std::string& input)
{
    std::string result = input;
    if (!result.empty() && result.back() != '/') {
        result += '/';
    }
    return result;
}
int FtpCtrl::init(std::string& configPath)
{
    ftpConfigFilePath = configPath;
    m_ftpClient = new embeddedmz::CFTPClient([](const std::string& logMsg) -> void { std::cout << "[LOG]\t" << logMsg << std::endl; });
    getFtpTasks.clear();
    putFtpTasks.clear();

    pugi::xml_document xdoc;

    pugi::xml_parse_result result = xdoc.load_file(ftpConfigFilePath.c_str());
    if (!result) {
        std::cerr << "FtpCtrl::init() error:  file load err!" << std::endl;
        return 1;
    }

    pugi::xml_node root = xdoc.child("FTPCLIENTSETUP");

    pugi::xml_node nodeList = root.child("FTPCLIENTLIST");

    for (pugi::xml_node node = nodeList.child("FTPCLIENTUSER"); node; node = node.next_sibling("FTPCLIENTUSER")) {
        auto addSprit = [](const std::string& str) -> std::string {
            std::string res = str;
            if (!res.empty() && res.back() != '/')
                res += '/';
            return res;
        };
        FtpCtrlTask task{
            .taskId = node.child("FTPCLIENTID").text().as_int(),
            .port = node.child("FTPPORT").text().as_int(),
            .ipAddress = node.child("FTPIPADDRESS").text().as_string(),
            .userName = node.child("USERNAME").text().as_string(),
            .password = node.child("PASSWORD").text().as_string(),
            .remotePath = addSprit(node.child("ROMTEPATH").text().as_string()),
            .localPath = addSprit(node.child("LOCALPATH").text().as_string()),
            .backPath = addSprit(node.child("BACKPATH").text().as_string()),
            .putEnable = node.child("PUTENABLE").text().as_int(),
            .getEnable = node.child("GETENABLE").text().as_int(),
            .backEnable = node.child("BACKENABLE").text().as_int(),
            .fileType = node.child("FILETYPE").text().as_string(),
        };
        if (task.getEnable) {
            getFtpTasks.emplace_back(task);
        } else if (task.putEnable) {
            putFtpTasks.emplace_back(task);
        } else {
            std::cerr << "FtpCtrl::init() error:  task is not get or put task!" << std::endl;
        }
    }

    //    std::cout << "get ftp tasks:" << std::endl;
    //    for (const auto& task : getFtpTasks) {
    //        task.print();
    //    }
    //    std::cout << "set ftp tasks:" << std::endl;
    //    for (const auto& task : putFtpTasks) {
    //        task.print();
    //    }
    return 0;
}
std::vector<std::string> splitStringByNewLine(const std::string& inputString)
{
    std::vector<std::string> result;
    std::string line;
    std::istringstream iss(inputString);

    while (std::getline(iss, line))
        result.emplace_back(line);

    return result;
}
int FtpCtrl::getFtpFile(const FtpCtrlTask& ftpTask)
{
    int bRet = getInstance()->m_ftpClient->InitSession(ftpTask.ipAddress, ftpTask.port, ftpTask.userName, ftpTask.password, embeddedmz::CFTPClient::FTP_PROTOCOL::FTP, embeddedmz::CFTPClient::SettingsFlag::ENABLE_LOG);
    std::cout << "FtpCtrl::getFtpFile InitSession : " << (bRet ? "succ" : "err") << std::endl;
    if (!bRet) return 1;

    std::string pathList;
    bRet = getInstance()->m_ftpClient->List(ftpTask.remotePath, pathList);
    std::vector<std::string> fileNames = splitStringByNewLine(pathList);

    for (const auto& fileName : fileNames) {
        bRet = getInstance()->m_ftpClient->DownloadFile(ftpTask.localPath + fileName, ftpTask.remotePath + fileName);
        //    bRet = getInstance()->m_ftpClient->DownloadWildcard(ftpTask.localPath, ftpTask.remotePath + "/*");
        std::cout << "FtpCtrl::getFtpFile DownloadFile : " << (bRet ? "succ" : "err")
                  << std::endl
                  << ftpTask.localPath + fileName << std::endl;
        if (!bRet) return 3;
        if (fs::exists(ftpTask.localPath + fileName)) {
            bRet = getInstance()->m_ftpClient->RemoveFile(ftpTask.remotePath + fileName);
            std::cout << "FtpCtrl::getFtpFile RemoveRemoteFile : " << (bRet ? "succ" : "err") << std::endl;
            if (!bRet) return 2;
        } else
            std::cerr << "local file didnot exist:" << ftpTask.localPath + fileName << std::endl;
    }

    getInstance()->m_ftpClient->CleanupSession();
    return 0;
};

int FtpCtrl::putFtpFile(const FtpCtrlTask& ftpTask)
{
    if (!(fs::exists(ftpTask.localPath) && fs::is_directory(ftpTask.localPath))) {
        std::cerr << "ftpTask.localPath dir did not exist " << ftpTask.localPath << std::endl;
        return 1;
    }

    int bRet = getInstance()->m_ftpClient->InitSession(ftpTask.ipAddress, ftpTask.port, ftpTask.userName, ftpTask.password, embeddedmz::CFTPClient::FTP_PROTOCOL::FTP, embeddedmz::CFTPClient::SettingsFlag::ENABLE_LOG);

    if (!bRet) {
        std::cerr << "FtpCtrl::putFtpFile InitSession failed " << ftpTask.ipAddress << std::endl;
        return 2;
    }

    for (const auto& entry : fs::directory_iterator(ftpTask.localPath)) {
        if (!entry.is_regular_file()) {
            std::cerr << "ftpTask.localPath file did not exist " << entry.path().string() << std::endl;
            return 3;
        }
        bRet = getInstance()->m_ftpClient->UploadFile(entry.path(), ftpTask.remotePath + entry.path().filename().string(), true);
        std::cout << "FtpCtrl::UploadFile upload file : " << (bRet ? "succ" : "err") << "\t" << entry.path()
                  << std::endl;
        if (!bRet)
            return 4;

        embeddedmz::CFTPClient::FileInfo ResFileInfo = {0, 0.0};
        bRet = getInstance()->m_ftpClient->Info(ftpTask.remotePath + entry.path().filename().string(), ResFileInfo);
        std::cout << (bRet ? "succ" : "err") << ResFileInfo.tFileMTime << entry.path() << std::endl;
        if (bRet && (ResFileInfo.tFileMTime > 0))
            std::filesystem::remove(entry.path());
        else
            std::cerr << "remote file didnot exist:" << ftpTask.remotePath + entry.path().filename().string() << std::endl;
    }

    getInstance()->m_ftpClient->CleanupSession();
    return 0;
}

FtpCtrl::~FtpCtrl()
{
    delete m_ftpClient;
}
