@echo off

set bat_dir=%~dp0
echo %bat_dir%

C:
chdir E:\0_luyinbi\
start E:\cygwin64\bin\mintty.exe -e /bin/bash --login -i -c "cd $bat_dir; ./build.sh sysconfig; source ./build/envsetup.sh; exec bash -rcfile ~/.bashrc;"