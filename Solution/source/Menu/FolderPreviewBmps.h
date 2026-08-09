#pragma once

#include <string>
#include <vector>

namespace DxHookIMG {
	class DxTexture;
}

namespace sub
{
	namespace FolderPreviewBmps_catind
	{
		extern bool& bFolderBmpsEnabled;
		extern std::string previewPng;
		extern std::vector<std::pair<std::string, DxHookIMG::DxTexture>> vFolderBmps;

		decltype(vFolderBmps)::iterator SearchBmps(const std::string& filePath);

		void GetThisFolderBmpPaths(const std::string& path, decltype(vFolderBmps)& newBmps);
		void PopulateFolderBmps();

		void DrawBmp(const std::string& folderPath);
	}

}


