@echo off
echo.
echo Preparing symbolic links for WebRTC...
echo.

set failure=0

call:dolink . build ..\webrtc-deps\webrtc-build
if "%failure%" neq "0" goto:done_with_error
call:dolink third_party\yasm\source patched-yasm ..\..\..\..\webrtc-deps\patched-yasm
if "%failure%" neq "0" goto:done_with_error
call:dolink third_party\opus src ..\..\..\webrtc-deps\opus
if "%failure%" neq "0" goto:done_with_error
call:dolink third_party libsrtp ..\..\webrtc-deps\libsrtp
if "%failure%" neq "0" goto:done_with_error
call:dolink third_party libvpx ..\..\webrtc-deps\libvpx
if "%failure%" neq "0" goto:done_with_error
call:dolink third_party libyuv ..\..\webrtc-deps\libyuv
if "%failure%" neq "0" goto:done_with_error
call:dolink third_party\tools gyp ..\..\..\webrtc-deps\webrtc-gyp
if "%failure%" neq "0" goto:done_with_error

goto:done


:dolink
if NOT EXIST %~1\nul call:failure -1 "%~1 does not exist!"
if "%failure%" neq "0" goto:eof

pushd %~1

IF EXIST .\%~2\nul goto:alreadyexists

IF NOT EXIST %~3\nul call:failure -1 "%~3 does not exist!"
if "%failure%" neq "0" popd
if "%failure%" neq "0" goto:eof

echo In path "%~1" creating symbolic link for "%~2" to "%~3"
mklink /J %~2 %~3
if %errorlevel% neq 0 call:failure %errorlevel% "Could not create symbolic link to %~2 from %~3"
popd
if "%failure%" neq "0" goto:eof

goto:eof

:alreadyexists
popd
goto:eof

:failure
echo.
cd
echo ERROR: %~2
echo.

set failure=%~1

goto:eof

:done_with_error

exit /b %failure%
goto:eof

:done

echo.
echo Success: WebRTC ready.
echo.
