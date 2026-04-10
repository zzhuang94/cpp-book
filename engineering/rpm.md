# RPM 打包

> 在 `C++` 项目的生命周期里，"代码能编译"只是第一步。
> 怎么把编译出来的二进制、配置、脚本打成一个**可分发、可安装、可升级、可卸载**的包，才是工程交付的关键一环。

很多开发者刚进入 `C++` 项目时，关注点集中在：

- 代码怎么写
- 编译怎么过
- 功能怎么测

但当你真正要把程序部署到线上机器时，很快会遇到另一类问题：

- 怎么保证所有机器安装的版本一致
- 怎么在安装时自动创建目录、建立软链接
- 怎么在卸载时清理干净，不留残余
- 怎么管理服务的开机自启、日志轮转、定时任务
- 怎么记录每次发布改了什么

这些问题背后，几乎都指向同一个答案：**包管理**。

在 `Red Hat` 系 Linux 发行版（CentOS、RHEL、Fedora 等）上，标准的包管理格式就是 `RPM`。

这篇文档会按照"先建立整体认知，再逐段拆解"的顺序，带你理解 RPM 打包的完整流程。读完之后，你应该能完全看懂一个真实项目的 `packaging/` 目录里的每一行代码。

----

# 什么是 RPM

> `RPM` 全称 `RPM Package Manager`，是一种用于 Linux 系统的软件包格式和管理工具。

一个 `.rpm` 文件本质上是一个压缩归档，里面包含：

- 要安装到系统上的文件（二进制、配置、脚本等）
- 元信息（包名、版本、依赖关系、描述等）
- 安装/卸载时要执行的脚本

你可以把 RPM 想象成一个"带说明书的快递包裹"：

- 包裹里是你的程序文件
- 说明书上写着怎么安装、放到哪里、需要什么前置条件
- 还附带了"开箱后执行"和"退货前执行"的操作指引

装包用 `rpm -ivh`，升级用 `rpm -Uvh`，卸载用 `rpm -e`，查询用 `rpm -q`。这些是最基础的操作命令。

----

# 整体流程

> RPM 打包不是"把文件压缩一下"那么简单，它有一套严格的流水线。

从源码到最终 `.rpm` 文件，整体流程是这样的：

```text
源代码仓库
   ↓
打包脚本：归档源码为 .tar.gz
   ↓
rpmbuild 读取 .spec 文件
   ↓
%prep：解压源码包
   ↓
%build：编译源码
   ↓
%install：将产物安装到虚拟根目录
   ↓
%files：声明哪些文件要打入 RPM
   ↓
生成 .rpm 文件
```

整个过程的核心控制文件就是 `.spec` 文件。可以说，**读懂了 `.spec`，就读懂了 RPM 打包的 80%**。

----

# 项目目录结构

> 在动手写 `.spec` 之前，先搞清楚一个打包目录通常长什么样。

以本项目的 `packaging/` 目录为例：

```text
packaging/
├── Makefile                    # 打包入口，调用 rpmbuild
├── control                     # 辅助脚本，负责归档源码
├── service/                    # demo-server 的打包资源
│   ├── demo-server.spec    # .spec 文件（核心）
│   ├── demo-server_init_d  # SysV init 启停脚本
│   ├── demo-server_cron    # 定时任务配置
│   └── demo-server_logrotate # 日志轮转配置
├── login/                      # demo-login 的打包资源
│   ├── demo-login.spec
│   ├── demo-login_init_d
│   ├── demo-login_cron
│   └── demo-login_logrotate
├── mqtt/                       # demo-mqtt 的打包资源
├── plugin/                     # demo-plugin 的打包资源
├── manager/                    # demo-manager 的打包资源
└── status/                     # demo-status 的打包资源
```

每个子服务都有自己的一组文件：

| 文件 | 作用 |
|------|------|
| `*.spec` | RPM 打包规范，核心中的核心 |
| `*_init_d` | SysV 风格的服务启停脚本，装到 `/etc/init.d/` |
| `*_cron` | cron 定时任务，装到 `/etc/cron.d/` |
| `*_logrotate` | 日志轮转规则，装到 `/etc/logrotate.d/` |

这种"一个服务一个子目录"的组织方式，可以让同一个代码仓库产出多个独立的 RPM 包。

----

# rpmbuild 的工作目录

> `rpmbuild` 命令在执行时，需要一组固定名称的子目录。

