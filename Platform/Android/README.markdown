## Building for Android

This library is all set up to compile as a shared library for Android. At present it targets android-9 and uses gcc v4.7. There is no real JNI layer at present, and it has only been built on OS X. I assume that a working NDK on any platform will have success, though.

Please refer to the file HACKING at the root of the project for low-level details you'll need to write portable code in this project.

### Prerequisites

* Android NDK release 8e
* Correct some bugs in NDK 8e by following this tutorial:
    http://developer.appcelerator.com/blog/2013/03/correcting-a-bug-in-the-latest-google-ndk-r8e.html
* Have "zsh" command shell installed (MacOSX should have it by default)
* Clone this branch from the readium-sdk repository. All these instructions apply to this branch only.
* A lot of patience

### To build and debug in Eclipse

* Android Eclipse ADT SDK
* Android Eclipse NDK Plugin by following this tutorial:
    http://tools.android.com/recent/usingthendkplugin
* At least run once the `ndk-compile.sh` command-line compilation (see Compilation below) to generate the includes
* Import the "ePub3-Library" eclipse project from Platform/Android subdirectory:
    File -> Import... -> Existing Projects into Workspace -> select the subdirectory and the project
* Configure the build command (as described in the tutorial above) to the following:
    `ndk-build -C . V=1 NDK_APPLICATION_MK=Application.mk NDK_PROJECT_PATH="${workspace_loc:/ePub3-Library}" APP_BUILD_SCRIPT=Android.mk NDK_DEBUG=1`
* Build the ePub3-Library project

### Compilation

ndk-compile.sh:
```bash
usage: ndk-compile.sh <command> [<path-to-ndk>]
       Available commands are:
           help           Shows this help message.
           build          Builds this native project.
           build-debug    Builds this native project with debug enabled.
           clean          Cleans this project build objects.

       <path-to-ndk>  The path to the Android NDK to use.

       The path to NDK can be set by a global environment variable ANDROID_NDK,
       to avoid being passed by argument to this script.
```

1. From a Terminal, go into the Platform/Android subdirectory of the readium-sdk repository (this directory).
2. Run `ndk-compile.sh build [<path-to-ndk>]` to build. My invocation looks like this, for example:

```bash
ndk-compile.sh build
```

Everything else should be set up appropriately by that script.

### Maven Package

Once the `ndk-compile.sh` command above has been executed, the native library
and Java code can be packaged into standard Maven packages and deployed to
repositories to be referenced as dependencies from Android applications.
Assuming that the Android artifacts have been deployed to a local repository
(with the [maven-android-sdk-deployer](https://github.com/simpligility/maven-android-sdk-deployer),
for example), packages can be produced with the following command:

```bash
$ mvn clean package
```

The created artifacts can then be deployed to any repository (with `mvn deploy`)
and then referenced from any Android application with the following dependency
declaration (in Maven syntax, but applicable to any other build system that
uses Maven packages):

```
<dependency>
  <groupId>org.readium</groupId>
  <artifactId>readium-sdk-android-runtime</artifactId>
  <version>0.20.0</version>
</dependency>
<dependency>
  <groupId>org.readium</groupId>
  <artifactId>libepub3</artifactId>
  <version>0.20.0</version>
  <classifier>armeabi-v7a</classifier>
  <type>so</type>
</dependency>
```

### Linux Notes (Ubuntu 13.04 64bit):


everything as above for OSX, but

* had problems with lower/upcase for libzip's Config.h -> make it lowercase for
  Platform/Android/include/libzip/config.h
  ePub3/ThirdParty/libzip/config.h
* final linker step failed using the provided libboost_regex.a -> replace with
  https://github.com/mcxiaoke/boost-ndk/raw/master/boost/android/lib/libboost_regex.a
