
local function endsWith(s, suffix)
  return #s >= #suffix and s:find(suffix, #s - #suffix + 1, true)
end

local function splitPathName(path)
  local nameStart, _, name = path:find('([^/\\]+)[/\\]?$')
  local dir = path:sub(0, nameStart - 2)
  return dir, name
end                                  

local function splitExt(path)
  local extStart, _, ext = path:find('[^/\\.][.]([^/\\.]*)$')
  if extStart ~= nil then
    local name = path:sub(0, extStart)
    return name, ext
  else
    return path, nil
  end
end

local function unpackLibPlatformInfo( libsRootPath, libPlatformInfo )
  local projectPath, includePath, libPath, libName = unpack( libPlatformInfo )

  if projectPath ~= nil and not path.isabsolute( projectPath ) then 
    projectPath = libsRootPath .. '/' .. projectPath 
  end
  
  if includePath ~= nil and not path.isabsolute( includePath ) then 
    includePath = libsRootPath .. '/' .. includePath 
  end
  
  if libPath ~= nil and not path.isabsolute( libPath ) then 
    libPath = libsRootPath .. '/' .. libPath 
  end

  return projectPath, includePath, libPath, libName
end

local function addDependencyPathsToCurrentSolution(libraries, libsRootPath, platform)
  for libName, libDef in pairs(libraries) do
    local libPlatformInfo = libDef[platform]

    if libPlatformInfo ~= nil then
      local projectPath, includePath, libPath, libName = unpackLibPlatformInfo( libsRootPath, libPlatformInfo )
      
      if includePath ~= nil then
        -- premake calls
        includedirs {
          includePath
        }
      end

      if libPath ~= nil then 
        -- premake calls
        libdirs {
          libPath
        }
      end
    end
  end
end

local function addProjectsToCurrentSolution(libraries, libsRootPath, platform)
  for libName, libDef in pairs(libraries) do
    local libPlatformInfo = libDef[platform]

    if libPlatformInfo ~= nil then
      local projectPath, includePath, libPath, libName = unpackLibPlatformInfo(libsRootPath, libPlatformInfo)

      if projectPath ~= nil then
        local fileDir, fileName      = splitPathName(projectPath)
        local fileBaseName, fileExt  = splitExt(fileName)
        
        -- local uuidValue = getUuid(projectPath)
        -- assert(uuidValue ~= nil, 'Unable to get UUID from project: ' .. projectPath)
                
        -- premake calls
        external(fileBaseName)
          location(fileDir)
          -- uuid(uuidValue)
          kind('StaticLib')
          language('C++')

        print('Added external lib: ' .. libName .. ' at ' .. projectPath)
      end
    end
  end
end

local function addLibrariesToCurrentProject( libraries, libsRootPath, platform )
  for libName, libDef in pairs( libraries ) do
    local projectPath, includePath, libPath, libName = unpackLibPlatformInfo( libsRootPath, libDef )

    if projectPath ~= nil then
      local fileDir, fileName     = splitPathName( projectPath )
      local fileBaseName, fileExt = splitExt( fileName )
      links { fileBaseName } 
    else
      if includePath ~= nil then includedirs { includePath } end
      if libPath ~= nil then     libdirs { libPath } end
      if libPath ~= nil then     links { libName } end
    end
  end
end


-- local function setResourcesDir(path)
--   local items = {}

--   -- adding both files and directories
--   local f = io.popen("ls -1 " .. path)
--   for line in f:lines() do
--     if line.sub(1, 1) ~= '.' then
--       table.insert(items, path .. '/' .. line)
--     end
--   end

--   -- premake call
--   files(items)
--   resources(items)
-- end



return {
  merge                                       = merge,
  addDependencyPathsToCurrentSolution         = addDependencyPathsToCurrentSolution,
  addProjectsToCurrentSolution                = addProjectsToCurrentSolution,
  addLibrariesToCurrentProject                = addLibrariesToCurrentProject,
  setResourcesDir                             = setResourcesDir,
}