```text
packaging/
├── BUILD/       # 源码解压和编译的工作目录
├── BUILDROOT/   # 模拟安装的虚拟根目录
├── SRPMS/       # 生成的源码 RPM（.src.rpm）
├── SPECS/       # .spec 文件（本项目直接放在子目录里）
├── SOURCES/     # 源码归档（.tar.gz）
└── x86_64/      # 最终生成的二进制 RPM（目录名取决于架构）
```

这些目录不需要你手动全部创建。在本项目的 `Makefile` 中，打包前会自动创建必要的目录：

```makefile
package: tar
	@mkdir -p {BUILD,BUILDROOT,SRPMS,SPECS}
	@rpmbuild --define "_topdir ${PWD}" --define "_sourcedir ${PWD}" \
		--define "_rpmdir ${PWD}" -ba $(PKG_PATH)/$(PKG_NAME).spec
```

这里有两个关键参数：

- `_topdir`：告诉 rpmbuild 以当前目录为工作根目录，而不是默认的 `~/rpmbuild`
- `_sourcedir`：告诉 rpmbuild 到当前目录找 `.tar.gz` 源码包

`-ba` 表示同时构建二进制包和源码包。

----

# .spec 文件：逐段拆解

> `.spec` 文件是 RPM 打包的灵魂。它用一种特定的格式，描述了"这个包是什么、怎么编译、怎么安装、装了什么、安装前后要做什么"。

下面按照 `.spec` 文件从上到下的顺序，逐段拆解。

## 宏定义区

```ini
%define __os_install_post %{nil}
%define debug_package %{nil}
%define release_id 0
```

`%define` 用来定义 RPM 宏，类似于 C 语言中的 `#define`。

| 宏 | 含义 |
|----|------|
| `%{nil}` | 空值，相当于"什么都不做" |
| `__os_install_post` | 控制 rpmbuild 在 `%install` 之后自动执行的后处理步骤。设为 `%{nil}` 表示跳过，常见原因是避免自动 strip 或自动生成依赖等默认行为干扰打包 |
| `debug_package` | 控制是否生成 debuginfo 子包。设为 `%{nil}` 表示不生成，可以加快打包速度、减小产物体积 |
| `release_id` | 自定义宏，后面在 `Release` 字段中引用 |

> 宏在 `.spec` 文件中使用 `%{宏名}` 的方式引用。rpmbuild 在解析 `.spec` 时会先展开所有宏，再执行具体指令。

## 头部信息区（Preamble）

```ini
Name:       demo-server
Version:    1.2.5
Release:    %{release_id}
Summary:    Demo server
Requires:   base-framework >= 3.4.0-41

AutoReqProv: no

Group:      System Environment/Daemons
License:    Demo
Packager:   zhangsan
Source0:    %{name}-%{version}.tar.gz
BuildRoot:  %{_tmppath}/%{name}-%{version}-root
```

这一段是包的"身份证"，每个字段的含义如下：

| 字段 | 含义 | 示例 |
|------|------|------|
| `Name` | 包名。整个打包流程中最核心的标识 | `demo-server` |
| `Version` | 版本号，通常是 `主.次.修` 格式 | `1.2.5` |
| `Release` | 发布序号。同一个 Version 下的多次构建用这个区分 | `0` |
| `Summary` | 一句话描述 | `Demo server` |
| `Requires` | 安装时的依赖。可以指定版本范围 | `base-framework >= 3.4.0-41` |
| `AutoReqProv` | 是否让 rpmbuild 自动扫描依赖。设为 `no` 表示手动管理 | `no` |
| `Group` | 包所属的分类（新版 RPM 已弱化此字段） | `System Environment/Daemons` |
| `License` | 许可证 | `Demo` |
| `Packager` | 打包者 | `zhangsan` |
| `Source0` | 源码归档文件名 | `demo-server-1.2.5.tar.gz` |
| `BuildRoot` | 虚拟安装根目录的路径 | `/tmp/demo-server-1.2.5-root` |

这里有几个内置宏值得记住：

| 宏 | 展开为 |
|----|--------|
| `%{name}` | `Name` 字段的值 |
| `%{version}` | `Version` 字段的值 |
| `%{_tmppath}` | 临时目录路径，通常是 `/var/tmp` 或 `/tmp` |

所以 `Source0: %{name}-%{version}.tar.gz` 实际上就是 `demo-server-1.2.5.tar.gz`。

### 关于 NVR

RPM 的完整标识由三部分组成，业内称为 **NVR**（Name-Version-Release）：

```text
demo-server-1.2.5-0.x86_64.rpm
    ↑ Name      ↑ Version ↑ Release ↑ 架构
```

