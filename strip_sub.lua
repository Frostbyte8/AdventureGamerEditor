function file_exists(fileName)

    local file=io.open(fileName, "r")

    if file~=nil then 

        file:close()
        return true

    else

        return false

    end

end

-- Actual Code

function attemptFix()

    fileName = arg[1]

    --[[ for i = 1, 5 do

        if file_exists(fileName) then
            break
        else
            sleep(3)
        end

    end--]]

    if file_exists(fileName) == false then
        return
    end

    local lines = {}

    for line in io.lines(fileName) do
        table.insert(lines, line)
    end

    slnFile = io.open(fileName, "w")

    for i, l in ipairs(lines) do
        if string.find(l, "ALL_BUILD") == nil then
            slnFile:write(l .. "\n")
        end
    end

    slnFile:close()

end

--if file_exists(".cmake_config_fix.lock") then
    --print("Script is locked (Probably already running)")
    --return
--end

--local FILE_LOCK_NAME = '.cmake_config_fix.lock'
--local lockFile = io.open(FILE_LOCK_NAME, "w")

--lockFile:write(" ")
--lockFile:close()

attemptFix()
--os.remove(FILE_LOCK_NAME)