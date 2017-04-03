local defaultAbi = 'armeabi-v7a'

function addXCodeInfoFile(projectLocation)
  local templateFileName = _WORKING_DIR .. "/template.plist"
  local destinationFile  = projectLocation .. "/Info.plist"

  print( templateFileName )
  print( destinationFile )

  if( false == os.isdir(projectLocation) ) then
    local created = os.mkdir( projectLocation )
  end 
    
  -- pathToTemplateFile
  os.copyfile( templateFileName, destinationFile )
end

function addXCodeBuildSettings()
  xcodebuildsettings {
    'INFOPLIST_FILE             = "../Info.plist"',
    'CODE_SIGN_IDENTITY         = "iPhone Developer: Jared Watt (EW32GEM29U)"',
    -- 'PROVISIONING_PROFILE       = "42785B49-0ADA-4A41-A4FA-90BA54B50693"',
    'SDKROOT                    = iphoneos',
    'ARCHS                      = "armv7"',
    'TARGETED_DEVICE_FAMILY     = "1,2"',
    'VALID_ARCHS                = "armv7"',
    'IPHONEOS_DEPLOYMENT_TARGET = "8.1"',
    'FRAMEWORK_SEARCH_PATHS     = "$(SRCROOT)/../../Dependencies/boost_1_55_0/ios/framework"',
    --'ASSETCATALOG_COMPILER_APPICON_NAME     = "AppIcon"',
    --'ASSETCATALOG_COMPILER_LAUNCHIMAGE_NAME = "LaunchImage"',
    'PRODUCT_NAME               = "Sample"'
  }  
end

function addFrameworksToCurrentSolutionConfiguration(strongFrameworks, weakFrameworks)
  local linkOptionsString = ''
  for i, name in ipairs(strongFrameworks) do
    linkOptionsString = linkOptionsString .. " -framework " .. name 
  end
  for i, name in ipairs(weakFrameworks) do
    linkOptionsString = linkOptionsString .. " -weak_framework " .. name 
  end

  -- premake call
  linkoptions { linkOptionsString }
end


return {
  addXCodeInfoFile                            = addXCodeInfoFile,
  addXCodeBuildSettings                       = addXCodeBuildSettings,
  addFrameworksToCurrentSolutionConfiguration = addFrameworksToCurrentSolutionConfiguration,
}