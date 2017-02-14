@echo off
for /l %%y in (0,10,100) do (
  for /l %%z in (0,10,100) do (
    "..\x64\Release\Traffic_v1.exe" 0 %%y %%z
    echo 0 %%y %%z
  )
)
pause