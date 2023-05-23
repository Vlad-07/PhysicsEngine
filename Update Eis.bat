@echo OFF
pushd %~dp0\
git submodule update --remote
popd
IF %ERRORLEVEL% NEQ 0 (
  PAUSE
)