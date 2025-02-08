#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Windows Header Files
#include <windows.h>

// Application instance
#include <optional>
#include <string>
#include <vector>

#include "WinVerCheck.hpp"

namespace hdr {
	enum class Status {
		Unsupported = 0,
		Off = 1,
		On = 2
	};

	/// Display information
	struct Display
	{
		/// Display name
		std::wstring name;
		/// HDR status
		Status status;
	};

	static const bool use_win11_24h2_color_functions = IsWindows11_24H2OrGreater();//24h2标记

	// 遍历所有显示器
	template<typename F>  void ForEachDisplay(F func)
	{
		uint32_t pathCount = 0;
		uint32_t modeCount = 0;

		if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount, &modeCount) != ERROR_SUCCESS)
			return;

		std::vector<DISPLAYCONFIG_PATH_INFO> paths(pathCount);
		std::vector<DISPLAYCONFIG_MODE_INFO> modes(modeCount);

		if (QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths.data(), &modeCount, modes.data(), 0) != ERROR_SUCCESS)
			return;

		for (const auto& path : paths) {
			const auto& mode = modes.at(path.targetInfo.modeInfoIdx);
			func(mode);
		}
	}

	// 获取显示器的 HDR 状态
	Status GetDisplayHDRStatus(const DISPLAYCONFIG_MODE_INFO& mode);

	// 获取所有显示器的 HDR 状态
	Status GetWindowsHDRStatus();

	// 设置单个显示器的 HDR 状态
	std::optional<Status> SetDisplayHDRStatus(const DISPLAYCONFIG_MODE_INFO& mode, bool enable);

	// 设置所有显示器的 HDR 状态
	std::optional<Status> SetWindowsHDRStatus(bool enable);

	// 切换所有显示器的 HDR 状态
	std::optional<Status> ToggleHDRStatus();

} // namespace hdr
