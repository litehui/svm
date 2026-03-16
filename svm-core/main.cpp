#include <iostream>
#include <string.h>

#include "service/show.h"
#include "service/use.h"
#include "service/backupSysEvn.h"
#include "service/add.h"
#include "service/delete.h"
#include "service/config.h"
#include "service/sdkconfig.h"
#include "version.h"

void help(bool is_error) {
    if (is_error) {
        std::cout << "==============================" << std::endl;
        std::cout << "This is an incorrect parameter" << std::endl;
        std::cout << "==============================" << std::endl;
    }
    std::cout << "This is svm(sdk version management). " << std::endl;
    std::cout << "Version: " << PROJECT_VERSION << std::endl;
    std::cout << "Usage: svm [help|show|use|add|delete|config|sdkconfig|backup]" << std::endl;
    std::cout << "Usage: svm help" << std::endl;
    std::cout << "Usage: svm show" << std::endl;
    std::cout << "Usage: svm use <sdk_name> <sdk_version>" << std::endl;
    std::cout << "Usage: svm add <sdk_name> <sdk_version> [source_path]" << std::endl;
    std::cout << "Usage: svm delete <sdk_name> [version]" << std::endl;
    std::cout << "Usage: svm config <key> <value>" << std::endl;
    std::cout << "Usage: svm sdkconfig <sdk> <home_env> <path_folders>" << std::endl;
    std::cout << "Usage: svm sdkconfig show [sdk]" << std::endl;
    std::cout << "Usage: svm backup" << std::endl;
    std::cout << "Config keys: repositoryBasePath, linkBasePath" << std::endl;

}

int main(int argc, char** argv) {
    if (argc==2&&strcmp(argv[1],"help")==0) {
      help(false);
    } else if (argc==2&&strcmp(argv[1],"show")==0) {
        show();
    } else if (argc==4&&strcmp(argv[1],"use")==0) {
        use(argv[2],argv[3]);
    } else if (argc==4&&strcmp(argv[1],"add")==0) {
        add(argv[2],argv[3]);
    } else if (argc==5&&strcmp(argv[1],"add")==0) {
        add(argv[2],argv[3],argv[4]);
    } else if (argc==3&&strcmp(argv[1],"delete")==0) {
        removeSdk(argv[2]);
    } else if (argc==4&&strcmp(argv[1],"delete")==0) {
        removeSdk(argv[2],argv[3]);
    } else if (argc==4&&strcmp(argv[1],"config")==0) {
        config(argv[2],argv[3]);
    } else if (argc==5&&strcmp(argv[1],"sdkconfig")==0) {
        sdkconfig(argv[2], argv[3], argv[4]);
    } else if (argc==3&&strcmp(argv[1],"sdkconfig")==0&&strcmp(argv[2],"show")==0) {
        sdkconfig_show();
    } else if (argc==4&&strcmp(argv[1],"sdkconfig")==0&&strcmp(argv[2],"show")==0) {
        sdkconfig_show(argv[3]);
    } else if(argc==2&&strcmp(argv[1],"backup")==0){
        backupSysEvn();
    } else {
        help(true);
    }
    return 0;
}