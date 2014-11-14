# Note for building Windows 8

- Use VS 2013 Pro.

- Open the VS solution file in `readium-sdk\Platform\Windows\ReadiumSDK`. The project will need to be updated to compile under VS2013.

- Getting things to compile is largely a matter of commenting out certain macros and replacing them with correct function calls.
- Other issues are things like Windows API functions being called incorrectly.
- Once the code compiles there are a TON of linker issues. This is resolved but adding the correct source code files to the project (a lot of require .cpp/.h/etc files are on the file system but not included in the project).
- The Windows Launcher app will not build the included ePub3 project (it doesn't generate a .lib nor .dll). I'm getting around this for now by building the SDK using the plain ole SDK project and manually copying the lib and dll for the launcher.
- Launcher will still not run as certain bits of the SDK are not properly exported. The way around this is explained in the HACKING document found in the root of the SDK repo. Basically I just added "EPUB3_EXPORT" to everything that the launcher project complained about.

- IMPORTANT: commented out L414-426 in CPP2JS.cpp in the launcher project as GetSkippablesCount() and GetEscapablesCount() were blowing up the linker.

## Some Potentially Useful Links

 - [DLLs in Visual C++](http://msdn.microsoft.com/en-us/library/1ez7dh12.aspx)
 - [Porting Legacy Windows Apps To WinRT](http://msdn.microsoft.com/en-us/magazine/jj651574.aspx)

## Using DLLs in Windows Store Apps

 - http://stackoverflow.com/questions/18767105/using-a-dll-in-a-windows-store-application
 - http://www.kraigbrockschmidt.com/2013/08/05/dlls-store-apps-winrt-components/
 - [Whitelisted Win32 and COM APIs for Windows Store Apps](http://msdn.microsoft.com/en-us/library/windows/apps/br205757.aspx)
 - [Consuming DLLs in WinRT Apps](http://blog.nuclex-games.com/2012/03/how-to-consume-dlls-in-a-winrt-metro-project/)
 - [Using the Windows App Certification Kit](http://msdn.microsoft.com/en-us/library/windows/apps/hh694081.aspx) (to verify WinStore apps are a-okay)