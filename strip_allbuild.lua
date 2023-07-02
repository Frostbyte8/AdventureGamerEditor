function file_exists(fileName)

    local file=io.open(fileName, "r")

    if file~=nil then 

        file:close()
        return true

    else

        return false

    end

end

-- Obtain project name

projectName = ""

if(file_exists("CMakelists.txt")) then

    for line in io.lines("Cmakelists.txt") do
        
        if string.find(line, "project%(") ~= nil then
            sPos = string.find(line, "%(") + 1
            ePos = string.find(line, "%)") - 1
            projectName = string.sub(line, sPos, ePos)
            break  
        end
        

    end

else

   print("Cannot find CMakelists.txt")
   return

end

slnFile = "out/" .. projectName .. ".sln"
io.popen("lua strip_sub.lua " .. slnFile)