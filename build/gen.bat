@echo off
del release\*.exe 
del release\*.obj 
for %%x in (60,90,120,150) do (
    for %%y in (0,1,2) do (
        nmake /f %%x_%%y.Release clean
        nmake /f %%x_%%y.Release
	del release\*.obj
    )
)
