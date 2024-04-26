// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;

public class VisOnEarth : ModuleRules
{
	public VisOnEarth(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDefinitions.Add("FLOWFIELD_USE_COMPUTE = true");
		PublicDefinitions.Add("FLOWFIELD_USE_VARIABLE_LINEWIDTH = true");
		PublicDefinitions.Add("FLOWFIELD_USE_INDIRECT_DRAW = false");

		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
				// Path.Combine(ModuleDirectory, "../ThirdParty/netCDF4.9.2-NC3-Win64/include")
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		// 作用是将PluginDir/Resouces/proj4/这个路径下的所有文件在打包时作为NonUFS文件拷贝到打包对应目录中。
		RuntimeDependencies.Add(Path.Combine("$(PluginDir)", "Resources", "NCData", "..."),StagedFileType.NonUFS);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"RHI",
				"RenderCore",
				"Json", 
				"JsonUtilities", 
				// For GeoReference Transform
				"CesiumRuntime",
				// For UI
				"UMG",
				"Slate",
				"SlateCore"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Projects"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
		// string libPrefix;
		// string libPostfix;
		// string platform;
		// string releasePostfix = "";
		// string debugPostfix = "d";
		//
		// bool preferDebug = (Target.Configuration == UnrealTargetConfiguration.Debug || Target.Configuration == UnrealTargetConfiguration.DebugGame);
		// string postfix = releasePostfix;
		// if (Target.Platform == UnrealTargetPlatform.Win64)
		// {
		// 	platform = "Windows-x64";
		// 	libPostfix = ".lib";
		// 	libPrefix = "";
		// }
		// else {
		// 	platform = "Unknown";
		// 	libPostfix = ".Unknown";
		// 	libPrefix = "Unknown";
		// }
		// string NetCDFLibPath = Path.Combine(ModuleDirectory, "../ThirdParty/netCDF4.9.2-NC3-Win64/lib");
		//
		// string[] libs = new string[]
		// {
		// 	"hdf5_hl",
		// 	"hdf5_tools",
		// 	"jpeg",
		// 	"libcurl_imp",
		// 	"libhdf",
		// 	"libhdf5",
		// 	"libhdf5_hl",
		// 	"libhdf5_tools",
		// 	"libmfhdf",
		// 	"libxdr",
		// 	"mfhdf",
		// 	"netcdf",
		// 	"xdr",
		// 	"zlib",
		// 	"zlibstatic"
		// };
		//
		// PublicAdditionalLibraries.AddRange(libs.Select(lib => Path.Combine(NetCDFLibPath, lib + libPostfix)));
		// Console.WriteLine(libs.Select(lib => Path.Combine(NetCDFLibPath, lib + libPostfix)));
	}
}
