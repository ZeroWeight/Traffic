@echo off
cd "C:\Users\Zero Weight\Documents\Traffic\Traffic_v1\build"
del release\*.exe 
del release\*.obj 
for %%x in (90,60,120,150) do (
    for %%y in (0,1,2) do (
        nmake /f %%x_%%y.Release clean
        nmake /f %%x_%%y.Release
	del release\*.obj
    )
    nmake /f %%x_C1.Release
    del release\*.obj
    nmake /f %%x_C2.Release
    del release\*.obj
)
