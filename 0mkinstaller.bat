pushd documentation
del kvant.pdf
@if %errorlevel% neq 0 goto :err
make FILE=kvant
@if %errorlevel% neq 0 goto :err
popd
qmake -makefile
@if %errorlevel% neq 0 goto :err
nmake release
@if %errorlevel% neq 0 goto :err
makensis kvant.nsi
@if %errorlevel% neq 0 goto :err
@echo "--------- Everything seems ok ----------"
exit /b 0

:err
@echo "========== ERROR OCCURED ==============="
exit /b 1
