#include <chrono>
#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <regex>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

const std::string CONFIG_FILE = ".config";

std::string getip() {
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);

  char hostName[256];
  gethostname(hostName, sizeof(hostName));

  struct addrinfo hints = {};
  hints.ai_family = AF_INET;
  struct addrinfo *info;

  getaddrinfo(hostName, nullptr, &hints, &info);

  char ipAddress[INET_ADDRSTRLEN];
  struct sockaddr_in *sockaddr_ipv4 = (struct sockaddr_in *)info->ai_addr;
  inet_ntop(AF_INET, &(sockaddr_ipv4->sin_addr), ipAddress, INET_ADDRSTRLEN);

  freeaddrinfo(info);
  WSACleanup();

  return std::string(ipAddress);
}

std::string post(const std::string &url, const std::string &postData) {
  CURL *curl;
  CURLcode res;
  std::string response;

  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                     [](void *contents, size_t size, size_t nmemb,
                        std::string *userp) -> size_t {
                       size_t totalSize = size * nmemb;
                       userp->append((char *)contents, totalSize);
                       return totalSize;
                     });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }

  return response;
}

std::string extractValue(const std::string &line, const std::string &key) {
  size_t pos = line.find(key);
  if (pos != std::string::npos) {
    return line.substr(pos + key.length()); // 提取标签之后的部分
  }
  return "";
}

void saveConfig(const std::string &username, const std::string &password,
                const std::string &ipaddress) {
  std::ofstream configFile(CONFIG_FILE);
  if (configFile.is_open()) {
    configFile << "账号：" << username << std::endl;
    configFile << "密码：" << password << std::endl;
    configFile << "地址：" << ipaddress << std::endl;
    configFile.close();
    std::cout << "配置信息已保存至 " << CONFIG_FILE << std::endl;
  } else {
    std::cerr << "无法打开文件保存配置！" << std::endl;
  }
}

bool loadConfig(std::string &username, std::string &password,
                std::string &ipaddress) {
  std::ifstream configFile(CONFIG_FILE);
  if (configFile.is_open()) {
    std::string line;

    std::getline(configFile, line);
    username = extractValue(line, "账号：");

    std::getline(configFile, line);
    password = extractValue(line, "密码：");

    std::getline(configFile, line);
    ipaddress = extractValue(line, "地址：");

    configFile.close();
    return !username.empty() && !password.empty() && !ipaddress.empty();
  } else {
    return false;
  }
}

int main() {
  SetConsoleCP(CP_UTF8);
  SetConsoleOutputCP(CP_UTF8);
  std::string USERNAME, PASSWORD, LOGIN_IP;
  if (!loadConfig(USERNAME, PASSWORD, LOGIN_IP)) {
    std::cout << "第一次使用或配置丢失，请手动输入账号、密码和地址："
              << std::endl;
    std::cout << "账号: ";
    std::cin >> USERNAME;
    std::cout << "密码: ";
    std::cin >> PASSWORD;
    std::cout << "登录ip地址: ";
    std::cin >> LOGIN_IP;
    saveConfig(USERNAME, PASSWORD, LOGIN_IP);
  }

  std::string IP_ADDRESS = getip();
  std::string url1 =
      "http://" + LOGIN_IP +
      ":801/eportal/?c=ACSetting&a=Login&protocol=http:&hostname=" + LOGIN_IP +
      "&iTermType=1&wlanuserip=" + IP_ADDRESS +
      "&wlanacip=null&wlanacname=null&mac=00-00-00-00-00-00&ip=" + IP_ADDRESS +
      "&enAdvert=0&queryACIP=0&jsVersion=2.4.3&loginMethod=1";
  std::string cont =
      "DDDDD=" + USERNAME + "&upass=" + PASSWORD +
      "&R1=0&R2=0&R3=0&R6=0&para=00&0MKKey=123456&buttonClicked=&redirect_url=&"
      "err_flag=&username=&password=&user=&cmd=&Login=&v6ip=";

  std::string LOG = post(url1, cont);

  std::regex acipRegex("wlanacip=([0-9.]+)");
  std::regex acnameRegex("wlanacname=([A-Za-z0-9-]+)");
  std::smatch match;

  std::string ACIP = "null";
  std::string ACNAME = "null";

  if (std::regex_search(LOG, match, acipRegex)) {
    ACIP = match[1];
  }
  if (std::regex_search(LOG, match, acnameRegex)) {
    ACNAME = match[1];
    char lastChar = ACNAME.back();
    if (lastChar == 'A') {
      ACNAME.back() = 'B';
    } else if (lastChar == 'B') {
      ACNAME.back() = 'A';
    }
  }

  std::string url2 =
      "http://" + LOGIN_IP +
      ":801/eportal/?c=ACSetting&a=Login&protocol=http:&hostname=" + LOGIN_IP +
      "&iTermType=1&wlanuserip=" + IP_ADDRESS + "&wlanacip=" + ACIP +
      "&wlanacname=" + ACNAME + "&mac=00-00-00-00-00-00&ip=" + IP_ADDRESS +
      "&enAdvert=0&queryACIP=0&jsVersion=2.4.3&loginMethod=1";

  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::string response = post(url2, cont);
  return 0;
}