这个命名规则非常重要，因为 RPM 系统根据 NVR 来判断：

- 两个包是不是同一个软件
- 安装的版本是否比已有版本更新
- 升级时该替换哪个包

## %description

```ini
%description
Demo server
```

包的详细描述。`rpm -qi 包名` 时会显示这段文字。

## %prep — 准备阶段

```ini
%prep
%setup -q -n %{name}-%{version}
```

`%prep` 是构建流水线的第一步，负责把源码准备好。

`%setup` 是 RPM 内置的一个宏命令，它会自动完成：

1. 切换到 `BUILD/` 目录
2. 删除之前残留的同名目录
3. 解压 `Source0` 指定的 `.tar.gz`
4. 进入解压后的目录

常用参数：

| 参数 | 含义 |
|------|------|
| `-q` | quiet，减少输出 |
| `-n 目录名` | 指定解压后的目录名。默认是 `%{name}-%{version}` |

经过 `%setup` 之后，工作目录变成了 `BUILD/demo-server-1.2.5/`，源代码已经就绪。

## %build — 编译阶段

```ini
%build
echo "build"
cd %{_builddir}/%{name}-%{version}/src
make -C third-party -j 4
make -C libglobal -j 4
make -C librpc -j 4
make -C app/server
```

`%build` 里放的就是你平时编译项目用的命令。

`%{_builddir}` 是 rpmbuild 内置宏，指向 `BUILD/` 目录。所以上面这段实际做的事情是：

1. 进入源码的 `src/` 目录
2. 按顺序编译第三方库、全局库、RPC 库和应用服务
3. `-j 4` 表示 4 个并行编译任务

不同的包根据自身需求，`%build` 的内容差异很大。例如 `demo-manager` 不需要编译（它只是打包已有的二进制和静态资源），所以它的 `%build` 只有一句：

```ini
%build
echo "Needn't build..."
```

而 `demo-plugin` 里编译的是 Go 语言写的程序：

```ini
%build
make -C app/plugin/go-demo-handler
make -C app/plugin/online-status
```

> 核心理解：`%build` 就是告诉 rpmbuild "怎么编译你的代码"。你在命令行怎么编译，这里就怎么写。

## %install — 安装阶段

> 这是最容易被误解的一个阶段。`%install` **不是真的往目标机器上装软件**，而是往一个虚拟根目录里"模拟安装"。

```ini
%install
rm -rf ${RPM_BUILD_ROOT}/*

mkdir -p $RPM_BUILD_ROOT/opt/soft/demo-server/bin
mkdir -p $RPM_BUILD_ROOT/opt/soft/demo-server/sbin
mkdir -p $RPM_BUILD_ROOT/opt/soft/demo-server/conf
mkdir -p $RPM_BUILD_ROOT/opt/soft/demo-server/libs

mkdir -p $RPM_BUILD_ROOT/data/demo-server/var
mkdir -p $RPM_BUILD_ROOT/etc/init.d
mkdir -p $RPM_BUILD_ROOT/etc/cron.d
mkdir -p $RPM_BUILD_ROOT/etc/logrotate.d

cp -f %_topdir/service/demo-server_logrotate $RPM_BUILD_ROOT/etc/logrotate.d/demo-server
cp -fr %{_builddir}/%{name}-%{version}/src/app/server/login/login $RPM_BUILD_ROOT/opt/soft/demo-server/sbin/
```

### BuildRoot 是什么

`$RPM_BUILD_ROOT`（也写作 `%{buildroot}`）就是 `.spec` 头部定义的 `BuildRoot` 路径。

你可以把它想象成一个"假的根文件系统"。在这个假根目录里，你按照最终要安装到真实系统上的路径结构，把文件一个个放进去。

```text
$RPM_BUILD_ROOT/
├── opt/soft/demo-server/
│   ├── bin/          # 运维脚本
│   ├── sbin/         # 二进制程序
│   ├── conf/         # 配置文件
│   └── libs/         # 依赖库
├── data/demo-server/var/  # 运行时数据目录
├── etc/
│   ├── init.d/       # 服务启停脚本
│   ├── cron.d/       # 定时任务
│   └── logrotate.d/  # 日志轮转
```

rpmbuild 最终会把 `$RPM_BUILD_ROOT` 下的所有文件打进 `.rpm`。用户安装时，这些文件会按同样的路径释放到真实系统上。

### 这个阶段要做的事

