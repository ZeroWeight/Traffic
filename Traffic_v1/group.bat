@echo off
for /l %%x in (0,10,100) do (
  for /l %%y in (0,10,100) do (
    for /l %%z in (0,10,100) do (
      "..\x64\Release\Traffic_v1.exe" %%x %%y %%z
      echo %%x %%y %%z
    )
  )
)
pause