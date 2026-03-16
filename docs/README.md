# SVM-Core 使用手册

## 项目简介

SVM-Core 是一个 SDK 版本管理工具，支持导入本地 SDK 并管理系统环境变量的 SDK 指向。它可以帮助开发者在不同版本的 SDK 之间快速切换，无需手动修改环境变量。

主要功能：
- 管理多个 SDK 的不同版本
- 快速切换 SDK 版本
- 自动更新系统和用户环境变量
- 支持配置 SDK 环境变量名称和 Path 文件夹
- 提供备份和恢复系统环境变量的功能
- 支持配置 repository 和 link 文件夹位置

## 安装说明

1. **编译项目**
   - 使用 CMake 构建项目
   - 确保系统已安装 C++ 编译器

2. **运行环境**
   - Windows 操作系统
   - 需要管理员权限（修改系统环境变量时）

3. **目录结构**
   - `repository/`：存储 SDK 版本
   - `link/`：存储指向当前使用 SDK 版本的符号链接
   - `config/`：存储配置文件
   - `service/`：命令实现
   - `util/`：工具类

## 命令列表

### 1. use 命令

**功能**：使用指定版本的 SDK

**语法**：
```
svm use <sdk> <version>
```

**示例**：
```
svm use jdk 1.8.0_202
svm use maven 3.6.3
svm use gradle 6.8.3
svm use tomcat 9.0.41
```

**说明**：
- 会创建指向指定 SDK 版本的符号链接
- 自动更新系统和用户环境变量
- 备份当前系统环境变量
- 对于 Maven，会同时设置 MAVEN_HOME 和 M2_HOME 环境变量

### 2. show 命令

**功能**：显示所有 SDK 及其版本，标记当前使用的版本

**语法**：
```
svm show
```

**说明**：
- 列出所有已安装的 SDK 和版本
- 用 `*` 标记当前系统环境变量使用的版本

### 3. add 命令

**功能**：添加新的 SDK 或 SDK 版本

**语法**：
```
svm add <sdk> <version> <path>
```

**示例**：
```
svm add jdk 1.8.0_202 D:\SDKs\jdk1.8.0_202
svm add maven 3.6.3 D:\SDKs\apache-maven-3.6.3
```

**说明**：
- 会将指定路径的 SDK 复制到 repository 目录
- 如果是新的 SDK，会在 sdk_config.json 中添加默认配置

### 4. delete 命令

**功能**：删除指定 SDK 或 SDK 版本

**语法**：
```
svm delete <sdk> [version]
```

**示例**：
```
svm delete jdk 1.8.0_202  # 删除指定版本
svm delete maven            # 删除整个 SDK
```

**说明**：
- 如果删除的是当前使用的版本，会提示用户确认
- 确认后会删除相关的系统和用户环境变量
- 删除整个 SDK 时会删除所有版本

### 5. config 命令

**功能**：配置 repository 和 link 文件夹位置

**语法**：
```
svm config <repositoryPath> <linkPath>
```

**示例**：
```
svm config D:\SDKs\repository D:\SDKs\link
```

**说明**：
- 修改配置文件中的路径
- 迁移现有文件到新位置
- 更新相关的环境变量和符号链接

### 6. sdkconfig 命令

**功能**：管理 SDK 环境变量配置

**语法**：
```
svm sdkconfig [sdk] [homeEnv] [pathFolders...]
```

**示例**：
```
svm sdkconfig                    # 显示所有 SDK 配置
svm sdkconfig maven MAVEN_HOME bin  # 修改 Maven 配置
```

**说明**：
- 查看或修改 SDK 的环境变量名称和 Path 文件夹
- 配置会保存在 sdk_config.json 文件中

### 7. backup 命令

**功能**：备份系统和用户环境变量

**语法**：
```
svm backup
```

**说明**：
- 生成带时间戳的备份文件
- 备份文件保存在 config 目录下
- 文件名格式：backupSysEnv_YYYYMMDD_HHMMSS.txt

## 配置说明

### 1. 应用配置文件

**文件**：`config/application.properties`

**配置项**：
- `repositoryBasePath`：SDK 仓库路径
- `linkBasePath`：符号链接路径

### 2. SDK 配置文件

**文件**：`config/sdk_config.json`

**配置格式**：
```json
{
  "sdk名称": {
    "homeEnv": "环境变量名称",
    "pathFolders": ["bin", "lib"]
  }
}
```

**示例**：
```json
{
  "gradle": {
    "homeEnv": "GRADLE_HOME",
    "pathFolders": ["bin"]
  },
  "jdk": {
    "homeEnv": "JAVA_HOME",
    "pathFolders": ["bin"]
  },
  "maven": {
    "homeEnv": "MAVEN_HOME",
    "pathFolders": ["bin"]
  },
  "tomcat": {
    "homeEnv": "CATALINA_HOME",
    "pathFolders": ["bin"]
  }
}
```

## 常见问题

### 1. 权限问题

**问题**：修改系统环境变量失败
**解决方案**：以管理员身份运行命令行

### 2. 符号链接创建失败

**问题**：创建符号链接时出现错误
**解决方案**：
- 确保命令行以管理员身份运行
- 检查目标路径是否存在
- 检查链接路径是否已存在

### 3. 环境变量未生效

**问题**：设置环境变量后，新的终端中仍然显示旧值
**解决方案**：
- 重启终端
- 或者执行 `refreshenv` 命令（如果安装了 Chocolatey）

### 4. SDK 版本切换后不生效

**问题**：使用 `svm use` 切换版本后，`java -version` 等命令仍显示旧版本
**解决方案**：
- 重启终端
- 检查 Path 环境变量中 SDK 相关路径的顺序
- 确保 SDK 路径在 Path 环境变量的前面

## 故障排除

### 1. 查看日志

- 备份文件会记录环境变量的变化
- 命令执行过程中的错误会输出到控制台

### 2. 恢复环境变量

- 如果环境变量设置错误，可以从备份文件中恢复
- 备份文件位于 `config/` 目录下，文件名包含时间戳

### 3. 重置配置

- 删除 `config/application.properties` 文件，会使用默认路径
- 删除 `config/sdk_config.json` 文件，会使用默认配置

## 注意事项

1. **管理员权限**：修改系统环境变量需要管理员权限
2. **路径长度**：Windows 系统对路径长度有限制，建议使用较短的路径
3. **符号链接**：需要 Windows 10 或以上版本，或启用开发者模式
4. **备份**：每次修改环境变量前会自动备份，建议定期查看备份文件
5. **兼容性**：对于 Maven，会同时设置 MAVEN_HOME 和 M2_HOME 以确保兼容性

## 版本历史

- **1.0.0**：初始版本，支持 JDK、Maven、Gradle、Tomcat 的管理
- **1.1.0**：添加 add、delete、config、sdkconfig 命令
- **1.2.0**：改进环境变量管理，添加时间戳备份功能

## 联系方式

如有问题或建议，请联系项目维护者。