1. **清理**：先 `rm -rf` 掉旧的 BuildRoot
2. **建目录**：用 `mkdir -p` 创建目标路径结构
3. **拷贝文件**：
   - 从 `%{_builddir}` 拷贝编译产物（二进制、配置文件等）
   - 从 `%_topdir`（即 `packaging/`）拷贝打包资源（init 脚本、cron 配置等）
4. **设权限**：用 `chmod` 设置文件权限

> 关键理解：`%install` 里的路径全部是 `$RPM_BUILD_ROOT/真实路径`。你写 `/opt/soft/xxx`，实际上文件是放在 `$RPM_BUILD_ROOT/opt/soft/xxx` 里，不会真的写到系统的 `/opt/soft/` 下。

## %clean — 清理阶段

```ini
%clean
rm -rf %{buildroot}
```

构建完成后删除虚拟根目录。在现代 rpmbuild 中这一步是可选的，但很多 `.spec` 出于兼容性仍然会写上。

## %files — 文件清单

> `%files` 告诉 rpmbuild：**最终 `.rpm` 包里要包含哪些文件**。

```ini
%files
%defattr(644,root,root,-)
/opt/soft/demo-server/conf/login.conf
/opt/soft/demo-server/conf/connector.conf
/etc/cron.d/demo-server
/etc/logrotate.d/demo-server

%defattr(755,root,root,-)
/opt/soft/demo-server/sbin/login
/opt/soft/demo-server/sbin/connector
/opt/soft/demo-server/bin/*.sh
/etc/init.d/demo-server
```

### %defattr 的含义

```ini
%defattr(文件权限, 所有者, 所属组, 目录权限)
```

| 参数 | 含义 |
|------|------|
| 第一个 | 文件的默认权限 |
| 第二个 | 文件的所有者 |
| 第三个 | 文件的所属组 |
| 第四个 | 目录的默认权限。`-` 表示使用系统默认 |

在上面的例子中：

- 配置文件用 `644`（所有者可读写，其他人只读）
- 可执行文件和脚本用 `755`（所有者可读写执行，其他人可读执行）

### 路径规则

`%files` 里列的路径是**最终安装到系统上的绝对路径**，不带 `$RPM_BUILD_ROOT` 前缀。

可以使用通配符：

```ini
/opt/soft/demo-server/bin/*.sh      # 匹配所有 .sh 文件
/opt/soft/demo-server/bin/*/*.sh    # 匹配子目录下的 .sh 文件
/opt/soft/connector/demo-status/sbin/*   # 匹配所有文件
/opt/soft/connector/demo-status/conf/*.cf
/opt/soft/connector/demo-status/conf/*.json
```

### 重要规则

1. `%install` 里拷贝的文件，必须在 `%files` 里声明，否则 rpmbuild 会报错（unpackaged files）
2. `%files` 里声明的文件，必须在 `$RPM_BUILD_ROOT` 中实际存在
3. 两边必须严格对应

----

# 安装卸载脚本（Scriptlets）

> RPM 最强大的特性之一，是可以在安装和卸载的前后自动执行脚本。

`.spec` 文件支持四个脚本钩子：

| 指令 | 执行时机 | 典型用途 |
|------|----------|----------|
| `%pre` | 安装前 | 创建用户、检查前置条件 |
| `%post` | 安装后 | 创建目录、建立软链接、注册服务、启动服务 |
| `%preun` | 卸载前 | 停止服务 |
| `%postun` | 卸载后 | 清理软链接、移除配置 |

这四个脚本里写的就是普通的 Shell 命令。

## %post — 安装后脚本

这是项目中最常见、逻辑最重的一个钩子。以 `demo-server.spec` 为例：

```bash
%post

# 1. 创建日志目录并建立软链接
mkdir -p /data/demo-server/logs
if [ ! -L /opt/soft/demo-server/logs ]; then
    rm -rf /opt/soft/demo-server/logs
    ln -s /data/demo-server/logs /opt/soft/demo-server/logs
fi

# 2. 创建运行时数据目录并建立软链接
mkdir -p /data/demo-server/var
if [ ! -L /opt/soft/demo-server/var ]; then
    rm -rf /opt/soft/demo-server/var
    ln -s /data/demo-server/var /opt/soft/demo-server/var
fi

# 3. 在 base-framework 的服务配置目录下建立软链接
mkdir -p /opt/soft/base-framework/conf/service

login_conf="/opt/soft/base-framework/conf/service/login.conf"
if [ -f ${login_conf} ]; then
    rm -f ${login_conf}
fi
if [ ! -L ${login_conf} ]; then
    ln -s /opt/soft/demo-server/conf/login.conf ${login_conf}
fi

# 4. 注册开机自启
chkconfig --level 2346 demo-server on
```

