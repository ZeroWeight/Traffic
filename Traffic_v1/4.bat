@echo off
for /l %%y in (0,10,100) do (
  for /l %%z in (0,10,100) do (
    "..\x64\Release\Traffic_v1.exe" 40 %%y %%z
    echo 40 %%y %%z
  )
)
pause