# C++跨平台项目模板  
## 开发环境  
CMake、Vcpkg  
#### 平台差异  
Windows：MinGW  
Linux：GCC  
## 安装Vcpkg包管理工具  
使用Scoop包管理工具安装Vcpkg  
## 如何为项目添加一个包？  
1.在[第三方Vcpkg包搜索](https://vcpkg.roundtrip.dev)网址中搜索你想要的包  
2.将搜索到的包名称添加到项目根目录的vcpkg.json文件里的dependencies字段中，然后重新生成CMake项目就会自动下载dependencies字段中的包  
3.将生成CMake项目时Vcpkg工具链提示的**包CMake代码**添加到/app/CMakeLists.txt文件里的最后面，并根据自己的情况修改**包CMake代码**字段  
## 更加详细的Vcpkg使用说明？  
更加详细的Vcpkg使用说明请参考[微软官方文档](https://learn.microsoft.com/vcpkg)  
## 更多详细的CMake的使用说明？  
更多详细的CMake的使用说明请参考[CMake官方文档的cmake-presets文档](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html)  