这段脚本做了四件事：

1. **日志目录重定向**：把日志目录从安装路径 `/opt/soft/` 软链到数据盘 `/data/`。这是生产环境常见做法——程序装在系统盘，日志写到数据盘
2. **运行时数据目录重定向**：同理，`var` 目录也软链到数据盘
3. **服务配置注册**：把配置文件软链到 `base-framework` 的服务配置目录，让进程管理框架能发现并管理这个服务
4. **开机自启**：通过 `chkconfig` 注册 SysV 服务

> 为什么用软链接而不是直接拷贝？因为软链接保证了只有一份真实文件，升级包时新配置自动生效，不需要额外同步。

## %preun — 卸载前脚本

```bash
%preun
if [ $1 -eq 0 ]; then
    service demo-server stop
fi
```

这里有一个非常重要的细节：`$1` 参数。

RPM 在调用 scriptlet 时会传入一个参数，表示此次操作完成后系统上还剩多少个该包的实例：

| `$1` 的值 | 含义 |
|------------|------|
| `0` | 完全卸载（uninstall） |
| `1` | 升级时卸载旧版本（update） |
| `2+` | 其他情况 |

所以 `if [ $1 -eq 0 ]` 的意思是：**只有在完全卸载时才停止服务**。如果是升级（`$1 -eq 1`），就不停服务，避免升级过程中出现服务中断。

这个判断模式几乎是所有 RPM 包的标准写法。

## %postun — 卸载后脚本

```bash
%postun
function remove_link() {
    if [ -f $1 -o -L $1 ]; then
        rm -f $1
    fi
}

remove_link /opt/soft/base-framework/conf/service/login.conf
remove_link /opt/soft/base-framework/conf/service/connector.conf
remove_link /opt/soft/base-framework/conf/service/dispatch.conf
```

卸载后清理 `%post` 中创建的软链接。

这里定义了一个 `remove_link` 函数来统一处理，逻辑是：如果目标是一个普通文件（`-f`）或者符号链接（`-L`），就删掉它。

## 四个脚本的执行顺序

在不同场景下，这四个脚本的执行顺序不同：

### 全新安装

```text
%pre  (新包)    $1=1
%install         (文件释放)
%post (新包)    $1=1
```

### 升级安装

```text
%pre    (新包)    $1=2
%install           (新包文件释放)
%post   (新包)    $1=2
%preun  (旧包)    $1=1
         (旧包文件删除)
%postun (旧包)    $1=1
```

### 完全卸载

```text
%preun  (当前包)  $1=0
         (文件删除)
%postun (当前包)  $1=0
```

理解这个顺序，才能正确处理升级场景下的服务不中断问题。

----

# %changelog — 变更日志

```ini
%changelog
* Wed Aug 20 2025  zhangsan zhangsan@example.com 1.2.5-0
- release 1.2.5-0  Add more features in connector.

* Wed May 14 2025  zhangsan zhangsan@example.com 1.2.4-0
- release 1.2.4-0  Fixed crash in connector.

* Mon May 12 2025  zhangsan zhangsan@example.com 1.2.3-0
- release 1.2.3-0  Optimize connector memory.
```

`%changelog` 记录每次发版的变更内容，格式是固定的：

```text
* 星期 月 日 年  姓名 邮箱 版本-发布号
- 变更说明
```

这个日志的价值在于：

- `rpm -q --changelog 包名` 可以直接查看变更历史
- 不需要去翻 git log，安装了包就能看到它的演进过程
- 是正式的发布记录，而不只是开发日志

> 每次升级版本或发布新包时，应该在 `%changelog` 最上方添加新条目。条目按时间倒序排列。

----

# RPM 宏速查

> 宏是 RPM 打包中无处不在的概念。下面是 `.spec` 文件中最常见的内置宏。

| 宏 | 含义 | 典型展开值 |
|----|------|------------|
| `%{name}` | 包名 | `demo-server` |
| `%{version}` | 版本号 | `1.2.5` |
| `%{release}` | 发布号 | `0` |
| `%{_builddir}` | 编译工作目录 | `<_topdir>/BUILD` |
| `%{_topdir}` | rpmbuild 工作根目录 | 默认 `~/rpmbuild`，本项目通过 `--define` 改为 `packaging/` |
| `%{buildroot}` | 虚拟安装根目录 | 等同于 `$RPM_BUILD_ROOT` |
| `%{_tmppath}` | 临时文件目录 | `/var/tmp` 或 `/tmp` |
| `%{nil}` | 空值 | （空） |

