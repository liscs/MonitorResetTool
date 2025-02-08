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

	static const bool use_win11_24h2_color_functions = IsWindows11_24H2OrGreater();//24h2���

	// ����������ʾ��
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

	// ��ȡ��ʾ���� HDR ״̬
	Status GetDisplayHDRStatus(const DISPLAYCONFIG_MODE_INFO& mode);

	// ��ȡ������ʾ���� HDR ״̬
	Status GetWindowsHDRStatus();

	// ���õ�����ʾ���� HDR ״̬
	std::optional<Status> SetDisplayHDRStatus(const DISPLAYCONFIG_MODE_INFO& mode, bool enable);

	// ����������ʾ���� HDR ״̬
	std::optional<Status> SetWindowsHDRStatus(bool enable);

	// �л�������ʾ���� HDR ״̬
	std::optional<Status> ToggleHDRStatus();

} // namespace hdr
