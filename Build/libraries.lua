-- Format: { projectPath, includePath, libPath, libname } 

return {
  cppzmq = {       
    nil,                                                                 
    'C:/Program Files/ZeroMQ/include;C:/Users/Jared Watt/Documents/Develop/cppzmq',                    
    'C:/Program Files/ZeroMQ/lib',                  
    'libzmq-v141-mt-4_2_3.lib' 
  },

  Nebulous_Common = {       
    nil,                                                                 
    'Nebulous',                    
    'Nebulous/lib/debug',                  
    'Common_d.lib' 
  },
  Nebulous_Alpha = {       
    nil,                                                                 
    'Nebulous',                    
    'Nebulous/lib/debug',                  
    'Alpha_d.lib' 
  },
  Nebulous_Audio = {       
    nil,                                                                 
    'Nebulous',                    
    'Nebulous/lib/debug',                  
    'Audio_d.lib' 
  },
  Nebulous_Beta = {       
    nil,                                                                 
    'Nebulous',                    
    'Nebulous/lib/debug',                  
    'Beta_d.lib' 
  },

}