你也可以用 `rpm --eval '%{_topdir}'` 来查看任意宏的展开结果。

自定义宏通过 `%define` 定义，通过命令行 `--define` 覆盖：

```bash
# .spec 里定义
%define release_id 0

# 命令行覆盖
rpmbuild --define "release_id 5" ...
```

----

# 打包辅助脚本

> 除了 `.spec` 文件，打包流程还依赖两个辅助脚本：`Makefile` 和 `control`。

## Makefile

`Makefile` 是打包的入口。执行 `make PKG_NAME=demo-server` 就能完成从归档到出包的全过程。

```makefile
PKG_PATH := service
PKG_NAME := demo-server

ifeq ($(PKG_NAME), demo-login)
    PKG_PATH := login
endif
# ... 其他包名到子目录的映射 ...

PKG_VERSION = $(shell grep '^Version:' $(PKG_PATH)/$(PKG_NAME).spec | awk '{print $$2}')
```

这段代码做了两件事：

1. **根据 `PKG_NAME` 确定子目录路径** — 比如 `demo-login` 对应 `login/` 目录
2. **从 `.spec` 文件中提取版本号** — 用 `grep` 和 `awk` 解析 `Version:` 行，保证 Makefile 和 `.spec` 的版本始终一致

核心 target 是 `package`：

```makefile
package: tar
	@mkdir -p {BUILD,BUILDROOT,SRPMS,SPECS}
	@rpmbuild --define "_topdir ${PWD}" --define "_sourcedir ${PWD}" \
		--define "_rpmdir ${PWD}" -ba $(PKG_PATH)/$(PKG_NAME).spec
```

它先执行 `tar` target（调用 `control` 脚本归档源码），再调用 `rpmbuild` 构建 RPM。

## control 脚本

`control` 是一个 Bash 脚本，核心功能是把 Git 仓库的源码打成 `.tar.gz`。

```bash
function git_archive_all() {
    local PROJECT_NAME=$1

    # 归档主项目
    git archive --format=tar --prefix=$PROJECT_NAME/ HEAD | (tar xpf -)

    # 归档所有 git submodule
    git submodule foreach | while read subdir; do
        subdir=${subdir#*\'}
        subdir=${subdir%*\'}
        cd "${subdir}"
        commit=$(git rev-parse HEAD)
        git archive --format=tar --prefix="$subdir/" "$commit" | \
            (cd "$curDir/$PROJECT_NAME/" && tar xpf -)
        cd "$topdir"
    done

    # 打包成 .tar.gz
    tar zcpf ${PROJECT_NAME}.tar.gz ${PROJECT_NAME}
}
```

这个函数做的事情：

1. 用 `git archive` 把主项目导出（不含 `.git` 目录和 gitignore 的文件）
2. 遍历所有 git submodule，逐个归档并合并到主目录中
3. 把合并后的目录打成 `名称-版本.tar.gz`
4. 顺便把 submodule 的最后 commit ID 写入版本文件，方便追溯

> 为什么用 `git archive` 而不是直接 `tar` 整个目录？因为 `git archive` 只导出版本控制中的文件，自动排除了 `.git` 目录、编译产物、临时文件等。这保证了源码归档是"干净"的。

----

# 附属配置文件

> 一个生产级的 RPM 包，通常不只是装一个二进制程序，还要配套服务管理、日志轮转和定时任务。

## SysV init 脚本

文件名形如 `demo-server_init_d`，安装到 `/etc/init.d/demo-server`。

```bash
SYSTEMCTL_SKIP_REDIRECT=1
. /etc/rc.d/init.d/functions

proc=demo-server
RETVAL=0

start() {
    ulimit -n 2000000
    /opt/soft/demo-server/bin/start.sh
    RETVAL=$?
    return 0
}

stop() {
    /opt/soft/demo-server/bin/stop.sh
    RETVAL=$?
    return 0
}

restart() {
    /opt/soft/demo-server/bin/restart.sh
    RETVAL=$?
    return 0
}

case "$1" in
    start)  start   ;;
    stop)   stop    ;;
    restart) restart ;;
    *)
        echo $"Usage: %prog {start|stop|restart|reload|status}"
        RETVAL=2
esac

exit $RETVAL
```

这是标准的 SysV init 脚本格式，让你可以用 `service demo-server start/stop/restart` 来管理服务。

