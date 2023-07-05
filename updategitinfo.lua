-- Start by getting our Git Info

local result = io.popen("git rev-parse HEAD", "r")
local gitInfo = result:read("*all")
result:close()
gitInfo = string.sub(gitInfo, 1, 10)

-- Now Generate the File

localFilePath = string.gsub(arg[0], "updategitinfo.lua", "")
localFilePath = localFilePath .. ".\\src\\gitinfo.h"
local fileOut, err = io.open(localFilePath, "w")

if fileOut==nil then
    print(err)
end

fileOut:write("#ifndef __GITINFO_H__\n")
fileOut:write("#define __GITINFO_H__\n\n")

fileOut:write("#include <string>\n\n")

fileOut:write('const std::string GIT_VERSION_INFO = "' .. gitInfo .. '"\n\n')

fileOut:write("#endif // __GITINFO_H__")
fileOut:close()
