# svm

SDK Version Management Tool - 管理多版本SDK的命令行工具

## 核心功能

- 📁 仓库配置：设置远程SDK仓库链接目录
- 🔄 版本切换：快速切换当前使用的SDK版本
- 🔍 版本查询：查看本地缓存/远程仓库的SDK版本列表

## 使用指南

1. 配置仓库

在application.properties保存仓库地址与链接地址
```properties
# when you change linkBasePath, you must restart your computer, after run "use" function.
# path example:C:\svm\repository
repositoryBasePath=
linkBasePath=
```

2. 查看可用sdk  
```shell
svm show
```

3. 切换版本
```shell
svm use [sdk] [version]
sdk eg:jdk
version eg:v1.8.0_332
```

## 目录结构
```
svm-core/
├── config                      # 储存配置文件目录
│   └── application.properties  # 配置文件
├── link/                       # 默认软链接目录
├── repository/                 # 默认仓库目录
├── service/                    # 服务目录
│   ├── show                    # 展示可用sdk
│   └── use                     # 切换sdk
├── util/                       # 工具类目录
│   ├── ConfigUtil              # 操作properties
│   └── SysEnvUtil              # 操作系统环境变量
├── CMakeLists.txt
└── main.cpp
```

## TODO
- [ ] 查看当前版本

## 未来计划
- [ ] 取消使用sdk
- [ ] 从中央仓库或镜像install，查看可下载sdk，可以删除本地缓存的SDK
- [ ] 用shell配置application.properties

## 附
- 发送邮件至litehui@foxmail.com获取repository
- 推荐openjdk下载地址：https://www.openlogic.com/openjdk-downloads