几个关键点：

- `. /etc/rc.d/init.d/functions`：引入 Red Hat 系统的 init 函数库
- `ulimit -n 2000000`：在启动前设置文件描述符上限，这对高并发网络服务非常重要
- 实际的启停逻辑委托给 `bin/start.sh`、`bin/stop.sh` 等脚本

安装到 `/etc/init.d/` 后，配合 `chkconfig` 就可以实现开机自启。

## cron 定时任务

```cron
0 0 * * * root /usr/sbin/logrotate -f /etc/logrotate.d/demo-server
```

每天凌晨 0 点强制执行一次日志轮转。安装到 `/etc/cron.d/`。

cron 表达式格式：

```text
分 时 日 月 周 用户 命令
0  0  *  *  *  root /usr/sbin/logrotate -f ...
```

## logrotate 日志轮转

```conf
/opt/soft/demo-server/logs/*log
{
    daily
    minsize 100M
    rotate 10
    dateext
    noolddir
    compress
    missingok
    delaycompress
    copytruncate
    su root root
}
```

| 指令 | 含义 |
|------|------|
| `daily` | 每天检查一次是否需要轮转 |
| `minsize 100M` | 只有日志大于 100MB 时才轮转 |
| `rotate 10` | 最多保留 10 个历史文件 |
| `dateext` | 历史文件用日期后缀（如 `.log-20250820`）而不是数字后缀 |
| `compress` | 历史文件用 gzip 压缩 |
| `delaycompress` | 延迟一个周期再压缩（最近一个不压缩，方便排查问题） |
| `copytruncate` | 先拷贝再清空，而不是移动后创建新文件。这对不支持重新打开日志文件的程序非常关键 |
| `missingok` | 日志文件不存在也不报错 |
| `su root root` | 以 root 身份执行轮转 |

安装到 `/etc/logrotate.d/`。

----

# 完整打包实战

> 现在把所有知识串起来，走一遍完整的打包流程。

## 第一步：确定要打哪个包

```bash
cd packaging/
make PKG_NAME=demo-server
```

或者：

```bash
make PKG_NAME=demo-login
make PKG_NAME=demo-plugin
make PKG_NAME=demo-manager
make PKG_NAME=demo-mqtt
make PKG_NAME=demo-status
```

## 第二步：Makefile 解析包名

Makefile 根据 `PKG_NAME` 确定子目录路径和版本号，然后依次执行 `tar` 和 `package` 两个 target。

## 第三步：control 脚本归档源码

`control` 脚本通过 `git archive` 导出源码，连同 submodule 一起打包成 `demo-server-1.2.5.tar.gz`，放到 `packaging/` 目录下。

## 第四步：rpmbuild 开始工作

rpmbuild 读取 `.spec` 文件，按顺序执行：

```text
%prep    → 解压 tar.gz 到 BUILD/demo-server-1.2.5/
%build   → 在源码目录里 make 编译
%install → 把产物拷贝到 BUILDROOT 的虚拟根目录
%files   → 根据文件清单打包成 .rpm
```

## 第五步：产出 .rpm

最终在 `packaging/x86_64/`（或对应架构）目录下生成 `demo-server-1.2.5-0.x86_64.rpm`。

## 第六步：部署安装

拿到 `.rpm` 后，到目标机器上：

```bash
# 全新安装
rpm -ivh demo-server-1.2.5-0.x86_64.rpm

# 升级安装
rpm -Uvh demo-server-1.2.5-0.x86_64.rpm
```

安装时 RPM 会自动执行 `%post` 中的脚本（创建目录、建软链接、注册服务）。

----

# 版本管理策略

> 理解版本号的含义和管理规范，是保证可靠发布的基础。

## Version 与 Release 的关系

```text
Version: 1.2.5
Release: 0
```

- `Version`：代表功能版本。有新功能、重大改动时升级
- `Release`：同一功能版本下的构建序号。修 bug、改配置、调参数时递增

实际发版时：

| 场景 | 操作 |
|------|------|
| 新增功能 | 升 Version，Reset 为 0 |
| 修复 bug | 升 Release |
| 改配置重新打包 | 升 Release |

## 发版流程

1. 修改 `.spec` 中的 `Version` 或 `Release`
2. 在 `%changelog` 最上方添加新条目
3. 提交代码
4. 执行 `make PKG_NAME=xxx` 打包
5. 分发 `.rpm` 到目标机器

----

# Requires 依赖管理

```ini
Requires: base-framework >= 3.4.0-41
```

