#include <iostream>
#include <string.h>

#include "service/show.h"
#include "service/use.h"
#include "service/backupSysEvn.h"
#include "version.h"

void help(bool is_error) {
    if (is_error) {
        std::cout << "==============================" << std::endl;
        std::cout << "This is an incorrect parameter" << std::endl;
        std::cout << "==============================" << std::endl;
    }
    std::cout << "This is svm(sdk version management). " << std::endl;
    std::cout << "Version: " << PROJECT_VERSION << std::endl;
    std::cout << "Usage: svm [help|show|use]" << std::endl;
    std::cout << "Usage: svm help" << std::endl;
    std::cout << "Usage: svm show" << std::endl;
    std::cout << "Usage: svm use <sdk_name> <sdk_version>" << std::endl;

}

int main(int argc, char** argv) {
    if (argc==2&&strcmp(argv[1],"help")==0) {
      help(false);
    } else if (argc==2&&strcmp(argv[1],"show")==0) {
        show();
    } else if (argc==4&&strcmp(argv[1],"use")==0) {
        use(argv[2],argv[3]);
    } else if(argc==2&&strcmp(argv[1],"backup")==0){
        backupSysEvn();
    } else {
        help(true);
    }
    return 0;
}