`Requires` 声明了安装本包时，系统上必须已经安装的其他包。

- `>=` 表示最低版本要求
- 也可以用 `=`（精确版本）、`<=`（最高版本）等

```ini
Requires: base-framework >= 3.5.5-1 base-tools >= 4.0.0-19
```

多个依赖用空格分隔。如果依赖不满足，`rpm -ivh` 会拒绝安装并给出提示。

```ini
AutoReqProv: no
```

这行告诉 rpmbuild 不要自动扫描二进制文件的共享库依赖。默认情况下 rpmbuild 会分析 ELF 文件并自动添加 `Requires`，但对于自带依赖库的项目，这个自动行为反而会引入不需要的系统依赖，所以手动关掉。

----

# 注意事项

### 1. %install 中的路径一定要带 $RPM_BUILD_ROOT 前缀

这是最常见的新手错误。如果你直接写 `cp xxx /opt/soft/...`，那就是往**构建机**的真实系统目录里写文件了。正确做法是始终通过 `$RPM_BUILD_ROOT` 中转。

### 2. %files 中列的文件必须和 %install 中安装的严格对应

多了会报 `unpackaged files`，少了会导致文件没有进入 RPM，安装后找不到。

### 3. 注意 scriptlet 中 $1 参数的判断

不做 `$1` 判断的话，升级时 `%preun` 会停掉服务、`%postun` 会清理配置，导致升级过程中服务中断甚至配置丢失。

### 4. 文件权限要在 %files 中显式声明

不要依赖文件在构建机上碰巧拥有的权限。用 `%defattr` 统一设置默认值，必要时对单个文件用 `%attr(mode,user,group)` 覆盖。

### 5. Source 归档的目录结构要和 %setup 预期一致

`%setup -q -n %{name}-%{version}` 期望 tar 包解压后的顶层目录是 `demo-server-1.2.5`。如果归档时目录名不对，`%prep` 阶段就会失败。

### 6. 宏展开可以用 rpm --eval 调试

```bash
rpm --eval '%{_topdir}'
rpm --eval '%{_builddir}'
rpm --eval '%{name}'    # 这个只在 rpmbuild 上下文中有效
```

遇到不确定宏展开成什么的时候，随时用这个命令确认。

### 7. 多包共仓库时注意隔离

本项目的 `packaging/` 下同时维护 6 个包。它们共享同一个源码归档和 Makefile，但各自有独立的 `.spec` 和附属文件。打包时通过 `PKG_NAME` 参数选择要构建的包。

这种模式的好处是集中管理，代价是每次打任何包都要归档整个仓库的源码。如果仓库很大，可以考虑拆分。

----

# 常用命令速查

```bash
# 打包
make PKG_NAME=demo-server              # 构建指定包

# 安装/升级/卸载
rpm -ivh package.rpm                       # 全新安装
rpm -Uvh package.rpm                       # 升级安装（没有旧版本时等同于 -ivh）
rpm -e package-name                        # 卸载

# 查询
rpm -qa | grep demo                     # 查找已安装的包
rpm -qi package-name                       # 查看包信息
rpm -ql package-name                       # 查看包安装了哪些文件
rpm -qf /path/to/file                     # 查看某个文件属于哪个包
rpm -q --changelog package-name            # 查看变更日志
rpm -qp --scripts package.rpm             # 查看包中的脚本

# 调试
rpm --eval '%{_topdir}'                    # 查看宏展开值
rpmbuild --nobuild spec-file.spec          # 只解析 .spec 不实际构建
```

----

# 小结

RPM 打包的核心，可以浓缩成一句话：

**用 `.spec` 文件告诉 rpmbuild 怎么编译、怎么安装、装了什么、安装前后做什么。**

如果你只先记住最关键的几件事，可以抓住这些：

- `.spec` 文件是打包的核心控制文件，由头部信息、`%prep`、`%build`、`%install`、`%files`、scriptlets、`%changelog` 组成
- `%install` 是往虚拟根目录里模拟安装，不是真的装到系统上
- `%files` 必须和 `%install` 严格对应
- scriptlets（`%post`、`%preun` 等）让你可以在安装卸载时自动执行运维操作
- `$1` 参数区分"完全卸载"和"升级时卸载旧版"，这个判断对服务连续性至关重要
- 一个完整的生产包通常还包含 init 脚本、cron 任务和 logrotate 配置

当你能用这些概念去对照 `packaging/` 目录里的每一行代码时，RPM 打包对你来说就不再是黑箱了。
